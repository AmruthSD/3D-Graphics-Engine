#include <terrain.hpp>

void addQuad(ChunkMesh &mesh, glm::vec3 a, glm::vec3 b, glm::vec3 c,
             glm::vec3 d, Biome biome) {
  uint8_t bID = (uint8_t)biome;

  // Triangle 1 (a, b, c)
  mesh.vertices.push_back({a, bID, glm::vec2(0.0f, 0.0f)});
  mesh.vertices.push_back({b, bID, glm::vec2(1.0f, 0.0f)});
  mesh.vertices.push_back({c, bID, glm::vec2(1.0f, 1.0f)});

  // Triangle 2 (a, c, d)
  mesh.vertices.push_back({a, bID, glm::vec2(0.0f, 0.0f)});
  mesh.vertices.push_back({c, bID, glm::vec2(1.0f, 1.0f)});
  mesh.vertices.push_back({d, bID, glm::vec2(0.0f, 1.0f)});
}

Terrain::Terrain(Camera &camera_, long long seed_)
    : camera(camera_), seed(seed_), pool(std::thread::hardware_concurrency()) {}

Terrain::Terrain(Camera &camera_)
    : Terrain(camera_, std::chrono::high_resolution_clock::now()
                           .time_since_epoch()
                           .count()) {}

void Terrain::requestChunk(int chunkX, int chunkZ,
                           std::function<void(Chunk)> callback) {
  Chunk chunk{chunkX, chunkZ};

  {
    std::lock_guard<std::mutex> lock(chunkMutex);
    if (ChunkMeshes.find(chunk) != ChunkMeshes.end())
      return;
  }

  pool.enqueue([chunk, callback]() { callback(chunk); });
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

      float continent =
          (continentNoise.GetNoise((float)worldX, (float)worldZ) + 1.0f) * 0.5f;

      Biome biome;
      int height;

      // ----------- OCEAN ------------
      if (continent < 0.48f) {
        biome = Biome::Ocean;
        height = SEA_LEVEL;
      } else {

        float biomeVal =
            (biomeNoise.GetNoise((float)worldX, (float)worldZ) + 1.0f) * 0.5f;

        float h =
            (heightNoise.GetNoise((float)worldX, (float)worldZ) + 1.0f) * 0.5f;

        // -------- PLAINS (dominant biome)
        if (biomeVal < 0.65f) {

          biome = Biome::Plain;

          h = h * h; // flatten heavily
          height = SEA_LEVEL + (int)(h * 6);
        }

        // -------- MOUNTAINS (rare + sharp)
        else if (biomeVal < 0.85f) {

          biome = Biome::Mountain;

          float m = pow(h, 4.0f); // very sharp peaks
          height = SEA_LEVEL + (int)(m * 60);
        }

        // -------- SNOW (very rare highlands)
        else {

          biome = Biome::Snow;

          float s = pow(h, 2.5f);
          height = SEA_LEVEL + 35 + (int)(s * 30);
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