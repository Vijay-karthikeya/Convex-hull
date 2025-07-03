#include <iostream>
#include <vector>
#include <array>
#include <GL/glut.h>
#include "Points.h"
#include "Con_algo.h"

using namespace std;

struct color { float x, y, z; };
constexpr array<color, 15> color_palette = { {
    {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f},
    {1.0f, 0.5f, 0.0f}, {0.5f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f}, {0.5f, 1.0f, 0.0f},
    {0.5f, 1.0f, 0.5f}, {0.14f, 0.14f, 0.55f}, {0.74f, 0.56f, 0.56f}, {1.0f, 0.25f, 0.0f},
    {0.3f, 0.18f, 0.3f}, {0.72f, 0.45f, 0.20f}, {0.75f, 0.75f, 0.75f}
} };
constexpr color colourpicker(int a) { return color_palette[a % color_palette.size()]; }

vector<Point3d> hull3d;
vector<face> faces3d;
vector<Point2d> hull2d;
vector<Side> edges2d;
int mode = 0; // 0 = points, 1 = 3d hull, 2 = 2d hull

float rotateX = 0.0f, rotateY = 0.0f;
int prevMouseX = -1, prevMouseY = -1;
bool isDragging = false;

void display3D();
void display2D();
void displayPoints3D();
void reshape3D(int, int);
void reshape2D(int, int);
void timer(int);
void mouse(int, int, int, int);
void motion(int, int);

void init() {
    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);
}

int main(int argc, char** argv) {
    int n, choice;
    cout << "Enter number of points: "; cin >> n;
    cout << "Choose mode: 0 = points, 1 = 3D convex hull, 2 = 2D convex hull: "; cin >> mode;

    if (mode == 2) {
        cout << "Enter 2D points (x y):\n";
        for (int i = 0; i < n; ++i) {
            float x, y; cin >> x >> y;
            hull2d.emplace_back(x, y);
        }
        edges2d = convexHull2d(hull2d);
    }
    else {
        cout << "Enter 3D points (x y z):\n";
        for (int i = 0; i < n; ++i) {
            float x, y, z; cin >> x >> y >> z;
            hull3d.emplace_back(x, y, z);
        }
        if (mode == 1) faces3d = convexhull3d(hull3d);
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Convex Hull Visualizer");

    if (mode == 2) {
        glutDisplayFunc(display2D);
        glutReshapeFunc(reshape2D);
    }
    else if (mode == 1) {
        glutDisplayFunc(display3D);
        glutReshapeFunc(reshape3D);
        glutMouseFunc(mouse);
        glutMotionFunc(motion);
    }
    else {
        glutDisplayFunc(displayPoints3D);
        glutReshapeFunc(reshape3D);
        glutMouseFunc(mouse);
        glutMotionFunc(motion);
    }

    glutTimerFunc(0, timer, 0);
    init();
    glutMainLoop();
    return 0;
}

void display3D() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -10.0f);
    glRotatef(rotateX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    for (int i = 0; i < faces3d.size(); ++i) {
        color c = colourpicker(i);
        glColor3f(c.x, c.y, c.z);
        for (int j : {faces3d[i].a, faces3d[i].b, faces3d[i].c}) {
            Point3d& p = hull3d[j];
            glVertex3f(p.x * 0.1f, p.y * 0.1f, p.z * 0.1f);
        }
    }
    glEnd();
    glutSwapBuffers();
}

void displayPoints3D() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -10.0f);
    glRotatef(rotateX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotateY, 0.0f, 1.0f, 0.0f);
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < hull3d.size(); ++i) {
        color c = colourpicker(i);
        glColor3f(c.x, c.y, c.z);
        glVertex3f(hull3d[i].x * 0.1f, hull3d[i].y * 0.1f, hull3d[i].z * 0.1f);
    }
    glEnd();
    glutSwapBuffers();
}

void display2D() {
    static size_t current = 0;
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glBegin(GL_POLYGON);
    for (size_t i = 0; i < min(current, edges2d.size()); ++i) {
        color c = colourpicker(i);
        glColor3f(c.x, c.y, c.z);
        glVertex2f(edges2d[i].p1.x * 0.1f, edges2d[i].p1.y * 0.1f);
        glVertex2f(edges2d[i].p2.x * 0.1f, edges2d[i].p2.y * 0.1f);
    }
    glEnd();
    glutSwapBuffers();
    if (current < edges2d.size()) ++current;
}

void reshape3D(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w / h, 1.0, 50.0);
    glMatrixMode(GL_MODELVIEW);
}

void reshape2D(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
}

void timer(int) {
    glutPostRedisplay();
    glutTimerFunc(50, timer, 0);
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        isDragging = (state == GLUT_DOWN);
        prevMouseX = x;
        prevMouseY = y;
    }
}

void motion(int x, int y) {
    if (isDragging) {
        rotateY += (x - prevMouseX);
        rotateX += (y - prevMouseY);
        prevMouseX = x;
        prevMouseY = y;
        glutPostRedisplay();
    }
}
