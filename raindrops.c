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
#define numberOfHearts 40
int SPEED = 100;

/*The struct is a heart */
struct Drop{
  float x;
  float y;
  float height;
  float scale;
  float maxSize;
};

struct Drop drops[numberOfHearts];

void makeNewDrop(int i){
  drops[i].x = rand() % wa.width;
  drops[i].y = rand() % wa.height;
  drops[i].height = 0;
  drops[i].scale = 0;
  drops[i].maxSize = rand() % 100;
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
                     "blue",
                     &reds, &redx);

  /* growing drops stuff
     This will put vars initially*/
  int i = 0;
  for (i = 0; i < numberOfHearts; ++i){
    makeNewDrop(i);
  }

  /* draw something */
  while (1){
    /* removing the drops from the screen*/
    XSetForeground(dpy, g, 
      BlackPixelOfScreen(DefaultScreenOfDisplay(dpy)));
    XFillRectangle(dpy, double_buffer, g, 0, 0, wa.width, wa.height);

    /* set foreground color */
    XSetForeground(dpy, g, reds.pixel);

    int i;
    for (i = 0; i < numberOfHearts; ++i){
      //code to draw a heart
      float s = drops[i].scale;
      float x = drops[i].x;
      float y = drops[i].y;
      float h = drops[i].height;
      //if the drop is low enough "hit" the water
      if (h > y){
        //to actually draw the circle
        float t;
        for (t = 0; t < 2 * 3.1415926; t += 0.01){
          XFillArc(dpy, double_buffer, g,
            ((((wa.height - y)/300)+2) * s * cos(t)) + x,
            (s * sin(t)) + y,
            3, 3, 0, 360 * 64);
        }
      drops[i].scale += (.05);
      }
      //else keep droping raindrop
      else{
        //Note, I may change this to have a raindrop shape, for now it's a dot
        XFillRectangle(dpy, double_buffer, g, x, h, 5, 5);
        drops[i].height += .5;
      }

      /*If the heart is too large remove it*/
      if(s > drops[i].maxSize){
        makeNewDrop(i);
      }

    }

    /* copy the buffer to the window*/
    XCopyArea(dpy, double_buffer, root, g, 0, 0, wa.width, wa.height, 0, 0);

    /* flush changes and sleep */
    XFlush(dpy);
    usleep (SPEED);
  }
  XCloseDisplay (dpy);
}

