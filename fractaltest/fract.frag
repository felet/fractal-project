#version 150

in vec3 color;
uniform mat3 inColor;

out vec4 out_Color;
in vec3 normal;
in vec3 position_to_frag;
in vec2 TexCoord;
uniform sampler2D texUnit;
uniform int setTexture;

uniform vec3 lightSourcesDirPosArr[4];
uniform vec3 lightSourcesColorArr[4];
uniform float specularExponent[4];
uniform bool isDirectional[4];

uniform mat4 totalMatrix;
uniform vec3 camera_position;
vec3 sum = vec3(0.0f);
vec3 r,v;
void main(void)
{
        if(setTexture==0){
            out_Color = texture(texUnit, TexCoord);
        }
        else if(setTexture==1){
            vec3 normalized_normal = normalize(normal);

            vec3 sum = vec3(0.0f);
            for(int i=0; i<4; i++){
                if(isDirectional[i]){
                    r = normalize(2 * normalized_normal * 
                                    dot(normalize(lightSourcesDirPosArr[i]), normalized_normal) - normalize(lightSourcesDirPosArr[i]));
                    v = normalize(camera_position - position_to_frag);
                    sum = sum + lightSourcesColorArr[i] * max(dot(normalize(lightSourcesDirPosArr[i]), normalized_normal), 0.0f);
                    sum = sum + lightSourcesColorArr[i] * max(pow(dot(r,v), specularExponent[i]),0.0f);
                }
                else{
                    r = normalize(2 * normalized_normal * 
                                dot(normalize(lightSourcesDirPosArr[i]-position_to_frag), normalized_normal) - 
                                normalize(lightSourcesDirPosArr[i] - position_to_frag));
                    v = normalize(camera_position - position_to_frag);
                    sum = sum + lightSourcesColorArr[i] * max(dot(normalized_normal,normalize(lightSourcesDirPosArr[i]-position_to_frag)), 0.0f);
                    sum = sum + lightSourcesColorArr[i] * pow(max(dot(r,v),0.0f), specularExponent[i]);
                }
            }
            sum *= texture(texUnit, TexCoord).xyz;
            sum = normalize(sum);
            out_Color = vec4(sum, 1.0 );
        }
        else if(setTexture==2)
        {
            out_Color = normalize(vec4(TexCoord.x*0.5, TexCoord.y*0.2, 0.2, 1.0));
        }
        else
            out_Color = vec4(0.0,0.0,0.0,1.0);
}
