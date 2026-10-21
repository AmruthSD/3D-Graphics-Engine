#!/usr/bin/env bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "Compiling Vulkan shaders..."
glslc shader.vert -o vert.spv
glslc shader.frag -o frag.spv
glslc raytracer.comp -o raytrace.comp.spv
echo "Shaders compiled successfully."
