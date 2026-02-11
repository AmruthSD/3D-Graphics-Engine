#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in uint inBiome;
// layout(location = 2) in vec2 inUV;

layout(location = 0) out vec3 fragColor;

// layout(location = 0) out vec2 fragUV;
// layout(location = 1) flat out uint fragBiome;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

/*
Biome IDs (must match CPU side):
0 = Ocean
1 = Plain
2 = Mountain
3 = Snow
*/

vec3 biomeColor(uint biome) {
    switch (biome) {
        case 0: return vec3(0.4, 0.7, 1.0);  // Ocean (light blue)
        case 1: return vec3(0.2, 0.8, 0.2);  // Plain
        case 2: return vec3(0.5, 0.5, 0.5);  // Mountain
        case 3: return vec3(0.95, 0.95, 1.0); // Snow
        default: return vec3(1.0, 0.0, 1.0);
    }
}

void main() {
    gl_Position =
        ubo.proj *
        ubo.view *
        ubo.model *
        vec4(inPosition, 1.0);

    // fragUV = inUV;
    // fragBiome = inBiome;

        fragColor = biomeColor(inBiome);

}