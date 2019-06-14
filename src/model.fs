#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
in vec4 FragPosLightSpace;

uniform sampler2D texture_diffuse1;
uniform sampler2D shadowMap;
uniform vec3 lightPos;
uniform vec3 viewPos;

float diffuseStrength = 1.0;
float specularStrength = 1.0;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // 取得当前片元在光源视角下的深度
    float currentDepth = projCoords.z;
    // 检查当前片元是否在阴影中
    float shadow = currentDepth - 0.005 > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

void main()
{   
	vec3 color = texture(texture_diffuse1, TexCoords).rgb;
	vec3 lightColor = vec3(1.0);
	vec3 norm = normalize(Normal);
	//ambient
	vec3 ambient = 0.2 * color;
	//specular
	vec3 lightDir = normalize(lightPos - FragPos);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir,Normal);
	float spec = pow(max(dot(Normal, normalize(lightDir + viewDir)), 0.0), 32.0);
	vec3 specular = specularStrength * spec * lightColor;
	//diffuse
	float diff = max(dot(lightDir, norm), 0.0);
	vec3 diffuse = diffuseStrength * diff * lightColor;

	//shadow
	float shadow = ShadowCalculation(FragPosLightSpace);       
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    FragColor = vec4(lighting, 1.0);
}