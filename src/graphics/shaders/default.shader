#shader vertex
#version 460 core

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec3 aColor;

out vec3 color;

void main() {
    gl_Position = vec4(aPosition.x, aPosition.y, 0.0f, 1.0f);
    color = aColor;
}

#shader fragment
#version 460 core

in vec3 color;

out vec4 fragColor;

void main() {
    fragColor = vec4(color, 0.5f);
}
