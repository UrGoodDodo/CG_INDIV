const char* VertexShader = R"(
#version 330 core

layout (location = 0) in vec3 coord;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 coordtex;

out vec2 coord_tex;
out vec3 normal;
out vec3 pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
    
void main() 
{ 
    gl_Position = projection * view * model * vec4(coord, 1.0);
    coord_tex = coordtex;
	pos = vec3(model * vec4(coord, 1.0));
	normal = mat3(transpose(inverse(model))) * norm;
}
)";


const char* FragShader = R"(
#version 330 core

uniform	vec3 position;
uniform	vec3 direction;
uniform	vec3 direction_spot; 
  
uniform	vec3 ambient;
uniform	vec3 diffuse;
uniform	vec3 specular;

uniform	float constant_atten;
uniform	float linear_atten;
uniform	float quadratic_atten;

uniform	float cutOff;
uniform	float outerCutOff;


in vec2 coord_tex;
in vec3 pos;
in vec3 normal;

uniform sampler2D tex;
uniform vec3 viewPos;
uniform vec3 airshipPos;
uniform int shininess;

out vec4 FragColor;

void main()  
{

	// ќбычный направленный свет

	vec3 norm = normalize(normal);
	vec3 lightDir;

	lightDir = normalize(-direction);
	vec3 tambient = ambient * texture(tex, coord_tex).rgb; 
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 tdiffuse = diffuse * (diff * texture(tex, coord_tex).rgb); 
	vec3 viewDir = normalize(viewPos - pos);
	vec3 reflectDir = reflect(-lightDir, norm);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1);
	vec3 tspecular = specular * (spec * texture(tex, coord_tex).rgb); 

	vec3 result = (tambient*2 + tdiffuse + tspecular);

	//+ свет от прожектора
	
	lightDir = normalize(airshipPos - pos);
	diff = max(dot(norm, lightDir), 0.0);
	tdiffuse = diffuse * (diff * texture(tex, coord_tex).rgb); 
	reflectDir = reflect(-lightDir, norm);  
	spec = pow(max(dot(viewDir, reflectDir), 0.0), 1);
	tspecular = specular * (spec * texture(tex, coord_tex).rgb);

	float dist = length(airshipPos - pos);
	float atten = 1.0 / (constant_atten + linear_atten * dist + quadratic_atten * (dist* dist));			
	tdiffuse *= atten;
	tspecular *= atten;  
			
	float theta = dot(lightDir, normalize(-direction_spot)); 
	float intensity = clamp((theta - outerCutOff) / (cutOff - outerCutOff), 0.0, 1.0);
	tdiffuse  *= intensity;
	tspecular *= intensity;

	result +=  tdiffuse + tspecular;
	result +=  tdiffuse;
			
	FragColor = vec4(result, 1.0);
} 
)";
