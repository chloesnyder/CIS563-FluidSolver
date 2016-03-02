#version 330 core


uniform mat4 u_viewProj;

in vec3 v_pos;
in vec3 v_col;

out vec3 f_col;

void main(){
        f_col = v_col;
        gl_Position = u_viewProj * vec4(v_pos, 1);
}
