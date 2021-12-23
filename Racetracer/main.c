/* A simple ray tracer */
#include "helper.h"
/* Will contain the raw image */

//create a struct to represent for both index and pointer to array img
typedef struct mystructforimg
{
  unsigned char* pointerToArray;
  int* index;
} mystructforimg;

//pixelcount variable and mutex
int pixelcount = 0;
pthread_mutex_t mutex;

material materials[3];
sphere spheres[3];
light lights[3];


void *draw_thread(void *ptr){
  ray r;
  
  mystructforimg* z =  (mystructforimg*)ptr;  
  int indextoloop = *z->index;

  printf("Thread: %d from - %d  to - %d \n",indextoloop,HEIGHT* indextoloop/NUM_THREAD,HEIGHT*(indextoloop+1)/NUM_THREAD);
  
  int x, y;
    //we create numthread for number part of image that we want to didive.
    //eg, when 
    for (y = HEIGHT* indextoloop/NUM_THREAD ; y <  HEIGHT*(indextoloop+1)/NUM_THREAD ; y++) {
    for (x = 0; x < WIDTH; x++) {

      float red = 0;
      float green = 0;
      float blue = 0;

      int level = 0;
      float coef = 1.0;

      r.start.x = x;
      r.start.y = y;
      r.start.z = -2000;

      r.dir.x = 0;
      r.dir.y = 0;
      r.dir.z = 1;

      do {
        /* Find closest intersection */
        float t = 20000.0f;
        int currentSphere = -1;
       
        unsigned int i;
        for (i = 0; i < 3; i++) {
          if (intersectRaySphere( &r, &spheres[i], &t))
            currentSphere = i;
        }
        if (currentSphere == -1) break;

        vector scaled = vectorScale(t, &r.dir);
        vector newStart = vectorAdd( &r.start, &scaled);

        /* Find the normalfor this new vector at the point of intersection */
        vector n = vectorSub( &newStart, &spheres[currentSphere].pos);
        float temp = vectorDot( &n, &n);
        if (temp == 0) break;

        temp = 1.0f / sqrtf(temp);
        n = vectorScale(temp, &n);

        /* Find the material to determine the colour */
        material currentMat = materials[spheres[currentSphere].material];

        /* Find the value of the light at this point */
        unsigned int j;
        for (j = 0; j < 3; j++) {
          light currentLight = lights[j];
          vector dist = vectorSub( &currentLight.pos, &newStart);
          if (vectorDot( &n, &dist) <= 0.0f) continue;
          float t = sqrtf(vectorDot( &dist, &dist));
          if (t <= 0.0f) continue;

          ray lightRay;
          lightRay.start = newStart;
          lightRay.dir = vectorScale((1 / t), &dist);

          /* Lambert diffusion */
          float lambert = vectorDot( &lightRay.dir, &n) * coef;
          red += lambert * currentLight.intensity.red * currentMat.diffuse.red;
          green += lambert * currentLight.intensity.green * currentMat.diffuse.green;
          blue += lambert * currentLight.intensity.blue * currentMat.diffuse.blue;
        }
        /* Iterate over the reflection */
        coef *= currentMat.reflection;

        /* The reflected ray start and direction */
        r.start = newStart;
        float reflect = 2.0f * vectorDot( &r.dir, &n);
        vector tmp = vectorScale(reflect, &n);
        r.dir = vectorSub( &r.dir, &tmp);

        level++;

      } while ((coef > 0.0f) && (level < 15));

      z->pointerToArray[(x + y * WIDTH) * 3 + 0] = (unsigned char) min(red * 255.0f, 255.0f);
      z->pointerToArray[(x + y * WIDTH) * 3 + 1] = (unsigned char) min(green * 255.0f, 255.0f);
      z->pointerToArray[(x + y * WIDTH) * 3 + 2] = (unsigned char) min(blue * 255.0f, 255.0f);

      //increase the pixelcount
      pthread_mutex_lock(&mutex);
      pixelcount++;
      pthread_mutex_unlock(&mutex);
    }
    

  }
    //free pointer
    free(ptr);
    pthread_exit(0); /* Exit */
}


int main(int argc, char * argv[]) {
  materials[0].diffuse.red = 1;
  materials[0].diffuse.green = 0;
  materials[0].diffuse.blue = 0;
  materials[0].reflection = 0.2;

  materials[1].diffuse.red = 0;
  materials[1].diffuse.green = 1;
  materials[1].diffuse.blue = 0;
  materials[1].reflection = 0.5;

  materials[2].diffuse.red = 0;
  materials[2].diffuse.green = 0;
  materials[2].diffuse.blue = 1;
  materials[2].reflection = 0.9;

  
  spheres[0].pos.x = 200;
  spheres[0].pos.y = 300;
  spheres[0].pos.z = 0;
  spheres[0].radius = 100;
  spheres[0].material = 0;

  spheres[1].pos.x = 400;
  spheres[1].pos.y = 400;
  spheres[1].pos.z = 0;
  spheres[1].radius = 100;
  spheres[1].material = 1;

  spheres[2].pos.x = 500;
  spheres[2].pos.y = 140;
  spheres[2].pos.z = 0;
  spheres[2].radius = 100;
  spheres[2].material = 2;

  

  lights[0].pos.x = 0;
  lights[0].pos.y = 240;
  lights[0].pos.z = -100;
  lights[0].intensity.red = 1;
  lights[0].intensity.green = 1;
  lights[0].intensity.blue = 1;

  lights[1].pos.x = 3200;
  lights[1].pos.y = 3000;
  lights[1].pos.z = -1000;
  lights[1].intensity.red = 0.6;
  lights[1].intensity.green = 0.7;
  lights[1].intensity.blue = 1;

  lights[2].pos.x = 600;
  lights[2].pos.y = 0;
  lights[2].pos.z = -100;
  lights[2].intensity.red = 0.3;
  lights[2].intensity.green = 0.5;
  lights[2].intensity.blue = 1;
  //move the global to local
  unsigned char img[3 * WIDTH * HEIGHT];
  //initialize mutex
  pthread_mutex_init(&mutex, NULL);
  pthread_t threads[NUM_THREAD];
    int i;
    //create threads
    for(i = 0; i < NUM_THREAD; i++){

      //cast struct to pointer instead of int k that show below in the comment
      mystructforimg* d = malloc(sizeof(mystructforimg));
      d->pointerToArray = img;
      d->index =  malloc(sizeof(int));
      *d->index = i;
      
      //cast k to function before create the struct
      //int* k = malloc(sizeof(int)); create space for k in the memory
      //*k = i; give the value of i to pointer k

	    if(pthread_create(&threads[i], NULL, &draw_thread,d /* k */) != 0){
        perror("Failed to create threads!");
      }
    }
    //join threads
    for(i = 0; i < NUM_THREAD; i++){
	    if(pthread_join(threads[i], NULL) != 0){
        perror("Failed to join threads!");
      }
    }
    //destroy mutex
    pthread_mutex_destroy(&mutex);
  
  /*pthread_t thread1, thread2;  
    
    pthread_create (&thread1, NULL, (void *)&draw_thread, NULL);
    pthread_create (&thread2, NULL, (void *)&draw_thread, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
  */
  saveppm("image.ppm", img, WIDTH, HEIGHT);
  printf("Pixelcount %d\n", pixelcount);

  return 0;
}