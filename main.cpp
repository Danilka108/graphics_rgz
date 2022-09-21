#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <math.h>

struct Complex {
  double x;
  double y;
};

void resize(int width, int height) {}

const int ITER_NUMBER = 50;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

double scale = 1;
double x_correction_factor = 0;
double y_correction_factor = 0;

int prev_mouse_x = NULL;
int prev_mouse_y = NULL;

bool is_pressed = false;

void mouse(int button, int state, int x, int y) {
  is_pressed = state == GLUT_LEFT_BUTTON && state == GLUT_DOWN;

  if (button == 3) {
    scale += 1;
  }

  if(button == 4) {
    scale -= 1;
  }

  prev_mouse_y = y;
  prev_mouse_x = x;

}

void mouseMotion(int x, int y) {
  if (prev_mouse_y == NULL || prev_mouse_x == NULL || !is_pressed) {
    prev_mouse_y = y;
    prev_mouse_x = x;
    return;
  }
  x_correction_factor -= x - prev_mouse_x;
  y_correction_factor += y - prev_mouse_y;

  prev_mouse_y = y;
  prev_mouse_x = x;
}

void mouseEntry(int state) {
  if (state == GLUT_LEFT) {
    is_pressed = false;
  }

  if (state == GLUT_ENTERED) {
    is_pressed = true;
  }
}

void display() {

  glViewport(0, 0, 800, 800);

  glPointSize(1);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBegin(GL_POINTS);

  for (double y = -WINDOW_HEIGHT / scale / 2; y < WINDOW_HEIGHT / scale / 2;
       y += 1 / scale) {
    for (double x = -WINDOW_WIDTH / scale / 2; x < WINDOW_WIDTH / scale / 2;
         x += 1 / scale) {
      Complex z, z1, c, c1;

      z.x = (x + x_correction_factor / scale) * 0.01;
      z.y = (y + y_correction_factor / scale) * 0.01;
      c.x = z.x;
      c.y = z.y;

      int max = 16;

      int n = 0;
      while (((z.x * z.x + z.y * z.y) < max) && (n < ITER_NUMBER)) {
        z1 = z;
        c1 = c;
        z.x = (z1.x * z1.x) - (z1.y * z1.y) + c.x;
        z.y = 2 * z1.x * z1.y + c.y;
        c.x = c1.x / 2 + z.x;
        c.y = c1.y / 2 + z.y;
        n++;
      }

      if (n < ITER_NUMBER) {
        double a = 35 * n % 255;
        double b = 30 * n % 255;
        double c = 25 * n % 255;
        glColor3d(a / 255.0, b / 255.0, c / 255.0);
        glVertex2d(x * scale, y * scale);
      }
    }
  }

  glEnd();

  glFlush();
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
  glOrtho(-WINDOW_WIDTH / 2, WINDOW_WIDTH / 2, -WINDOW_HEIGHT / 2,
          WINDOW_HEIGHT / 2, 0, 2);
  gluLookAt(0, 0, 1, 0, 0, 0, 0, 1, 0);
  glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowPosition(50, 10);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutCreateWindow("Graphics Rgz");
  glutReshapeFunc(resize);
  glutMouseFunc(mouse);
  glutMotionFunc(mouseMotion);
  init();
  glutDisplayFunc(display);
  glutEntryFunc(mouseEntry);
  glutIdleFunc(display);
  glutMainLoop();
  return 0;
}
