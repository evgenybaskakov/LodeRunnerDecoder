/*

Created by Evgeny Baskakov, 2004


Original LODE RUNNER level decoder
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Cell code: XY, where X - row code, Y - item code.

Value interval for Y: [0, F] (16 possible values).


ENCODING TABLE:

0.  0: ladder     1.  0: concrete   2.  0: brick      3.  0: player
    1: concrete       1: ladder         1: empty          1: enemy
Ex  2: brick      Bx  2: empty      7x  2: ladder     Dx  2: N/A
    3: empty          3: brick          3: concrete       3: N/A
    4: chest          4: empty-2        4: brick-2        4: N/A
    5: empty-2        5: chest          5: bar            5: N/A
    6: brick-2        6: bar            6: chest          6: N/A
    7: bar            7: brick-2        7: empty-2        7: N/A
    8: N/A            8: N/A            8: player         8: brick
    9: N/A            9: N/A            9: enemy          9: empty
    A: player         A: enemy          A: N/A            A: ladder
    B: enemy          B: player         B: N/A            B: concrete
    С: N/A            С: N/A            С: N/A            С: brick-2
    D: N/A            D: N/A            D: N/A            D: bar
    E: N/A            E: N/A            E: N/A            E: chest
    F: N/A            F: N/A            F: N/A            F: empty-2

4.  0: enemy      5.  0: N/A        6.  0: N/A        7.  0: empty-2
    1: player         1: N/A            1: N/A            1: chest
Bx  2: N/A        Ex  2: N/A        5x  2: N/A        7x  2: bar
    3: N/A            3: N/A            3: N/A            3: brick-2
    4: N/A            4: enemy          4: enemy          4: concrete
    5: N/A            5: player         5: player         5: ladder
    6: N/A            6: N/A            6: N/A            6: empty
    7: N/A            7: N/A            7: N/A            7: brick
    8: empty          8: bar            8: bar            8: N/A
    9: brick          9: brick-2        9: brick-2        9: N/A
    A: concrete       A: empty-2        A: empty-2        A: N/A
    B: ladder         B: chest          B: chest          B: N/A
    С: bar            С: empty          С: empty          С: N/A
    D: brick-2        D: brick          D: brick          D: N/A
    E: empty-2        E: concrete       E: concrete       E: enemy
    F: chest          F: ladder         F: ladder         F: player

8.  0: N/A        9.  0: N/A        A.  0: chest      B.  0: N/A
    1: N/A            1: N/A            1: empty-2        1: N/A
2x  2: N/A        3x  2: player     9x  2: brick-2    1x  2: N/A
    3: N/A            3: enemy          3: bar            3: N/A
    4: N/A            4: N/A            4: ladder         4: player
    5: N/A            5: N/A            5: concrete       5: enemy
    6: enemy          6: N/A            6: brick          6: N/A
    7: player         7: N/A            7: empty          7: N/A
    8: empty-2        8: ladder         8: N/A            8: brick-2
    9: chest          9: concrete       9: N/A            9: bar
    A: bar            A: brick          A: N/A            A: chest
    B: brick-2        B: empty          B: N/A            B: empty-2
    С: concrete       С: chest          С: N/A            С: brick
    D: ladder         D: empty-2        D: N/A            D: empty
    E: empty          E: brick-2        E: player         E: ladder
    F: brick          F: bar            F: enemy          F: concrete

C.  0: N/A        D.  0: N/A        E.  0: brick-2    F.  0: chest
    1: N/A            1: N/A            1: bar            1: empty-2
Cx  2: player     8x  2: N/A        6x  2: chest      Cx  2: brick-2
    3: enemy          3: N/A            3: empty-2        3: bar
    4: N/A            4: N/A            4: brick          4: ladder
    5: N/A            5: N/A            5: empty          5: concrete
    6: N/A            6: enemy          6: ladder         6: brick
    7: N/A            7: player         7: concrete       7: empty
    8: ladder         8: empty-2        8: N/A            8: N/A (empty?)
    9: concrete       9: chest          9: N/A            9: N/A (empty?)
    A: brick          A: bar            A: N/A            A: N/A (empty?)
    B: empty          B: brick-2        B: N/A            B: N/A (empty?)
    С: chest          С: concrete       С: player         С: N/A (empty?)
    D: empty-2        D: ladder         D: enemy          D: N/A (empty?)
    E: brick-2        E: empty          E: N/A            E: player
    F: bar            F: brick          F: N/A            F: enemy


ENCODING METHOD:

0->1: odd/even positions are swapped
1->2: odd/even positions are swapped; then odd/even position pairs are swapped
2->3: 8-element tuples are swapped
3->4: odd/event positions are swapped
4->5: in each 8-element tuple position pairs are swapped: 1st and 3rd, 2nd and 4th
5->6: nothing changed
6->7: 8-element tuples are swapped; then odd/even position pairs are swapped
7->8: 8-element tuples are swapped
8->9: in each 8-element tuple position pairs are swapped: 1st and 3rd, 2nd and 4th;
      then odd/event positions are swapped
9->A: 8-element tuples are swapped;
      then in each 8-element tuple position pairs are swapped: 1st and 3rd, 2nd and 4th
A->B: 8-element tuples are swapped; then odd/even position pairs are swapped
B->C: in each 8-element tuple position pairs are swapped: 1st and 4th, 2nd and 3rd
С->D: in each 8-element tuple position pairs are swapped: 1st and 3rd, 2nd and 4th;
      then odd/event positions are swapped
D->E: 8-element tuples are swapped;
      then in each 8-element tuple position pairs are swapped: 1st and 2nd, 3rd and 4th;
      then odd/event positions are swapped
E->F: odd/even positions are swapped

*/


