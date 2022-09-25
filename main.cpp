#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>

typedef GLdouble FloatType;

struct Complex {
  FloatType real;
  FloatType imag;
};

const int DEFAULT_ITERS_OF_FORUMA_CALC = 100;
const FloatType MAX_COMPLEX_NUMBER_ABSOLUTE_VALUE = 16;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

const int RENDER_QUARTER_WIDTH = 400;
const int RENDER_QUARTER_HEIGHT = 400;

const FloatType RENDER_STEP = 1;
const FloatType ZOOM_STEP = 0.1;

const FloatType DEFAULT_ZOOM = 5;

void init();
void onMouseStateChangeListener(int button, int state, int x, int y);
void onMouseMotionListener(int x, int y);
void onMouseEntryListener(int state);
void onResizeListener(int width, int height);
void onDisplayListener();
void renderAxes();
void renderFrame();
void updateItersOfFormulaCalc();
int calcProximityDegreeToCenter(FloatType x, FloatType y);
bool isAllowableComplexValue(Complex number);
void calcIterableFormula(Complex &z, Complex &c);
void renderPoint(int n, int x, int y);

FloatType zoom = DEFAULT_ZOOM;

FloatType scaleFactor = 1 / exp(DEFAULT_ZOOM);

FloatType xShiftFactor = 0;
FloatType yShiftFactor = 0;

int prevMouseXPos = NULL;
int prevMouseYPos = NULL;

bool doRender = true;

bool isMouseLeftBtnPressed = false;

int itersOfFormulaCalc = DEFAULT_ITERS_OF_FORUMA_CALC;

int main(int argc, char **argv) {
  glutInit(&argc, argv);

  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowPosition(50, 10);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

  glutCreateWindow("Graphics Rgz");

  glutMouseFunc(onMouseStateChangeListener);
  glutMotionFunc(onMouseMotionListener);
  glutEntryFunc(onMouseEntryListener);

  glutReshapeFunc(onResizeListener);
  glutDisplayFunc(onDisplayListener);
  glutPostRedisplay();
  glutIdleFunc(onDisplayListener);

  init();

  glutMainLoop();
  return 0;
}

void init() {
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-RENDER_QUARTER_WIDTH, RENDER_QUARTER_WIDTH, -RENDER_QUARTER_HEIGHT,
          RENDER_QUARTER_HEIGHT, 0, 2);
  gluLookAt(0, 0, 1, 0, 0, 0, 0, 1, 0);
  glMatrixMode(GL_MODELVIEW);
}

void onMouseStateChangeListener(int button, int state, int x, int y) {
  isMouseLeftBtnPressed = state == GLUT_LEFT_BUTTON && state == GLUT_DOWN;

  if (button == 3) {
    zoom += ZOOM_STEP;
  }

  if (button == 4) {
    zoom = zoom - ZOOM_STEP < DEFAULT_ZOOM ? DEFAULT_ZOOM : zoom - ZOOM_STEP;
  }

  if (button == 3 || button == 4) {
    doRender = true;
    scaleFactor = 1 / exp(zoom);
    updateItersOfFormulaCalc();
  }

  prevMouseYPos = y;
  prevMouseXPos = x;
}

void onMouseMotionListener(int x, int y) {
  if (prevMouseYPos == NULL || prevMouseXPos == NULL ||
      !isMouseLeftBtnPressed) {
    prevMouseYPos = y;
    prevMouseXPos = x;

    return;
  }

  if (prevMouseXPos != x || prevMouseYPos != y)
    doRender = true;

  xShiftFactor -= (FloatType)(x - prevMouseXPos) * scaleFactor;
  yShiftFactor += (FloatType)(y - prevMouseYPos) * scaleFactor;

  prevMouseYPos = y;
  prevMouseXPos = x;
}

void onMouseEntryListener(int state) {
  if (state == GLUT_LEFT) {
    isMouseLeftBtnPressed = false;
  }

  if (state == GLUT_ENTERED) {
    isMouseLeftBtnPressed = true;
  }
}

void onResizeListener(int width, int height) {}

void onDisplayListener() {
  if (!doRender) {
    glFlush();
    return;
  }

  doRender = false;

  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  glPointSize(1);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  renderAxes();
  renderFrame();

  glFlush();
}

void renderAxes() {
  glBegin(GL_LINES);

  glColor3d(1, 1, 1);

  glVertex2d(0, RENDER_QUARTER_HEIGHT);
  glVertex2d(0, -RENDER_QUARTER_HEIGHT);

  glVertex2d(RENDER_QUARTER_WIDTH, 0);
  glVertex2d(-RENDER_QUARTER_WIDTH, 0);

  glEnd();
}

Complex z, c, tempZ, tempC;

void renderFrame() {
  glBegin(GL_POINTS);

  for (int x = -RENDER_QUARTER_WIDTH; x < RENDER_QUARTER_WIDTH;
       x += RENDER_STEP) {
    for (int y = -RENDER_QUARTER_HEIGHT; y < RENDER_QUARTER_HEIGHT;
         y += RENDER_STEP) {

      int degree = calcProximityDegreeToCenter(x * scaleFactor + xShiftFactor,
                                               y * scaleFactor + yShiftFactor);

      renderPoint(degree, x, y);
    }
  }

  glEnd();
}

void updateItersOfFormulaCalc() {
  int value = round(zoom) * 10;
  itersOfFormulaCalc = value < DEFAULT_ITERS_OF_FORUMA_CALC
                           ? DEFAULT_ITERS_OF_FORUMA_CALC
                           : value;
}

int calcProximityDegreeToCenter(FloatType x, FloatType y) {
  Complex z, c;

  z.real = x;
  z.imag = y;
  c.real = z.real;
  c.imag = z.imag;

  int n = 0;

  for (; isAllowableComplexValue(z) && n < itersOfFormulaCalc; n++) {
    calcIterableFormula(z, c);
  }

  return n;
}

/*
 * Is the absolute value of complex number less than the constant value
 */
bool isAllowableComplexValue(Complex number) {
  return (number.real * number.real + number.imag * number.imag) <
         MAX_COMPLEX_NUMBER_ABSOLUTE_VALUE;
}

/*
 * Iterable formula for calculating fractal:
 * f(z) = z^2 + c
 */
void calcIterableFormula(Complex &z, Complex &c) {
  Complex tempZ = z;
  Complex tempC = c;

  z.real = (tempZ.real * tempZ.real) - (tempZ.imag * tempZ.imag) + c.real;
  z.imag = 2 * tempZ.real * tempZ.imag + c.imag;
  c.real = tempC.real / 2 + z.real;
  c.imag = tempC.imag / 2 + z.imag;
}

void renderPoint(int n, int x, int y) {
  if (n >= itersOfFormulaCalc) {
    return;
  }

  GLfloat r = (35 * n) % 255;
  GLfloat g = (30 * n) % 255;
  GLfloat b = (25 * n) % 255;

  glColor3f(r / 255., g / 255., b / 255.);
  glVertex2d(x, y);
}
