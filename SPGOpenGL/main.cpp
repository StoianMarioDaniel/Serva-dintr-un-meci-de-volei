#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <vector>
#include <cmath>
#include <stdio.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>

#define PI glm::pi<float>()

GLuint shader_programme, vao, vbo;
glm::mat4 projectionMatrix, viewMatrix;
// Uniform ID-uri
GLuint modelMatrixID_uniform;
GLuint viewMatrixID_uniform;
GLuint projectionMatrixID_uniform;
GLuint colorID;

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

// Pozitia de baza a jucatorului/bratului
float playerBaseX = 0.0f;
float playerBaseY = 1.0f;
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


// Varfuri pentru un cub unitar (Pozitie + Normala)
float cubeVertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,   0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,   0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,   0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,   0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,   0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,   0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

// Mingea
glm::vec3 ballPosition;
glm::vec3 ballVelocity(0.0f);
float ballRadius = 0.4f;
bool ballIsServed = false; bool ballIsHit = false; bool ballInToss = false;
const float GRAVITY = 9.81f;

// Fileu
glm::vec3 netPosition(0.0f, 0.0f, 0.0f);
glm::vec3 netDimensions(courtPlayingAreaW, 2.43f, 0.1f);


// Culori obiecte
glm::vec3 handColor(0.96f, 0.76f, 0.62f);
glm::vec3 ballColor(1.0f, 1.0f, 0.8f);
glm::vec3 netColor(0.1f, 0.1f, 0.1f);
glm::vec3 lineColor(1.0f, 1.0f, 1.0f);

// Sfera (mingea)
GLuint sphereVao, sphereVbo, sphereIbo;
std::vector<float> sphereVertices_data; std::vector<unsigned int> sphereIndices_data;
int sphereStacks = 30; int sphereSlices = 30;

// Podea (terenul de joc)
GLuint floorVao, floorVbo, floorIbo;
std::vector<float> floorVertices_data; std::vector<unsigned int> floorIndices_data;

// Tavan
GLuint ceilingVao, ceilingVbo, ceilingIbo;
std::vector<float> ceilingVertices_data; std::vector<unsigned int> ceilingIndices_data;
float overallRoomW = 25.0f;
float overallRoomL = 40.0f;
float ceilingH_thickness = 1.0f;


glm::vec3 courtOrangeColor(1.0f, 0.55f, 0.0f);
glm::vec3 wallColor(0.8f, 0.8f, 0.75f);
glm::vec3 ceilingColor(0.9f, 0.9f, 0.9f);

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
glm::vec3 lightColor1 = glm::vec3(0.9f, 0.9f, 0.8f);
glm::vec3 lightPos2 = glm::vec3(overallRoomW / 4.0f, 11.0f, 0.0f);
glm::vec3 lightColor2 = glm::vec3(0.9f, 0.9f, 0.8f);
glm::vec3 globalAmbientColor = glm::vec3(0.2f, 0.2f, 0.25f);


std::string textFileRead(const char* fn) { std::ifstream ifile(fn); if (!ifile.is_open()) { std::cerr << "Eroare la deschiderea fisierului: " << fn << std::endl; return ""; } std::string filetext; std::string line; while (std::getline(ifile, line)) { filetext.append(line + "\n"); } return filetext; }
float lerp(float start, float end, float t_lerp) { t_lerp = glm::clamp(t_lerp, 0.0f, 1.0f); return start + t_lerp * (end - start); }

