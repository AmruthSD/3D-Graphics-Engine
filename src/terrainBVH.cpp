#include <terrain.hpp>

struct BVHBuildEntry {
  int start;
  int count;
};

glm::vec3 minVec(const glm::vec3 &a, const glm::vec3 &b) {
  return glm::min(a, b);
}

glm::vec3 maxVec(const glm::vec3 &a, const glm::vec3 &b) {
  return glm::max(a, b);
}

void computeBounds(const std::vector<TriangleGPU> &tris, int start, int count,
                   glm::vec3 &outMin, glm::vec3 &outMax) {
  outMin = glm::vec3(FLT_MAX);
  outMax = glm::vec3(-FLT_MAX);

  for (int i = 0; i < count; i++) {
    const auto &t = tris[start + i];

    outMin = minVec(outMin, minVec(minVec(t.v0, t.v1), t.v2));
    outMax = maxVec(outMax, maxVec(maxVec(t.v0, t.v1), t.v2));
  }
}

void Terrain::buildBVH(std::vector<TriangleGPU> &triangles,
                       std::vector<BVHNodeGPU> &nodes) {
  nodes.clear();

  if (triangles.empty())
    return;

  // Recursive lambda
  std::function<int(int, int)> buildNode = [&](int start, int count) -> int {
    int nodeIndex = nodes.size();
    nodes.push_back({});

    glm::vec3 boundsMin, boundsMax;
    computeBounds(triangles, start, count, boundsMin, boundsMax);

    // Leaf condition
    if (count <= 4) {
      BVHNodeGPU leaf{};
      leaf.minBounds = boundsMin;
      leaf.maxBounds = boundsMax;
      leaf.leftFirst = start;
      leaf.count = count;

      nodes[nodeIndex] = leaf;
      return nodeIndex;
    }

    // Compute centroid bounds
    glm::vec3 centroidMin(FLT_MAX);
    glm::vec3 centroidMax(-FLT_MAX);

    for (int i = 0; i < count; i++) {
      const auto &t = triangles[start + i];
      glm::vec3 centroid = (t.v0 + t.v1 + t.v2) / 3.0f;

      centroidMin = minVec(centroidMin, centroid);
      centroidMax = maxVec(centroidMax, centroid);
    }

    glm::vec3 extent = centroidMax - centroidMin;

    int axis = 0;
    if (extent.y > extent.x)
      axis = 1;
    if (extent.z > extent[axis])
      axis = 2;

    float splitPos = centroidMin[axis] + extent[axis] * 0.5f;

    // Partition triangles by centroid
    int mid = start;
    for (int i = start; i < start + count; i++) {
      const auto &t = triangles[i];
      glm::vec3 centroid = (t.v0 + t.v1 + t.v2) / 3.0f;

      if (centroid[axis] < splitPos) {
        std::swap(triangles[i], triangles[mid]);
        mid++;
      }
    }

    int leftCount = mid - start;

    // Handle bad splits
    if (leftCount == 0 || leftCount == count) {
      leftCount = count / 2;
      mid = start + leftCount;
    }

    int leftChild = buildNode(start, leftCount);
    int rightChild = buildNode(mid, count - leftCount);

    BVHNodeGPU node{};
    node.minBounds = boundsMin;
    node.maxBounds = boundsMax;
    node.leftFirst = leftChild;
    node.count = 0; // internal node

    nodes[nodeIndex] = node;

    return nodeIndex;
  };

  buildNode(0, triangles.size());
}