#define LEDARRAY_D 2
#define LEDARRAY_C 3
#define LEDARRAY_B 4
#define LEDARRAY_A 5
#define LEDARRAY_G 6
#define LEDARRAY_DI 7
#define LEDARRAY_CLK 8
#define LEDARRAY_LAT 9

#define WAIT_MS 1000
#define MAX_ROWS 16
#define MAX_COLS 16

unsigned char world[MAX_ROWS][MAX_COLS];

const unsigned char  world2[1][32] = 
{
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

unsigned char Display_Buffer[2];

void initWorld()
{
  randomSeed(analogRead(0));
  for (byte r = 0; r < MAX_ROWS; r++) 
  {
    for (byte c = 0;  c < MAX_COLS; c++)
    {
      //world[r][c] = random(1000) > 700 ? 1 : 0;
      world[r][c] = 0;
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
  pinMode(LEDARRAY_D, OUTPUT); 
  pinMode(LEDARRAY_C, OUTPUT);
  pinMode(LEDARRAY_B, OUTPUT);
  pinMode(LEDARRAY_A, OUTPUT);
  pinMode(LEDARRAY_G, OUTPUT);
  pinMode(LEDARRAY_DI, OUTPUT);
  pinMode(LEDARRAY_CLK, OUTPUT);
  pinMode(LEDARRAY_LAT, OUTPUT);  
  initWorld();
}

void render()
{
  Display(world2);
}

void Display(const unsigned char dat[][32])
{
  unsigned char i;
  for( i = 0 ; i < 16 ; i++ )
  {
    digitalWrite(LEDARRAY_G, HIGH);   
    
    Display_Buffer[0] = dat[0][i];    
    Display_Buffer[1] = dat[0][i+16];

    Send(Display_Buffer[1]);
    Send(Display_Buffer[0]);

    digitalWrite(LEDARRAY_LAT, HIGH);          
    delayMicroseconds(1);
  
    digitalWrite(LEDARRAY_LAT, LOW);
    delayMicroseconds(1);

    Scan_Line(i);             

    digitalWrite(LEDARRAY_G, LOW);
    
    delayMicroseconds(500);
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
    //delay(2000); // dramatic pause
  }  
}

int last = 0;
void loop()
{
  render();
  //delay(WAIT_MS);
  if (millis() - last >= 2000)
  {
    simulate();
    last = millis();
  }
}

void Scan_Line( unsigned char m)
{  
  switch(m)
  {
    case 0:     
      digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, LOW);          
      break;
    case 1:         
      digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, HIGH);     
      break;
    case 2:         
      digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, LOW);     
      break;
    case 3:         
      digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, HIGH);    
      break;
    case 4:
      digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, LOW);     
      break;
    case 5:
      digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, HIGH);    
      break;
    case 6:
      digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, LOW);    
      break;
    case 7:
      digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, HIGH);     
      break;
    case 8:
      digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, LOW);     
      break;
    case 9:
      digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, HIGH);    
      break;  
    case 10:
      digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, LOW);    
      break;
    case 11:
      digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, HIGH);     
      break;
    case 12:
      digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, LOW);    
      break;
    case 13:
      digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, HIGH);     
      break;
    case 14:
      digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, LOW);     
      break;
    case 15:
      digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, HIGH);    
      break;
    default : break;  
  }
}

//****************************************************
      
//****************************************************
void Send( unsigned char dat)
{
  unsigned char i;
  digitalWrite(LEDARRAY_CLK, LOW);
  delayMicroseconds(1);;  
  digitalWrite(LEDARRAY_LAT, LOW);
  delayMicroseconds(1);;

  for( i = 0 ; i < 8 ; i++ )
  {
    if( dat&0x01 )
    {
      //digitalWrite(LEDARRAY_DI, LOW);
      digitalWrite(LEDARRAY_DI, HIGH);  
    }
    else
    {
      //digitalWrite(LEDARRAY_DI, HIGH);  
      digitalWrite(LEDARRAY_DI, LOW);
    }

    delayMicroseconds(1);
    digitalWrite(LEDARRAY_CLK, HIGH);         
      delayMicroseconds(1);
    digitalWrite(LEDARRAY_CLK, LOW);
      delayMicroseconds(1);   
    dat >>= 1;
      
  }     
}
