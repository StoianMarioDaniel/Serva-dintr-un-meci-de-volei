#version 400 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord; 

uniform vec3 objectColor;
uniform vec3 viewPos;
uniform vec3 ambientColor; 


uniform vec3 lightPos1;    
uniform vec3 lightColor1;  
uniform vec3 lightPos2;    
uniform vec3 lightColor2;  


uniform sampler2D textureSampler; 
uniform bool useTexture;         
uniform bool isNetTexture; 

vec3 CalculatePointLight(vec3 lightP, vec3 lightC, vec3 normalN, vec3 fragP, vec3 viewDirV) {
    vec3 ambient = ambientColor * lightC; 
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

    vec3 totalLighting = vec3(0.0);
    totalLighting += CalculatePointLight(lightPos1, lightColor1, norm, FragPos, viewDir);
    totalLighting += CalculatePointLight(lightPos2, lightColor2, norm, FragPos, viewDir);
    totalLighting += vec3(0.05, 0.05, 0.05); 

    vec3 surfaceBaseColor;

    if (useTexture) {
        vec4 texSample = texture(textureSampler, TexCoord);
        
        if (isNetTexture) { 
            float whitenessThreshold = 0.85; 
            if (texSample.r > whitenessThreshold && texSample.g > whitenessThreshold && texSample.b > whitenessThreshold) {
                discard; 
            }
        }
        surfaceBaseColor = texSample.rgb; 
    } else {
        surfaceBaseColor = objectColor;
    }
    
    vec3 finalLitColor = totalLighting * surfaceBaseColor;
    FragColor = vec4(finalLitColor, 1.0);
}