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

// Shader, VAO/VBO pentru cub (mana/pereti/fileu/linii)
GLuint shader_programme, vao, vbo;
glm::mat4 projectionMatrix, viewMatrix;
GLuint matrixID; // Uniform pentru modelViewProjectionMatrix
GLuint colorID;  // Uniform pentru objectColor

// Animatie mana
float animationProgress = 0.0f;
const float animationDuration = 1.5f;
bool isAnimatingForward = false;
bool isAnimatingBackward = false;
int lastUpdateTime = 0;

// Pozitia de baza a jucatorului/bratului
float playerBaseX = 0.0f; // Jucatorul va fi centrat pe X pentru usurinta cu terenul
float playerBaseY = 1.0f; // Inaltimea bazei bratului deasupra PODELEI (care va fi la Y=0)
float playerBaseZ = 6.0f; // Jucatorul in terenul sau, aproape de linia de 3m

// Keyframes pentru animatia mainii
const int NUM_POSES = 5;
const int NUM_JOINTS = 11;
float keyframePoses[NUM_POSES][NUM_JOINTS] = {
    {  0.0f,      PI / 4.0f,  PI / 2.5f, 0.0f,     0.0f,      0.0f,  PI / 3.0f, PI / 3.0f, -PI / 6.0f, PI / 4.0f, PI / 4.0f },
    { -PI / 10.0f, -PI / 2.0f, PI / 1.5f, 0.0f,     PI / 4.0f, 0.0f,  PI / 10.0f,PI / 10.0f,-PI / 4.0f, PI / 8.0f, PI / 8.0f },
    {  0.0f,       PI / 2.2f,  PI / 10.0f,PI / 12.0f,-PI / 3.0f, 0.0f,  PI / 12.0f,PI / 12.0f,-PI / 5.0f, PI / 6.0f, PI / 6.0f },
    {  PI / 12.0f, PI / 3.0f,  PI / 8.0f, PI / 8.0f,  0.0f,      0.0f,  PI / 10.0f,PI / 10.0f,-PI / 6.0f, PI / 8.0f, PI / 8.0f },
    {  PI / 10.0f, 0.0f,       PI / 6.0f, PI / 6.0f,  PI / 10.0f,0.0f,  PI / 6.0f, PI / 6.0f, -PI / 8.0f, PI / 10.0f,PI / 10.0f }
};

// Unghiuri curente pentru articulatiile mainii
float currentShoulderY, currentShoulderZ, currentElbow;
float currentWristPronation;
float currentWristZ, currentWristY;
float currentFingerKnuckle, currentFingerMid;
float currentThumbBaseZ, currentThumbKnuckle, currentThumbMid;

// Dimensiuni parti mana
float armLength = 2.5f; float armWidth = 0.6f; float forearmLength = 2.0f; float forearmWidth = 0.5f;
float palmLength = 1.2f; float palmWidth = 1.2f; float palmDepth = 0.3f;
float fingerLength = 1.0f; float fingerWidth = 0.18f; float thumbLength = 0.7f; float thumbWidth = 0.20f;

// Unghiuri pentru control manual (daca e cazul)
float shoulderAngleZ = keyframePoses[0][1]; float shoulderAngleY = keyframePoses[0][0]; float elbowAngle = keyframePoses[0][2];
float wristPronationAngle = keyframePoses[0][3]; float wristAngleZ = keyframePoses[0][4]; float wristAngleY = keyframePoses[0][5];
float fingerAngles[5][2] = { {keyframePoses[0][6], keyframePoses[0][7]}, {keyframePoses[0][6], keyframePoses[0][7]}, {keyframePoses[0][6], keyframePoses[0][7]}, {keyframePoses[0][6], keyframePoses[0][7]}, {keyframePoses[0][9], keyframePoses[0][10]} };
float thumbBaseAngleZ = keyframePoses[0][8];

// Varfuri pentru un cub unitar (folosit pentru mana, pereti, fileu, linii)
float cubeVertices[] = { -0.5f,-0.5f,0.5f,0.5f,-0.5f,0.5f,0.5f,0.5f,0.5f,-0.5f,-0.5f,0.5f,0.5f,0.5f,0.5f,-0.5f,0.5f,0.5f,-0.5f,-0.5f,-0.5f,-0.5f,0.5f,-0.5f,0.5f,0.5f,-0.5f,-0.5f,-0.5f,-0.5f,0.5f,0.5f,-0.5f,0.5f,-0.5f,-0.5f,-0.5f,-0.5f,-0.5f,-0.5f,-0.5f,0.5f,-0.5f,0.5f,0.5f,-0.5f,-0.5f,-0.5f,-0.5f,0.5f,0.5f,-0.5f,0.5f,-0.5f,0.5f,-0.5f,-0.5f,0.5f,0.5f,-0.5f,0.5f,0.5f,0.5f,0.5f,-0.5f,-0.5f,0.5f,0.5f,0.5f,0.5f,-0.5f,0.5f,-0.5f,0.5f,0.5f,0.5f,0.5f,0.5f,-0.5f,-0.5f,0.5f,0.5f,0.5f,-0.5f,-0.5f,0.5f,-0.5f,-0.5f,-0.5f,-0.5f,-0.5f,-0.5f,-0.5f,-0.5f,0.5f,-0.5f,-0.5f,-0.5f,-0.5f,0.5f,0.5f,-0.5f,-0.5f,0.5f,-0.5f,0.5f };