void updateCameraFront() {
    glm::vec3 front;
    front.x = cos(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
    front.y = sin(glm::radians(cameraPitch));
    front.z = sin(glm::radians(cameraYaw)) * cos(glm::radians(cameraPitch));
    cameraFront = glm::normalize(front);
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
        -halfW, -halfH,  halfD,  0.0f, 0.0f, 1.0f,   halfW, -halfH,  halfD,  0.0f, 0.0f, 1.0f,   halfW,  halfH,  halfD,  0.0f, 0.0f, 1.0f,  -halfW,  halfH,  halfD,  0.0f, 0.0f, 1.0f,
        -halfW, -halfH, -halfD,  0.0f, 0.0f,-1.0f,   halfW, -halfH, -halfD,  0.0f, 0.0f,-1.0f,   halfW,  halfH, -halfD,  0.0f, 0.0f,-1.0f,  -halfW,  halfH, -halfD,  0.0f, 0.0f,-1.0f,
        -halfW,  halfH,  halfD,  0.0f, 1.0f, 0.0f,   halfW,  halfH,  halfD,  0.0f, 1.0f, 0.0f,   halfW,  halfH, -halfD,  0.0f, 1.0f, 0.0f,  -halfW,  halfH, -halfD,  0.0f, 1.0f, 0.0f,
        -halfW, -halfH,  halfD,  0.0f,-1.0f, 0.0f,   halfW, -halfH,  halfD,  0.0f,-1.0f, 0.0f,   halfW, -halfH, -halfD,  0.0f,-1.0f, 0.0f,  -halfW, -halfH, -halfD,  0.0f,-1.0f, 0.0f,
         halfW, -halfH,  halfD,  1.0f, 0.0f, 0.0f,   halfW, -halfH, -halfD,  1.0f, 0.0f, 0.0f,   halfW,  halfH, -halfD,  1.0f, 0.0f, 0.0f,   halfW,  halfH,  halfD,  1.0f, 0.0f, 0.0f,
        -halfW, -halfH,  halfD, -1.0f, 0.0f, 0.0f,  -halfW, -halfH, -halfD, -1.0f, 0.0f, 0.0f,  -halfW,  halfH, -halfD, -1.0f, 0.0f, 0.0f,  -halfW,  halfH,  halfD, -1.0f, 0.0f, 0.0f
    };
    indices = {
        0, 1, 2,   0, 2, 3,    4, 5, 6,   4, 6, 7,    8, 9, 10,  8, 10,11,
        12,13,14,  12,14,15,   16,17,18,  16,18,19,   20,21,22,  20,22,23
    };
}


