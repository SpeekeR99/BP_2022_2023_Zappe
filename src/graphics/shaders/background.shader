#shader vertex
#version 460 core

layout(location = 0) in vec4 position;

void main() {
    gl_Position = position;
}

#shader fragment
#version 460 core

layout(location = 0) out vec4 color;

void main() {
    color = vec4(29.0 / 255.0, 29.0 / 255.0, 29.0 / 255.0, 1.0);
}
