#version 150

in vec3 in_Position;
in vec2 inTexCoord;
in vec3 in_Normal;
in vec3 in_Color;
out vec3 color;
out vec3 normal;
out vec2 TexCoord;
out vec3 position_to_frag;

uniform mat4 totalMatrix;
uniform mat4 transformation;
uniform int setTexture;
vec4 position;
out vec3 pos;
out vec3 colornorm;
void main(void)
{
    colornorm = in_Normal;
    color = in_Color;
    if(setTexture == 0)
        TexCoord = inTexCoord;
    else
    {
        position = transformation*vec4(in_Position, 1.0);
        TexCoord = normalize(position.xy);
        pos = in_Position;
    }

    gl_Position = totalMatrix*vec4(in_Position, 1.0);
	position_to_frag = vec3( transformation*vec4(in_Position, 1.0));
	normal = vec3( transformation*vec4(in_Normal, 1.0));
}
