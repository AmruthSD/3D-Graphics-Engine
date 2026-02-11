#version 450

layout(set = 0, binding = 1) uniform sampler2D biomeAtlas;

// layout(location = 0) in vec2 fragUV;
// layout(location = 1) flat in uint fragBiome;

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main() {

    // float textureCount = 4.0; // number of biome textures in atlas
    // float sliceWidth = 1.0 / textureCount;

    // float u = (float(fragBiome) + fragUV.x) * sliceWidth;
    // float v = fragUV.y;

    // outColor = texture(biomeAtlas, vec2(u, v));

    outColor = vec4(fragColor, 1.0);
}

