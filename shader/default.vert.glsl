#version 450

layout(location = 0) in ivec2 vertex;

void main()
{
    vec2 normalVertex = vec2(float(vertex.x), float(vertex.y)) / vec2(400.0, 225.0);
    normalVertex = normalVertex * 2. - 1.;

    gl_Position = vec4(normalVertex, 0.0, 1.0);
}
