#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

using namespace std;

// Globals.
static float latAngle = 0.0; // Latitudinal angle of moon.
static float longAngle = 0.0; // Longitudinal angle of moon.
static int viewMap = 0;
const static int animationPeriod = 50; // Time interval between frames.
const static int slices = 50;

class Planet {
public:
    Planet();

    Planet(float distance, float radius, unsigned char r, unsigned char g, unsigned char b);

    float getDistance() { return distance; }

    float getAngle() { return angle; }

    float getRadius() { return radius; }

    void draw();

    void update();

    void drawOrbit();

private:
    float distance, angle, radius, angVelocity;
    float color[4];
};

Planet::Planet() {}

Planet::Planet(float distance, float radius, unsigned char r, unsigned char g, unsigned char b) {
    this->distance = distance;
    this->radius = radius;
    this->angle = rand() % 360;
    color[0] = r / 255.0;
    color[1] = g / 255.0;
    color[2] = b / 255.0;
    color[3] = 1.0;
    angVelocity = (8 - (distance - 8) / 8) / 4;
}

void Planet::draw() {
    glMaterialfv(GL_FRONT, GL_AMBIENT, color);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, color);

    glPushMatrix();
    glRotatef(angle, 0.0, 0.0, 1.0);
    glTranslatef(distance, 0.0, 0.0);
    glutSolidSphere(radius, slices, slices);
    glPopMatrix();
}

void Planet::update() {
    angle += angVelocity;
    if (angle > 360.0) angle -= 360.0;
}

void Planet::drawOrbit() {
    glDisable(GL_LIGHTING);
    glBegin(GL_LINE_LOOP);
    glColor3f(0.5, 0.5, 0.5);
    for (int i = 0; i <= slices; i++) {
        glVertex3f(distance * cos((float) i / slices * M_PI * 2), distance * sin((float) i / slices * M_PI * 2), 0);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

Planet planet[8];

class Spacecraft {
public:
    Spacecraft();

    Spacecraft(float x, float y, float z, float angleA, float angleB);

    void draw();

    float getX() { return x; }

    float getY() { return y; }

    float getZ() { return z; }

    float getAngleA() { return angleA; }

    float getAngleB() { return angleB; }

    void rotateCCW();

    void rotateCW();

    void rotateUp();

    void rotateDown();

    void forward();

    void backward();

private:
    float x, y, z, angleA, angleB;
};

Spacecraft::Spacecraft() {}

Spacecraft::Spacecraft(float x, float y, float z, float angleA, float angleB) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->angleA = angleA;
    this->angleB = angleB;
}

void Spacecraft::draw() {
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(angleA, 0.0, 0.0, 1.0);
    glRotatef(angleB, 0.0, 1.0, 0.0);
    glRotatef(90.0, 0.0, 1.0, 0.0);
    glColor3f(1.0, 1.0, 1.0);
    glutSolidCone(1.0, 3.0, slices, slices);
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void Spacecraft::rotateCCW() {
    angleA += 5.0;
    if (angleA > 360.0) angleA -= 360.0;
}

void Spacecraft::rotateCW() {
    angleA -= 5.0;
    if (angleA < 0.0) angleA += 360.0;
}

void Spacecraft::forward() {
    x += cos(angleB * M_PI / 180.0) * cos(angleA * M_PI / 180.0);
    y += cos(angleB * M_PI / 180.0) * sin(angleA * M_PI / 180.0);
    z += sin(angleB * M_PI / 180.0);
}

void Spacecraft::backward() {
    x -= cos(angleB * M_PI / 180.0) * cos(angleA * M_PI / 180.0);
    y -= cos(angleB * M_PI / 180.0) * sin(angleA * M_PI / 180.0);
    z -= sin(angleB * M_PI / 180.0);
}

void Spacecraft::rotateUp() {
    angleB += 5.0;
    if (angleB > 360.0) angleB -= 360.0;
}

void Spacecraft::rotateDown() {
    angleB -= 5.0;
    if (angleB < 0.0) angleB += 360.0;
}

Spacecraft spacecraft = Spacecraft(-60, -60, 0, 45, 0);

void drawSolarSystem() {
    float sunPos[] = {0.0, 0.0, 0.0, 1.0};
    float sunReflection[] = {0.0, 0.0, 0.0, 1.0};
    float sunEmission[] = {253 / 255.0, 184 / 255.0, 19 / 255.0, 1.0};

    float plSpec[] = {0.0, 0.0, 0.0, 1.0};
    float plEmission[] = {0.0, 0.0, 0.0, 1.0};

    float moonAmbDif[] = {1.0, 1.0, 1.0, 1.0};

    // Material properties of sun.
    glMaterialfv(GL_FRONT, GL_AMBIENT, sunReflection);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, sunReflection);
    glMaterialfv(GL_FRONT, GL_SPECULAR, sunReflection);
    glMaterialfv(GL_FRONT, GL_EMISSION, sunEmission);

    // Light0 and its sphere positioned.
    glPushMatrix();
    glLightfv(GL_LIGHT0, GL_POSITION, sunPos);
    glutSolidSphere(5.5, slices, slices);
    glPopMatrix();

    // Material properties of planets.
    glMaterialfv(GL_FRONT, GL_SPECULAR, plSpec);
    glMaterialfv(GL_FRONT, GL_EMISSION, plEmission);

    // Draw Planets.
    for (int i = 0; i < 8; i++) {
        planet[i].draw();
        planet[i].drawOrbit();
        if (i == 5) {
            glPushMatrix();
            glRotatef(planet[i].getAngle(), 0.0, 0.0, 1.0);
            glTranslatef(planet[i].getDistance(), 0.0, 0.0);
            glRotatef(-20, 0.0, 1.0, 0.0);
            glutSolidTorus(0.25, planet[i].getRadius() + 0.5, slices, slices);
            glPopMatrix();
        }
    }

    // Material properties of moon.
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, moonAmbDif);

    // Draw moon.
    glPushMatrix();
    glRotatef(planet[2].getAngle(), 0.0, 0.0, 1.0);
    glTranslatef(planet[2].getDistance(), 0.0, 0.0);
    glRotatef(latAngle, 0.0, 1.0, 0.0);
    glTranslatef(2, 0, 0);
    glutSolidSphere(.5, slices, slices);
    glPopMatrix();
}