#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>


#define LEVEL_SIZE 0x200
#define COLUMN_SIZE 16
#define POS_NUM 16
#define COLUMNS_NUM 26

#define ERROR     -1
#define LADDER     0
#define CONCRETE   1
#define BRICK      2
#define EMPTY      3
#define GOLD       4
#define EMPTY_2    5
#define BRICK_2    6
#define POLE       7
#define PLAYER     8
#define ENEMY      9


const int row_codes[COLUMN_SIZE] = {
  0xE, 0xB, 0x7, 0xD, 0xB, 0xE, 0x5, 0x7,
  0x2, 0x3, 0x9, 0x1, 0xC, 0x8, 0x6, 0xC
};


int cell_codes[COLUMN_SIZE][POS_NUM];
int level[COLUMN_SIZE][COLUMNS_NUM];
int print_raw_level = 0;

void init_level() {
   int i, j;

   for(i = 0; i < COLUMN_SIZE; i++)
      for(j = 0; j < COLUMNS_NUM; j++)
         level[i][j] = 0;
}


void init_cell_codes() {
  // init 0th element
  cell_codes[0][ 0] = LADDER;
  cell_codes[0][ 1] = CONCRETE;
  cell_codes[0][ 2] = BRICK;
  cell_codes[0][ 3] = EMPTY;
  cell_codes[0][ 4] = GOLD;
  cell_codes[0][ 5] = EMPTY_2;
  cell_codes[0][ 6] = BRICK_2;
  cell_codes[0][ 7] = POLE;
  cell_codes[0][ 8] = ERROR;
  cell_codes[0][ 9] = ERROR;
  cell_codes[0][10] = PLAYER;
  cell_codes[0][11] = ENEMY;
  cell_codes[0][12] = ERROR;
  cell_codes[0][13] = ERROR;
  cell_codes[0][14] = ERROR;
  cell_codes[0][15] = ERROR;

  // init 1st element
  cell_codes[1][ 0] = cell_codes[0][1];
  cell_codes[1][ 1] = cell_codes[0][0];
  cell_codes[1][ 2] = cell_codes[0][3];
  cell_codes[1][ 3] = cell_codes[0][2];
  cell_codes[1][ 4] = cell_codes[0][5];
  cell_codes[1][ 5] = cell_codes[0][4];
  cell_codes[1][ 6] = cell_codes[0][7];
  cell_codes[1][ 7] = cell_codes[0][6];
  cell_codes[1][ 8] = cell_codes[0][9];
  cell_codes[1][ 9] = cell_codes[0][8];
  cell_codes[1][10] = cell_codes[0][11];
  cell_codes[1][11] = cell_codes[0][10];
  cell_codes[1][12] = cell_codes[0][13];
  cell_codes[1][13] = cell_codes[0][12];
  cell_codes[1][14] = cell_codes[0][15];
  cell_codes[1][15] = cell_codes[0][14];

  // init 2nd element
  cell_codes[2][ 0] = cell_codes[1][3];
  cell_codes[2][ 1] = cell_codes[1][2];
  cell_codes[2][ 2] = cell_codes[1][1];
  cell_codes[2][ 3] = cell_codes[1][0];
  cell_codes[2][ 4] = cell_codes[1][7];
  cell_codes[2][ 5] = cell_codes[1][6];
  cell_codes[2][ 6] = cell_codes[1][5];
  cell_codes[2][ 7] = cell_codes[1][4];
  cell_codes[2][ 8] = cell_codes[1][11];
  cell_codes[2][ 9] = cell_codes[1][10];
  cell_codes[2][10] = cell_codes[1][9];
  cell_codes[2][11] = cell_codes[1][8];
  cell_codes[2][12] = cell_codes[1][15];
  cell_codes[2][13] = cell_codes[1][14];
  cell_codes[2][14] = cell_codes[1][13];
  cell_codes[2][15] = cell_codes[1][12];

  // init 3rh element
  cell_codes[3][ 0] = cell_codes[2][8];
  cell_codes[3][ 1] = cell_codes[2][9];
  cell_codes[3][ 2] = cell_codes[2][10];
  cell_codes[3][ 3] = cell_codes[2][11];
  cell_codes[3][ 4] = cell_codes[2][12];
  cell_codes[3][ 5] = cell_codes[2][13];
  cell_codes[3][ 6] = cell_codes[2][14];
  cell_codes[3][ 7] = cell_codes[2][15];
  cell_codes[3][ 8] = cell_codes[2][0];
  cell_codes[3][ 9] = cell_codes[2][1];
  cell_codes[3][10] = cell_codes[2][2];
  cell_codes[3][11] = cell_codes[2][3];
  cell_codes[3][12] = cell_codes[2][4];
  cell_codes[3][13] = cell_codes[2][5];
  cell_codes[3][14] = cell_codes[2][6];
  cell_codes[3][15] = cell_codes[2][7];

  // init 4th element
  cell_codes[4][ 0] = cell_codes[3][1];
  cell_codes[4][ 1] = cell_codes[3][0];
  cell_codes[4][ 2] = cell_codes[3][3];
  cell_codes[4][ 3] = cell_codes[3][2];
  cell_codes[4][ 4] = cell_codes[3][5];
  cell_codes[4][ 5] = cell_codes[3][4];
  cell_codes[4][ 6] = cell_codes[3][7];
  cell_codes[4][ 7] = cell_codes[3][6];
  cell_codes[4][ 8] = cell_codes[3][9];
  cell_codes[4][ 9] = cell_codes[3][8];
  cell_codes[4][10] = cell_codes[3][11];
  cell_codes[4][11] = cell_codes[3][10];
  cell_codes[4][12] = cell_codes[3][13];
  cell_codes[4][13] = cell_codes[3][12];
  cell_codes[4][14] = cell_codes[3][15];
  cell_codes[4][15] = cell_codes[3][14];

  // init 5th element
  cell_codes[5][ 0] = cell_codes[4][4];
  cell_codes[5][ 1] = cell_codes[4][5];
  cell_codes[5][ 2] = cell_codes[4][6];
  cell_codes[5][ 3] = cell_codes[4][7];
  cell_codes[5][ 4] = cell_codes[4][0];
  cell_codes[5][ 5] = cell_codes[4][1];
  cell_codes[5][ 6] = cell_codes[4][2];
  cell_codes[5][ 7] = cell_codes[4][3];
  cell_codes[5][ 8] = cell_codes[4][12];
  cell_codes[5][ 9] = cell_codes[4][13];
  cell_codes[5][10] = cell_codes[4][14];
  cell_codes[5][11] = cell_codes[4][15];
  cell_codes[5][12] = cell_codes[4][8];
  cell_codes[5][13] = cell_codes[4][9];
  cell_codes[5][14] = cell_codes[4][10];
  cell_codes[5][15] = cell_codes[4][11];

  // init 6th element
  cell_codes[6][ 0] = cell_codes[5][0];
  cell_codes[6][ 1] = cell_codes[5][1];
  cell_codes[6][ 2] = cell_codes[5][2];
  cell_codes[6][ 3] = cell_codes[5][3];
  cell_codes[6][ 4] = cell_codes[5][4];
  cell_codes[6][ 5] = cell_codes[5][5];
  cell_codes[6][ 6] = cell_codes[5][6];
  cell_codes[6][ 7] = cell_codes[5][7];
  cell_codes[6][ 8] = cell_codes[5][8];
  cell_codes[6][ 9] = cell_codes[5][9];
  cell_codes[6][10] = cell_codes[5][10];
  cell_codes[6][11] = cell_codes[5][11];
  cell_codes[6][12] = cell_codes[5][12];
  cell_codes[6][13] = cell_codes[5][13];
  cell_codes[6][14] = cell_codes[5][14];
  cell_codes[6][15] = cell_codes[5][15];

  // init 7th element
  cell_codes[7][ 0] = cell_codes[6][10];
  cell_codes[7][ 1] = cell_codes[6][11];
  cell_codes[7][ 2] = cell_codes[6][8];
  cell_codes[7][ 3] = cell_codes[6][9];
  cell_codes[7][ 4] = cell_codes[6][14];
  cell_codes[7][ 5] = cell_codes[6][15];
  cell_codes[7][ 6] = cell_codes[6][12];
  cell_codes[7][ 7] = cell_codes[6][13];
  cell_codes[7][ 8] = cell_codes[6][2];
  cell_codes[7][ 9] = cell_codes[6][3];
  cell_codes[7][10] = cell_codes[6][0];
  cell_codes[7][11] = cell_codes[6][1];
  cell_codes[7][12] = cell_codes[6][6];
  cell_codes[7][13] = cell_codes[6][7];
  cell_codes[7][14] = cell_codes[6][4];
  cell_codes[7][15] = cell_codes[6][5];

  // init 8th element
  cell_codes[8][ 0] = cell_codes[7][8];
  cell_codes[8][ 1] = cell_codes[7][9];
  cell_codes[8][ 2] = cell_codes[7][10];
  cell_codes[8][ 3] = cell_codes[7][11];
  cell_codes[8][ 4] = cell_codes[7][12];
  cell_codes[8][ 5] = cell_codes[7][13];
  cell_codes[8][ 6] = cell_codes[7][14];
  cell_codes[8][ 7] = cell_codes[7][15];
  cell_codes[8][ 8] = cell_codes[7][0];
  cell_codes[8][ 9] = cell_codes[7][1];
  cell_codes[8][10] = cell_codes[7][2];
  cell_codes[8][11] = cell_codes[7][3];
  cell_codes[8][12] = cell_codes[7][4];
  cell_codes[8][13] = cell_codes[7][5];
  cell_codes[8][14] = cell_codes[7][6];
  cell_codes[8][15] = cell_codes[7][7];

  // init 9th element
  cell_codes[9][ 0] = cell_codes[8][5];
  cell_codes[9][ 1] = cell_codes[8][4];
  cell_codes[9][ 2] = cell_codes[8][7];
  cell_codes[9][ 3] = cell_codes[8][6];
  cell_codes[9][ 4] = cell_codes[8][1];
  cell_codes[9][ 5] = cell_codes[8][0];
  cell_codes[9][ 6] = cell_codes[8][3];
  cell_codes[9][ 7] = cell_codes[8][2];
  cell_codes[9][ 8] = cell_codes[8][13];
  cell_codes[9][ 9] = cell_codes[8][12];
  cell_codes[9][10] = cell_codes[8][15];
  cell_codes[9][11] = cell_codes[8][14];
  cell_codes[9][12] = cell_codes[8][9];
  cell_codes[9][13] = cell_codes[8][8];
  cell_codes[9][14] = cell_codes[8][11];
  cell_codes[9][15] = cell_codes[8][10];

  // init 10th element
  cell_codes[10][ 0] = cell_codes[9][12];
  cell_codes[10][ 1] = cell_codes[9][13];
  cell_codes[10][ 2] = cell_codes[9][14];
  cell_codes[10][ 3] = cell_codes[9][15];
  cell_codes[10][ 4] = cell_codes[9][8];
  cell_codes[10][ 5] = cell_codes[9][9];
  cell_codes[10][ 6] = cell_codes[9][10];
  cell_codes[10][ 7] = cell_codes[9][11];
  cell_codes[10][ 8] = cell_codes[9][4];
  cell_codes[10][ 9] = cell_codes[9][5];
  cell_codes[10][10] = cell_codes[9][6];
  cell_codes[10][11] = cell_codes[9][7];
  cell_codes[10][12] = cell_codes[9][0];
  cell_codes[10][13] = cell_codes[9][1];
  cell_codes[10][14] = cell_codes[9][2];
  cell_codes[10][15] = cell_codes[9][3];

  // init 11th element
  cell_codes[11][ 0] = cell_codes[10][10];
  cell_codes[11][ 1] = cell_codes[10][11];
  cell_codes[11][ 2] = cell_codes[10][8];
  cell_codes[11][ 3] = cell_codes[10][9];
  cell_codes[11][ 4] = cell_codes[10][14];
  cell_codes[11][ 5] = cell_codes[10][15];
  cell_codes[11][ 6] = cell_codes[10][12];
  cell_codes[11][ 7] = cell_codes[10][13];
  cell_codes[11][ 8] = cell_codes[10][2];
  cell_codes[11][ 9] = cell_codes[10][3];
  cell_codes[11][10] = cell_codes[10][0];
  cell_codes[11][11] = cell_codes[10][1];
  cell_codes[11][12] = cell_codes[10][6];
  cell_codes[11][13] = cell_codes[10][7];
  cell_codes[11][14] = cell_codes[10][4];
  cell_codes[11][15] = cell_codes[10][5];

  // init 12th element
  cell_codes[12][ 0] = cell_codes[11][6];
  cell_codes[12][ 1] = cell_codes[11][7];
  cell_codes[12][ 2] = cell_codes[11][4];
  cell_codes[12][ 3] = cell_codes[11][5];
  cell_codes[12][ 4] = cell_codes[11][2];
  cell_codes[12][ 5] = cell_codes[11][3];
  cell_codes[12][ 6] = cell_codes[11][0];
  cell_codes[12][ 7] = cell_codes[11][1];
  cell_codes[12][ 8] = cell_codes[11][14];
  cell_codes[12][ 9] = cell_codes[11][15];
  cell_codes[12][10] = cell_codes[11][12];
  cell_codes[12][11] = cell_codes[11][13];
  cell_codes[12][12] = cell_codes[11][10];
  cell_codes[12][13] = cell_codes[11][11];
  cell_codes[12][14] = cell_codes[11][8];
  cell_codes[12][15] = cell_codes[11][9];

  // init 13th element
  cell_codes[13][ 0] = cell_codes[12][5];
  cell_codes[13][ 1] = cell_codes[12][4];
  cell_codes[13][ 2] = cell_codes[12][7];
  cell_codes[13][ 3] = cell_codes[12][6];
  cell_codes[13][ 4] = cell_codes[12][1];
  cell_codes[13][ 5] = cell_codes[12][0];
  cell_codes[13][ 6] = cell_codes[12][3];
  cell_codes[13][ 7] = cell_codes[12][2];
  cell_codes[13][ 8] = cell_codes[12][13];
  cell_codes[13][ 9] = cell_codes[12][12];
  cell_codes[13][10] = cell_codes[12][15];
  cell_codes[13][11] = cell_codes[12][14];
  cell_codes[13][12] = cell_codes[12][9];
  cell_codes[13][13] = cell_codes[12][8];
  cell_codes[13][14] = cell_codes[12][11];
  cell_codes[13][15] = cell_codes[12][10];

  // init 14th element
  cell_codes[14][ 0] = cell_codes[13][11];
  cell_codes[14][ 1] = cell_codes[13][10];
  cell_codes[14][ 2] = cell_codes[13][9];
  cell_codes[14][ 3] = cell_codes[13][8];
  cell_codes[14][ 4] = cell_codes[13][15];
  cell_codes[14][ 5] = cell_codes[13][14];
  cell_codes[14][ 6] = cell_codes[13][13];
  cell_codes[14][ 7] = cell_codes[13][12];
  cell_codes[14][ 8] = cell_codes[13][3];
  cell_codes[14][ 9] = cell_codes[13][2];
  cell_codes[14][10] = cell_codes[13][1];
  cell_codes[14][11] = cell_codes[13][0];
  cell_codes[14][12] = cell_codes[13][7];
  cell_codes[14][13] = cell_codes[13][6];
  cell_codes[14][14] = cell_codes[13][5];
  cell_codes[14][15] = cell_codes[13][4];

  // init 15th element
  cell_codes[15][ 0] = cell_codes[14][2];
  cell_codes[15][ 1] = cell_codes[14][3];
  cell_codes[15][ 2] = cell_codes[14][0];
  cell_codes[15][ 3] = cell_codes[14][1];
  cell_codes[15][ 4] = cell_codes[14][6];
  cell_codes[15][ 5] = cell_codes[14][7];
  cell_codes[15][ 6] = cell_codes[14][4];
  cell_codes[15][ 7] = cell_codes[14][5];
  cell_codes[15][ 8] = cell_codes[14][10];
  cell_codes[15][ 9] = cell_codes[14][11];
  cell_codes[15][10] = cell_codes[14][8];
  cell_codes[15][11] = cell_codes[14][9];
  cell_codes[15][12] = cell_codes[14][14];
  cell_codes[15][13] = cell_codes[14][15];
  cell_codes[15][14] = cell_codes[14][12];
  cell_codes[15][15] = cell_codes[14][13];
}


