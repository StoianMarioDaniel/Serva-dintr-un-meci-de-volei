#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <vector>
#include <cmath>
#include <stdio.h>
#include <cstdlib> 
#include <ctime>   

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>
#include <glm/gtc/random.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define PI glm::pi<float>()

// Shader, VAO/VBO pentru cub (TOATE cuburile vor folosi acest VAO/VBO)
GLuint shader_programme, vao, vbo;
glm::mat4 projectionMatrix, viewMatrix;
// Uniform ID-uri
GLuint modelMatrixID_uniform;
GLuint viewMatrixID_uniform;
GLuint projectionMatrixID_uniform;
GLuint colorID;
GLuint textureSamplerID;
GLuint useTextureID;
GLuint isNetTextureID;
GLuint floorWoodTextureID;
GLuint wallTextureID;
GLuint ceilingTextureID;
GLuint netTextureID;

// VAO/VBO pentru podeaua generala a camerei
GLuint generalFloorVao, generalFloorVbo, generalFloorIbo;
std::vector<float> generalFloorVertices_data;
std::vector<unsigned int> generalFloorIndices_data;


// Animatie mana
float animationProgress = 0.0f;
const float animationDuration = 1.5f;
bool isAnimatingForward = false;
bool isAnimatingBackward = false;
int lastUpdateTime = 0;

// Dimensiuni Teren de Joc
float courtPlayingAreaW = 9.0f;
float courtPlayingAreaL = 18.0f;
float courtPlayingAreaThickness = 0.1f;
float floorTopSurfaceY = 0.0f;

// Pozitia de baza a jucatorului/bratului
float playerBaseX = 0.0f;
float playerBaseY = 1.3f;
float playerBaseZ = -(courtPlayingAreaL / 2.0f) - 1.5f;


// Keyframes pentru animatia mainii
const int NUM_POSES = 5;
const int NUM_JOINTS = 11;
float keyframePoses[NUM_POSES][NUM_JOINTS] = {
    {  0.05f,     PI / 7.0f,  PI / 2.5f, 0.0f,      0.0f,       0.0f,     PI / 4.0f, PI / 4.0f, -PI / 8.0f, PI / 5.0f, PI / 5.0f },
    { -PI / 8.0f,  PI / 2.0f,  PI / 1.7f, 0.15f,    -PI / 3.5f,  0.0f,     PI / 7.0f, PI / 7.0f, -PI / 7.0f, PI / 6.0f, PI / 6.0f },
    { -PI / 5.0f,  PI / 1.6f,  PI / 1.8f, 0.3f,     -PI / 2.8f,  0.05f,    PI / 9.0f, PI / 9.0f, -PI / 6.0f, PI / 7.0f, PI / 7.0f },
    {  0.0f,      PI / 4.0f,  PI / 10.0f, PI / 2.0f,  PI / 3.0f,  0.0f,     0.0f,      0.0f,      -PI / 10.0f,0.0f,      0.0f },
    {  PI / 6.0f, -PI / 5.0f, PI / 3.5f, PI / 2.5f,  PI / 7.0f,  0.0f,     PI / 5.0f, PI / 5.0f, -PI / 9.0f, PI / 7.0f, PI / 7.0f }
};
int hitKeyframeIndex = 3;


// Unghiuri curente pentru articulatiile mainii
float currentShoulderY, currentShoulderZ, currentElbow;
float currentWristPronation, currentWristZ, currentWristY;
float currentFingerKnuckle, currentFingerMid;
float currentThumbBaseZ, currentThumbKnuckle, currentThumbMid;

// Dimensiuni parti mana
float armLength = 2.5f * 0.75f; float armWidth = 0.6f * 0.75f;
float forearmLength = 2.0f * 0.75f; float forearmWidth = 0.5f * 0.75f;
float palmLength = 1.2f * 0.75f; float palmWidth = 1.2f * 0.75f;
float palmDepth = 0.3f * 0.75f; float fingerLength = 1.0f * 0.75f;
float fingerWidth = 0.18f * 0.75f; float thumbLength = 0.7f * 0.75f;
float thumbWidth = 0.20f * 0.75f;


// Varfuri pentru un cub unitar (Pozitie + Normala + Coordonate Textura)
float cubeVertices[] = {
    // Poziții          // Normale           // Coordonate Textură
    // Spate
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
    // Fata
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
    // Stanga
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    // Dreapta
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     // Jos
     -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
      0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
     -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     // Sus
     -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
      0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
     -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};


// Mingea
glm::vec3 ballPosition;
glm::vec3 ballVelocity(0.0f);
float ballRadius = 0.4f;
bool ballIsServed = false; bool ballIsHit = false; bool ballInToss = false;
const float GRAVITY = 9.81f;

// Variabile pentru Curba Bezier a Mingii
bool ballOnBezierPath = false;
float bezierTime = 0.0f;
const float bezierDuration = 1.1f;
glm::vec3 bezierP0, bezierP1, bezierP2, bezierP3;

// For showing Bezier path
bool showBezierPath = false;
GLuint bezierCurveVao, bezierCurveVbo;


// Fileu
glm::vec3 netPosition(0.0f, 0.0f, 0.0f);
float netActualHeight = 1.0f;
float netThickness = 0.02f;
float netBaseElevation = 0.8f;

// Stâlpi Fileu
float netPoleHeight = netBaseElevation + netActualHeight + 0.05f;
float netPoleRadius = 0.05f;
glm::vec3 netPoleColor(0.3f, 0.3f, 0.3f);
glm::vec3 netMeshColor(1.0f, 1.0f, 1.0f);


// Culori obiecte
glm::vec3 handColor(0.96f, 0.76f, 0.62f);
glm::vec3 ballColor(1.0f, 1.0f, 0.8f);
glm::vec3 lineColor(1.0f, 1.0f, 1.0f);

// Sfera (mingea)
GLuint sphereVao, sphereVbo, sphereIbo;
std::vector<float> sphereVertices_data; std::vector<unsigned int> sphereIndices_data;
int sphereStacks = 30; int sphereSlices = 30;

// Teren de joc (cel portocaliu)
GLuint courtFloorVao, courtFloorVbo, courtFloorIbo;
std::vector<float> courtFloorVertices_data;
std::vector<unsigned int> courtFloorIndices_data;


// Tavan
GLuint ceilingVao, ceilingVbo, ceilingIbo;
std::vector<float> ceilingVertices_data; std::vector<unsigned int> ceilingIndices_data;
float overallRoomW = 25.0f;
float overallRoomL = 40.0f;
float ceilingH_thickness = 1.0f;


glm::vec3 courtOrangeColor(1.0f, 0.55f, 0.0f);
glm::vec3 wallFallbackColor(0.8f, 0.8f, 0.75f);
glm::vec3 ceilingFallbackColor(0.9f, 0.9f, 0.9f);
glm::vec3 generalFloorFallbackColor(0.3f, 0.3f, 0.3f);


const float LINE_WIDTH_DIM = 0.05f;
const float LINE_THICKNESS_VISUAL_DIM = 0.02f;

