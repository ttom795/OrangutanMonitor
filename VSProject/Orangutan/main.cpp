#include <windows.h>
#include <iostream>
#include <string>
#include <GL/glut.h>
#include "getPage.h"

using namespace std;
MEMORYSTATUSEX statex;
float pageCurrent;
int pageTotal;
bool preciseMode = false;
float buttonColorY[3] = { 0, 0.75, 0 };
float buttonColorN[3] = { 0.75, 0, 0 };
float buttonX = 0.7, buttonY = 0.0, buttonSize = 0.2;

float lerp(float a, float b, float t) {
    return a + t * (b - a);
}
float remap(float value, float low1, float high1, float low2, float high2) {
    return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

bool checkOverlap(float pointX, float pointY, float objectX, float objectY, float objectSize) {
    return (pointY <= objectY+objectSize && pointY >= objectY &&
        pointX <= objectX+objectSize && pointX >= objectX);
}
void mouseFunc(int button, int state, int x, int y) {
    float xPer = (x / 256.0);
    float yPer = (256.0 - y) / 256.0;
    float startX = buttonX/2+0.5;
    float startY = buttonY/2+0.5;

    if (checkOverlap(xPer, yPer, startX,startY, buttonSize/2) && state == 0) {
        preciseMode = !preciseMode;
    }
}

void idle() {
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);
    pageCurrent = (statex.ullTotalPageFile - statex.ullTotalPhys) / (1024 * 1024);
}
void drawLine(float startX, float startY, float endX, float endY) {
    glBegin(GL_LINES);
    glVertex2f(startX, startY);
    glVertex2f(endX, endY);
    glEnd();
}
void drawBar(float startX, float startY, float width, float value) {
    glBegin(GL_QUADS);
    glVertex2f(startX, startY);
    glVertex2f(startX + width, startY);
    glVertex2f(startX + width, value);
    glVertex2f(startX, value);
    glEnd();
}
void drawString(float x, float y, string name) {
    glPushMatrix();
    glRasterPos2f(x, y);
    const char* cvert = name.c_str();
    for (int i = 0; i < name.size(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, cvert[i]);
    }
    glPopMatrix();
}

float bar, current;
void display()
{
    current = lerp(current, pageCurrent, 0.1);
    bar = lerp(bar, (current/pageTotal), 0.1);
    float tempBar = remap(bar, 0, 1, -0.7, 0.8);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1, 1, 1);
    drawLine(-0.85, 0.8, -0.55, 0.8);
    drawLine(-0.85, 0.05, -0.55, 0.05);
    drawLine(-0.85, -0.7, -0.55, -0.7);
    drawBar(-0.8, -0.7, 0.2, tempBar);
    drawString(-0.85, 0.85, ("Max Page Size: "+to_string((int)pageTotal)+"MB"));
    drawString(-0.50, 0.05, ("50%"));

    if (preciseMode) {
        drawString(-0.8, -0.8, (to_string(current) + "/" + to_string(pageTotal)));
        drawString(-0.8, -0.9, (to_string(current / pageTotal * 100)) + "%");
        glColor3f(buttonColorY[0], buttonColorY[1], buttonColorY[2]);
    }
    else {
        drawString(-0.8, -0.8, (to_string((int)round(current)) + "/" + to_string((int)round(pageTotal))));
        drawString(-0.8, -0.9, (to_string((int)(round(current / pageTotal * 100))) + "%"));
        glColor3f(buttonColorN[0], buttonColorN[1], buttonColorN[2]);
    }
    glRectf(buttonX, buttonY, buttonX+buttonSize, buttonY+buttonSize);
    drawString(-0.15, 0.05, "Detail Mode");

    glutSwapBuffers();
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(256, 256);
    glOrtho(0, 256, 256, 0, -1, 1);
    glutCreateWindow("Orangutan");
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    getPage* p = new getPage();
    pageTotal = p->getMaxSize();
    FreeConsole();
    glutMouseFunc(mouseFunc);
    glutMainLoop();
    return 0;
}