#include<stdlib.h>
#include<X11/Xlib.h>

#include "vroot.h"
#include <stdio.h>

/* Window Attributes for screen size (and maybe other stuff later) */
XWindowAttributes wa;

/*gobal vars up here for quc change */
int cellSize = 25;
int DeathRate = 10;
int SPEED = 150000;

/* dimensions of the screen */
#define BOARD_WIDTH wa.width / cellSize
#define BOARD_HEIGHT  wa.height / cellSize


/* set everthing to either one or zero randomly */
void initialize_board (int board[][BOARD_HEIGHT], int DEATHboard[][BOARD_HEIGHT]) {
  int i, j;
  int percent_Living = (random() % 40) + 10;
  for (i=0; i<BOARD_WIDTH; i++) for (j=0; j<BOARD_HEIGHT; j++){
    board[i][j] = random() % 100 < percent_Living;
    DEATHboard = 0;
  } 
}

/*
 * The Game of Life
 *
 * a cell is born, if it has exactly three neighbours 
 * a cell dies of loneliness, if it has less than two neighbours 
 * a cell dies of overcrowding, if it has more than three neighbours 
 * a cell survives to the next generation, if it does not die of loneliness 
 * or overcrowding 
 *
 * In my version, a 2D array of ints is used.  A 1 cell is on, a 0 cell is off.
 * The game plays 100 rounds, printing to the screen each time.  'x' printed
 * means on, space means 0.
 *
 */


/* add to a width index, wrapping around like a cylinder */

int xadd (int i, int a) {
  i += a;
  while (i < 0) i += BOARD_WIDTH;
  while (i >= BOARD_WIDTH) i -= BOARD_WIDTH;
  return i;
}

/* add to a height index, wrapping around */

int yadd (int i, int a) {
  i += a;
  while (i < 0) i += BOARD_HEIGHT;
  while (i >= BOARD_HEIGHT) i -= BOARD_HEIGHT;
  return i;
}

/* return the number of on cells adjacent to the i,j cell */

int adjacent_to (int board[][BOARD_HEIGHT], int i, int j) {
  int k, l, count;

  count = 0;

  /* go around the cell */

  for (k=-1; k<=1; k++) for (l=-1; l<=1; l++)

    /* only count if at least one of k,l isn't zero */

    if (k || l)
      if (board[xadd(i,k)][yadd(j,l)]) count++;
  return count;
}

void play (int board[][BOARD_HEIGHT], int DEATHboard[][BOARD_HEIGHT]) {
/*
  (copied this from some web page, hence the English spellings...)

  1.STASIS : If, for a given cell, the number of on neighbours is 
  exactly two, the cell maintains its status quo into the next 
  generation. If the cell is on, it stays on, if it is off, it stays off.

  2.GROWTH : If the number of on neighbours is exactly three, the cell 
  will be on in the next generation. This is regardless of the cell's     current state.

  3.DEATH : If the number of on neighbours is 0, 1, 4-8, the cell will 
  be off in the next generation.
*/


  int i, j, a, newboard[BOARD_WIDTH][BOARD_HEIGHT];
  /* for each cell, apply the rules of Life */
  for (i=0; i<BOARD_WIDTH; i++) for (j=0; j<BOARD_HEIGHT; j++) {
    a = adjacent_to (board, i, j);
    if (a == 2) newboard[i][j] = board[i][j];
    if (a == 3) newboard[i][j] = 1;
    if (a < 2) newboard[i][j] = 0;
    if (a > 3) newboard[i][j] = 0;

    /* kill off cells that have been there too long*/
    if(board[i][j]){
      if (DEATHboard[i][j]++ > DeathRate){
        //weird loophole to get rid of squares
        if (a == 3){
          board[i - 1][j] = 0;
          board[i + 1][j] = 0;
          board[i][j - 1] = 0;
          board[i][j + 1] = 0;
        }
        board[i][j] = 0;
        DEATHboard[i][j] = 0;
      }
    }
  }

  /* copy the new board back into the old board */
  for (i=0; i<BOARD_WIDTH; i++) for (j=0; j<BOARD_HEIGHT; j++) {
    board[i][j] = newboard[i][j];
  }
}

/* This check the board to see if 
   all the cells are dead, return true is so*/
int gameOver(int board[][BOARD_HEIGHT]){
  int i, j;
  for (i=0; i<BOARD_WIDTH; i++) for (j=0; j<BOARD_HEIGHT; j++) {  
    if(board[i][j])
      return 0;
  }
  return 1;
}


/*This draws the game of Life board 
   on the screen using xwindow functions*/
void draw(int board[][BOARD_HEIGHT], Display *dpy, Window root, GC g){
      int i, j;
    /* for each row */
    for (j=0; j<BOARD_HEIGHT; j++) {
      /* print each column position... */
      for (i=0; i<BOARD_WIDTH; i++) {
        if (board[i][j] == 1){
          XFillArc(dpy, root, g,
             i * cellSize, j * cellSize,
              cellSize, cellSize, 0, 360 * 64);
        }
      }
    }
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
                     "red",
                     &reds, &redx);

  /* set foreground color */
  XSetForeground(dpy, g, reds.pixel);

  /* GAME OF LIFE STUFF*/
  int board[BOARD_WIDTH][BOARD_HEIGHT];
  int DEATHboard[BOARD_WIDTH][BOARD_HEIGHT];
  initialize_board (board, DEATHboard);

  /* draw something */
  while (1){
    /* Clearing the screen for next turn*/
    XClearWindow(dpy, root);

    //playing a turn of game of life
    play (board, DEATHboard);

    /*Printing the board on the screen*/
    draw (board, dpy, root, g);

    /*if game is over, reset and play again*/
    if(gameOver(board)){
      initialize_board (board, DEATHboard);
      draw (board, dpy, root, g);
      XFlush(dpy);
      usleep (SPEED * 5);
    }

    /* flush changes and sleep */
    XFlush(dpy);
    usleep (SPEED);
  }
  XCloseDisplay (dpy);
}