// Drawing routine.
void drawScene(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, 600, 600);

    if (!viewMap) {
        glLoadIdentity();

        float x = spacecraft.getX();
        float y = spacecraft.getY();
        float z = spacecraft.getZ();
        float angleA = spacecraft.getAngleA();
        float angleB = spacecraft.getAngleB();
        gluLookAt(x, y, z,
                  x + cos(angleB * M_PI / 180.0) * cos(angleA * M_PI / 180.0),
                  y + cos(angleB * M_PI / 180.0) * sin(angleA * M_PI / 180.0),
                  z + sin(angleB * M_PI / 180.0),
                  0, 0, cos(angleB * M_PI / 180.0));

        drawSolarSystem();

        glViewport(400, 0, 200, 200);
    }

    glLoadIdentity();
    gluLookAt(0, 0, 70, 0, 0, 0, 0, 1, 0);

    drawSolarSystem();

    // Draw Spacecraft.
    spacecraft.draw();

    glutSwapBuffers();
}

// Timer function.
void animate(int value) {
    latAngle += 5.0;
    if (latAngle > 360.0) latAngle -= 360.0;
    longAngle += 1.0;
    if (longAngle > 360.0) longAngle -= 360.0;
    for (int i = 0; i < 8; i++) {
        planet[i].update();
    }

    glutPostRedisplay();
    glutTimerFunc(animationPeriod, animate, 0);
}

// Initialization routine.
void setup(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST); // Enable depth testing.

    // Turn on OpenGL lighting.
    glEnable(GL_LIGHTING);

    // Light property vectors.
    float lightAmb[] = {0.2, 0.2, 0.2, 1.0};
    float lightDifAndSpec[] = {1.0, 1.0, 1.0, 1.0};

    // Light0 properties.
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec);

    glEnable(GL_LIGHT0); // Enable particular light source.
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); // Enable local viewpoint

    // Cull back faces.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Initialize planets.
    planet[0] = Planet(8, 1, 183, 184, 185);
    planet[1] = Planet(16, 1.5, 227, 158, 28);
    planet[2] = Planet(24, 1.5, 0, 66, 129);
    planet[3] = Planet(32, 1, 156, 46, 53);
    planet[4] = Planet(40, 3.5, 206, 165, 137);
    planet[5] = Planet(48, 3.0, 205, 160, 86);
    planet[6] = Planet(56, 2.5, 147, 184, 190);
    planet[7] = Planet(64, 2.5, 63, 84, 186);

    animate(0);
}

// OpenGL window reshape routine.
void resize(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y) {
    switch (key) {
        case 27:
            exit(0);
        case 'w':
            spacecraft.forward();
            glutPostRedisplay();
            break;
        case 's':
            spacecraft.backward();
            glutPostRedisplay();
            break;
        case 'm':
            viewMap = 1 - viewMap;
            glutPostRedisplay();
            break;
        default:
            break;
    }
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y) {
    if (key == GLUT_KEY_UP) spacecraft.rotateUp();
    if (key == GLUT_KEY_DOWN) spacecraft.rotateDown();
    if (key == GLUT_KEY_LEFT) spacecraft.rotateCCW();
    if (key == GLUT_KEY_RIGHT) spacecraft.rotateCW();
    glutPostRedisplay();
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void) {
    std::cout << "Interaction:" << std::endl << '\n'
              << "Press the up/down/left/right arrow keys to turn the spacecraft." << '\n'
              << "Press the w/s keys to move the spacecraft." << std::endl;
}

// Main routine.
int main(int argc, char **argv) {
    printInteraction();
    glutInit(&argc, argv);

    glutInitContextVersion(4, 2);
    glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("Solar System Simulation");
    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyInput);
    glutSpecialFunc(specialKeyInput);

    glewExperimental = GL_TRUE;
    glewInit();

    setup();

    glutMainLoop();
}