glm::vec3 cameraPos = glm::vec3(playerBaseX + 9.0f, 4.0f, playerBaseZ + 3.0f);
glm::vec3 cameraFront = glm::vec3(-1.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float cameraSpeed = 0.35f;
float cameraRotationSpeed = 2.5f;

float cameraYaw = -200.0f;
float cameraPitch = -15.0f;

// Variabile Iluminare
GLuint lightPos1ID, lightColor1ID;
GLuint lightPos2ID, lightColor2ID;
GLuint viewPosID_uniform;
GLuint ambientColorID;

glm::vec3 lightPos1 = glm::vec3(-overallRoomW / 4.0f, 11.0f, 0.0f);
glm::vec3 lightColor1 = glm::vec3(0.45f, 0.45f, 0.4f);
glm::vec3 lightPos2 = glm::vec3(overallRoomW / 4.0f, 11.0f, 0.0f);
glm::vec3 lightColor2 = glm::vec3(0.45f, 0.45f, 0.4f);
glm::vec3 globalAmbientColor = glm::vec3(0.1f, 0.1f, 0.12f);

int serviceCount = 0;
const int HIT_NET_AFTER_SERVICES = 3;
bool ballIsFallingAfterNetHit = false;


std::string textFileRead(const char* fn) { std::ifstream ifile(fn); if (!ifile.is_open()) { std::cerr << "Eroare la deschiderea fisierului: " << fn << std::endl; return ""; } std::string filetext; std::string line; while (std::getline(ifile, line)) { filetext.append(line + "\n"); } return filetext; }
float lerp(float start, float end, float t_lerp) { t_lerp = glm::clamp(t_lerp, 0.0f, 1.0f); return start + t_lerp * (end - start); }

void updateCameraFront() {
    glm::vec3 front;
    front.x = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
    front.y = sin(glm::radians(cameraPitch));
    front.z = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
    cameraFront = glm::normalize(front);
}

float randomFloat(float min, float max) {
    if (min >= max) return min; 
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}


GLuint loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format = GL_RGB, internalFormat = GL_RGB8;
        if (nrComponents == 1) { format = GL_RED; internalFormat = GL_R8; }
        else if (nrComponents == 3) { format = GL_RGB; internalFormat = GL_RGB8; }
        else if (nrComponents == 4) { format = GL_RGBA; internalFormat = GL_RGBA8; }
        else { std::cerr << "Texture '" << path << "' loaded with " << nrComponents << " components. Defaulting to RGB/RGB8." << std::endl; }

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (path && strcmp(path, "net_texture.jpg") == 0) {
            if (glewIsSupported("GL_EXT_texture_filter_anisotropic")) {
                GLfloat maxAnisotropy;
                glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
                glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
            }
            else {
            }
        }

        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        stbi_image_free(data);

    }
    else {
        std::cerr << "Texture failed to load at path: " << path << std::endl;
        std::cerr << "STBI_FAILURE_REASON: " << stbi_failure_reason() << std::endl;
        stbi_image_free(data);
        textureID = 0;
    }
    return textureID;
}


void generateSphere() {
    sphereVertices_data.clear(); sphereIndices_data.clear();
    for (int i = 0; i <= sphereStacks; ++i) {
        float stackAngle = PI * (float)i / sphereStacks;
        float xy_coord = ballRadius * sin(stackAngle); float z_coord = ballRadius * cos(stackAngle);
        for (int j = 0; j <= sphereSlices; ++j) {
            float sectorAngle = 2 * PI * (float)j / sphereSlices;
            float x_coord = xy_coord * cos(sectorAngle); float y_coord = xy_coord * sin(sectorAngle);
            sphereVertices_data.push_back(x_coord); sphereVertices_data.push_back(y_coord); sphereVertices_data.push_back(z_coord);
            glm::vec3 normal = glm::normalize(glm::vec3(x_coord, y_coord, z_coord));
            sphereVertices_data.push_back(normal.x); sphereVertices_data.push_back(normal.y); sphereVertices_data.push_back(normal.z);
        }
    }
    for (int i = 0; i < sphereStacks; ++i) {
        for (int j = 0; j < sphereSlices; ++j) {
            int first = (i * (sphereSlices + 1)) + j; int second = first + sphereSlices + 1;
            sphereIndices_data.push_back(first); sphereIndices_data.push_back(second); sphereIndices_data.push_back(first + 1);
            sphereIndices_data.push_back(second); sphereIndices_data.push_back(second + 1); sphereIndices_data.push_back(first + 1);
        }
    }
}

void generateCuboidVertices(float width, float height, float depth, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    vertices.clear(); indices.clear();
    float halfW = width / 2.0f; float halfH = height / 2.0f; float halfD = depth / 2.0f;
    vertices = {
        -halfW, -halfH,  halfD,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, halfW, -halfH,  halfD,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, halfW,  halfH,  halfD,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, -halfW,  halfH,  halfD,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
        -halfW, -halfH, -halfD,  0.0f, 0.0f,-1.0f,  1.0f, 0.0f, halfW, -halfH, -halfD,  0.0f, 0.0f,-1.0f,  0.0f, 0.0f, halfW,  halfH, -halfD,  0.0f, 0.0f,-1.0f,  0.0f, 1.0f, -halfW,  halfH, -halfD,  0.0f, 0.0f,-1.0f,  1.0f, 1.0f,
        -halfW,  halfH,  halfD,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, halfW,  halfH,  halfD,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, halfW,  halfH, -halfD,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, -halfW,  halfH, -halfD,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
        -halfW, -halfH,  halfD,  0.0f,-1.0f, 0.0f,  0.0f, 1.0f, halfW, -halfH,  halfD,  0.0f,-1.0f, 0.0f,  1.0f, 1.0f, halfW, -halfH, -halfD,  0.0f,-1.0f, 0.0f,  1.0f, 0.0f, -halfW, -halfH, -halfD,  0.0f,-1.0f, 0.0f,  0.0f, 0.0f,
         halfW, -halfH,  halfD,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, halfW, -halfH, -halfD,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, halfW,  halfH, -halfD,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, halfW,  halfH,  halfD,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
         -halfW, -halfH,  halfD, -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -halfW, -halfH, -halfD, -1.0f, 0.0f, 0.0f,  1.0f, 0.0f, -halfW,  halfH, -halfD, -1.0f, 0.0f, 0.0f,  1.0f, 1.0f, -halfW,  halfH,  halfD, -1.0f, 0.0f, 0.0f,  0.0f, 1.0f
    };
    indices = {
        0, 1, 2,   0, 2, 3, 4, 5, 6,   4, 6, 7, 8, 9, 10,  8, 10,11, 12,13,14,  12,14,15, 16,17,18,  16,18,19, 20,21,22,  20,22,23
    };
}


glm::vec3 CalculateBezierPoint(float t, const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3) {
    float u = 1.0f - t; float tt = t * t; float uu = u * u; float uuu = uu * u; float ttt = tt * t;
    glm::vec3 point = uuu * p0; point += 3.0f * uu * t * p1; point += 3.0f * u * tt * p2; point += ttt * p3;
    return point;
}


