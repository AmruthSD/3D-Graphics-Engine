#pragma once

#include <FastNoiseLite.h>
#include <camera.hpp>
#include <common.hpp>
#include <threadPool.hpp>

struct Coordinate {
  int x;
  int z;
  int height;
  Biome biome;
};

class Terrain {

  long long seed;
  std::mutex chunkMutex;

  Camera &camera;
  ThreadPool pool;

public:
  Terrain(Camera &camera, long long seed);
  Terrain(Camera &camera);
  unordered_map<Chunk, ChunkMesh, ChunkHash> ChunkMeshes;
  void generateChunk(int chunkX, int chunkZ);
  void requestChunk(int chunkX, int chunkZ,
                    std::function<void(Chunk)> callback);
  void deleteChunk(int chunkX, int chunkZ);
};