#version 400 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  

uniform vec3 objectColor; // Culoarea de baza a obiectului (material difuz)
uniform vec3 lightPos1;   // Pozitia primei surse de lumina
uniform vec3 lightColor1; // Culoarea primei surse de lumina
uniform vec3 lightPos2;   // Pozitia celei de-a doua surse de lumina
uniform vec3 lightColor2; // Culoarea celei de-a doua surse de lumina
uniform vec3 viewPos;     // Pozitia camerei/observatorului

uniform vec3 ambientColor; // Culoarea luminii ambientale globale

// Functie pentru calculul iluminarii de la o singura sursa
vec3 CalculatePointLight(vec3 lightP, vec3 lightC, vec3 normalN, vec3 fragP, vec3 viewDirV) {
    // Ambient
    float ambientStrength = 0.2; // Crestem putin componenta ambientala
    vec3 ambient = ambientStrength * lightC * ambientColor; // Lumina ambientala specifica sursei X ambient global
  	
    // Diffuse 
    vec3 lightDir = normalize(lightP - fragP);
    float diff = max(dot(normalN, lightDir), 0.0);
    vec3 diffuse = diff * lightC;
    
    // Specular
    float specularStrength = 0.6; // Componenta speculara mai puternica
    vec3 reflectDir = reflect(-lightDir, normalN);
    float spec = pow(max(dot(viewDirV, reflectDir), 0.0), 32); // Shininess = 32
    vec3 specular = specularStrength * spec * lightC; 
    
    return (ambient + diffuse + specular);
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 lighting = vec3(0.0);
    lighting += CalculatePointLight(lightPos1, lightColor1, norm, FragPos, viewDir);
    lighting += CalculatePointLight(lightPos2, lightColor2, norm, FragPos, viewDir);
    
    // Combinam iluminarea cu culoarea obiectului
    // Culoarea finala este produsul dintre culoarea calculata a luminii si culoarea obiectului
    vec3 result = lighting * objectColor;
    FragColor = vec4(result, 1.0);
}