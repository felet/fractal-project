#version 150
in vec3 normal;
in vec3 position_to_frag;
in vec2 texCoord;
out vec4 out_Color;

uniform sampler2D texUnit;
uniform int setTexture;

uniform vec3 lightSourcesDirPosArr[4];
uniform vec3 lightSourcesColorArr[4];
uniform float specularExponent[4];
uniform bool isDirectional[4];

uniform mat4 totalMatrix;
uniform vec3 camera_position;
uniform float time;
uniform float lightBeat;
in vec4 position;
bool isFilled(int x, int y)
{
	int t=0;
    while(x>0 || y>0)
    {
        if(x%3==1 && y%3==1)
            return false;
        x /= 3;
        y /= 3;

	t++;
	if ( t > 4)
	break;
    }
    return true;
}

vec4 phong(){
    vec3 nNormal = normalize(normal);
    vec3 eyePosition = vec3(0.0);
    vec3 eyeDirection = normalize(eyePosition-position_to_frag);
    vec3 lightDirection[4], reflectedLightDirection[4];
    vec4 lightColor[4];
    float diffuseStrength[4], specularStrength[4];
    for (int i=0;i<4;i++)
    {
        if (isDirectional[i])
            lightDirection[i] = normalize(lightSourcesDirPosArr[i]);
        else
            lightDirection[i] = normalize(lightSourcesDirPosArr[i]-position_to_frag);

        reflectedLightDirection[i] = normalize(-reflect(lightDirection[i],nNormal));
        lightColor[i] = vec4(lightSourcesColorArr[i],1.0);
        diffuseStrength[i] = max(dot(lightDirection[i],nNormal),0);
        if (diffuseStrength[i] > 0.0)
        {
            specularStrength[i] = pow(max(dot(reflectedLightDirection[i],eyeDirection),0.01),specularExponent[i]);
        }
    }
    vec4 sum=vec4(0.0);
    for(int i=0;i<4;i++)
        sum += (diffuseStrength[i]+specularStrength[i])*lightColor[i];
    if(setTexture != 1)
        sum *= texture(texUnit,texCoord);
	return sum;
}
void main(void)
{
    if(setTexture==1)
    {
		out_Color = phong()*lightBeat;
    }
    else if (setTexture==0)
        out_Color = texture(texUnit,texCoord);
    else if (setTexture==2)
    {
        float light = lightBeat*2.0;
        vec2 temp = texCoord;
        temp *= 1000.0;
        if(isFilled(int(temp.x), int(temp.y)))
            out_Color = phong()*vec4(1.0, cos(time/1000.0) , sin(time/1200.0), 1.0)*light;
        else
            out_Color = vec4(0.0);
	}
	else
        out_Color = vec4(0.0);
}