void drawCube(const glm::mat4& modelMatrixIn, const glm::vec3& color) {
    glUniform1i(isNetTextureID, GL_FALSE);
    glUniform1i(useTextureID, 0);
    glUniform3fv(colorID, 1, glm::value_ptr(color));
    glUniformMatrix4fv(modelMatrixID_uniform, 1, GL_FALSE, glm::value_ptr(modelMatrixIn));
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void drawSphere(const glm::mat4& modelMatrixIn, const glm::vec3& color) {
    glUniform1i(isNetTextureID, GL_FALSE);
    glUniform1i(useTextureID, 0);
    glUniform3fv(colorID, 1, glm::value_ptr(color));
    glUniformMatrix4fv(modelMatrixID_uniform, 1, GL_FALSE, glm::value_ptr(modelMatrixIn));
    glBindVertexArray(sphereVao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereIbo);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(sphereIndices_data.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void drawGeneratedCuboid(GLuint vao_cuboid, GLuint ibo_cuboid, size_t numIndices,
    const glm::mat4& modelMatrixIn, const glm::vec3& fallbackColor,
    bool textureThisCuboid, GLuint specificTextureID) {

    if (specificTextureID != netTextureID) {
        glUniform1i(isNetTextureID, GL_FALSE);
    }

    if (textureThisCuboid && specificTextureID != 0) {
        glUniform1i(useTextureID, 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, specificTextureID);
        glUniform1i(textureSamplerID, 0);
    }
    else {
        glUniform1i(useTextureID, 0);
        glUniform3fv(colorID, 1, glm::value_ptr(fallbackColor));
    }
    glUniformMatrix4fv(modelMatrixID_uniform, 1, GL_FALSE, glm::value_ptr(modelMatrixIn));
    glBindVertexArray(vao_cuboid);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cuboid);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(numIndices), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    if (textureThisCuboid && specificTextureID != 0) {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}


void update() {
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    int timeElapsed = currentTime - lastUpdateTime;
    if (timeElapsed <= 0) { timeElapsed = 1; }
    float deltaTime = (float)timeElapsed / 1000.0f;
    lastUpdateTime = currentTime;

    float totalAnimationRange = (float)(NUM_POSES - 1);
    float animationSpeed = totalAnimationRange / animationDuration;

    bool needsRedraw = false;

    if (isAnimatingForward) {
        animationProgress += animationSpeed * deltaTime;
        if (animationProgress >= totalAnimationRange) {
            animationProgress = totalAnimationRange;
            isAnimatingForward = false;
        }
        needsRedraw = true;
    }
    else if (isAnimatingBackward) {
        animationProgress -= animationSpeed * deltaTime;
        if (animationProgress <= 0.0f) {
            animationProgress = 0.0f;
            isAnimatingBackward = false;
        }
        needsRedraw = true;
    }

    int pose1_idx = static_cast<int>(floor(animationProgress));
    int pose2_idx = static_cast<int>(ceil(animationProgress));
    pose1_idx = glm::clamp(pose1_idx, 0, NUM_POSES - 1);
    pose2_idx = glm::clamp(pose2_idx, 0, NUM_POSES - 1);
    float t_lerp_val = (pose1_idx == pose2_idx) ? 0.0f : (animationProgress - (float)pose1_idx);
    t_lerp_val = glm::clamp(t_lerp_val, 0.0f, 1.0f);

    currentShoulderY = lerp(keyframePoses[pose1_idx][0], keyframePoses[pose2_idx][0], t_lerp_val);
    currentShoulderZ = lerp(keyframePoses[pose1_idx][1], keyframePoses[pose2_idx][1], t_lerp_val);
    currentElbow = lerp(keyframePoses[pose1_idx][2], keyframePoses[pose2_idx][2], t_lerp_val);
    currentWristPronation = lerp(keyframePoses[pose1_idx][3], keyframePoses[pose2_idx][3], t_lerp_val);
    currentWristZ = lerp(keyframePoses[pose1_idx][4], keyframePoses[pose2_idx][4], t_lerp_val);
    currentWristY = lerp(keyframePoses[pose1_idx][5], keyframePoses[pose2_idx][5], t_lerp_val);
    currentFingerKnuckle = lerp(keyframePoses[pose1_idx][6], keyframePoses[pose2_idx][6], t_lerp_val);
    currentFingerMid = lerp(keyframePoses[pose1_idx][7], keyframePoses[pose2_idx][7], t_lerp_val);
    currentThumbBaseZ = lerp(keyframePoses[pose1_idx][8], keyframePoses[pose2_idx][8], t_lerp_val);
    currentThumbKnuckle = lerp(keyframePoses[pose1_idx][9], keyframePoses[pose2_idx][9], t_lerp_val);
    currentThumbMid = lerp(keyframePoses[pose1_idx][10], keyframePoses[pose2_idx][10], t_lerp_val);

    if (ballIsServed) {
        if (ballOnBezierPath) {
            bezierTime += deltaTime / bezierDuration;
            glm::vec3 previousBallPosition = ballPosition;
            ballPosition = CalculateBezierPoint(glm::clamp(bezierTime, 0.0f, 1.0f), bezierP0, bezierP1, bezierP2, bezierP3);

            if (serviceCount >= HIT_NET_AFTER_SERVICES && !ballIsFallingAfterNetHit) {
                float netPlayerSideZ = netPosition.z - netThickness / 2.0f;
                float netOpponentSideZ = netPosition.z + netThickness / 2.0f;

                float netTopY = floorTopSurfaceY + netBaseElevation + netActualHeight;
                float netBottomY = floorTopSurfaceY + netBaseElevation;

                bool potentialHit = (previousBallPosition.z < netPlayerSideZ && ballPosition.z >= netPlayerSideZ - ballRadius * 0.5f);


                if (potentialHit &&
                    ballPosition.x >= -courtPlayingAreaW / 2.0f - ballRadius && ballPosition.x <= courtPlayingAreaW / 2.0f + ballRadius &&
                    ballPosition.y >= netBottomY - ballRadius && ballPosition.y <= netTopY + ballRadius) {

                    std::cout << "Ball hit the net on service #" << serviceCount << "!" << std::endl;
                    ballOnBezierPath = false;
                    ballIsFallingAfterNetHit = true;

                    ballVelocity = glm::vec3(randomFloat(-0.5f, 0.5f),  
                        randomFloat(0.1f, 0.8f),   
                        -randomFloat(1.0f, 2.2f));

                    ballPosition.z = netPlayerSideZ - ballRadius;

                    if (ballPosition.y > netTopY) ballPosition.y = netTopY - ballRadius * 0.05f; 
                    else if (ballPosition.y < netBottomY) ballPosition.y = netBottomY + ballRadius; 
                }
            }

            if (bezierTime >= 1.0f && !ballIsFallingAfterNetHit) {
                ballOnBezierPath = false; ballIsServed = false; ballPosition.y = floorTopSurfaceY + ballRadius;
            }
            else if (ballPosition.y <= (floorTopSurfaceY + ballRadius - 0.01f) && bezierTime < 1.0f && !ballIsFallingAfterNetHit) {
                ballPosition.y = floorTopSurfaceY + ballRadius;
            }
        }
        else if (ballInToss) {
            ballVelocity.y -= GRAVITY * deltaTime * 1.25f; ballPosition += ballVelocity * deltaTime;
            float hitMomentTriggerProgress = static_cast<float>(hitKeyframeIndex);
            float animationProgressCurrentFrame = animationProgress;
            if (isAnimatingForward && animationProgressCurrentFrame >= hitMomentTriggerProgress && !ballIsHit) {
                ballIsHit = true; ballInToss = false; ballOnBezierPath = true; bezierTime = 0.0f;
                bezierP0 = ballPosition; if (bezierP0.y < floorTopSurfaceY + ballRadius) { bezierP0.y = floorTopSurfaceY + ballRadius; }

                if (serviceCount >= HIT_NET_AFTER_SERVICES) {
                    std::cout << "Service #" << serviceCount << ": Aiming for the net!" << std::endl;
                    float targetX_net = randomFloat(-courtPlayingAreaW / 2.0f * 0.7f, courtPlayingAreaW / 2.0f * 0.7f); 
                    float netTopSurfaceY = floorTopSurfaceY + netBaseElevation + netActualHeight;
                    float netBottomSurfaceY = floorTopSurfaceY + netBaseElevation;
                    // Tinteste un punct pe fata plasei dinspre jucator
                    bezierP3 = glm::vec3(targetX_net, randomFloat(netBottomSurfaceY + ballRadius * 1.5f, netTopSurfaceY - ballRadius * 0.5f), netPosition.z - netThickness / 2.0f - ballRadius * 0.1f);


                    float arcHeightFactorNet = randomFloat(0.3f, 0.7f);
                    bezierP1 = bezierP0 + glm::vec3((bezierP3.x - bezierP0.x) * 0.3f, 0.0f, (bezierP3.z - bezierP0.z) * 0.4f);
                    bezierP1.y = glm::max(bezierP0.y + arcHeightFactorNet, bezierP3.y + arcHeightFactorNet + 0.2f);
                    bezierP2 = bezierP3 + glm::vec3((bezierP0.x - bezierP3.x) * 0.3f, 0.0f, (bezierP0.z - bezierP3.z) * 0.4f);
                    bezierP2.y = glm::max(bezierP3.y + arcHeightFactorNet * 0.8f, bezierP0.y + arcHeightFactorNet - 0.05f);
                    if (bezierP1.y < bezierP3.y) bezierP1.y = bezierP3.y + 0.1f;
                    if (bezierP2.y < bezierP3.y) bezierP2.y = bezierP3.y + 0.05f;
                }
                else {
                    float targetX = randomFloat(-courtPlayingAreaW / 2.0f + ballRadius + 0.7f, courtPlayingAreaW / 2.0f - ballRadius - 0.7f);
                    float targetZ = randomFloat(netPosition.z + 2.0f, courtPlayingAreaL / 2.0f - ballRadius - 1.0f);
                    bezierP3 = glm::vec3(targetX, floorTopSurfaceY + ballRadius, targetZ);
                    float netTopVisibleY = floorTopSurfaceY + netBaseElevation + netActualHeight;
                    float clearanceOverNet = 0.7f; float controlPointBaseHeight = netTopVisibleY + clearanceOverNet;
                    float distP0P3_XZ = glm::distance(glm::vec2(bezierP0.x, bezierP0.z), glm::vec2(bezierP3.x, bezierP3.z));
                    float arcHeightFactor = distP0P3_XZ * 0.25f;
                    float p1_y_candidate = bezierP0.y + arcHeightFactor;
                    float p2_y_base_offset = (bezierP0.y > netTopVisibleY) ? arcHeightFactor * 0.6f : arcHeightFactor * 0.4f;
                    float p2_y_candidate = bezierP3.y + p2_y_base_offset;
                    bezierP1 = bezierP0 + glm::vec3((bezierP3.x - bezierP0.x) * 0.20f, 0.0f, (bezierP3.z - bezierP0.z) * 0.28f);
                    bezierP1.y = glm::max(controlPointBaseHeight, p1_y_candidate);
                    bezierP2 = bezierP3 + glm::vec3((bezierP0.x - bezierP3.x) * 0.20f, 0.0f, (bezierP0.z - bezierP3.z) * 0.28f);
                    bezierP2.y = glm::max(controlPointBaseHeight * 0.80f, p2_y_candidate);
                    if (abs(bezierP0.z - netPosition.z) < 1.8f && bezierP0.y < netTopVisibleY + 0.8f) {
                        bezierP1.y = glm::max(bezierP1.y, bezierP0.y + 1.0f); bezierP2.y = glm::max(bezierP2.y, bezierP0.y + 0.8f);
                    }
                    if (bezierP0.y <= floorTopSurfaceY + ballRadius + 0.1f) {
                        bezierP1.y = glm::max(bezierP1.y, floorTopSurfaceY + ballRadius + 1.8f); bezierP2.y = glm::max(bezierP2.y, floorTopSurfaceY + ballRadius + 1.5f);
                    }
                }
            }
            if (ballPosition.y < (floorTopSurfaceY + ballRadius) && !ballIsHit) {
                ballPosition.y = floorTopSurfaceY + ballRadius; ballVelocity.y = 0;
            }
        }
        else if (ballIsFallingAfterNetHit) {
            ballVelocity.y -= GRAVITY * deltaTime * 1.5f;
            ballPosition += ballVelocity * deltaTime;

            if (ballPosition.y < (floorTopSurfaceY + ballRadius)) {
                ballPosition.y = floorTopSurfaceY + ballRadius;
                ballVelocity = glm::vec3(0.0f);
                ballIsFallingAfterNetHit = false;
                ballIsServed = false;
                std::cout << "Ball landed after hitting the net." << std::endl;
                serviceCount = 0;
            }
        }
        needsRedraw = true;
    }

    if (needsRedraw) { glutPostRedisplay(); }
}

void display() {
    updateCameraFront();
    viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shader_programme);

    glUniform3fv(lightPos1ID, 1, glm::value_ptr(lightPos1));
    glUniform3fv(lightColor1ID, 1, glm::value_ptr(lightColor1));
    glUniform3fv(lightPos2ID, 1, glm::value_ptr(lightPos2));
    glUniform3fv(lightColor2ID, 1, glm::value_ptr(lightColor2));
    glUniform3fv(viewPosID_uniform, 1, glm::value_ptr(cameraPos));
    glUniform3fv(ambientColorID, 1, glm::value_ptr(globalAmbientColor));
    glUniformMatrix4fv(viewMatrixID_uniform, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(projectionMatrixID_uniform, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    float generalFloorYPos = -0.05f;
    float ceilingBottomSurfaceY = 12.0f;
    float wallVisualThickness = 0.2f;

    glUniform1i(isNetTextureID, GL_FALSE);
    glm::mat4 modelMatrix = glm::mat4(1.0f);

    // --- Podeaua Generală ---
    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, generalFloorYPos - (0.05f / 2.0f), 0.0f));
    drawGeneratedCuboid(generalFloorVao, generalFloorIbo, generalFloorIndices_data.size(), modelMatrix, generalFloorFallbackColor, true, floorWoodTextureID);

    // --- Terenul de Joc ---
    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, floorTopSurfaceY - courtPlayingAreaThickness / 2.0f, 0.0f));
    drawGeneratedCuboid(courtFloorVao, courtFloorIbo, courtFloorIndices_data.size(), modelMatrix, courtOrangeColor, false, 0);

    // --- Liniile Terenului ---
    float linesYCenterPosition = floorTopSurfaceY + LINE_THICKNESS_VISUAL_DIM / 2.0f;
    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((courtPlayingAreaW - LINE_WIDTH_DIM) / 2.0f, linesYCenterPosition, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(LINE_WIDTH_DIM, LINE_THICKNESS_VISUAL_DIM, courtPlayingAreaL));
    drawCube(modelMatrix, lineColor);

    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-(courtPlayingAreaW - LINE_WIDTH_DIM) / 2.0f, linesYCenterPosition, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(LINE_WIDTH_DIM, LINE_THICKNESS_VISUAL_DIM, courtPlayingAreaL));
    drawCube(modelMatrix, lineColor);

    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, linesYCenterPosition, (courtPlayingAreaL - LINE_WIDTH_DIM) / 2.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(courtPlayingAreaW, LINE_THICKNESS_VISUAL_DIM, LINE_WIDTH_DIM));
    drawCube(modelMatrix, lineColor);

    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, linesYCenterPosition, -(courtPlayingAreaL - LINE_WIDTH_DIM) / 2.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(courtPlayingAreaW, LINE_THICKNESS_VISUAL_DIM, LINE_WIDTH_DIM));
    drawCube(modelMatrix, lineColor);

    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, linesYCenterPosition, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(courtPlayingAreaW, LINE_THICKNESS_VISUAL_DIM, LINE_WIDTH_DIM));
    drawCube(modelMatrix, lineColor);

    float attackLineOffsetFromCenter = 3.0f;
    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, linesYCenterPosition, attackLineOffsetFromCenter));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(courtPlayingAreaW, LINE_THICKNESS_VISUAL_DIM, LINE_WIDTH_DIM));
    drawCube(modelMatrix, lineColor);

    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, linesYCenterPosition, -attackLineOffsetFromCenter));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(courtPlayingAreaW, LINE_THICKNESS_VISUAL_DIM, LINE_WIDTH_DIM));
    drawCube(modelMatrix, lineColor);

    // --- Tavanul ---
    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, ceilingBottomSurfaceY + ceilingH_thickness / 2.0f, 0.0f));
    drawGeneratedCuboid(ceilingVao, ceilingIbo, ceilingIndices_data.size(), modelMatrix, ceilingFallbackColor, true, ceilingTextureID);

    // --- Pereții ---
    glUniform1i(isNetTextureID, GL_FALSE);
    if (wallTextureID != 0) {
        glUniform1i(useTextureID, 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, wallTextureID);
        glUniform1i(textureSamplerID, 0);
    }
    else {
        glUniform1i(useTextureID, 0);
        glUniform3fv(colorID, 1, glm::value_ptr(wallFallbackColor));
    }
    float roomVisualHeight = ceilingBottomSurfaceY - floorTopSurfaceY;
    float wallCenterY = floorTopSurfaceY + (roomVisualHeight / 2.0f);

    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, wallCenterY, -overallRoomL / 2.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(overallRoomW, roomVisualHeight, wallVisualThickness));
    glUniformMatrix4fv(modelMatrixID_uniform, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glBindVertexArray(vao); glDrawArrays(GL_TRIANGLES, 0, 36);

    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-overallRoomW / 2.0f, wallCenterY, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(wallVisualThickness, roomVisualHeight, overallRoomL));
    glUniformMatrix4fv(modelMatrixID_uniform, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(overallRoomW / 2.0f, wallCenterY, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(wallVisualThickness, roomVisualHeight, overallRoomL));
    glUniformMatrix4fv(modelMatrixID_uniform, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, wallCenterY, overallRoomL / 2.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(overallRoomW, roomVisualHeight, wallVisualThickness));
    glUniformMatrix4fv(modelMatrixID_uniform, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    if (wallTextureID != 0) { glBindTexture(GL_TEXTURE_2D, 0); }


    // --- Fileul și Stâlpii ---
    float netMeshCenterY = floorTopSurfaceY + netBaseElevation + (netActualHeight / 2.0f);

    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(netPosition.x, netMeshCenterY, netPosition.z));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(courtPlayingAreaW, netActualHeight, netThickness));

    if (netTextureID != 0) {
        glUniform1i(useTextureID, 1);
        glUniform1i(isNetTextureID, GL_TRUE);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, netTextureID);
        glUniform1i(textureSamplerID, 0);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUniform3fv(colorID, 1, glm::value_ptr(netMeshColor));
    }
    else {
        glUniform1i(useTextureID, 0);
        glUniform1i(isNetTextureID, GL_FALSE);
        glUniform3fv(colorID, 1, glm::value_ptr(netMeshColor));
    }
    glUniformMatrix4fv(modelMatrixID_uniform, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    if (netTextureID != 0) {
        glDisable(GL_BLEND);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Stâlpii fileului
    glUniform1i(isNetTextureID, GL_FALSE);
    glUniform1i(useTextureID, 0);

    float poleCenterY = floorTopSurfaceY + (netPoleHeight / 2.0f);

    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-courtPlayingAreaW / 2.0f - netPoleRadius, poleCenterY, netPosition.z));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(netPoleRadius, netPoleHeight, netPoleRadius));
    drawCube(modelMatrix, netPoleColor);

    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(courtPlayingAreaW / 2.0f + netPoleRadius, poleCenterY, netPosition.z));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(netPoleRadius, netPoleHeight, netPoleRadius));
    drawCube(modelMatrix, netPoleColor);

    glUniform1i(isNetTextureID, GL_FALSE);
    glUniform1i(useTextureID, 0);

    glUniform1i(isNetTextureID, GL_FALSE);
    if (ballIsServed || ballInToss || ballOnBezierPath || ballIsFallingAfterNetHit || (!ballIsServed && !isAnimatingForward && !isAnimatingBackward)) {
        modelMatrix = glm::translate(glm::mat4(1.0f), ballPosition);
        drawSphere(modelMatrix, ballColor);
    }

    glUniform1i(isNetTextureID, GL_FALSE);
    glm::mat4 baseTransformMatrix = glm::mat4(1.0f);
    baseTransformMatrix = glm::translate(baseTransformMatrix, glm::vec3(playerBaseX, floorTopSurfaceY + playerBaseY, playerBaseZ));
    float initialArmYRotation = -PI / 2.0f;
    baseTransformMatrix = glm::rotate(baseTransformMatrix, initialArmYRotation, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 currentMatrix = baseTransformMatrix;
    currentMatrix = glm::rotate(currentMatrix, currentShoulderY, glm::vec3(0.0f, 1.0f, 0.0f)); currentMatrix = glm::rotate(currentMatrix, currentShoulderZ, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 elbowJointMatrix = glm::translate(currentMatrix, glm::vec3(armLength, 0.0f, 0.0f));
    glm::mat4 armDrawMatrix = glm::translate(currentMatrix, glm::vec3(armLength / 2.0f, 0.0f, 0.0f));
    drawCube(glm::scale(armDrawMatrix, glm::vec3(armLength, armWidth, armWidth)), handColor);

    currentMatrix = elbowJointMatrix; currentMatrix = glm::rotate(currentMatrix, currentElbow, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 wristJointMatrix = glm::translate(currentMatrix, glm::vec3(forearmLength, 0.0f, 0.0f));
    glm::mat4 forearmDrawMatrix = glm::translate(currentMatrix, glm::vec3(forearmLength / 2.0f, 0.0f, 0.0f));
    drawCube(glm::scale(forearmDrawMatrix, glm::vec3(forearmLength, forearmWidth, forearmWidth)), handColor);

    currentMatrix = wristJointMatrix; currentMatrix = glm::rotate(currentMatrix, currentWristPronation, glm::vec3(1.0f, 0.0f, 0.0f)); currentMatrix = glm::rotate(currentMatrix, currentWristZ, glm::vec3(0.0f, 0.0f, 1.0f)); currentMatrix = glm::rotate(currentMatrix, currentWristY, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 palmCenterMatrix = glm::translate(currentMatrix, glm::vec3(palmLength / 2.0f, 0.0f, 0.0f));
    drawCube(glm::scale(palmCenterMatrix, glm::vec3(palmLength, palmWidth, palmDepth)), handColor);

    float fingerSpacing = palmWidth * 0.8f / 3.0f; float fingerStartXOffset = palmLength / 2.0f; float fingerStartYOffsetBase = (palmWidth / 2.0f) - (fingerSpacing * 0.5f) - fingerWidth / 2.0f;
    for (int i = 0; i < 4; ++i) {
        glm::mat4 fingerBasePosMatrix = glm::translate(palmCenterMatrix, glm::vec3(fingerStartXOffset, fingerStartYOffsetBase - i * fingerSpacing, 0.0f));
        glm::mat4 knuckleMatrix = glm::rotate(fingerBasePosMatrix, currentFingerKnuckle, glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 knuckleDrawMatrix = glm::translate(knuckleMatrix, glm::vec3(fingerLength / 2.0f, 0.0f, 0.0f));
        drawCube(glm::scale(knuckleDrawMatrix, glm::vec3(fingerLength, fingerWidth, fingerWidth)), handColor);
        glm::mat4 midJointMatrix = glm::translate(knuckleMatrix, glm::vec3(fingerLength, 0.0f, 0.0f));
        glm::mat4 midSegmentMatrix = glm::rotate(midJointMatrix, currentFingerMid, glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 midDrawMatrix = glm::translate(midSegmentMatrix, glm::vec3(fingerLength / 2.0f, 0.0f, 0.0f));
        drawCube(glm::scale(midDrawMatrix, glm::vec3(fingerLength, fingerWidth, fingerWidth)), handColor);
    }
    glm::mat4 thumbMatrix = glm::translate(palmCenterMatrix, glm::vec3(palmLength * 0.15f, palmWidth / 2.0f + thumbWidth * 0.5f, 0.0f));
    thumbMatrix = glm::rotate(thumbMatrix, currentThumbBaseZ, glm::vec3(0.0f, 0.0f, 1.0f)); thumbMatrix = glm::rotate(thumbMatrix, currentThumbKnuckle, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 thumbBaseDrawMatrix = glm::translate(thumbMatrix, glm::vec3(thumbLength / 2.0f, 0.0f, 0.0f));
    drawCube(glm::scale(thumbBaseDrawMatrix, glm::vec3(thumbLength, thumbWidth, thumbWidth)), handColor);
    glm::mat4 thumbKnuckleJointMatrix = glm::translate(thumbMatrix, glm::vec3(thumbLength, 0.0f, 0.0f));
    glm::mat4 thumbKnuckleSegmentMatrix = glm::rotate(thumbKnuckleJointMatrix, currentThumbMid, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 thumbKnuckleDrawMatrix = glm::translate(thumbKnuckleSegmentMatrix, glm::vec3(thumbLength / 2.0f, 0.0f, 0.0f));
    drawCube(glm::scale(thumbKnuckleDrawMatrix, glm::vec3(thumbLength, thumbWidth, thumbWidth)), handColor);

    glUniform1i(isNetTextureID, GL_FALSE);
    if (showBezierPath && ballOnBezierPath) {
        glUniform1i(useTextureID, 0);
        glm::mat4 pointModelMatrix; float controlPointSize = 0.15f;
        pointModelMatrix = glm::translate(glm::mat4(1.0f), bezierP0); pointModelMatrix = glm::scale(pointModelMatrix, glm::vec3(controlPointSize)); drawCube(pointModelMatrix, glm::vec3(1.0f, 0.0f, 0.0f));
        pointModelMatrix = glm::translate(glm::mat4(1.0f), bezierP1); pointModelMatrix = glm::scale(pointModelMatrix, glm::vec3(controlPointSize)); drawCube(pointModelMatrix, glm::vec3(0.0f, 1.0f, 0.0f));
        pointModelMatrix = glm::translate(glm::mat4(1.0f), bezierP2); pointModelMatrix = glm::scale(pointModelMatrix, glm::vec3(controlPointSize)); drawCube(pointModelMatrix, glm::vec3(0.0f, 0.0f, 1.0f));
        pointModelMatrix = glm::translate(glm::mat4(1.0f), bezierP3); pointModelMatrix = glm::scale(pointModelMatrix, glm::vec3(controlPointSize)); drawCube(pointModelMatrix, glm::vec3(1.0f, 1.0f, 0.0f));

        std::vector<glm::vec3> curveVertices; const int numCurveSegments = 50;
        for (int i = 0; i <= numCurveSegments; ++i) { float t = static_cast<float>(i) / static_cast<float>(numCurveSegments); curveVertices.push_back(CalculateBezierPoint(t, bezierP0, bezierP1, bezierP2, bezierP3)); }
        glUniform3fv(colorID, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
        glUniformMatrix4fv(modelMatrixID_uniform, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
        glBindVertexArray(bezierCurveVao); glBindBuffer(GL_ARRAY_BUFFER, bezierCurveVbo);
        glBufferData(GL_ARRAY_BUFFER, curveVertices.size() * sizeof(glm::vec3), curveVertices.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(curveVertices.size()));
        glBindVertexArray(0);
    }
    glFlush();
}

void init() {
    srand(static_cast<unsigned int>(time(0)));

    const GLubyte* renderer = glGetString(GL_RENDERER); const GLubyte* version = glGetString(GL_VERSION); printf("Renderer: %s\n", renderer); printf("OpenGL version supported %s\n", version);
    glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
    glEnable(GL_MULTISAMPLE);

    glewExperimental = GL_TRUE; GLenum err = glewInit(); if (GLEW_OK != err) { fprintf(stderr, "Error initializing GLEW: %s\n", glewGetErrorString(err)); exit(1); } printf("Using GLEW %s\n", glewGetString(GLEW_VERSION));

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    generateSphere();
    glGenVertexArrays(1, &sphereVao); glBindVertexArray(sphereVao);
    glGenBuffers(1, &sphereVbo); glBindBuffer(GL_ARRAY_BUFFER, sphereVbo); glBufferData(GL_ARRAY_BUFFER, sphereVertices_data.size() * sizeof(float), sphereVertices_data.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &sphereIbo); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereIbo); glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices_data.size() * sizeof(unsigned int), sphereIndices_data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    generateCuboidVertices(courtPlayingAreaW, courtPlayingAreaThickness, courtPlayingAreaL, courtFloorVertices_data, courtFloorIndices_data);
    glGenVertexArrays(1, &courtFloorVao); glBindVertexArray(courtFloorVao);
    glGenBuffers(1, &courtFloorVbo); glBindBuffer(GL_ARRAY_BUFFER, courtFloorVbo); glBufferData(GL_ARRAY_BUFFER, courtFloorVertices_data.size() * sizeof(float), courtFloorVertices_data.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &courtFloorIbo); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, courtFloorIbo); glBufferData(GL_ELEMENT_ARRAY_BUFFER, courtFloorIndices_data.size() * sizeof(unsigned int), courtFloorIndices_data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    generateCuboidVertices(overallRoomW, ceilingH_thickness, overallRoomL, ceilingVertices_data, ceilingIndices_data);
    glGenVertexArrays(1, &ceilingVao); glBindVertexArray(ceilingVao);
    glGenBuffers(1, &ceilingVbo); glBindBuffer(GL_ARRAY_BUFFER, ceilingVbo); glBufferData(GL_ARRAY_BUFFER, ceilingVertices_data.size() * sizeof(float), ceilingVertices_data.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &ceilingIbo); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ceilingIbo); glBufferData(GL_ELEMENT_ARRAY_BUFFER, ceilingIndices_data.size() * sizeof(unsigned int), ceilingIndices_data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    float generalFloorThickness = 0.05f;
    generateCuboidVertices(overallRoomW * 1.5f, generalFloorThickness, overallRoomL * 1.5f, generalFloorVertices_data, generalFloorIndices_data);
    glGenVertexArrays(1, &generalFloorVao); glBindVertexArray(generalFloorVao);
    glGenBuffers(1, &generalFloorVbo); glBindBuffer(GL_ARRAY_BUFFER, generalFloorVbo); glBufferData(GL_ARRAY_BUFFER, generalFloorVertices_data.size() * sizeof(float), generalFloorVertices_data.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &generalFloorIbo); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, generalFloorIbo); glBufferData(GL_ELEMENT_ARRAY_BUFFER, generalFloorIndices_data.size() * sizeof(unsigned int), generalFloorIndices_data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    floorWoodTextureID = loadTexture("wood_texture.jpg"); if (floorWoodTextureID == 0) { fprintf(stderr, "Nu s-a putut incarca textura 'wood_texture.jpg' pentru podeaua generala.\n"); }
    wallTextureID = loadTexture("new_wall_texture.jpg"); if (wallTextureID == 0) { fprintf(stderr, "Nu s-a putut incarca textura 'new_wall_texture.jpg' pentru pereti.\n"); }
    ceilingTextureID = loadTexture("ceiling_texture.jpg"); if (ceilingTextureID == 0) { fprintf(stderr, "Nu s-a putut incarca textura 'ceiling_texture.jpg' pentru tavan.\n"); }
    netTextureID = loadTexture("net_texture.jpg"); if (netTextureID == 0) { fprintf(stderr, "Nu s-a putut incarca textura 'net_texture.jpg' pentru fileu.\n"); }

    std::string vstext = textFileRead("vertex.vert"); std::string fstext = textFileRead("fragment.frag"); if (vstext.empty() || fstext.empty()) { fprintf(stderr, "Eroare la citirea shaderelor.\n"); exit(1); }
    const char* vertex_shader_text = vstext.c_str(); const char* fragment_shader_text = fstext.c_str();
    GLuint vs = glCreateShader(GL_VERTEX_SHADER); glShaderSource(vs, 1, &vertex_shader_text, NULL); glCompileShader(vs); GLint success; GLchar infoLog[512]; glGetShaderiv(vs, GL_COMPILE_STATUS, &success); if (!success) { glGetShaderInfoLog(vs, 512, NULL, infoLog); std::cerr << "EROARE::SHADER::VERTEX::COMPILARE_ESUATA\n" << infoLog << std::endl; exit(1); }
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER); glShaderSource(fs, 1, &fragment_shader_text, NULL); glCompileShader(fs); glGetShaderiv(fs, GL_COMPILE_STATUS, &success); if (!success) { glGetShaderInfoLog(fs, 512, NULL, infoLog); std::cerr << "EROARE::SHADER::FRAGMENT::COMPILARE_ESUATA\n" << infoLog << std::endl; exit(1); }
    shader_programme = glCreateProgram(); glAttachShader(shader_programme, fs); glAttachShader(shader_programme, vs); glLinkProgram(shader_programme); glGetProgramiv(shader_programme, GL_LINK_STATUS, &success); if (!success) { glGetProgramInfoLog(shader_programme, 512, NULL, infoLog); std::cerr << "EROARE::SHADER::PROGRAM::LINKING_ESUAT\n" << infoLog << std::endl; exit(1); }
    glDeleteShader(vs); glDeleteShader(fs);

    glUseProgram(shader_programme);
    modelMatrixID_uniform = glGetUniformLocation(shader_programme, "model");
    viewMatrixID_uniform = glGetUniformLocation(shader_programme, "view");
    projectionMatrixID_uniform = glGetUniformLocation(shader_programme, "projection");
    colorID = glGetUniformLocation(shader_programme, "objectColor");
    viewPosID_uniform = glGetUniformLocation(shader_programme, "viewPos");
    lightPos1ID = glGetUniformLocation(shader_programme, "lightPos1");
    lightColor1ID = glGetUniformLocation(shader_programme, "lightColor1");
    lightPos2ID = glGetUniformLocation(shader_programme, "lightPos2");
    lightColor2ID = glGetUniformLocation(shader_programme, "lightColor2");
    ambientColorID = glGetUniformLocation(shader_programme, "ambientColor");
    textureSamplerID = glGetUniformLocation(shader_programme, "textureSampler");
    useTextureID = glGetUniformLocation(shader_programme, "useTexture");
    isNetTextureID = glGetUniformLocation(shader_programme, "isNetTexture");
    if (isNetTextureID == -1) { std::cerr << "Warning: Uniform 'isNetTexture' not found in shader." << std::endl; }

    glGenVertexArrays(1, &bezierCurveVao); glGenBuffers(1, &bezierCurveVbo);
    glBindVertexArray(bezierCurveVao); glBindBuffer(GL_ARRAY_BUFFER, bezierCurveVbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); glBindVertexArray(0);

    currentShoulderY = keyframePoses[0][0]; currentShoulderZ = keyframePoses[0][1]; currentElbow = keyframePoses[0][2];
    currentWristPronation = keyframePoses[0][3]; currentWristZ = keyframePoses[0][4]; currentWristY = keyframePoses[0][5];
    currentFingerKnuckle = keyframePoses[0][6]; currentFingerMid = keyframePoses[0][7];
    currentThumbBaseZ = keyframePoses[0][8]; currentThumbKnuckle = keyframePoses[0][9]; currentThumbMid = keyframePoses[0][10];
    ballPosition = glm::vec3(playerBaseX, (floorTopSurfaceY + playerBaseY) + 0.5f, playerBaseZ + palmLength + ballRadius + 1.0f);
    updateCameraFront();
}

void keyboard(unsigned char key, int x, int y) {
    static bool contactKeyframeOriginalsStored = false;
    static float originalContactWristPronation; static float originalContactWristY;
    if (!contactKeyframeOriginalsStored) {
        if (hitKeyframeIndex >= 0 && hitKeyframeIndex < NUM_POSES) {
            originalContactWristPronation = keyframePoses[hitKeyframeIndex][3];
            originalContactWristY = keyframePoses[hitKeyframeIndex][5];
            contactKeyframeOriginalsStored = true;
        }
    }
    bool cameraChanged = false;
    switch (key) {
    case 'j':
        if (!ballIsServed && !ballIsFallingAfterNetHit) {
            serviceCount++;
            isAnimatingForward = true; isAnimatingBackward = false; animationProgress = 0.0f;
            ballIsServed = true; ballInToss = true; ballIsHit = false; ballOnBezierPath = false;
            ballIsFallingAfterNetHit = false;
            ballPosition = glm::vec3(playerBaseX - 0.1f, (floorTopSurfaceY + playerBaseY) + 1.0f, playerBaseZ + 0.5f);
            ballVelocity = glm::vec3(0.05f, 8.8f, 1.1f);
            std::cout << "Serve initiated. Ball toss. Service #" << serviceCount << std::endl;
        } break;
    case 'K': isAnimatingBackward = true; isAnimatingForward = false; break;
    case ' ':
        serviceCount = 0;
        isAnimatingForward = false; isAnimatingBackward = false; animationProgress = 0.0f;
        ballIsServed = false; ballInToss = false; ballIsHit = false; ballOnBezierPath = false; bezierTime = 0.0f;
        ballIsFallingAfterNetHit = false;
        ballPosition = glm::vec3(playerBaseX, (floorTopSurfaceY + playerBaseY) + 0.5f, playerBaseZ + palmLength + ballRadius + 1.0f);
        ballVelocity = glm::vec3(0.0f);
        if (contactKeyframeOriginalsStored && hitKeyframeIndex >= 0 && hitKeyframeIndex < NUM_POSES) {
            keyframePoses[hitKeyframeIndex][3] = originalContactWristPronation;
            keyframePoses[hitKeyframeIndex][5] = originalContactWristY;
        }
        currentShoulderY = keyframePoses[0][0]; currentShoulderZ = keyframePoses[0][1]; currentElbow = keyframePoses[0][2];
        currentWristPronation = keyframePoses[0][3]; currentWristZ = keyframePoses[0][4]; currentWristY = keyframePoses[0][5];
        currentFingerKnuckle = keyframePoses[0][6]; currentFingerMid = keyframePoses[0][7];
        currentThumbBaseZ = keyframePoses[0][8]; currentThumbKnuckle = keyframePoses[0][9]; currentThumbMid = keyframePoses[0][10];
        std::cout << "Animation Reset. Contact keyframe restored." << std::endl;
        cameraChanged = true; break;
    case 'w': cameraPos += cameraSpeed * cameraFront; cameraChanged = true; break;
    case 's': cameraPos -= cameraSpeed * cameraFront; cameraChanged = true; break;
    case 'a': cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed; cameraChanged = true; break;
    case 'd': cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed; cameraChanged = true; break;
    case 'q': cameraPos -= cameraSpeed * cameraUp; cameraChanged = true; break;
    case 'e': cameraPos += cameraSpeed * cameraUp; cameraChanged = true; break;
    case 'i': cameraPitch += cameraRotationSpeed; cameraChanged = true; break;
    case 'k': cameraPitch -= cameraRotationSpeed; cameraChanged = true; break;
    case 'l': cameraYaw += cameraRotationSpeed; cameraChanged = true; break;
    case 'h': cameraYaw -= cameraRotationSpeed; cameraChanged = true; break;
    case 'p': showBezierPath = !showBezierPath; std::cout << "Show Bezier Path: " << (showBezierPath ? "ON" : "OFF") << std::endl; cameraChanged = true; break;
    case '1': if (hitKeyframeIndex >= 0 && hitKeyframeIndex < NUM_POSES) { keyframePoses[hitKeyframeIndex][3] += 0.05f; std::cout << "Contact WristPronation: " << keyframePoses[hitKeyframeIndex][3] << std::endl; } break;
    case '2': if (hitKeyframeIndex >= 0 && hitKeyframeIndex < NUM_POSES) { keyframePoses[hitKeyframeIndex][3] -= 0.05f; std::cout << "Contact WristPronation: " << keyframePoses[hitKeyframeIndex][3] << std::endl; } break;
    case '3': if (hitKeyframeIndex >= 0 && hitKeyframeIndex < NUM_POSES) { keyframePoses[hitKeyframeIndex][5] += 0.05f; std::cout << "Contact WristY: " << keyframePoses[hitKeyframeIndex][5] << std::endl; } break;
    case '4': if (hitKeyframeIndex >= 0 && hitKeyframeIndex < NUM_POSES) { keyframePoses[hitKeyframeIndex][5] -= 0.05f; std::cout << "Contact WristY: " << keyframePoses[hitKeyframeIndex][5] << std::endl; } break;
    case 27: glutLeaveMainLoop(); break;
    default: break;
    }
    if (cameraPitch > 89.0f) cameraPitch = 89.0f; if (cameraPitch < -89.0f) cameraPitch = -89.0f;
    if (cameraChanged) { glutPostRedisplay(); }
}
void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    projectionMatrix = glm::perspective(glm::radians(50.0f), (float)w / h, 0.1f, 200.0f);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(1280, 720);
    glutCreateWindow("Serviciu Volei OpenGL - Bezier Path");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(update);
    lastUpdateTime = glutGet(GLUT_ELAPSED_TIME);
    glutMainLoop();
    glDeleteProgram(shader_programme);
    glDeleteVertexArrays(1, &vao); glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &sphereVao); glDeleteBuffers(1, &sphereVbo); glDeleteBuffers(1, &sphereIbo);
    glDeleteVertexArrays(1, &courtFloorVao); glDeleteBuffers(1, &courtFloorVbo); glDeleteBuffers(1, &courtFloorIbo);
    glDeleteVertexArrays(1, &ceilingVao); glDeleteBuffers(1, &ceilingVbo); glDeleteBuffers(1, &ceilingIbo);
    glDeleteVertexArrays(1, &generalFloorVao); glDeleteBuffers(1, &generalFloorVbo); glDeleteBuffers(1, &generalFloorIbo);
    glDeleteVertexArrays(1, &bezierCurveVao); glDeleteBuffers(1, &bezierCurveVbo);
    if (floorWoodTextureID != 0) { glDeleteTextures(1, &floorWoodTextureID); }
    if (wallTextureID != 0) { glDeleteTextures(1, &wallTextureID); }
    if (ceilingTextureID != 0) { glDeleteTextures(1, &ceilingTextureID); }
    if (netTextureID != 0) { glDeleteTextures(1, &netTextureID); }
    return 0;
}