// Mingea
glm::vec3 ballPosition(playerBaseX, playerBaseY + 0.5f, playerBaseZ - 1.0f);
glm::vec3 ballVelocity(0.0f);
float ballRadius = 0.4f;
bool ballIsServed = false;
bool ballIsHit = false;
bool ballInToss = false;
const float GRAVITY = 9.81f;
int hitKeyframeIndex = 2;

// Fileu
glm::vec3 netPosition(0.0f, 0.0f, 0.0f); // Y si Z vor fi ajustate in display relativ la podea si centrul terenului
glm::vec3 netDimensions(9.0f, 2.43f, 0.1f); // Latime teren, Inaltime oficiala, Grosime fileu

// Culori obiecte
glm::vec3 handColor(0.96f, 0.76f, 0.62f);
glm::vec3 ballColor(1.0f, 1.0f, 0.8f);
glm::vec3 netColor(0.1f, 0.1f, 0.1f);
glm::vec3 lineColor(1.0f, 1.0f, 1.0f);

// Variabile pentru Sfera (mingea)
GLuint sphereVao, sphereVbo, sphereIbo;
std::vector<float> sphereVertices_data;
std::vector<unsigned int> sphereIndices_data;
int sphereStacks = 30;
int sphereSlices = 30;

// Variabile pentru Podea (terenul de joc)
GLuint floorVao, floorVbo, floorIbo;
std::vector<float> floorVertices_data;
std::vector<unsigned int> floorIndices_data;
float courtPlayingAreaW = 9.0f;
float courtPlayingAreaL = 18.0f;
float courtPlayingAreaThickness = 0.1f;

// Variabile pentru Tavan
GLuint ceilingVao, ceilingVbo, ceilingIbo;
std::vector<float> ceilingVertices_data;
std::vector<unsigned int> ceilingIndices_data;
// Tavanul va acoperi o zona mai mare decat terenul
float overallRoomW = 20.0f; // Latimea totala a salii
float overallRoomL = 30.0f; // Lungimea totala a salii
float ceilingH_thickness = 1.0f;


// Culori pentru elementele salii
glm::vec3 courtOrangeColor(1.0f, 0.55f, 0.0f);
glm::vec3 wallColor(0.8f, 0.8f, 0.75f);
glm::vec3 ceilingColor(0.9f, 0.9f, 0.9f);

// Dimensiuni Linii Teren
const float LINE_WIDTH_DIM = 0.05f;
const float LINE_THICKNESS_VISUAL_DIM = 0.02f;


std::string textFileRead(const char* fn) { std::ifstream ifile(fn); if (!ifile.is_open()) { std::cerr << "Eroare la deschiderea fisierului: " << fn << std::endl; return ""; } std::string filetext; std::string line; while (std::getline(ifile, line)) { filetext.append(line + "\n"); } return filetext; }
float lerp(float start, float end, float t_lerp) { t_lerp = glm::clamp(t_lerp, 0.0f, 1.0f); return start + t_lerp * (end - start); }

void generateSphere() {
    sphereVertices_data.clear();
    sphereIndices_data.clear();
    for (int i = 0; i <= sphereStacks; ++i) {
        float stackAngle = PI * (float)i / sphereStacks;
        float xy_coord = ballRadius * sin(stackAngle);
        float z_coord = ballRadius * cos(stackAngle);
        for (int j = 0; j <= sphereSlices; ++j) {
            float sectorAngle = 2 * PI * (float)j / sphereSlices;
            float x_coord = xy_coord * cos(sectorAngle);
            float y_coord = xy_coord * sin(sectorAngle);
            sphereVertices_data.push_back(x_coord);
            sphereVertices_data.push_back(y_coord);
            sphereVertices_data.push_back(z_coord);
        }
    }
    for (int i = 0; i < sphereStacks; ++i) {
        for (int j = 0; j < sphereSlices; ++j) {
            int first = (i * (sphereSlices + 1)) + j;
            int second = first + sphereSlices + 1;
            sphereIndices_data.push_back(first);
            sphereIndices_data.push_back(second);
            sphereIndices_data.push_back(first + 1);
            sphereIndices_data.push_back(second);
            sphereIndices_data.push_back(second + 1);
            sphereIndices_data.push_back(first + 1);
        }
    }
}

