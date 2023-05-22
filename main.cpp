#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

using namespace std;

// Globals.
static float latAngle = 0.0; // Latitudinal angle of moon.
static float longAngle = 0.0; // Longitudinal angle of moon.
static int animationPeriod = 50; // Time interval between frames.
const static int slices = 50;

class Planet {
public:
    Planet();

    Planet(float distance, float radius, unsigned char r, unsigned char g, unsigned char b);

    float getDistance() { return distance; }

    float getAngle() { return angle; }

    float getRadius() { return radius; }

    float getAngVelocity() { return angVelocity; }

    void draw();

    void update();

private:
    float distance, angle, radius, angVelocity;
    float color[4];

    void drawOrbit();
};

Planet::Planet() {}

// Planet constructor.
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

// Function to draw Planet.
void Planet::draw() {
    drawOrbit();

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

    Spacecraft(float x, float y, float angle);

    void draw();

    void update();

    float getX() { return x; }

    float getY() { return y; }

    float getAngle() { return angle; }

    void setAngVelocity(float v) { angVelocity = v; }

private:
    float x, y, angle, angVelocity;
};

Spacecraft::Spacecraft() {}

Spacecraft::Spacecraft(float x, float y, float angle) {
    this->x = x;
    this->y = y;
    this->angle = angle;
    angVelocity = 0.0;
}

void Spacecraft::draw() {
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(x, y, 0.0);
    glRotatef(angle, 0.0, 0.0, 1.0);
    glRotatef(90.0, 0.0, 1.0, 0.0);
    glColor3f(1.0, 1.0, 1.0);
    glutSolidCone(1.0, 3.0, slices, slices);
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void Spacecraft::update() {
    float tempX = x;
    float tempY = y;
    x = tempX * cos(angVelocity * M_PI / 180) - tempY * sin(angVelocity * M_PI / 180);
    y = tempX * sin(angVelocity * M_PI / 180) + tempY * cos(angVelocity * M_PI / 180);
}

Spacecraft spacecraft = Spacecraft(-60, -60, 45);

// Drawing routine.
void drawScene(void) {
    float sunPos[] = {0.0, 0.0, 0.0, 1.0};
    float sunReflection[] = {0.0, 0.0, 0.0, 1.0};
    float sunShine[] = {50.0};
    float sunEmission[] = {253 / 255.0, 184 / 255.0, 19 / 255.0, 1.0};

    float plSpec[] = {0.0, 0.0, 0.0, 1.0};
    float plShine[] = {50.0};
    float plEmission[] = {0.0, 0.0, 0.0, 1.0};

    float moonAmbDif[] = {1.0, 1.0, 1.0, 1.0};

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(0, 0, 70, 0, 0, 0, 0, 1, 0);

    // Material properties of sun.
    glMaterialfv(GL_FRONT, GL_AMBIENT, sunReflection);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, sunReflection);
    glMaterialfv(GL_FRONT, GL_SPECULAR, sunReflection);
    glMaterialfv(GL_FRONT, GL_SHININESS, sunShine);
    glMaterialfv(GL_FRONT, GL_EMISSION, sunEmission);

    // Light quadratic attenuation factor.
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0);

    // Light0 and its sphere positioned.
    glPushMatrix();
    glLightfv(GL_LIGHT0, GL_POSITION, sunPos);
    glutSolidSphere(5.5, slices, slices);
    glPopMatrix();

    // Material properties of planets.
    glMaterialfv(GL_FRONT, GL_SPECULAR, plSpec);
    glMaterialfv(GL_FRONT, GL_SHININESS, plShine);
    glMaterialfv(GL_FRONT, GL_EMISSION, plEmission);

    for (int i = 0; i < 8; i++) {
        planet[i].draw();
    }

    // Material properties of moon.
    glMaterialfv(GL_FRONT, GL_AMBIENT, moonAmbDif);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, moonAmbDif);

    // Draw moon.
    glPushMatrix();
    glRotatef(planet[2].getAngle(), 0.0, 0.0, 1.0);
    glTranslatef(planet[2].getDistance(), 0.0, 0.0);
    glRotatef(latAngle, 0.0, 1.0, 0.0);
    glTranslatef(2, 0, 0);
    glutSolidSphere(.5, slices, slices);
    glPopMatrix();

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
    spacecraft.update();

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

    // Initialize planets
    planet[0] = Planet(8, 1, 183, 184, 185);
    planet[1] = Planet(16, 1.5, 227, 158, 28);
    planet[2] = Planet(24, 1.5, 0, 66, 129);
    planet[3] = Planet(32, 1, 156, 46, 53);
    planet[4] = Planet(40, 3.5, 206, 165, 137);
    planet[5] = Planet(48, 3.5, 205, 160, 86);
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
            break;
        default:
            break;
    }
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y) {
    if (key == GLUT_KEY_DOWN) animationPeriod += 5;
    if (key == GLUT_KEY_UP) if (animationPeriod > 5) animationPeriod -= 5;
    glutPostRedisplay();
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void) {
    std::cout << "Interaction:" << std::endl;
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
