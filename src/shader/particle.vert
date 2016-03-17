#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 squareVertices;
layout(location = 1) in vec4 v_pos; // Position of the center of the particule and size of the square
layout(location = 2) in vec4 v_col; // Position of the center of the particule and size of the square

// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec4 particlecolor;

// Values that stay constant for the whole mesh.
uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
uniform mat4 u_viewProj; // Model-View-Projection matrix, but without the Model (the position is in BillboardPos; the orientation depends on the camera)

void main()
{
        float particleSize = v_pos.w; // because we encoded it this way.
        vec3 particleCenter_wordspace = v_pos.xyz;

        vec3 vertexPosition_worldspace =
                particleCenter_wordspace
                + CameraRight_worldspace * squareVertices.x * particleSize
                + CameraUp_worldspace * squareVertices.y * particleSize;

        // Output position of the vertex
        gl_Position = u_viewProj * vec4(vertexPosition_worldspace, 1.0f);

        // UV of the vertex. No special space for this one.
        UV = squareVertices.xy + vec2(0.5, 0.5);
        particlecolor = v_col;
}