void drawCube(const glm::mat4& modelMatrixIn, const glm::vec3& color) {
    glUniform3fv(colorID, 1, glm::value_ptr(color));
    glUniformMatrix4fv(modelMatrixID_uniform, 1, GL_FALSE, glm::value_ptr(modelMatrixIn));
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void drawSphere(const glm::mat4& modelMatrixIn, const glm::vec3& color) {
    glUniform3fv(colorID, 1, glm::value_ptr(color));
    glUniformMatrix4fv(modelMatrixID_uniform, 1, GL_FALSE, glm::value_ptr(modelMatrixIn));
    glBindVertexArray(sphereVao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereIbo);
    glDrawElements(GL_TRIANGLES, sphereIndices_data.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void drawGeneratedCuboid(GLuint vao_cuboid, GLuint ibo_cuboid, size_t numIndices, const glm::mat4& modelMatrixIn, const glm::vec3& color) {
    glUniform3fv(colorID, 1, glm::value_ptr(color));
    glUniformMatrix4fv(modelMatrixID_uniform, 1, GL_FALSE, glm::value_ptr(modelMatrixIn));
    glBindVertexArray(vao_cuboid);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cuboid);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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

    int pose1_idx = floor(animationProgress);
    int pose2_idx = ceil(animationProgress);

    pose1_idx = glm::clamp(pose1_idx, 0, NUM_POSES - 1);
    pose2_idx = glm::clamp(pose2_idx, 0, NUM_POSES - 1);

    float t_lerp_val;
    if (pose1_idx == pose2_idx) {
        t_lerp_val = 0.0f;
    }
    else {
        t_lerp_val = (animationProgress - (float)pose1_idx);
    }
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
        float floorLevel = 0.0f;
        if (ballInToss && !ballIsHit) {
            ballVelocity.y -= GRAVITY * deltaTime * 1.5f; ballPosition += ballVelocity * deltaTime;
            float hitMomentProgress = (float)hitKeyframeIndex;
            if (animationProgress >= hitMomentProgress - 0.20f && animationProgress <= hitMomentProgress + 0.20f) {
                float actualPlayerArmBaseY = floorLevel + playerBaseY;
                glm::mat4 palmTransform = glm::mat4(1.0f);
                palmTransform = glm::translate(palmTransform, glm::vec3(playerBaseX, actualPlayerArmBaseY, playerBaseZ));
                palmTransform = glm::rotate(palmTransform, -PI / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
                palmTransform = glm::rotate(palmTransform, currentShoulderY, glm::vec3(0.0f, 1.0f, 0.0f));
                palmTransform = glm::rotate(palmTransform, currentShoulderZ, glm::vec3(0.0f, 0.0f, 1.0f));
                palmTransform = glm::translate(palmTransform, glm::vec3(armLength, 0.0f, 0.0f));
                palmTransform = glm::rotate(palmTransform, currentElbow, glm::vec3(0.0f, 0.0f, 1.0f));
                palmTransform = glm::translate(palmTransform, glm::vec3(forearmLength, 0.0f, 0.0f));
                palmTransform = glm::rotate(palmTransform, currentWristPronation, glm::vec3(1.0f, 0.0f, 0.0f));
                palmTransform = glm::rotate(palmTransform, currentWristZ, glm::vec3(0.0f, 0.0f, 1.0f));
                palmTransform = glm::rotate(palmTransform, currentWristY, glm::vec3(0.0f, 1.0f, 0.0f));
                glm::vec3 palmGlobalCenter = glm::vec3(palmTransform * glm::vec4(palmLength / 2.0f, 0.0f, 0.0f, 1.0f));
                if (glm::distance(ballPosition, palmGlobalCenter) < (palmLength * 0.7f + ballRadius)) {
                    ballIsHit = true; ballInToss = false;
                    float targetXNetCenter = netPosition.x; float deltaX = targetXNetCenter - ballPosition.x;
                    ballVelocity = glm::vec3(deltaX * 0.1f,
                        6.5f + ((ballPosition.y - (actualPlayerArmBaseY + armLength * 0.6f)) * 0.2f),
                        14.0f + abs(playerBaseZ * 0.08f));
                    std::cout << "BALL HIT! Vel: X=" << ballVelocity.x << " Y=" << ballVelocity.y << " Z=" << ballVelocity.z << std::endl;
                }
            }
            if (ballPosition.y < (floorLevel + ballRadius) && !ballIsHit) {
                ballInToss = false; ballIsServed = false;
                isAnimatingForward = false;
                std::cout << "Ball hit the floor before being hit. Hand animation stopped." << std::endl;
            }
        }
        else if (ballIsHit) {
            ballVelocity.y -= GRAVITY * deltaTime; ballPosition += ballVelocity * deltaTime;
            if (ballPosition.y < (floorLevel + ballRadius) || abs(ballPosition.x) >(courtPlayingAreaW / 2.0f + 3.0f) || abs(ballPosition.z) > (courtPlayingAreaL / 2.0f + 5.0f)) {
            }
        }
        needsRedraw = true;
    }
    glutPostRedisplay();
}

void display() {
    updateCameraFront();
    viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

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


    float floorTopSurfaceY = 0.0f;
    float ceilingBottomSurfaceY = 12.0f;
    float wallVisualThickness = 0.2f;

    glm::mat4 courtModelMatrix = glm::mat4(1.0f);
    courtModelMatrix = glm::translate(courtModelMatrix, glm::vec3(0.0f, floorTopSurfaceY - courtPlayingAreaThickness / 2.0f, 0.0f));
    drawGeneratedCuboid(floorVao, floorIbo, floorIndices_data.size(), courtModelMatrix, courtOrangeColor);

    float linesYCenterPosition = floorTopSurfaceY + LINE_THICKNESS_VISUAL_DIM / 2.0f;
    glm::mat4 lineModelMatrix;
    lineModelMatrix = glm::mat4(1.0f); lineModelMatrix = glm::translate(lineModelMatrix, glm::vec3((courtPlayingAreaW - LINE_WIDTH_DIM) / 2.0f, linesYCenterPosition, 0.0f)); lineModelMatrix = glm::scale(lineModelMatrix, glm::vec3(LINE_WIDTH_DIM, LINE_THICKNESS_VISUAL_DIM, courtPlayingAreaL)); drawCube(lineModelMatrix, lineColor);
    lineModelMatrix = glm::mat4(1.0f); lineModelMatrix = glm::translate(lineModelMatrix, glm::vec3(-(courtPlayingAreaW - LINE_WIDTH_DIM) / 2.0f, linesYCenterPosition, 0.0f)); lineModelMatrix = glm::scale(lineModelMatrix, glm::vec3(LINE_WIDTH_DIM, LINE_THICKNESS_VISUAL_DIM, courtPlayingAreaL)); drawCube(lineModelMatrix, lineColor);
    lineModelMatrix = glm::mat4(1.0f); lineModelMatrix = glm::translate(lineModelMatrix, glm::vec3(0.0f, linesYCenterPosition, (courtPlayingAreaL - LINE_WIDTH_DIM) / 2.0f)); lineModelMatrix = glm::scale(lineModelMatrix, glm::vec3(courtPlayingAreaW, LINE_THICKNESS_VISUAL_DIM, LINE_WIDTH_DIM)); drawCube(lineModelMatrix, lineColor);
    lineModelMatrix = glm::mat4(1.0f); lineModelMatrix = glm::translate(lineModelMatrix, glm::vec3(0.0f, linesYCenterPosition, -(courtPlayingAreaL - LINE_WIDTH_DIM) / 2.0f)); lineModelMatrix = glm::scale(lineModelMatrix, glm::vec3(courtPlayingAreaW, LINE_THICKNESS_VISUAL_DIM, LINE_WIDTH_DIM)); drawCube(lineModelMatrix, lineColor);
    lineModelMatrix = glm::mat4(1.0f); lineModelMatrix = glm::translate(lineModelMatrix, glm::vec3(0.0f, linesYCenterPosition, 0.0f)); lineModelMatrix = glm::scale(lineModelMatrix, glm::vec3(courtPlayingAreaW, LINE_THICKNESS_VISUAL_DIM, LINE_WIDTH_DIM)); drawCube(lineModelMatrix, lineColor);
    float attackLineOffsetFromCenter = 3.0f;
    lineModelMatrix = glm::mat4(1.0f); lineModelMatrix = glm::translate(lineModelMatrix, glm::vec3(0.0f, linesYCenterPosition, attackLineOffsetFromCenter)); lineModelMatrix = glm::scale(lineModelMatrix, glm::vec3(courtPlayingAreaW, LINE_THICKNESS_VISUAL_DIM, LINE_WIDTH_DIM)); drawCube(lineModelMatrix, lineColor);
    lineModelMatrix = glm::mat4(1.0f); lineModelMatrix = glm::translate(lineModelMatrix, glm::vec3(0.0f, linesYCenterPosition, -attackLineOffsetFromCenter)); lineModelMatrix = glm::scale(lineModelMatrix, glm::vec3(courtPlayingAreaW, LINE_THICKNESS_VISUAL_DIM, LINE_WIDTH_DIM)); drawCube(lineModelMatrix, lineColor);

    glm::mat4 ceilingModelMatrix = glm::mat4(1.0f);
    ceilingModelMatrix = glm::translate(ceilingModelMatrix, glm::vec3(0.0f, ceilingBottomSurfaceY + ceilingH_thickness / 2.0f, 0.0f));
    drawGeneratedCuboid(ceilingVao, ceilingIbo, ceilingIndices_data.size(), ceilingModelMatrix, ceilingColor);

    float roomVisualHeight = ceilingBottomSurfaceY - floorTopSurfaceY;
    float wallCenterY = floorTopSurfaceY + (roomVisualHeight / 2.0f);
    glm::mat4 wallBackModelMatrix = glm::mat4(1.0f); wallBackModelMatrix = glm::translate(wallBackModelMatrix, glm::vec3(0.0f, wallCenterY, -overallRoomL / 2.0f)); wallBackModelMatrix = glm::scale(wallBackModelMatrix, glm::vec3(overallRoomW, roomVisualHeight, wallVisualThickness)); drawCube(wallBackModelMatrix, wallColor);
    glm::mat4 wallLeftModelMatrix = glm::mat4(1.0f); wallLeftModelMatrix = glm::translate(wallLeftModelMatrix, glm::vec3(-overallRoomW / 2.0f, wallCenterY, 0.0f)); wallLeftModelMatrix = glm::scale(wallLeftModelMatrix, glm::vec3(wallVisualThickness, roomVisualHeight, overallRoomL)); drawCube(wallLeftModelMatrix, wallColor);
    glm::mat4 wallRightModelMatrix = glm::mat4(1.0f); wallRightModelMatrix = glm::translate(wallRightModelMatrix, glm::vec3(overallRoomW / 2.0f, wallCenterY, 0.0f)); wallRightModelMatrix = glm::scale(wallRightModelMatrix, glm::vec3(wallVisualThickness, roomVisualHeight, overallRoomL)); drawCube(wallRightModelMatrix, wallColor);
    glm::mat4 wallFrontModelMatrix = glm::mat4(1.0f); wallFrontModelMatrix = glm::translate(wallFrontModelMatrix, glm::vec3(0.0f, wallCenterY, overallRoomL / 2.0f)); wallFrontModelMatrix = glm::scale(wallFrontModelMatrix, glm::vec3(overallRoomW, roomVisualHeight, wallVisualThickness)); drawCube(wallFrontModelMatrix, wallColor);

    glm::mat4 netDisplayMatrix = glm::mat4(1.0f);
    netDisplayMatrix = glm::translate(netDisplayMatrix, glm::vec3(netPosition.x, floorTopSurfaceY + netDimensions.y / 2.0f, netPosition.z));
    netDisplayMatrix = glm::scale(netDisplayMatrix, netDimensions); drawCube(netDisplayMatrix, netColor);

    if (ballIsServed || ballInToss || (!ballIsServed && !isAnimatingForward && !isAnimatingBackward)) {
        glm::mat4 ballModelMatrix = glm::mat4(1.0f); ballModelMatrix = glm::translate(ballModelMatrix, ballPosition);
        drawSphere(ballModelMatrix, ballColor); 
    }

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

    glFlush();
}

void init() {
    const GLubyte* renderer = glGetString(GL_RENDERER); const GLubyte* version = glGetString(GL_VERSION); printf("Renderer: %s\n", renderer); printf("OpenGL version supported %s\n", version);
    glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS); glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glewExperimental = GL_TRUE; GLenum err = glewInit(); if (GLEW_OK != err) { fprintf(stderr, "Error initializing GLEW: %s\n", glewGetErrorString(err)); exit(1); } printf("Using GLEW %s\n", glewGetString(GLEW_VERSION));

    glGenBuffers(1, &vbo); glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glGenVertexArrays(1, &vao); glBindVertexArray(vao); glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // Sfera (mingea) 
    generateSphere();
    glGenVertexArrays(1, &sphereVao); glBindVertexArray(sphereVao);
    glGenBuffers(1, &sphereVbo); glBindBuffer(GL_ARRAY_BUFFER, sphereVbo);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices_data.size() * sizeof(float), sphereVertices_data.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &sphereIbo); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices_data.size() * sizeof(unsigned int), sphereIndices_data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // Podea 
    generateCuboidVertices(courtPlayingAreaW, courtPlayingAreaThickness, courtPlayingAreaL, floorVertices_data, floorIndices_data);
    glGenVertexArrays(1, &floorVao); glBindVertexArray(floorVao);
    glGenBuffers(1, &floorVbo); glBindBuffer(GL_ARRAY_BUFFER, floorVbo);
    glBufferData(GL_ARRAY_BUFFER, floorVertices_data.size() * sizeof(float), floorVertices_data.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &floorIbo); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, floorIndices_data.size() * sizeof(unsigned int), floorIndices_data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // Tavan
    generateCuboidVertices(overallRoomW, ceilingH_thickness, overallRoomL, ceilingVertices_data, ceilingIndices_data);
    glGenVertexArrays(1, &ceilingVao); glBindVertexArray(ceilingVao);
    glGenBuffers(1, &ceilingVbo); glBindBuffer(GL_ARRAY_BUFFER, ceilingVbo);
    glBufferData(GL_ARRAY_BUFFER, ceilingVertices_data.size() * sizeof(float), ceilingVertices_data.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &ceilingIbo); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ceilingIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ceilingIndices_data.size() * sizeof(unsigned int), ceilingIndices_data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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


    currentShoulderY = keyframePoses[0][0]; currentShoulderZ = keyframePoses[0][1]; currentElbow = keyframePoses[0][2];
    currentWristPronation = keyframePoses[0][3]; currentWristZ = keyframePoses[0][4]; currentWristY = keyframePoses[0][5];
    currentFingerKnuckle = keyframePoses[0][6]; currentFingerMid = keyframePoses[0][7];
    currentThumbBaseZ = keyframePoses[0][8]; currentThumbKnuckle = keyframePoses[0][9]; currentThumbMid = keyframePoses[0][10];
    ballPosition = glm::vec3(playerBaseX, (0.0f + playerBaseY) + 0.5f, playerBaseZ + palmLength + ballRadius + 0.3f);
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
    switch (key) {
    case 'j': if (!ballIsServed) { isAnimatingForward = true; isAnimatingBackward = false; animationProgress = 0.0f; ballIsServed = true; ballInToss = true; ballIsHit = false; ballPosition = glm::vec3(playerBaseX - 0.1f, (0.0f + playerBaseY) + 1.0f, playerBaseZ + 0.5f); ballVelocity = glm::vec3(0.05f, 7.5f, 1.5f); std::cout << "Serve initiated. Ball toss." << std::endl; } break;
    case 'K': isAnimatingBackward = true; isAnimatingForward = false; break;
    case ' ': isAnimatingForward = false; isAnimatingBackward = false; animationProgress = 0.0f; ballIsServed = false; ballInToss = false; ballIsHit = false; ballPosition = glm::vec3(playerBaseX, (0.0f + playerBaseY) + 0.5f, playerBaseZ + palmLength + ballRadius + 0.3f); ballVelocity = glm::vec3(0.0f); if (contactKeyframeOriginalsStored && hitKeyframeIndex >= 0 && hitKeyframeIndex < NUM_POSES) { keyframePoses[hitKeyframeIndex][3] = originalContactWristPronation; keyframePoses[hitKeyframeIndex][5] = originalContactWristY; } currentShoulderY = keyframePoses[0][0]; currentShoulderZ = keyframePoses[0][1]; currentElbow = keyframePoses[0][2]; currentWristPronation = keyframePoses[0][3]; currentWristZ = keyframePoses[0][4]; currentWristY = keyframePoses[0][5]; currentFingerKnuckle = keyframePoses[0][6]; currentFingerMid = keyframePoses[0][7]; currentThumbBaseZ = keyframePoses[0][8]; currentThumbKnuckle = keyframePoses[0][9]; currentThumbMid = keyframePoses[0][10]; std::cout << "Animation Reset. Contact keyframe restored." << std::endl; break;

    case 'w': cameraPos += cameraSpeed * cameraFront; break;
    case 's': cameraPos -= cameraSpeed * cameraFront; break;
    case 'a': cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed; break;
    case 'd': cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed; break;
    case 'q': cameraPos -= cameraSpeed * cameraUp; break;
    case 'e': cameraPos += cameraSpeed * cameraUp; break;

    case 'i': cameraPitch += cameraRotationSpeed; break;
    case 'k': cameraPitch -= cameraRotationSpeed; break;
    case 'l': cameraYaw += cameraRotationSpeed; break;
    case 'h': cameraYaw -= cameraRotationSpeed; break;


    case '1': if (hitKeyframeIndex >= 0 && hitKeyframeIndex < NUM_POSES) { keyframePoses[hitKeyframeIndex][3] += 0.05f; std::cout << "Contact WristPronation: " << keyframePoses[hitKeyframeIndex][3] << std::endl; } break;
    case '2': if (hitKeyframeIndex >= 0 && hitKeyframeIndex < NUM_POSES) { keyframePoses[hitKeyframeIndex][3] -= 0.05f; std::cout << "Contact WristPronation: " << keyframePoses[hitKeyframeIndex][3] << std::endl; } break;
    case '3': if (hitKeyframeIndex >= 0 && hitKeyframeIndex < NUM_POSES) { keyframePoses[hitKeyframeIndex][5] += 0.05f; std::cout << "Contact WristY: " << keyframePoses[hitKeyframeIndex][5] << std::endl; } break;
    case '4': if (hitKeyframeIndex >= 0 && hitKeyframeIndex < NUM_POSES) { keyframePoses[hitKeyframeIndex][5] -= 0.05f; std::cout << "Contact WristY: " << keyframePoses[hitKeyframeIndex][5] << std::endl; } break;
    case 27: glutLeaveMainLoop(); break;
    default: break;
    }
    if (cameraPitch > 89.0f) cameraPitch = 89.0f;
    if (cameraPitch < -89.0f) cameraPitch = -89.0f;
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    projectionMatrix = glm::perspective(glm::radians(50.0f), (float)w / h, 0.1f, 200.0f);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv); glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutCreateWindow("Serviciu Volei OpenGL - Iluminare Corecta Mana");
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
    glDeleteVertexArrays(1, &floorVao); glDeleteBuffers(1, &floorVbo); glDeleteBuffers(1, &floorIbo);
    glDeleteVertexArrays(1, &ceilingVao); glDeleteBuffers(1, &ceilingVbo); glDeleteBuffers(1, &ceilingIbo);
    return 0;
}