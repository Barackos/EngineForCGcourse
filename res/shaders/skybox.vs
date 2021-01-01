#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 Normal;
uniform mat4 MVP;
uniform mat4 avi;

void main()
{
    TexCoords = aPos;
    mat4 MV = mat4(MVP[0], MVP[1], MVP[2] ,vec4(0,0,10,1));//3.125));
    // MV = mat4(1, 0, 0, 0,
    //           0, 1, 0, 0,
    //           0, 0, 1, avi, // 10
    //           0, 0, 0, 1);
    MV = mat4(1.73205, 0, 0, 0,
              0, 1.73205, 0, 0,
              0, 0, -1.0202, -0.897975,
              0, 0, -1, 1.1);
    // MV = mat4(MVP[0][0], MVP[1][0], MVP[2][0], 0,
    //           MVP[0][1], MVP[1][1], MVP[2][1], 0,
    //           MVP[0][2], MVP[1][2], MVP[2][2], MVP[3][2],
    //           MVP[0][3], MVP[1][3], MVP[2][3], MVP[3][3]);
    // MV = inverse(avi) * MVP;
    gl_Position = Normal * MV * vec4(aPos, 1.0);
}  