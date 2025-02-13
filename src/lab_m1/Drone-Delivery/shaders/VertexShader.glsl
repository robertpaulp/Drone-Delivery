#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Output values to fragment shader
out vec3 world_position;
out vec3 world_normal;
out float noise;

// 2D Random Function
float random(in vec2 st) {
    // A simple hash function based on the input vector
    // This creates a pseudo-random value between 0.0 and 1.0
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

// Smoothstep function for smooth interpolation
vec2 fade(vec2 t) {
    // Using the smoothstep function to ease the interpolation
    // Alternatively, you can use the smootherstep or a custom fade function
    return t * t * (3.0 - 2.0 * t);
}

float computeNoise(in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // Smooth Interpolation
    vec2 u = fade(f);

    // Mix 4 corners percentages using bilinear interpolation
    float mix1 = mix(a, b, u.x);
    float mix2 = mix(c, d, u.x);
    return mix(mix1, mix2, u.y);
}

void main()
{
    // Transform the vertex position to world space
    world_position = (Model * vec4(v_position, 1.0)).xyz;

    // Transform and normalize the normal vector
    world_normal = normalize(mat3(Model) * normalize(v_normal));

    // Compute noise based on the XZ position, scaled by 5 for frequency
    vec2 pos = vec2(world_position.xz * 5.0);
    noise = computeNoise(pos);

    // Increase the deformation amplitude by multiplying noise by a larger factor
    float deformationAmplitude = 0.5; // Adjust this value to control the deformation strength
    gl_Position = Projection * View * Model * vec4(v_position + vec3(0.0, noise * deformationAmplitude, 0.0), 1.0);
}