void bad_code(int code, int n, int col_no) {
   printf("bad code %X, column %d, pos %d\n", code, col_no, n);
   exit(1);
}


int decode(int code, int n, int col_no) {
   assert(n >= 0 && n < COLUMN_SIZE);

   // code adjustment for odd columns
   if(col_no % 2 == 1) {
      if(n % 2 == 0)
         n++;
      else
         n--;
   }

   // check the code
   if((code >> 4) != row_codes[n])
      bad_code(code, n, col_no);

   // do decode
   return cell_codes[n][code & 0x0f];
}


void decode_column(const char *buffer, int col_no) {
   int i;

   for(i = 0; i < COLUMN_SIZE; i++) {
      int elem = decode(((int)buffer[i]) & 0xFF, i, col_no);

      //printf("%02d ", elem);

      level[i][col_no] = elem;
   }

   //printf("\n");
}



void print_level() {
   int i, j;

   if(print_raw_level) {
      const int M_NOTHING  = 0;
      const int M_BRICK    = 1;
      const int M_CONCRETE = 2;
      const int M_LADDER   = 3;
      const int M_GOLD     = 4;
      const int M_PLAYER   = 5;
      const int M_ENEMY    = 6;
      const int M_POLE     = 7;

      printf("%d %d\n", COLUMNS_NUM, COLUMN_SIZE);

      for(i = 0; i < COLUMN_SIZE; i++) {
         for(j = 0; j < COLUMNS_NUM; j++) {
            int c;

            switch(level[i][j]) {
               case LADDER:    c = M_LADDER;    break;
               case CONCRETE:  c = M_CONCRETE;  break;
               case BRICK:     c = M_BRICK;     break;
               case EMPTY:     c = M_NOTHING;   break;
               case GOLD:      c = M_GOLD;      break;
               case EMPTY_2:   c = M_NOTHING;   break;
               case BRICK_2:   c = M_BRICK;     break;
               case POLE:      c = M_POLE;      break;
               case PLAYER:    c = M_PLAYER;    break;
               case ENEMY:     c = M_ENEMY;     break;
               default:
                  assert(0);
            }

            printf("%02d ", c);
         }

         printf("\n");
      }
   } else {
      char *ch;

      printf("    ");

      for(j = 0; j < COLUMNS_NUM; j++)
         printf(" %02d", j);

      printf("\n");

      for(i = 0; i < COLUMN_SIZE; i++) {
         printf("#%02d ", i);

         for(j = 0; j < COLUMNS_NUM; j++) {
            switch(level[i][j]) {
               case LADDER:    ch = "├─┤"; break;
               case CONCRETE:  ch = "███"; break;
               case BRICK:     ch = "▒▒▒"; break;
               case EMPTY:     ch = "..."; break;
               case GOLD:      ch = " o "; break;
               case EMPTY_2:   ch = ".?."; break;
               case BRICK_2:   ch = "▓?▓"; break;
               case POLE:      ch = "───"; break;
               case PLAYER:    ch = " P "; break;
               case ENEMY:     ch = " E "; break;
               default:
                  assert(0);
            }

            printf("%s", ch);
         }

         printf("\n");
      }
   }
}


