#version 400 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  

uniform vec3 objectColor; 
uniform vec3 lightPos1;   
uniform vec3 lightColor1; 
uniform vec3 lightPos2; 
uniform vec3 lightColor2; 
uniform vec3 viewPos;     

uniform vec3 ambientColor; 
vec3 CalculatePointLight(vec3 lightP, vec3 lightC, vec3 normalN, vec3 fragP, vec3 viewDirV) {
    float ambientStrength = 0.2; 
    vec3 ambient = ambientStrength * lightC * ambientColor; 
  	
    vec3 lightDir = normalize(lightP - fragP);
    float diff = max(dot(normalN, lightDir), 0.0);
    vec3 diffuse = diff * lightC;
    
    float specularStrength = 0.6; 
    vec3 reflectDir = reflect(-lightDir, normalN);
    float spec = pow(max(dot(viewDirV, reflectDir), 0.0), 32); 
    vec3 specular = specularStrength * spec * lightC; 
    
    return (ambient + diffuse + specular);
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 lighting = vec3(0.0);
    lighting += CalculatePointLight(lightPos1, lightColor1, norm, FragPos, viewDir);
    lighting += CalculatePointLight(lightPos2, lightColor2, norm, FragPos, viewDir);
    
    vec3 result = lighting * objectColor;
    FragColor = vec4(result, 1.0);
}