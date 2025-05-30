#version 400 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal; // Noul atribut pentru normale

out vec3 FragPos;   // Pozitia fragmentului in spatiul lumii
out vec3 Normal;    // Normala in spatiul lumii

uniform mat4 model; // Matricea de model (separata de view si projection)
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    // Transformam normala in spatiul lumii folosind transpusa inversei matricii de model
    // (pentru a gestiona corect scalarea non-uniforma)
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}