int main(int argc, char **argv) {
  char buffer[COLUMN_SIZE];
  char *filename;
  FILE *fin;
  FILE *fout = stdout;
  size_t rd;
  int level_no;
  int ch;
  int cnt;
  int i;

  if(argc != 2 && argc != 3 && argc != 4) {
     printf("decode <datafile> [level [raw]]\n");
     return 1;
  }

  if(argc >= 3) {
     const char *lno = argv[2];

     level_no = atoi(lno) - 1;

     if(level_no < 0) {
        printf("level must be > 0\n");
        return 1;
     }
  } else {
     level_no = 0;
  }

  if(argc == 4) {
     const char *raw = argv[3];

     if(strcmp(raw, "raw") == 0) {
        print_raw_level = 1;
     } else {
        printf("unknown parameter '%s'\n", raw);
        return 1;
     }
  }

  filename = argv[1];
  fin = fopen(filename, "rb");

  if(!fin) {
     printf("cannot open %s\n", filename);
     return 1;
  }

  init_cell_codes();
  init_level();

  if(fseek(fin, LEVEL_SIZE * level_no, SEEK_SET) != 0) {
     printf("cannot read level %d (%s)\n", level_no + 1, strerror(errno));
     return 1;
  }

  for(cnt = 0; cnt < COLUMNS_NUM; cnt++) {
     if((rd = fread(buffer, 1, COLUMN_SIZE, fin)) != COLUMN_SIZE) {
        if(rd == 0)
           printf("cannot read level %d\n", level_no);
        else
           printf("error reading level %d: read only %lu bytes instead of %d\n", level_no, rd, COLUMN_SIZE);

        return 1;
     }

     decode_column(buffer, cnt);
  }

  fclose(fin);

  print_level();

  return 0;
}
