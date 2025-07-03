#include <iostream>
#include <array>
#include "Points.h"
#include "Con_algo.h"
#include <GL/glut.h>
#include <GL/GLU.h>
#include <GL/GL.h>

using namespace std;

void display();
void drawpoints();
void reshape(int, int);
void timer(int);
void mouseButton(int button, int state, int x, int y);
void mouseMotion(int x, int y);

void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);
}

struct color {
    float x, y, z;
};

constexpr array<color, 15> color_palette = { {
    {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f},
    {0.0f, 0.0f, 1.0f}, {1.0f, 0.5f, 0.0f}, {0.5f, 0.0f, 1.0f},
    {0.0f, 1.0f, 1.0f}, {0.5f, 1.0f, 0.0f}, {0.5f, 1.0f, 0.5f},
    {0.14f, 0.14f, 0.55f}, {0.74f, 0.56f, 0.56f}, {1.0f, 0.25f, 0.0f},
    {0.3f, 0.18f, 0.3f}, {0.72f, 0.45f, 0.20f}, {0.75f, 0.75f, 0.75f}
} };

constexpr color colourpicker(int a) {
    return color_palette[a % color_palette.size()];
}

vector<Point3d> hull;
vector<face> v;

float m1, m2, m3;
float angle = 360.0;

// Mouse interaction state
int lastX = 0, lastY = 0;
bool leftButtonPressed = false;
float rotX = 0.0f, rotY = 0.0f;

int main(int argc, char** argv) {
    int n, r, m;
    cout << "Enter number of points: ";
    cin >> n;
    cout << "For points enter 0 or for the convex hull enter 1: ";
    cin >> r;
    cout << "Enter the mode of rotation (1/2/3/4/5): ";
    cin >> m;
    cout << "Enter the points: " << endl;
    for (int i = 0; i < n; i++) {
        int a1, a2, a3;
        cin >> a1 >> a2 >> a3;
        hull.push_back({ a1, a2, a3 });
    }
    cout << "Use the mouse button to scroll around" << '\n';
    switch (m) {
    case 1: m1 = 1.0f; m2 = 1.0f; m3 = 0.0f; break;
    case 2: m1 = 0.0f; m2 = 1.0f; m3 = 1.0f; break;
    case 3: m1 = 1.0f; m2 = 0.0f; m3 = 1.0f; break;
    case 4: m1 = 1.0f; m2 = 1.0f; m3 = 1.0f; break;
    default: m1 = -3.0f; m2 = 2.0f; m3 = -1.0f;
    }

    v = convexhull3d(hull);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(200, 100);
    glutInitWindowSize(600, 600);
    glutCreateWindow(r == 1 ? "Convex Hull" : "Points");

    glutDisplayFunc(r == 1 ? display : drawpoints);
    glutReshapeFunc(reshape);
    glutTimerFunc(0, timer, 0);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    init();
    glutMainLoop();
}

void applyRotation() {
    glRotatef(rotX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);
    glRotatef(angle, m1, m2, m3);  // Optional: comment this to disable auto spin
}

void drawpoints() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -10.0);
    applyRotation();

    glPointSize(5.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < hull.size(); i++) {
        color cp = colourpicker(i);
        glColor3f(cp.x, cp.y, cp.z);
        glVertex3f(hull[i].x / 10.0f, hull[i].y / 10.0f, hull[i].z / 10.0f);
    }
    glEnd();
    glutSwapBuffers();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -10.0);
    applyRotation();

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < v.size(); i++) {
        color cp = colourpicker(i);
        glColor3f(cp.x, cp.y, cp.z);
        glVertex3f(hull[v[i].a].x / 10.0f, hull[v[i].a].y / 10.0f, hull[v[i].a].z / 10.0f);
        glVertex3f(hull[v[i].b].x / 10.0f, hull[v[i].b].y / 10.0f, hull[v[i].b].z / 10.0f);
        glVertex3f(hull[v[i].c].x / 10.0f, hull[v[i].c].y / 10.0f, hull[v[i].c].z / 10.0f);
    }
    glEnd();
    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, static_cast<float>(w) / h, 1.0, 50.0);
    glMatrixMode(GL_MODELVIEW);
}

void timer(int) {
    angle -= 0.8f;
    if (angle < 360.0f) angle += 360.0f;
    glutPostRedisplay();
    glutTimerFunc(50 / 3, timer, 0);
}

void mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        leftButtonPressed = (state == GLUT_DOWN);
        lastX = x;
        lastY = y;
    }
}

void mouseMotion(int x, int y) {
    if (leftButtonPressed) {
        rotY += (x - lastX);
        rotX += (y - lastY);
        lastX = x;
        lastY = y;
        glutPostRedisplay();
    }
}