void generateCuboidVertices(float width, float height, float depth, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    vertices.clear();
    indices.clear();
    float halfW = width / 2.0f;
    float halfH = height / 2.0f;
    float halfD = depth / 2.0f;
    vertices = {
        -halfW, -halfH,  halfD,  halfW, -halfH,  halfD,  halfW,  halfH,  halfD, -halfW,  halfH,  halfD,
        -halfW, -halfH, -halfD,  halfW, -halfH, -halfD,  halfW,  halfH, -halfD, -halfW,  halfH, -halfD
    };
    indices = {
        0, 1, 2,  0, 2, 3,  1, 5, 6,  1, 6, 2,  5, 4, 7,  5, 7, 6,
        4, 0, 3,  4, 3, 7,  3, 2, 6,  3, 6, 7,  4, 5, 1,  4, 1, 0
    };
}

void drawCube(const glm::mat4& modelMatrix, const glm::vec3& color) {
    glUniform3fv(colorID, 1, glm::value_ptr(color));
    glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void drawSphere(const glm::mat4& modelMatrix, const glm::vec3& color) {
    glUniform3fv(colorID, 1, glm::value_ptr(color));
    glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
    glBindVertexArray(sphereVao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereIbo);
    glDrawElements(GL_TRIANGLES, sphereIndices_data.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void drawGeneratedCuboid(GLuint vao_cuboid, GLuint ibo_cuboid, size_t numIndices, const glm::mat4& modelMatrix, const glm::vec3& color) {
    glUniform3fv(colorID, 1, glm::value_ptr(color));
    glm::mat4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * modelMatrix;
    glUniformMatrix4fv(matrixID, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));
    glBindVertexArray(vao_cuboid);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cuboid);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void update() {
    int currentTime = glutGet(GLUT_ELAPSED_TIME); int timeElapsed = currentTime - lastUpdateTime; if (timeElapsed <= 0) { timeElapsed = 1; }
    float deltaTime = (float)timeElapsed / 1000.0f; lastUpdateTime = currentTime;
    float totalAnimationRange = (float)(NUM_POSES - 1); float animationSpeed = totalAnimationRange / animationDuration;
    bool needsRedraw = false;

    if (isAnimatingForward) { animationProgress += animationSpeed * deltaTime; if (animationProgress >= totalAnimationRange) { animationProgress = totalAnimationRange; } needsRedraw = true; }
    else if (isAnimatingBackward) { animationProgress -= animationSpeed * deltaTime; if (animationProgress <= 0.0f) { animationProgress = 0.0f; isAnimatingBackward = false; } needsRedraw = true; }

    int pose1_idx = floor(animationProgress); int pose2_idx = ceil(animationProgress);
    pose1_idx = glm::clamp(pose1_idx, 0, NUM_POSES - 1); pose2_idx = glm::clamp(pose2_idx, 0, NUM_POSES - 1);
    float t_lerp_val = animationProgress - pose1_idx;
    if (pose1_idx == pose2_idx || pose2_idx < pose1_idx) { t_lerp_val = 0.0f; }
    else { t_lerp_val = glm::clamp(t_lerp_val, 0.0f, 1.0f); }

    currentShoulderY = lerp(keyframePoses[pose1_idx][0], keyframePoses[pose2_idx][0], t_lerp_val); currentShoulderZ = lerp(keyframePoses[pose1_idx][1], keyframePoses[pose2_idx][1], t_lerp_val);
    currentElbow = lerp(keyframePoses[pose1_idx][2], keyframePoses[pose2_idx][2], t_lerp_val); currentWristPronation = lerp(keyframePoses[pose1_idx][3], keyframePoses[pose2_idx][3], t_lerp_val);
    currentWristZ = lerp(keyframePoses[pose1_idx][4], keyframePoses[pose2_idx][4], t_lerp_val); currentWristY = lerp(keyframePoses[pose1_idx][5], keyframePoses[pose2_idx][5], t_lerp_val);
    currentFingerKnuckle = lerp(keyframePoses[pose1_idx][6], keyframePoses[pose2_idx][6], t_lerp_val); currentFingerMid = lerp(keyframePoses[pose1_idx][7], keyframePoses[pose2_idx][7], t_lerp_val);
    currentThumbBaseZ = lerp(keyframePoses[pose1_idx][8], keyframePoses[pose2_idx][8], t_lerp_val); currentThumbKnuckle = lerp(keyframePoses[pose1_idx][9], keyframePoses[pose2_idx][9], t_lerp_val);
    currentThumbMid = lerp(keyframePoses[pose1_idx][10], keyframePoses[pose2_idx][10], t_lerp_val);

    if (!isAnimatingForward && !isAnimatingBackward && !ballIsServed) {
        shoulderAngleY = currentShoulderY; shoulderAngleZ = currentShoulderZ; elbowAngle = currentElbow; wristPronationAngle = currentWristPronation; wristAngleZ = currentWristZ; wristAngleY = currentWristY; thumbBaseAngleZ = currentThumbBaseZ;
        for (int i = 0; i < 4; ++i) { fingerAngles[i][0] = currentFingerKnuckle; fingerAngles[i][1] = currentFingerMid; }
        fingerAngles[4][0] = currentThumbKnuckle; fingerAngles[4][1] = currentThumbMid;
    }

    if (ballIsServed) {
        float floorLevel = 0.0f;
        if (ballInToss && !ballIsHit) {
            ballVelocity.y -= GRAVITY * deltaTime * 1.5f;
            ballPosition += ballVelocity * deltaTime;
            float hitProgressLowerBound = (float)hitKeyframeIndex - 0.15f;
            float hitProgressUpperBound = (float)hitKeyframeIndex + 0.15f;
            if (animationProgress >= hitProgressLowerBound && animationProgress <= hitProgressUpperBound) {
                float actualPlayerArmBaseY = floorLevel + playerBaseY;
                float approximateHitHeightMin = actualPlayerArmBaseY + armLength * 0.5f;
                float approximateHitHeightMax = actualPlayerArmBaseY + armLength + forearmLength + palmLength + ballRadius * 2.0f;
                if (ballPosition.y > approximateHitHeightMin && ballPosition.y < approximateHitHeightMax &&
                    abs(ballPosition.x - playerBaseX) < (palmWidth + forearmWidth + ballRadius) &&
                    abs(ballPosition.z - (playerBaseZ - palmLength)) < (palmLength + forearmLength + ballRadius)) {
                    ballIsHit = true;
                    ballInToss = false;
                    float targetXNetCenter = netPosition.x;
                    float deltaX = targetXNetCenter - ballPosition.x;
                    ballVelocity = glm::vec3(deltaX * 0.8f,
                        9.0f + ((ballPosition.y - (actualPlayerArmBaseY + armLength)) * 0.5f),
                        -15.0f - ((ballPosition.y - (actualPlayerArmBaseY + armLength)) * 1.0f));
                    std::cout << "BALL HIT! Velocity: X=" << ballVelocity.x << " Y=" << ballVelocity.y << " Z=" << ballVelocity.z << std::endl;
                }
            }
            if (ballPosition.y < (floorLevel + ballRadius) && !ballIsHit) {
                ballInToss = false;
                ballIsServed = false;
                std::cout << "Ball hit the floor before being hit." << std::endl;
            }
        }
        else if (ballIsHit) {
            ballVelocity.y -= GRAVITY * deltaTime;
            ballPosition += ballVelocity * deltaTime;
            if (ballPosition.y < (floorLevel + ballRadius) ||
                abs(ballPosition.x) >(courtPlayingAreaW / 2.0f + 2.0f) ||
                abs(ballPosition.z) > (courtPlayingAreaL / 2.0f + 2.0f)) {
                std::cout << "Ball out of bounds or on the floor after hit." << std::endl;
            }
        }
        needsRedraw = true;
    }
    if (needsRedraw) { glutPostRedisplay(); }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shader_programme);

    float floorTopSurfaceY = 0.0f;
    float ceilingBottomSurfaceY = 10.0f;
    float wallVisualThickness = 0.2f;

    // --- Podea (Terenul Portocaliu) ---
    glm::mat4 courtModelMatrix = glm::mat4(1.0f);
    courtModelMatrix = glm::translate(courtModelMatrix, glm::vec3(0.0f, floorTopSurfaceY - courtPlayingAreaThickness / 2.0f, 0.0f));
    drawGeneratedCuboid(floorVao, floorIbo, floorIndices_data.size(), courtModelMatrix, courtOrangeColor);

    // --- Linii Teren ---
    float linesYCenterPosition = floorTopSurfaceY + LINE_THICKNESS_VISUAL_DIM / 2.0f; // Liniile putin deasupra terenului
    glm::mat4 lineModelMatrix;

    // Linii Laterale (paralele cu Z, pe lungimea terenului)
    lineModelMatrix = glm::mat4(1.0f);
    lineModelMatrix = glm::translate(lineModelMatrix, glm::vec3((courtPlayingAreaW - LINE_WIDTH_DIM) / 2.0f, linesYCenterPosition, 0.0f));
    lineModelMatrix = glm::scale(lineModelMatrix, glm::vec3(LINE_WIDTH_DIM, LINE_THICKNESS_VISUAL_DIM, courtPlayingAreaL));
    drawCube(lineModelMatrix, lineColor);
    lineModelMatrix = glm::mat4(1.0f);
    lineModelMatrix = glm::translate(lineModelMatrix, glm::vec3(-(courtPlayingAreaW - LINE_WIDTH_DIM) / 2.0f, linesYCenterPosition, 0.0f));
    lineModelMatrix = glm::scale(lineModelMatrix, glm::vec3(LINE_WIDTH_DIM, LINE_THICKNESS_VISUAL_DIM, courtPlayingAreaL));
    drawCube(lineModelMatrix, lineColor);

    // Linii de Fund (paralele cu X, pe latimea terenului)
    lineModelMatrix = glm::mat4(1.0f);
    lineModelMatrix = glm::translate(lineModelMatrix, glm::vec3(0.0f, linesYCenterPosition, (courtPlayingAreaL - LINE_WIDTH_DIM) / 2.0f));
    lineModelMatrix = glm::scale(lineModelMatrix, glm::vec3(courtPlayingAreaW, LINE_THICKNESS_VISUAL_DIM, LINE_WIDTH_DIM));
    drawCube(lineModelMatrix, lineColor);
    lineModelMatrix = glm::mat4(1.0f);
    lineModelMatrix = glm::translate(lineModelMatrix, glm::vec3(0.0f, linesYCenterPosition, -(courtPlayingAreaL - LINE_WIDTH_DIM) / 2.0f));
    lineModelMatrix = glm::scale(lineModelMatrix, glm::vec3(courtPlayingAreaW, LINE_THICKNESS_VISUAL_DIM, LINE_WIDTH_DIM));
    drawCube(lineModelMatrix, lineColor);

    // Linia de Centru (paralela cu X)
    lineModelMatrix = glm::mat4(1.0f);
    lineModelMatrix = glm::translate(lineModelMatrix, glm::vec3(0.0f, linesYCenterPosition, 0.0f));
    lineModelMatrix = glm::scale(lineModelMatrix, glm::vec3(courtPlayingAreaW, LINE_THICKNESS_VISUAL_DIM, LINE_WIDTH_DIM));
    drawCube(lineModelMatrix, lineColor);

    // Liniile de Atac (paralele cu X)
    float attackLineOffsetFromCenter = 3.0f;
    lineModelMatrix = glm::mat4(1.0f);
    lineModelMatrix = glm::translate(lineModelMatrix, glm::vec3(0.0f, linesYCenterPosition, attackLineOffsetFromCenter));
    lineModelMatrix = glm::scale(lineModelMatrix, glm::vec3(courtPlayingAreaW, LINE_THICKNESS_VISUAL_DIM, LINE_WIDTH_DIM));
    drawCube(lineModelMatrix, lineColor);
    lineModelMatrix = glm::mat4(1.0f);
    lineModelMatrix = glm::translate(lineModelMatrix, glm::vec3(0.0f, linesYCenterPosition, -attackLineOffsetFromCenter));
    lineModelMatrix = glm::scale(lineModelMatrix, glm::vec3(courtPlayingAreaW, LINE_THICKNESS_VISUAL_DIM, LINE_WIDTH_DIM));
    drawCube(lineModelMatrix, lineColor);

    // --- Tavan ---
    glm::mat4 ceilingModelMatrix = glm::mat4(1.0f);
    ceilingModelMatrix = glm::translate(ceilingModelMatrix, glm::vec3(0.0f, ceilingBottomSurfaceY + ceilingH_thickness / 2.0f, 0.0f));
    drawGeneratedCuboid(ceilingVao, ceilingIbo, ceilingIndices_data.size(), ceilingModelMatrix, ceilingColor);

    // --- Pereti (aliniati cu dimensiunile generale ale salii, nu doar cu terenul) ---
    float roomVisualHeight = ceilingBottomSurfaceY - floorTopSurfaceY;
    float wallCenterY = floorTopSurfaceY + (roomVisualHeight / 2.0f);

    // Perete Spate (Z negativ)
    glm::mat4 wallBackModelMatrix = glm::mat4(1.0f);
    wallBackModelMatrix = glm::translate(wallBackModelMatrix, glm::vec3(0.0f, wallCenterY, -overallRoomL / 2.0f));
    wallBackModelMatrix = glm::scale(wallBackModelMatrix, glm::vec3(overallRoomW, roomVisualHeight, wallVisualThickness));
    drawCube(wallBackModelMatrix, wallColor);

    // Perete Stanga (X negativ)
    glm::mat4 wallLeftModelMatrix = glm::mat4(1.0f);
    wallLeftModelMatrix = glm::translate(wallLeftModelMatrix, glm::vec3(-overallRoomW / 2.0f, wallCenterY, 0.0f));
    wallLeftModelMatrix = glm::scale(wallLeftModelMatrix, glm::vec3(wallVisualThickness, roomVisualHeight, overallRoomL));
    drawCube(wallLeftModelMatrix, wallColor);

    // Perete Dreapta (X pozitiv)
    glm::mat4 wallRightModelMatrix = glm::mat4(1.0f);
    wallRightModelMatrix = glm::translate(wallRightModelMatrix, glm::vec3(overallRoomW / 2.0f, wallCenterY, 0.0f));
    wallRightModelMatrix = glm::scale(wallRightModelMatrix, glm::vec3(wallVisualThickness, roomVisualHeight, overallRoomL));
    drawCube(wallRightModelMatrix, wallColor);

    // --- Fileu ---
    glm::mat4 netDisplayMatrix = glm::mat4(1.0f);
    netDisplayMatrix = glm::translate(netDisplayMatrix, glm::vec3(netPosition.x, floorTopSurfaceY + netDimensions.y / 2.0f, netPosition.z));
    netDisplayMatrix = glm::scale(netDisplayMatrix, netDimensions);
    drawCube(netDisplayMatrix, netColor);

    // --- Mingea ---
    if (ballIsServed || ballInToss || (!ballIsServed && !isAnimatingForward && !isAnimatingBackward)) {
        glm::mat4 ballModelMatrix = glm::mat4(1.0f);
        ballModelMatrix = glm::translate(ballModelMatrix, ballPosition);
        drawSphere(ballModelMatrix, ballColor);
    }

    // --- Mana ---
    glm::mat4 baseTransformMatrix = glm::mat4(1.0f);
    baseTransformMatrix = glm::translate(baseTransformMatrix, glm::vec3(playerBaseX, floorTopSurfaceY + playerBaseY, playerBaseZ));
    float initialArmYRotation = PI / 2.0f;
    baseTransformMatrix = glm::rotate(baseTransformMatrix, initialArmYRotation, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 currentMatrix = baseTransformMatrix;

    currentMatrix = glm::rotate(currentMatrix, currentShoulderY, glm::vec3(0.0f, 1.0f, 0.0f));
    currentMatrix = glm::rotate(currentMatrix, currentShoulderZ, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 elbowJointMatrix = glm::translate(currentMatrix, glm::vec3(armLength, 0.0f, 0.0f));
    glm::mat4 armDrawMatrix = glm::translate(currentMatrix, glm::vec3(armLength / 2.0f, 0.0f, 0.0f));
    drawCube(glm::scale(armDrawMatrix, glm::vec3(armLength, armWidth, armWidth)), handColor);

    currentMatrix = elbowJointMatrix;
    currentMatrix = glm::rotate(currentMatrix, currentElbow, glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 wristJointMatrix = glm::translate(currentMatrix, glm::vec3(forearmLength, 0.0f, 0.0f));
    glm::mat4 forearmDrawMatrix = glm::translate(currentMatrix, glm::vec3(forearmLength / 2.0f, 0.0f, 0.0f));
    drawCube(glm::scale(forearmDrawMatrix, glm::vec3(forearmLength, forearmWidth, forearmWidth)), handColor);

    currentMatrix = wristJointMatrix;
    currentMatrix = glm::rotate(currentMatrix, currentWristPronation, glm::vec3(1.0f, 0.0f, 0.0f));
    currentMatrix = glm::rotate(currentMatrix, currentWristZ, glm::vec3(0.0f, 0.0f, 1.0f));
    currentMatrix = glm::rotate(currentMatrix, currentWristY, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 palmCenterMatrix = glm::translate(currentMatrix, glm::vec3(palmLength / 2.0f, 0.0f, 0.0f));
    drawCube(glm::scale(palmCenterMatrix, glm::vec3(palmLength, palmWidth, palmDepth)), handColor);

    float fingerSpacing = palmWidth * 0.8f / 3.0f;
    float fingerStartXOffset = palmLength / 2.0f;
    float fingerStartYOffsetBase = (palmWidth / 2.0f) - (fingerSpacing * 0.5f) - fingerWidth / 2.0f;
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
    thumbMatrix = glm::rotate(thumbMatrix, currentThumbBaseZ, glm::vec3(0.0f, 0.0f, 1.0f));
    thumbMatrix = glm::rotate(thumbMatrix, currentThumbKnuckle, glm::vec3(0.0f, 0.0f, 1.0f));
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

    // VAO/VBO pentru cubul generic (mana, pereti, fileu, linii)
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // Sfera 
    generateSphere();
    glGenVertexArrays(1, &sphereVao); glBindVertexArray(sphereVao);
    glGenBuffers(1, &sphereVbo); glBindBuffer(GL_ARRAY_BUFFER, sphereVbo);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices_data.size() * sizeof(float), sphereVertices_data.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &sphereIbo); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices_data.size() * sizeof(unsigned int), sphereIndices_data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); glBindVertexArray(0);

    // Podea (terenul de joc)
    generateCuboidVertices(courtPlayingAreaW, courtPlayingAreaThickness, courtPlayingAreaL, floorVertices_data, floorIndices_data);
    glGenVertexArrays(1, &floorVao); glBindVertexArray(floorVao);
    glGenBuffers(1, &floorVbo); glBindBuffer(GL_ARRAY_BUFFER, floorVbo);
    glBufferData(GL_ARRAY_BUFFER, floorVertices_data.size() * sizeof(float), floorVertices_data.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &floorIbo); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, floorIndices_data.size() * sizeof(unsigned int), floorIndices_data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); glBindVertexArray(0);

    // Tavan
    generateCuboidVertices(overallRoomW, ceilingH_thickness, overallRoomL, ceilingVertices_data, ceilingIndices_data); // Folosim dimensiunile salii
    glGenVertexArrays(1, &ceilingVao); glBindVertexArray(ceilingVao);
    glGenBuffers(1, &ceilingVbo); glBindBuffer(GL_ARRAY_BUFFER, ceilingVbo);
    glBufferData(GL_ARRAY_BUFFER, ceilingVertices_data.size() * sizeof(float), ceilingVertices_data.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &ceilingIbo); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ceilingIbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ceilingIndices_data.size() * sizeof(unsigned int), ceilingIndices_data.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    std::string vstext = textFileRead("vertex.vert"); std::string fstext = textFileRead("fragment.frag"); if (vstext.empty() || fstext.empty()) { fprintf(stderr, "Eroare la citirea shaderelor.\n"); exit(1); }
    const char* vertex_shader_text = vstext.c_str(); const char* fragment_shader_text = fstext.c_str();
    GLuint vs = glCreateShader(GL_VERTEX_SHADER); glShaderSource(vs, 1, &vertex_shader_text, NULL); glCompileShader(vs); GLint success; GLchar infoLog[512]; glGetShaderiv(vs, GL_COMPILE_STATUS, &success); if (!success) { glGetShaderInfoLog(vs, 512, NULL, infoLog); std::cerr << "EROARE::SHADER::VERTEX::COMPILARE_ESUATA\n" << infoLog << std::endl; }
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER); glShaderSource(fs, 1, &fragment_shader_text, NULL); glCompileShader(fs); glGetShaderiv(fs, GL_COMPILE_STATUS, &success); if (!success) { glGetShaderInfoLog(fs, 512, NULL, infoLog); std::cerr << "EROARE::SHADER::FRAGMENT::COMPILARE_ESUATA\n" << infoLog << std::endl; }
    shader_programme = glCreateProgram(); glAttachShader(shader_programme, fs); glAttachShader(shader_programme, vs); glLinkProgram(shader_programme); glGetProgramiv(shader_programme, GL_LINK_STATUS, &success); if (!success) { glGetProgramInfoLog(shader_programme, 512, NULL, infoLog); std::cerr << "EROARE::SHADER::PROGRAM::LINKING_ESUAT\n" << infoLog << std::endl; }
    glDeleteShader(vs); glDeleteShader(fs);
    matrixID = glGetUniformLocation(shader_programme, "modelViewProjectionMatrix"); if (matrixID == -1) { fprintf(stderr, "Nu s-a putut gasi uniform-ul 'modelViewProjectionMatrix'\n"); }
    colorID = glGetUniformLocation(shader_programme, "objectColor"); if (colorID == -1) { fprintf(stderr, "Nu s-a putut gasi uniform-ul 'objectColor'\n"); }

    currentShoulderY = keyframePoses[0][0]; currentShoulderZ = keyframePoses[0][1]; currentElbow = keyframePoses[0][2];
    currentWristPronation = keyframePoses[0][3]; currentWristZ = keyframePoses[0][4]; currentWristY = keyframePoses[0][5];
    currentFingerKnuckle = keyframePoses[0][6]; currentFingerMid = keyframePoses[0][7];
    currentThumbBaseZ = keyframePoses[0][8]; currentThumbKnuckle = keyframePoses[0][9]; currentThumbMid = keyframePoses[0][10];
    ballPosition = glm::vec3(playerBaseX, (0.0f + playerBaseY) + 0.5f, playerBaseZ - 1.0f); // Pozitie initiala minge
}

void keyboard(unsigned char key, int x, int y) {
    float angleStep = 0.05f; float fingerStep = 0.08f; bool manualControl = false;
    switch (key) {
    case 'j':
        if (!ballIsServed) {
            isAnimatingForward = true; isAnimatingBackward = false; manualControl = false;
            animationProgress = 0.0f;
            ballIsServed = true; ballInToss = true; ballIsHit = false;
            ballPosition = glm::vec3(playerBaseX - 0.2f, (0.0f + playerBaseY) + 2.0f, playerBaseZ - 1.5f);
            ballVelocity = glm::vec3(0.1f, 7.0f, -1.0f);
            std::cout << "Serve initiated. Ball toss." << std::endl;
        }
        break;
    case 'k': isAnimatingBackward = true; isAnimatingForward = false; manualControl = false; break;
    case ' ':
        isAnimatingForward = false; isAnimatingBackward = false; animationProgress = 0.0f;
        ballIsServed = false; ballInToss = false; ballIsHit = false;
        ballPosition = glm::vec3(playerBaseX, (0.0f + playerBaseY) + 0.5f, playerBaseZ - 0.5f);
        ballVelocity = glm::vec3(0.0f);
        currentShoulderY = keyframePoses[0][0]; currentShoulderZ = keyframePoses[0][1]; currentElbow = keyframePoses[0][2];
        currentWristPronation = keyframePoses[0][3]; currentWristZ = keyframePoses[0][4]; currentWristY = keyframePoses[0][5];
        currentFingerKnuckle = keyframePoses[0][6]; currentFingerMid = keyframePoses[0][7];
        currentThumbBaseZ = keyframePoses[0][8]; currentThumbKnuckle = keyframePoses[0][9]; currentThumbMid = keyframePoses[0][10];
        manualControl = false;
        std::cout << "Animation Reset." << std::endl;
        glutPostRedisplay();
        break;
    case 's': shoulderAngleZ += angleStep; manualControl = true; break; case 'w': shoulderAngleZ -= angleStep; manualControl = true; break;
    case 'a': shoulderAngleY += angleStep; manualControl = true; break; case 'd': shoulderAngleY -= angleStep; manualControl = true; break;
    case 'e': elbowAngle += angleStep; manualControl = true; break; case 'r': elbowAngle -= angleStep; manualControl = true; break;
    case 27: glutLeaveMainLoop(); break; default: break;
    }
    if (manualControl) {
        isAnimatingForward = false; isAnimatingBackward = false;
        currentShoulderY = shoulderAngleY; currentShoulderZ = shoulderAngleZ; currentElbow = elbowAngle;
        currentWristPronation = wristPronationAngle; currentWristZ = wristAngleZ; currentWristY = wristAngleY;
        currentThumbBaseZ = thumbBaseAngleZ;
        currentFingerKnuckle = fingerAngles[0][0]; currentFingerMid = fingerAngles[0][1];
        currentThumbKnuckle = fingerAngles[4][0]; currentThumbMid = fingerAngles[4][1];
        glutPostRedisplay();
    }
}

void reshape(int w, int h) {
    if (h == 0) h = 1; glViewport(0, 0, w, h);
    projectionMatrix = glm::perspective(glm::radians(50.0f), (float)w / h, 0.1f, 150.0f);

    float cameraActualY = (0.0f + playerBaseY) + 2.0f;
    float cameraX = playerBaseX;
    float cameraY = cameraActualY;
    float cameraZ = playerBaseZ + 10.0f;

    float lookAtX = playerBaseX;
    float lookAtY = cameraActualY - 1.0f;
    float lookAtZ = 0.0f;

    viewMatrix = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ),
        glm::vec3(lookAtX, lookAtY, lookAtZ),
        glm::vec3(0.0f, 1.0f, 0.0f));
}

int main(int argc, char** argv) {
    glutInit(&argc, argv); glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutCreateWindow("Serviciu Volei OpenGL - Teren Volei v2");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(update);
    lastUpdateTime = glutGet(GLUT_ELAPSED_TIME);
    glutMainLoop();

    glDeleteProgram(shader_programme);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &sphereVao);
    glDeleteBuffers(1, &sphereVbo);
    glDeleteBuffers(1, &sphereIbo);
    glDeleteVertexArrays(1, &floorVao);
    glDeleteBuffers(1, &floorVbo);
    glDeleteBuffers(1, &floorIbo);
    glDeleteVertexArrays(1, &ceilingVao);
    glDeleteBuffers(1, &ceilingVbo);
    glDeleteBuffers(1, &ceilingIbo);
    return 0;
}