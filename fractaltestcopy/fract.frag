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

void main(void)
{
    if(setTexture==1)
    {
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
        out_Color = sum;
    }
    else if (setTexture==0)
        out_Color = texture(texUnit,texCoord);
    else
        out_Color = vec4(0.0);
}
