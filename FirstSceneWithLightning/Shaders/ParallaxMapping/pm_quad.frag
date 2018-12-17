#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;

uniform float heightScale;

vec2 ReliefPM(vec2 inTexCoords, vec3 inViewDir) {
	const float _minLayers = 2.;
	const float _maxLayers = 32.;
	float _numLayers = mix(_maxLayers, _minLayers, abs(dot(vec3(0., 0., 1.), inViewDir)));

	float deltaDepth = 1./_numLayers;
	vec2 deltaTexcoord = heightScale * inViewDir.xy/(inViewDir.z * _numLayers);

	vec2 currentTexCoords = inTexCoords;
	float currentLayerDepth = 0.;

	float currentDepthValue = texture(texture_height1, currentTexCoords).r;
	while (currentDepthValue > currentLayerDepth) {
		currentLayerDepth += deltaDepth;
		currentTexCoords -= deltaTexcoord;
		currentDepthValue = texture(texture_height1, currentTexCoords).r;
	}
// ======
// Relief PM 
// ======

	deltaTexcoord *= 0.5;
	deltaDepth *= 0.5;

	currentTexCoords += deltaTexcoord;
	currentLayerDepth -= deltaDepth;

	const int _reliefSteps = 5;
	int currentStep = _reliefSteps;
	while (currentStep > 0) {
		currentDepthValue = texture(texture_height1, currentTexCoords).r;
		deltaTexcoord *= 0.5;
		deltaDepth *= 0.5;
		if (currentDepthValue > currentLayerDepth) {
			currentTexCoords -= deltaTexcoord;
			currentLayerDepth += deltaDepth;
		}
		else {
			currentTexCoords += deltaTexcoord;
			currentLayerDepth -= deltaDepth;
		}
		currentStep--;
	}
	return currentTexCoords;
}

void main()
{           
    // offset texture coordinates with Parallax Mapping
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec2 texCoords = fs_in.TexCoords;
    
    texCoords = ReliefPM(fs_in.TexCoords,  viewDir);
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;

    // obtain normal from normal map
    vec3 normal = texture(texture_normal1, texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);   
   
    // get diffuse color
    vec3 color = texture(texture_diffuse1, texCoords).rgb;
    // ambient
    vec3 ambient = 0.05 * color;
    // diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular    
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.2) * spec;
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}