#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 Position;

uniform sampler2D texture_diffuse1;
uniform vec3 cameraPos;
uniform samplerCube skybox;
uniform int reflectState;

void main()
{             
    float ratio = 1.00 / 1.52;
    vec3 I = normalize(Position - cameraPos);
	vec3 R;
	if (reflectState == 1) {
		R = reflect(I, normalize(Normal));
    } else {
		R = refract(I, normalize(Normal), ratio);
	}
	FragColor = vec4(texture(skybox, R).rgb, 1.0);
    //FragColor = texture(texture_diffuse1, TexCoords);
}

