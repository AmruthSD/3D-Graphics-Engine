#pragma once
#include <bits/stdc++.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

using namespace std;

struct vertex {
  float x, y, z;
};

struct triangle {
  array<vertex, 3> vertices;
};
