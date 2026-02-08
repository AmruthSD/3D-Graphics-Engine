#include <terrain.hpp>

void addQuad(ChunkMesh &mesh, glm::vec3 a, glm::vec3 b, glm::vec3 c,
             glm::vec3 d, Biome biome) {
  // Triangle 1
  mesh.vertices.push_back({a, (uint8_t)biome});
  mesh.vertices.push_back({b, (uint8_t)biome});
  mesh.vertices.push_back({c, (uint8_t)biome});

  // Triangle 2
  mesh.vertices.push_back({a, (uint8_t)biome});
  mesh.vertices.push_back({c, (uint8_t)biome});
  mesh.vertices.push_back({d, (uint8_t)biome});
}

Terrain::Terrain(Camera &camera_, long long seed_)
    : camera(camera_), seed(seed_), pool(std::thread::hardware_concurrency()) {}

Terrain::Terrain(Camera &camera_)
    : Terrain(camera_, std::chrono::high_resolution_clock::now()
                           .time_since_epoch()
                           .count()) {}

void Terrain::requestChunk(int chunkX, int chunkZ) {
  Chunk chunk{chunkX, chunkZ};

  {
    std::lock_guard<std::mutex> lock(chunkMutex);
    if (ChunkMeshes.find(chunk) != ChunkMeshes.end())
      return;
  }

  pool.enqueue([this, chunkX, chunkZ]() { generateChunk(chunkX, chunkZ); });
}

void Terrain::generateChunk(int chunkX, int chunkZ) {
  std::vector<Coordinate> result;
  result.reserve(CHUNK_SIZE * CHUNK_SIZE);

  FastNoiseLite continentNoise;
  continentNoise.SetSeed(static_cast<int>(seed));
  continentNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
  continentNoise.SetFrequency(0.002f);

  FastNoiseLite biomeNoise;
  biomeNoise.SetSeed(static_cast<int>(seed + 1337));
  biomeNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
  biomeNoise.SetFrequency(0.01f);

  FastNoiseLite heightNoise;
  heightNoise.SetSeed(static_cast<int>(seed + 4242));
  heightNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
  heightNoise.SetFrequency(0.05f);

  for (int x = 0; x < CHUNK_SIZE; x++) {
    for (int z = 0; z < CHUNK_SIZE; z++) {

      int worldX = chunkX * CHUNK_SIZE + x;
      int worldZ = chunkZ * CHUNK_SIZE + z;

      float c =
          (continentNoise.GetNoise((float)worldX, (float)worldZ) + 1.0f) * 0.5f;

      Biome biome;
      int height;

      if (c < 0.45f) {
        biome = Biome::Ocean;
        height = SEA_LEVEL;
      } else {
        float b =
            (biomeNoise.GetNoise((float)worldX, (float)worldZ) + 1.0f) * 0.5f;
        float h =
            (heightNoise.GetNoise((float)worldX, (float)worldZ) + 1.0f) * 0.5f;

        if (b < 0.4f) {
          biome = Biome::Plain;
          height = SEA_LEVEL + static_cast<int>(h * 20);
        } else if (b < 0.75f) {
          biome = Biome::Mountain;
          height = SEA_LEVEL + static_cast<int>(h * 60);
        } else {
          biome = Biome::Snow;
          height = SEA_LEVEL + 40 + static_cast<int>(h * 50);
        }
      }

      result.push_back({worldX, worldZ, height, biome});
    }
  }

  auto idx = [&](int lx, int lz) -> int { return lx * CHUNK_SIZE + lz; };

  auto inChunk = [&](int lx, int lz) -> bool {
    return lx >= 0 && lx < CHUNK_SIZE && lz >= 0 && lz < CHUNK_SIZE;
  };

  auto getHeight = [&](int lx, int lz) -> int {
    if (!inChunk(lx, lz))
      return SEA_LEVEL;
    return result[idx(lx, lz)].height;
  };

  ChunkMesh mesh;

  for (int x = 0; x < CHUNK_SIZE; x++) {
    for (int z = 0; z < CHUNK_SIZE; z++) {

      const Coordinate &cell = result[idx(x, z)];

      int wx = cell.x;
      int wz = cell.z;
      int h = cell.height;
      Biome biome = cell.biome;

      addQuad(mesh, {wx, h, wz}, {wx, h, wz + 1}, {wx + 1, h, wz + 1},
              {wx + 1, h, wz}, biome);

      int hr = getHeight(x + 1, z);
      if (h > hr) {
        addQuad(mesh, {wx + 1, h, wz}, {wx + 1, h, wz + 1},
                {wx + 1, hr, wz + 1}, {wx + 1, hr, wz}, biome);
      }

      int hl = getHeight(x - 1, z);
      if (h > hl) {
        addQuad(mesh, {wx, h, wz + 1}, {wx, h, wz}, {wx, hl, wz},
                {wx, hl, wz + 1}, biome);
      }

      int hf = getHeight(x, z + 1);
      if (h > hf) {
        addQuad(mesh, {wx + 1, h, wz + 1}, {wx, h, wz + 1}, {wx, hf, wz + 1},
                {wx + 1, hf, wz + 1}, biome);
      }

      int hb = getHeight(x, z - 1);
      if (h > hb) {
        addQuad(mesh, {wx, h, wz}, {wx + 1, h, wz}, {wx + 1, hb, wz},
                {wx, hb, wz}, biome);
      }
    }
  }

  ChunkMeshes[{chunkX, chunkZ}] = std::move(mesh);
}

void Terrain::deleteChunk(int chunkX, int chunkZ) {
  Chunk chunk{chunkX, chunkZ};

  std::lock_guard<std::mutex> lock(chunkMutex);
  ChunkMeshes.erase(chunk);
}