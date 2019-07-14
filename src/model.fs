#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform samplerCube depthCubeMap;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float far_plane;

float diffuseStrength = 1.0;
float specularStrength = 0.6;

float ShadowCalculation(vec3 fragPos)
{
    // Get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;
    // Use the light to fragment vector to sample from the depth map    
    float closestDepth = texture(depthCubeMap, fragToLight).r;
    // It is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth *= far_plane;
    // Now get current linear depth as the length between the fragment and 
    float currentDepth = length(fragToLight);
    // Now test for shadows
    float bias = 0.2;
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;

    //FragColor = vec4(vec3(closestDepth / far_plane), 1.0);

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
	vec3 reflectDir = reflect(-lightDir,norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;
	//diffuse
	float diff = max(dot(lightDir, norm), 0.0);
	vec3 diffuse = diffuseStrength * diff * lightColor;

	//shadow
	float shadow = ShadowCalculation(FragPos);       
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    FragColor = vec4(lighting, 1.0);

    //gamma correction
    float gamma = 2.2;
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}