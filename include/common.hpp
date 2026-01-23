#pragma once
#include <bits/stdc++.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

using namespace std;

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

struct vertex {
  float x, y, z;
};

struct triangle {
  array<vertex, 3> vertices;
};
