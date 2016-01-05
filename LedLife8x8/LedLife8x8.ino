#include <MaxMatrix.h>
int data = 8;    // DIN pin of MAX7219 module
int load = 9;    // CS pin of MAX7219 module
int clock = 10;  // CLK pin of MAX7219 module
int maxInUse = 1; //change this variable to set how many MAX7219's you'll use

#define WAIT_MS 1000
#define MAX_ROWS 8
#define MAX_COLS 8
byte world[MAX_ROWS][MAX_COLS];

MaxMatrix matrix(data, load, clock, maxInUse); // define module

void initWorld()
{
  randomSeed(analogRead(0));
  for (byte r = 0; r < MAX_ROWS; r++) 
  {
    for (byte c = 0;  c < MAX_COLS; c++)
    {
      world[r][c] = random(1000) > 700 ? 1 : 0;
      //world[r][c] = 0;
    }
  }
  
  /*
  // Blinker
  world[4][4] = 1;
  world[4][5] = 1;
  world[4][6] = 1;
  // Block
  world[1][1] = 1;
  world[1][2] = 1;
  world[2][1] = 1;
  world[2][2] = 1;
  // Beehive
  world[5][3] = 1;
  world[4][4] = 1;
  world[4][5] = 1;
  world[6][4] = 1;
  world[6][5] = 1;
  world[5][6] = 1;
  // Beacon
  world[2][2] = 1;
  world[2][3] = 1;
  world[3][2] = 1;
  world[3][3] = 1;  
  world[4][4] = 1;
  world[4][5] = 1;
  world[5][4] = 1;
  world[5][5] = 1;*/
}

void setup()
{
  initWorld();
  matrix.init(); // module initialize
  matrix.setIntensity(15);
}

void render()
{
  for (byte r = 0; r < MAX_ROWS; r++) 
  {
    for (byte c = 0;  c < MAX_COLS; c++)
    {
      matrix.setDot(c, r, world[r][c]);  
    }
  }
}


int xadd (int i, int a)
{
  i += a;
  while (i < 0) i += MAX_COLS;
  while (i >= MAX_COLS) i -= MAX_COLS;
  return i;
}

/* add to a height index, wrapping around */

int yadd (int i, int a) 
{
  i += a;
  while (i < 0) i += MAX_ROWS;
  while (i >= MAX_ROWS) i -= MAX_ROWS;
  return i;
}

int getNumberOfAdjacentCells(byte r, byte c)
{
  int count = 0;
  for (int k = -1; k <= 1; k++)
  {
    for (int l= -1; l <= 1; l++)
    if (k || l)
    {
      if (world[yadd(r, l)][xadd(c, k)])
      {
        count++;
      }
    }
  }
  return count;
}

void simulate()
{
  // Store new states in temporary matrix.
  byte tempWorld[MAX_ROWS][MAX_COLS];
  for (byte r = 0; r < MAX_ROWS; r++) 
  {
    for (byte c = 0;  c < MAX_COLS; c++)
    {
      int cells = getNumberOfAdjacentCells(r, c);
      if (cells == 2)
      {
        // stasis
        tempWorld[r][c] = world[r][c];
      }
      else if (cells == 3)
      {
        // growth
        tempWorld[r][c] = 1;
      }
      else 
      {
        // death
        tempWorld[r][c] = 0;
      }
    }
  }
  //  
  // Copy back to world matrix.
  //
  int alive = 0;
  for (byte r = 0; r < MAX_ROWS; r++) 
  {
    for (byte c = 0;  c < MAX_COLS; c++)
    {
      world[r][c] = tempWorld[r][c];
      alive += world[r][c];
    }
  }
  //
  // If all cells are dead. Re-populate the world.
  //
  if (alive == 0)
  {
    initWorld();
    delay(2000); // dramatic pause
  }  
}

void loop()
{
  render();
  delay(WAIT_MS);
  simulate();
}
