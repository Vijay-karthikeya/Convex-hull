#include <iostream>
#include <vector>

#include <GL/glut.h>
#include <GL/GLU.h> 
#include <GL/GL.h>

using namespace std;
// Code for purely graphics starts here
void display();
void drawpoints();
void reshape(int, int); 
void timer(int);

void init() {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_DEPTH_TEST);
}

struct colorpick {
    float x, y, z;
};

//graphics code ends

//Algorithm functions and structs start here

struct point3d {
    long x, y, z;
    point3d(long x = 0, long y = 0, long z = 0) : x(x), y(y), z(z) {}
    point3d operator-(const point3d& o) const {
        return point3d(x - o.x, y - o.y, z - o.z);
    }
    point3d cross(const point3d& o) const {
        return point3d(y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x);
    }
    long dot(const point3d& o) const {
        return x * o.x + y * o.y + z * o.z;
    }
};

// Face struct stores the indices of its vertices a,b,c
// It also stores an outward-facing normal vector norm
struct face {
    int a, b, c;
    point3d norm;
};

// finds area of triangle formed using three vertices. This is used for initialising the first face.
int area(point3d p1, point3d p2, point3d p3) {
    point3d ans = (p2 - p1).cross(p3 - p1);
    int a = ans.x;
    int b = ans.y;
    int c = ans.z;
    int area = a * a + b * b + c * c;
    return area;
}

vector<face> convexhull(vector<point3d> p) {
    int n = p.size();
    vector<face> f;

    // Consider an edge (a->b) visible if it is not a CCW edge of some current face
    // If an edge is alive but not its reverse, this is an exposed edge.
    // We should add new faces on the exposed edges.
    vector<vector<bool>> visible(n, vector<bool>(n, true));

    auto add_face = [&](int a, int b, int c) {
        f.push_back({ a, b, c, (p[b] - p[a]).cross(p[c] - p[a]) });
        visible[a][b] = visible[b][c] = visible[c][a] = false;
    };

    /* [&] means capture all variables within scope by reference then add a new face to list of faces f by adding the vertices
    and also we store the normal in the face struct format designed*/

    // Initialize the convex hull of the first 3 points as a
    // triangular disk with two faces of opposite orientation
    // We initialise the convex hull by making a triangle and setting the faces as not visible

    for (int i = 2;i < n;i++) {
        if (area(p[0], p[1], p[i]) != 0) {
            add_face(0, 1, i); 
            add_face(i, 1, 0); 
            break;
        }
    }

    for (int i = 2;i < n;i++) {
        // f2 will be the list of faces invisible to the added point p[i]
        vector<face> f2;
        for (auto& F : f) {
            if ((p[i] - p[F.a]).dot(F.norm) > 0) {
                // this face is visible to the new point, so mark its edges as visible
                visible[F.a][F.b] = visible[F.b][F.c] = visible[F.c][F.a] = true;
            }
            else {
                f2.push_back(F);
            }
        }
        // Add a new face for each exposed edge.
        // Only check edges of alive faces for being exposed.
        f.clear();
        for (auto& F : f2) {
            int arr[3] = { F.a, F.b, F.c };
            for (int j = 0;j < 3; j++) {
                int a = arr[j], b = arr[(j+1)%3]; //By doing this the values go as (1,2);(2,3);(3,1) making it anti-clockwise cyclic
                if (visible[b][a]) {
                    add_face(b, a, i);
                }
            }
            f.push_back(F);
        }
    }
    return f;
}

//Created to alternate through colors for distinction of faces
vector<colorpick> colourpicker(int a) {
    vector<colorpick> clr; 
    if (a % 15 == 0) clr.push_back({ 1.0,0.0,0.0 });
    else if (a % 15 == 1) clr.push_back({ 1.0,1.0,0.0 }); 
    else if (a % 15 == 2) clr.push_back({ 0.0,1.0,0.0 }); 
    else if (a % 15 == 3) clr.push_back({ 0.0,0.0,1.0 }); 
    else if (a % 15 == 4) clr.push_back({ 1.0,0.5,0.0 }); 
    else if (a % 15 == 5) clr.push_back({ 0.5,0.0,1.0 }); 
    else if (a % 15 == 6) clr.push_back({ 0.0,1.0,1.0 }); 
    else if (a % 15 == 7) clr.push_back({ 0.5,1.0,0.0 }); 
    else if (a % 15 == 8) clr.push_back({ 0.5,1.0,0.5 }); 
    else if (a % 15 == 9) clr.push_back({ 0.14,0.14,0.55 }); 
    else if (a % 15 == 10) clr.push_back({ 0.74,0.56,0.56 }); 
    else if (a % 15 == 11) clr.push_back({ 1.0,0.25,0.0 }); 
    else if (a % 15 == 12) clr.push_back({ 0.3,0.18,0.3 }); 
    else if (a % 15 == 13) clr.push_back({ 0.72,0.45,0.20 }); 
    else clr.push_back({ 0.75,0.75,0.75 }); 
    return clr;
}


