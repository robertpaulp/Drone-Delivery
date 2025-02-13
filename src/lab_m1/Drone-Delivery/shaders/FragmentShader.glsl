#version 330

in float noise;

out vec4 out_color;

void main()
{
    vec3 color_one = vec3(0.7, 0.9, 1);
    vec3 color_two = vec3(1, 1, 1);

    vec3 final_color = mix(color_one, color_two, noise);

    out_color = vec4(final_color, 1.0);
}