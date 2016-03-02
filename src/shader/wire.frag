#version 330 core

in vec3 f_col;
out vec4 out_Col;

void main() {
    out_Col = vec4(f_col.rgb, 1);
}