vector<point3d> hull;
vector<face> v;
int m1, m2, m3;
int main(int argc,char **argv) {
    int n;
    int r,m;

    cout << "Enter number of points:" << endl;
    cin >> n;
    cout << "For points enter 0 or for the convex hull enter 1: " << endl;
    cin >> r;
    cout << "Enter the mode of rotation(1/2/3/4/5): " << endl;
    cin >> m;
    cout << "Enter the points: " << endl;

    for (int i = 0;i < n;i++) {
        int a1, a2, a3;
        cin >> a1 >> a2 >> a3;
        hull.push_back({ a1,a2,a3 });
    }

    // This is for rotation
    if (m == 1) {
        m1 = 1.0;
        m2 = 1.0;
        m3 = 0.0;
    }
    else if (m == 2) {
        m1 = 0.0;
        m2 = 1.0;
        m3 = 1.0;
    }
    else if (m == 3) {
        m1 = 1.0;
        m2 = 0.0;
        m3 = 1.0;
    }
    else if (m==4) {
        m1 = 1.0;
        m2 = 1.0;
        m3 = 1.0;
    }
    else {
        m1 = -3.0;
        m2 = 2.0;
        m3 = -1.0;
    }

    v = convexhull(hull);
    if (r == 1)
    {
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
        glutInitWindowPosition(200, 100);
        glutInitWindowSize(600, 600);
        glutCreateWindow("hull");
        glutDisplayFunc(display);
        glutReshapeFunc(reshape);
        glutTimerFunc(0, timer, 0);
        init();
        glutMainLoop();
    }
    else
    {
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
        glutInitWindowPosition(200, 100);
        glutInitWindowSize(600, 600);
        glutCreateWindow("points");
        glutDisplayFunc(drawpoints);
        glutReshapeFunc(reshape);
        glutTimerFunc(0, timer, 0);
        init();
        glutMainLoop();
    }
}
float angle = 360.0;

void drawpoints(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -10.0);
    glRotatef(angle, m1, m2, m3);
    glPointSize(5.0);

    glBegin(GL_POINTS);
    for (int i = 0;i < hull.size();i++) {
        vector<colorpick> cp = colourpicker(i);
        glColor3f(cp[0].x,cp[0].y, cp[0].z);
        glVertex3f((hull[i].x*1.0/10), (hull[i].y*1.0/10), (hull[i].z*1.0/10));
    }
    glEnd();

    glutSwapBuffers();
}


void display()
{
	glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
    glTranslatef(0.0, 0.0, -10.0);
	glRotatef(angle, m1, m2, m3);
    
	glBegin(GL_TRIANGLES);
    for (int i = 0;i < v.size();i++) {
        vector<colorpick> cp = colourpicker(i);
        glColor3f(cp[0].x, cp[0].y, cp[0].z);
        glVertex3f((hull[v[i].a].x * 1.0 / 10), (hull[v[i].a].y * 1.0 / 10), (hull[v[i].a].z * 1.0 / 10));
        glVertex3f((hull[v[i].b].x * 1.0 / 10), (hull[v[i].b].y * 1.0 / 10), (hull[v[i].b].z * 1.0 / 10));
        glVertex3f((hull[v[i].c].x * 1.0 / 10), (hull[v[i].c].y * 1.0 / 10), (hull[v[i].c].z * 1.0 / 10));
    }
    glEnd();

	glutSwapBuffers();
}


void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

    gluPerspective(60, 2, 2.0, 50.0); //To view better in fullscreen change second argument to 2

	glMatrixMode(GL_MODELVIEW);
}

void timer(int)
{
    glutPostRedisplay();
    glutTimerFunc(50/3, timer, 0);
    angle -=0.8; 
    if (angle < 360.0) angle = angle + 360.0;
}
