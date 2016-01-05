#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library

#define CLK 11
#define OE  9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3

#define WAIT_MS 700
#define WAIT_SWIPE 70
#define MAX_ROWS 32
#define MAX_COLS 32

uint16_t world[MAX_ROWS][MAX_COLS];
uint16_t copyWorld[MAX_ROWS][MAX_COLS];

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, true);

uint16_t currentColor = 0;
uint16_t lastChanged = 0;
uint16_t countDown = 0;

uint8_t getColorVal()
{
  return 7;
  // return random(8);
}
uint16_t getColor(bool newColor)
{
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;
  if (newColor)
  {
    currentColor = random() % 7;
  }
  switch (currentColor)
  {
    case 0: // green
      g = getColorVal();
      break;
      
    case 1: // red    
      r = getColorVal();
      break;
      
    case 2:  // blue
      b = getColorVal();
      break;
      
    case 3:  // yellow
      r = getColorVal();
      g = getColorVal();
      break;
      
    case 4:  // cyan
      g = getColorVal();
      b = getColorVal();
      break;
      
    case 5:  // magenta
      r = getColorVal();
      b = getColorVal();      
      break;
      
    default:  // white
      r = getColorVal();
      b = getColorVal();
      g = getColorVal();
      break;
  }
  return matrix.Color333(r, g, b);
}

void initWorld()
{
  lastChanged = 0;

  uint16_t color = getColor(true);
  // swipe left to right
  for (int c = 0;  c < MAX_COLS; c++)
  {
    for (int r = 0; r < MAX_ROWS; r++)
    {
      matrix.drawPixel(max(0, c - 1), r, 0);
      matrix.drawPixel(c, r, color);
    }
    matrix.swapBuffers(true);
    delay(WAIT_SWIPE);
  }
  // init world
  for (int r = 0; r < MAX_ROWS; r++)
  {
    for (int c = 0;  c < MAX_COLS; c++)
    {
      world[r][c] = random(1000) > 700 ? getColor(false) : 0;
    }
  }
  // init world
  if (false)
  {
    // Blinker
    world[4][4] = 1;
    world[4][5] = 1;
    world[4][6] = 1;
  }
  if (false)
  {
    // Block
    world[1][1] = color;
    world[1][2] = color;
    world[2][1] = color;
    world[2][2] = color;
  }
  /*
  // Beehive
  world[5][3] = 1;
  world[4][4] = 1;
  world[4][5] = 1;
  world[6][4] = 1;
  world[6][5] = 1;
  world[5][6] = 1;
  */
  if (false)
  {
    // Beacon
    world[12][12] = color;
    world[12][13] = color;
    world[13][12] = color;
    world[13][13] = color;
    world[14][14] = color;
    world[14][15] = color;
    world[15][14] = color;
    world[15][15] = color;
  }
  // swipe right to left
  for (int c = MAX_COLS - 1;  c >= 0; c--)
  {
    for (int r = 0; r < MAX_ROWS; r++)
    {
      matrix.drawPixel(max(0, c - 1), r, color);
      matrix.drawPixel(c, r, world[r][c]);
    }
    matrix.swapBuffers(true);
    delay(WAIT_SWIPE);
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Hello LedLife");
  int seed = analogRead(11);
  delay(seed % 9);
  seed = seed * millis();
  Serial.println(seed);
  randomSeed(seed);
  matrix.begin();
  initWorld();
}

void render()
{  
  uint16_t color = getColor(false);
  for (byte r = 0; r < MAX_ROWS; r++)
  {
    for (byte c = 0;  c < MAX_COLS; c++)
    {
      uint16_t cc = 0;
      if (world[r][c] != 0)
      {
        cc = color;
      }
      matrix.drawPixel(c, r, cc);        
    }
  }
  matrix.swapBuffers(true);
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
    for (int l = -1; l <= 1; l++)
    {
      if (k || l)
      {
        if (world[yadd(r, l)][xadd(c, k)] != 0)
        {
          count++;
        }
      }
    }
  }
  return count;
}

void simulate()
{
  // Store new states in temporary matrix.
  for (byte r = 0; r < MAX_ROWS; r++)
  {
    for (byte c = 0;  c < MAX_COLS; c++)
    {
      int cells = getNumberOfAdjacentCells(r, c);
      if (cells == 2)
      {
        // stasis
        copyWorld[r][c] = world[r][c];
      }
      else if (cells == 3)
      {
        // growth
        copyWorld[r][c] = 1;//getColor(false);
      }
      else
      {
        // death
        copyWorld[r][c] = 0;
      }
    }
  }
  //
  // Copy back to world matrix.
  //
  int changed = 0;
  for (byte r = 0; r < MAX_ROWS; r++)
  {
    for (byte c = 0;  c < MAX_COLS; c++)
    {
      if (world[r][c] != copyWorld[r][c])
      {
        world[r][c] = copyWorld[r][c];
        changed++;
      }
    }
  }
  if (changed == lastChanged)
  {
    countDown--;
  }
  else
  {
    countDown = 10;    
  }
  lastChanged = changed;
  //
  // Re-populate the world?
  //
  if (changed == 0 || countDown <= 0)
  {
    delay(2000); // dramatic pause
    initWorld();
  }
}

void loop()
{
  render();
  delay(WAIT_MS);
  simulate();
}
