#version 150

in vec3 in_Position;
in vec2 inTexCoord;
in vec3 in_Normal;
in vec3 in_Color;
out vec3 color;
out vec3 normal;
out vec2 texCoord;
out vec3 position_to_frag;

uniform mat4 totalMatrix;
uniform mat4 projection;
uniform mat4 camera;
uniform mat4 translation;
uniform mat4 scaling;
uniform int setTexture;
uniform float time;
uniform int modeScale;

vec4 position;
mat4 transformation;

void main(void)
{
    color = in_Color;
    transformation = projection*translation;
    if(setTexture == 0 || setTexture == 2)
        texCoord = inTexCoord;
    else
    {
        position = transformation*vec4(in_Position, 1.0);
        texCoord = normalize(position.xy);
    }
    if(modeScale == 1)
    {
        /*
        position_to_frag = vec3(transformation*vec4(in_Position, 1.0));
        gl_Position = projection*camera*translation*scaling*vec4(in_Position, 1.0);
        normal = vec3(translation*vec4(in_Normal, 1.0));
        */
        position_to_frag = vec3(transformation*vec4(in_Position, 1.0));
        position = projection*camera*translation*scaling*vec4(in_Position, 1.0);
        gl_Position = position;
        normal = vec3(translation*vec4(in_Normal, 1.0));
        
    }
    else
    {
        position_to_frag = vec3(transformation*vec4(in_Position, 1.0));
        gl_Position = projection*camera*translation*vec4(in_Position, 1.0);
        normal = vec3(translation*vec4(in_Normal, 1.0));
    }
	//position_to_frag = vec3( projection*translation*vec4(in_Position, 1.0));
	//normal = vec3( transformation*vec4(in_Normal, 1.0));
}
