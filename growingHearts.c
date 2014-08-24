#include<stdlib.h>
#include<X11/Xlib.h>

#include "vroot.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

/* Window Attributes for screen size (and maybe other stuff later) */
XWindowAttributes wa;

/*gobal vars up here for quick change */
#define numberOfHearts 20
int SPEED = 100;

/*The struct is a heart */
struct Heart{
  float x;
  float y;
  float scale;
  float maxSize;
  float speed;

};

struct Heart hearts[numberOfHearts];

void makeNewHeart(int i){
  hearts[i].x = rand() % wa.width;
  hearts[i].y = rand() % wa.height;
  hearts[i].scale = 0;
  hearts[i].maxSize = rand() % 10;
  hearts[i].speed = (rand() % 100) + 100;
}

main (){
  Display *dpy;
  Window root;
  GC g;
  XImage *img;
  XColor redx, reds;

  Pixmap double_buffer;

  /* open the display (connect to the X server) */
  dpy = XOpenDisplay(getenv ("DISPLAY"));

  /* get the root window */
  root = DefaultRootWindow(dpy);

   /* get attributes of the root window */
  XGetWindowAttributes(dpy, root, &wa);

  /* create a GC for drawing in the window */
  g = XCreateGC(dpy, root, 0, NULL);

  /*create duble buffer*/
  double_buffer = XCreatePixmap(dpy, root, wa.width, wa.height, wa.depth);
  XSetForeground(dpy, g, 
    BlackPixelOfScreen(DefaultScreenOfDisplay(dpy)));
  XFillRectangle(dpy, double_buffer, g, 0, 0, wa.width, wa.height);


  /* allocate the red color */
  XAllocNamedColor(dpy,
                     DefaultColormapOfScreen(DefaultScreenOfDisplay(dpy)),
                     "pink",
                     &reds, &redx);

  /* growing hearts stuff
     This will put vars initially*/
  int i = 0;
  for (i = 0; i < numberOfHearts; ++i){
    makeNewHeart(i);
  }

  /* draw something */
  while (1){
    /* removing the hearts from the screen*/
    XSetForeground(dpy, g, 
      BlackPixelOfScreen(DefaultScreenOfDisplay(dpy)));
    XFillRectangle(dpy, double_buffer, g, 0, 0, wa.width, wa.height);

    /* set foreground color */
    XSetForeground(dpy, g, reds.pixel);

    int i;
    for (i = 0; i < numberOfHearts; ++i){
      //code to draw a heart
      float s = hearts[i].scale;
      float x = hearts[i].x;
      float y = hearts[i].y;
      float t;
      for (t = 0; t < 2 * 3.1415926; t += 0.01){
        XFillArc(dpy, double_buffer, g,
          s * 16 * pow(sin(t),3) + x,
          -s * ((13 * cos(t)) - (5 * cos(2 * t)) - (2 * cos(3 * t)) - (cos(4 * t))) + y,
          3, 3, 0, 360 * 64);
      }

      /*If the heart is too large remove it*/
      if(s > hearts[i].maxSize){
        makeNewHeart(i);
      }
      hearts[i].scale += (0.0001 * hearts[i].speed);
    }

    /* copy the buffer to the window*/
    XCopyArea(dpy, double_buffer, root, g, 0, 0, wa.width, wa.height, 0, 0);

    /* flush changes and sleep */
    XFlush(dpy);
    usleep (SPEED);
  }
  XCloseDisplay (dpy);
}

