#include <stdio.h>

#include <stdbool.h> /* Needed for boolean datatype */

#include <math.h>
#include <pthread.h>
#include <stdlib.h>

#define min(a, b)(((a) < (b)) ? (a) : (b))
#define NUM_THREAD 10
/* Width and height of out image */
#define WIDTH 800
#define HEIGHT 1200

/* The vector structure */
typedef struct {
  float x, y, z;
}
vector;

/* The sphere */
typedef struct {
  vector pos;
  float radius;
  int material;
}
sphere;

/* The ray */
typedef struct {
  vector start;
  vector dir;
}
ray;

/* Colour */
typedef struct {
  float red, green, blue;
}
colour;

/* Material Definition */
typedef struct {
  colour diffuse;
  float reflection;
}
material;

/* Lightsource definition */
typedef struct {
  vector pos;
  colour intensity;
}
light;


vector vectorSub(vector * v1, vector * v2);
float vectorDot(vector * v1, vector * v2);
vector vectorScale(float c, vector * v);
vector vectorAdd(vector * v1, vector * v2);
bool intersectRaySphere(ray * r, sphere * s, float * t);
void saveppm(char * filename, unsigned char * img, int width, int height);
void *draw_thread(void *ptr);