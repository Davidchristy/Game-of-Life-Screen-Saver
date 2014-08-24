#include<stdlib.h>
#include<X11/Xlib.h>

#include "vroot.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

/* Window Attributes for screen size (and maybe other stuff later) */
XWindowAttributes wa;

/*gobal vars up here for quc change */
#define numberOfCirlces 400
int distanceOfLine = 200;
float gravity = -1.5;
int SPEED = 100;


/*The struct is a circle */
struct Circle{
  int x;
  int y;
  int currY;
  int currX;
  float offsetY;
  float offsetX;
  int random;
};

struct Circle circles[numberOfCirlces];

void makeNewCircle(int i){
  circles[i].x = rand() % wa.width;
  circles[i].y = rand() % wa.height;
  circles[i].offsetX = 0;
  circles[i].offsetY = 0;
  circles[i].random = rand() % 2048;
}

float distanceFrom(int middleX, int middleY, int currentX, int currentY) {
  int tempX = abs(middleX - currentX);
  int tempY = abs(middleY - currentY);

  float distance = pow(pow(tempX, 2)+pow(tempY, 2), 0.5);

  return distance;
}

long long current_timestamp() {
  struct timeval te; 
  gettimeofday(&te, NULL); // get current time
  long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // caculate milliseconds
  return milliseconds;
}

main ()
{
  Display *dpy;
  Window root;
  GC g;
  XImage *img;
  XColor redx, reds;

  /* open the display (connect to the X server) */
  dpy = XOpenDisplay(getenv ("DISPLAY"));

  /* get the root window */
  root = DefaultRootWindow(dpy);

   /* get attributes of the root window */
  XGetWindowAttributes(dpy, root, &wa);

  /* create a GC for drawing in the window */
  g = XCreateGC(dpy, root, 0, NULL);

  /* allocate the red color */
  XAllocNamedColor(dpy,
                     DefaultColormapOfScreen(DefaultScreenOfDisplay(dpy)),
                     "blue",
                     &reds, &redx);

  /* set foreground color */
  XSetForeground(dpy, g, reds.pixel);

  /* Moving circles stuff
     This will put vars initially*/
  int i = 0;
  for (i = 0; i < numberOfCirlces; ++i){
    makeNewCircle(i);
  }

  /* draw something */
  while (1){
    /* Clearing the screen for next turn*/
    XClearWindow(dpy, root);

    // distanceOfLine += (random() % 3) - 1;

    int i = 0;
    for (i = 0; i < numberOfCirlces; ++i){
      /*If the circle is outside the screen*/
      if(circles[i].currX + (int) circles[i].offsetX < 0 ||
        circles[i].currY + (int) circles[i].offsetY < 0 ||
        circles[i].currX + (int) circles[i].offsetX > wa.width ||
        circles[i].currY + (int) circles[i].offsetY > wa.height){
        //it's outside the visable space
        makeNewCircle(i);
        }
      // float t = current_timestamp()/500.0f;
      // circles[i].currX = (int)(circles[i].x + distanceOfCirlces * cos((double) (t + circles[i].random)));
      // circles[i].currY = (int)(circles[i].y + distanceOfCirlces * sin((double) (t + circles[i].random)));
      circles[i].currX = (int)(circles[i].x);
      circles[i].currY = (int)(circles[i].y);

      int j = 0; 
      for(j = 0; j < numberOfCirlces; j++){
         distanceOfLine += ((random() % 3) - 1) /10;
         if(distanceFrom(circles[i].currX + (int)circles[i].offsetX, circles[i].currY + (int)circles[i].offsetY, circles[j].currX + (int)circles[j].offsetX, circles[j].currY + (int) circles[j].offsetY) < distanceOfLine)
         {
            // XDrawLine(dpy, root, g, circles[i].currX + circles[i].offsetX,
            //                         circles[i].currY + circles[i].offsetY,
            //                         circles[j].currX + circles[j].offsetX,
            //                         circles[j].currY + (int) circles[j].offsetY);

              if(circles[i].currY + circles[i].offsetY > circles[j].currY + circles[j].offsetY){
                circles[i].offsetY -= gravity;
              } else {
                circles[i].offsetY += gravity;
              }
              if(circles[i].currX + circles[i].offsetX > circles[j].currX + circles[j].offsetX){
                circles[i].offsetX -= gravity;
              } else {
                circles[i].offsetX += gravity;
              }
         }
      }
      XFillArc(dpy, root, g,
            circles[i].currX + (int) circles[i].offsetX,
            circles[i].currY + (int)circles[i].offsetY,
            10, 10, 0, 360 * 64);
    }
    /* flush changes and sleep */
    XFlush(dpy);
    usleep (SPEED);
  }
  XCloseDisplay (dpy);
}

