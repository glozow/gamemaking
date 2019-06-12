NAME:        mchange.c
**
** DESCRIPTION: Change!
**
** AUTHOR:      Alice Chang & Judy Tuan  -  University of California at Berkeley
**              Copyright (C) Alice Chang & Judy Tuan, 2002. All rights reserved.
**
** DATE:        2002-10-08
**
** UPDATE HIST: 2004-04-14 Upgraded to work with Gamesman3
**              2019-04-29 Upgraded to be generic (Gloria Zhao)
**
**
**************************************************************************/

/*************************************************************************
**
** Everything below here must be in every game file
**
**************************************************************************/

#include <stdio.h>
#include "gamesman.h"

/* The beginning position is:
**
**     O   O   O          1   2   3      which corresponds to 1*(3^0) +
**    / \ / \ / \        / \ / \ / \     1*(3^1) + 1*(3^2) + 2*(3^11) +
**   -   -   -   -      4   5   6   7    2*(3^12) + 2*(3^13) = 4605835
**  / \ / \ / \ /      / \ / \ / \ /
** -   -   -   -      8   9   10  11
**  \ / \ / \ /        \ / \ / \ /
**   X   X   X          12  13  14
**
*/

/* The beginning position is:
**
**       O   O   O   O
**      / \ / \ / \ / \
**     -   -   -   -  -
**    / \ / \ / \ / \ /
**   -   -   -   -  -
**  / \ / \ / \ / \ /
** -   -   -   -  -
** \ / \ / \ / \ /
**  x   x   x   x
**
**       1   2   3   4
**      / \ / \ / \ / \
**     5   6   7   8  9
**    / \ / \ / \ / \ /
**   10  11  12  13 14
**  / \ / \ / \ / \ /
**15  16  17  18  19
** \ / \ / \ / \ /
** 20  21  22  23
*/

POSITION gNumberOfPositions  = 86093442;

POSITION gInitialPosition    = 84499132;
POSITION kBadPosition        = -1; /* This can never be the rep. of a position */

STRING kAuthorName         = "Alice Chang and Judy Tuan";
STRING kGameName           = "Change!";
STRING kDBName             = "change";
BOOLEAN kPartizan           = TRUE;
BOOLEAN kDebugMenu          = TRUE;
BOOLEAN kGameSpecificMenu   = TRUE;
BOOLEAN kTieIsPossible      = FALSE;
BOOLEAN kLoopy               = FALSE;
BOOLEAN kDebugDetermineValue = FALSE;
void*    gGameSpecificTclInit = NULL;

STRING kHelpGraphicInterface =
        "The LEFT button puts an X or O (depending on whether you went first\n\
or second) on the spot the cursor was on when you clicked. The MIDDLE\n\
button does nothing, and the RIGHT button is the same as UNDO, in that\n\
it reverts back to your most recent position."                                                                                                                                                                                                                                   ;

STRING kHelpTextInterface    =
        "Players alternate turns moving one of their pieces along the lines. First\n\
select the piece by typing in the number the piece is in. Then select where\n\
you want to move the piece by typing in the corresponding space. Pieces may\n\
not move backward, jump pieces or turn corners "                                                                                                                                                                                                                                                    ;

STRING kHelpOnYourTurn =
        "Slide your piece to an empty space by first selecting your piece number\n\
the space number."                                                                                    ;

STRING kHelpStandardObjective =
        "To be the first player to occupy your opponent's spaces.";

STRING kHelpReverseObjective =
        "To force your opponent into getting three of his markers (either X or\n\
O) in a row, either horizontally, vertically, or diagonally. 3-in-a-row\n\
LOSES."                                                                                                                                                             ;

STRING kHelpTieOccursWhen =   /* Should follow 'A Tie occurs when... */
                            "the board fills up without either player getting three-in-a-row.";

STRING kHelpExample =
        "         ( 1 2 3 )           : - - -\n\
LEGEND:  ( 4 5 6 )  TOTAL:   : - - - \n\
         ( 7 8 9 )           : - - - \n\n\
Computer's move              :  3    \n\n\
         ( 1 2 3 )           : - - X \n\
LEGEND:  ( 4 5 6 )  TOTAL:   : - - - \n\
         ( 7 8 9 )           : - - - \n\n\
     Dan's move [(u)ndo/1-9] : { 2 } \n\n\
         ( 1 2 3 )           : - O X \n\
LEGEND:  ( 4 5 6 )  TOTAL:   : - - - \n\
         ( 7 8 9 )           : - - - \n\n\
Computer's move              :  6    \n\n\
         ( 1 2 3 )           : - O X \n\
LEGEND:  ( 4 5 6 )  TOTAL:   : - - X \n\
         ( 7 8 9 )           : - - - \n\n\
     Dan's move [(u)ndo/1-9] : { 9 } \n\n\
         ( 1 2 3 )           : - O X \n\
LEGEND:  ( 4 5 6 )  TOTAL:   : - - X \n\
         ( 7 8 9 )           : - - O \n\n\
Computer's move              :  5    \n\n\
         ( 1 2 3 )           : - O X \n\
LEGEND:  ( 4 5 6 )  TOTAL:   : - X X \n\
         ( 7 8 9 )           : - - O \n\n\
     Dan's move [(u)ndo/1-9] : { 7 } \n\n\
         ( 1 2 3 )           : - O X \n\
LEGEND:  ( 4 5 6 )  TOTAL:   : - X X \n\
         ( 7 8 9 )           : O - O \n\n\
Computer's move              :  4    \n\n\
         ( 1 2 3 )           : - O X \n\
LEGEND:  ( 4 5 6 )  TOTAL:   : X X X \n\
         ( 7 8 9 )           : O - O \n\n\
Computer wins. Nice try, Dan."    ;
STRING MoveToString(MOVE);

/*************************************************************************
**
** Everything above here must be in every game file
**
**************************************************************************/

/*************************************************************************
**
** Every variable declared here is only used in this file (game-specific)
**
**************************************************************************/
int WIDTH = 3;
int HEIGHT = 2;
int CORNER1 = 3;
int CORNER2 = 12;
int BOARDSIZE = 16;
uint64_t POSITION_OFFSET = 43046721;
#define BADSLOT -2                /* slot does not exist */
#define MAXH 3
#define MINH 1
#define MAXW 5
#define MINW 1

typedef enum possibleBoardPieces {
	Blank, o, x
} BlankOX;

typedef int SLOT;    /* A slot is the place where a piece moves from or to*/

char *gBlankOXString[] = { "-", "O", "X" };

/* Powers of 3 - this is the way I encode the position, as an integer */
uint64_t g3Array[] = { 1, 3, 9, 27, 81, 243, 729, 2187, 6561, 19683, 59049,
	          177147, 531441, 1594323, 4782969, 14348907, 43046721, 129140163, 387420489,
            1162261467, 3486784401, 10460353203, 31381059609, 94143178827, 282429536481 };

BOOLEAN gToTrapIsToWin = TRUE;  /* Being stuck means you lose. */

/* local function prototypes */
void PositionToBlankOX(POSITION, BlankOX*, BlankOX*);
POSITION BlankOXToPosition(BlankOX*, BlankOX);
BOOLEAN OkMove(BlankOX*, BlankOX, SLOT, int, int, SLOT*);
BOOLEAN FullSlots(BlankOX[]);
BOOLEAN Trapped(POSITION);
void MoveToSlots(MOVE, SLOT*, SLOT*);
SLOT GetToSlot(BlankOX, SLOT, int, int);
MOVE SlotsToMove(SLOT fromSlot, SLOT toSlot);

/************************************************************************
**
**              HELPER FUNCTIONS
**
************************************************************************/
int ConvertIndexToOutput(SLOT slot) {
  if (slot <= WIDTH) {
    slot++;
  } else if (slot < BOARDSIZE-WIDTH-1) {
    // fromSlot = fromSlot;
  } else {
    slot--;
  }
  return slot;
}

int ConvertOutputToIndex(SLOT slot) {
  if (slot <= WIDTH) {
    slot--;
  } else if (slot < BOARDSIZE-WIDTH-1) {
    // fromSlot = fromSlot;
  } else {
    slot++;
  }
  return slot;
}
int PairToIndex(int row, int col) {
  return (WIDTH + 1) * row + col;
}

int IndexToRow(int index) {
  return index / (WIDTH + 1);
}

int IndexToCol(int index) {
  return index % (WIDTH + 1);
}

void SetGenericNumPositions() {
  BOARDSIZE = (WIDTH + 1) * (HEIGHT + 2);
  gNumberOfPositions = g3Array[BOARDSIZE]*2;

}

void SetGenericPositionOffset() {
  BOARDSIZE = (WIDTH + 1) * (HEIGHT + 2);
  POSITION_OFFSET = g3Array[BOARDSIZE];
}


/************************************************************************
**
**        PRINTING HELPER FUNCTIONS
**
************************************************************************/

void PrintBoardString(POSITION position) {
  BlankOX theBlankOx[BOARDSIZE], whoseturn;
  PositionToBlankOX(position,theBlankOx,&whoseturn);
  int i;
  // FIRST ROW
  printf("\n");
  for (i = 1; i <= WIDTH; i++) {
    printf("%s ", gBlankOXString[(int)theBlankOx[ConvertOutputToIndex(i)]]);
  }
  printf("\n");
  // MIDDLE ROWS (THERE ARE HEIGHT OF THEM)
  for (i = WIDTH+1; i < BOARDSIZE-WIDTH-1; i++) {
    if (i > WIDTH+1 && IndexToCol(i) == 0) {
      printf("\n");
    }
    printf("%s ", gBlankOXString[(int)theBlankOx[ConvertOutputToIndex(i)]]);
  }
  // LAST ROW
  printf("\n");
  for (i = BOARDSIZE-WIDTH-1; i <= BOARDSIZE-2; i++) {
    printf("%s ", gBlankOXString[(int)theBlankOx[ConvertOutputToIndex(i)]]);
  }
}

void PrintReferenceBoard() {
  // FIRST ROW
  printf("  ");
  int i;
  for (i = 1; i <= WIDTH; i++) {
    printf("%d   ", i);
  }
  printf("\n");
  for (int w = 0; w < WIDTH; w++) {
    printf(" / \\");
  }
  printf("\n");

  // MIDDLE ROWS (THERE ARE HEIGHT OF THEM)
  for (i = WIDTH+1; i < BOARDSIZE-WIDTH-1; i++) {
    if (i > WIDTH+1 && IndexToCol(i) == 0) {
      printf("\n");
      for (int w = 0; w < WIDTH; w++) {
        if ((i-WIDTH) / 10 >= 1)
          printf("| \\ ");
        else
          printf("| \\ ");
      }
      printf("|  \n");
    }
    if (i / 10 < 1)
      printf("%d   ", i);
    else
      printf("%d  ", i);
  }
  // LAST ROW
  printf("\n");
  for (int w = 0; w < WIDTH; w++) {
    printf(" \\ /");
  }
  printf("\n  ");
  for (i = BOARDSIZE-WIDTH-1; i <= BOARDSIZE-2; i++) {
    printf("%d  ", i);
  }
}

void PrintActualBoard(POSITION position) {
  BlankOX theBlankOx[BOARDSIZE], whoseturn;
  PositionToBlankOX(position,theBlankOx,&whoseturn);
  int i;
  // FIRST ROW
  printf("\n  ");
  for (i = 1; i <= WIDTH; i++) {
    printf("%s   ", gBlankOXString[(int)theBlankOx[ConvertOutputToIndex(i)]]);
  }
  printf("\n");
  for (int w = 0; w < WIDTH; w++) {
    printf(" / \\");
  }
  printf("\n");

  // MIDDLE ROWS (THERE ARE HEIGHT OF THEM)
  for (i = WIDTH+1; i < BOARDSIZE-WIDTH-1; i++) {
    if (i > WIDTH+1 && IndexToCol(i) == 0) {
      printf("\n");
      for (int w = 0; w < WIDTH; w++) {
        printf("| \\ ");
      }
      printf("| \n");
    }
    printf("%s   ", gBlankOXString[(int)theBlankOx[ConvertOutputToIndex(i)]]);
  }
  // LAST ROW
  printf("\n");
  for (int w = 0; w < WIDTH; w++) {
    printf(" \\ /");
  }
  printf("\n  ");
  for (i = BOARDSIZE-WIDTH-1; i <= BOARDSIZE-2; i++) {
    printf("%s   ", gBlankOXString[(int)theBlankOx[ConvertOutputToIndex(i)]]);
  }
}


void SetGenericInitialPosition() {
  BOARDSIZE = (WIDTH + 1) * (HEIGHT + 2);
  CORNER1 = WIDTH;
  CORNER2 = BOARDSIZE - WIDTH - 1;

  BlankOX theBlankOX[BOARDSIZE], whoseTurn;
  whoseTurn = x;
  int i;
  for (i = 0; i < CORNER1; i++) {
    theBlankOX[i] = o;
  }
  for (i = CORNER1; i <= CORNER2; i++) {
    theBlankOX[i] = Blank;
  }
  for (i = CORNER2+1; i <= BOARDSIZE-1; i++) {
    theBlankOX[i] = x;
  }
  for (int i = 0; i < BOARDSIZE; i ++) {

  }
  theBlankOX[CORNER1] = Blank;
  theBlankOX[CORNER2] = Blank;

  gInitialPosition = BlankOXToPosition(theBlankOX, whoseTurn);
}

/************************************************************************
**
** NAME:        InitializeGame
**
** DESCRIPTION: Initializes the internal game variables
**
************************************************************************/

void InitializeGame() {
  SetGenericNumPositions();
  SetGenericPositionOffset();
  // SetGenericInitialPosition();
	gMoveToStringFunPtr = &MoveToString;
}


/************************************************************************
**
** NAME:        DebugMenu
**
** DESCRIPTION: Menu used to debug internal problems. Does nothing if
**              kDebugMenu == FALSE
**
************************************************************************/

void DebugMenu()
{
}

/************************************************************************
**
** NAME:        GameSpecificMenu
**
** DESCRIPTION: Menu used to change game-specific parameters, such as
**              the side of the board in an nxn Nim board, etc. Does
**              nothing if kGameSpecificMenu == FALSE
**
************************************************************************/

void GameSpecificMenu() {
  printf("\n\nGameSpecificMenu()\n\n");
  int temp;
  char tChar;
  do {
    printf("\n\t----- Game-specific options for %s -----\n\n", kGameName);

    printf("\tCurrent Initial Position:\n");
    PrintPosition(gInitialPosition, gPlayerName[kPlayerOneTurn], kHumansTurn);

    printf("\tI)\tChoose the (I)nitial position\n");
    printf("\n\tT)\t(T)rapping opponent toggle from %s to %s\n",
           gToTrapIsToWin ? "GOOD (WINNING)" : "BAD (LOSING)",
           !gToTrapIsToWin ? "GOOD (WINNING)" : "BAD (LOSING)");
    printf("\n\tW)\t(W)idth = Set Width of board.\n \t\tThe inital position will be reset to the default for the new board.");
    printf("\n\n\tH)\t(H)eight = Set Height of board.\n \t\tThe inital position will be reset to the default for the new board.");
    printf("\n\n\tB)\t(B)ack = Return to previous activity.\n");
    printf("\n\nSelect an option: ");

    switch(GetMyChar()) {
    case 'B': case 'b':
      return;
    case 'Q': case 'q':
      ExitStageRight();

    case 'W': case 'w':
      printf("Enter a width (%d through %d): ",MINW,MAXW); /* 1 through 4 */
      temp = GetMyInt();

      while(temp > MAXW || temp < MINW) {
        printf("Out of range\n");
        printf("Enter a width (%d through %d): ",MINW,MAXW);
        tChar = GetMyChar();
        temp = atoi(&tChar);
      }
      WIDTH = temp;
      BOARDSIZE = (HEIGHT+2)*(WIDTH+1);
      SetGenericNumPositions();
      SetGenericPositionOffset();
      printf("new: %d boardsize, %llu positions, %llu offset", BOARDSIZE, gNumberOfPositions, POSITION_OFFSET);
      SetGenericInitialPosition();
      break;
    case 'H': case 'h':
      printf("Enter a height (%d through %d): ",MINH,MAXH);
      temp = GetMyInt();
      while(temp > MAXH || temp < MINH) {
        printf("Out of range\n");
        printf("Enter a height (%d through %d): ",MINH,MAXH);
        tChar = GetMyChar();
        temp = atoi(&tChar);
      }
      HEIGHT = temp;
      BOARDSIZE = (HEIGHT+2)*(WIDTH+1);
      SetGenericNumPositions();
      SetGenericPositionOffset();
      printf("new: %d boardsize, %llu positions, %llu offset", BOARDSIZE, gNumberOfPositions, POSITION_OFFSET);
      SetGenericInitialPosition();
      break;

    case 'I': case 'i':
      gInitialPosition = GetInitialPosition();
      // printf("THE POSITION YOU SELECTED HAS BEEN CODED AS %llu", gInitialPosition);
      break;
    case 'T': case 't':
      gToTrapIsToWin = !gToTrapIsToWin;
      break;
    default:
      printf("\nSorry, I don't know that option. Try another.\n");
      HitAnyKeyToContinue();
      break;
    }
  } while(TRUE);
}

/************************************************************************
**
** NAME:        SetTclCGameSpecificOptions
**
** DESCRIPTION: Set the C game-specific options (called from Tcl)
**              Ignore if you don't care about Tcl for now.
**
************************************************************************/

void SetTclCGameSpecificOptions(int theOptions[])
{
	/* No need to have anything here, we have no extra options */
}

/************************************************************************
**
** NAME:        DoMove
**
** DESCRIPTION: Apply the move to the position.
**
** INPUTS:      POSITION thePosition : The old position
**              MOVE     theMove     : The move to apply.
**
** OUTPUTS:     (POSITION) : The position that results after the move.
**
** CALLS:       PositionToBlankOX(POSITION,*BlankOX)
**              BlankOX whoseturn(*BlankOX)
**
************************************************************************/

POSITION DoMove(POSITION thePosition, MOVE theMove)
{
	SLOT fromSlot, toSlot;
	BlankOX theBlankOX[BOARDSIZE], whoseturn;

	PositionToBlankOX(thePosition,theBlankOX,&whoseturn);
	MoveToSlots(theMove, &fromSlot, &toSlot);

	return(thePosition
	       + (whoseturn == o ? POSITION_OFFSET : -POSITION_OFFSET)
	       - (g3Array[fromSlot] * (int)whoseturn) /* take from slot */
	       + (g3Array[toSlot] * (int)whoseturn)); /* put in to slot */
}

/************************************************************************
**
** NAME:        GetInitialPosition
**
** DESCRIPTION: Ask the user for an initial position for testing. Store
**              it in the space pointed to by initialPosition;
**
** OUTPUTS:     POSITION initialPosition : The position to fill.
**
************************************************************************/

POSITION GetInitialPosition()
{
  BlankOX theBlankOX[BOARDSIZE], whoseTurn;
	printf("\n\n\t----- Get Initial Position -----\n");
	printf("\n\tPlease input the position to begin with.\n");
	printf("\tNote that it should be in the following format:\n");
  printf("\nIf you want a board like this:\n");
	PrintActualBoard(gInitialPosition);
  printf("\nThen type this: \n");
  printf("\n\n");
	PrintBoardString(gInitialPosition);

  int i = 1;
  signed char c;
  while(i < BOARDSIZE-1 && (c = getchar()) != EOF) {
    if(c == 'x' || c == 'X') {
      theBlankOX[ConvertOutputToIndex(i)] = x;
      i++;
    }
    else if(c == 'o' || c == 'O' || c == '0') {
      theBlankOX[ConvertOutputToIndex(i)] = o;
      i++;
    }
    else if(c == '-') {
      theBlankOX[ConvertOutputToIndex(i)] = Blank;
      i++;
    }
    else
      ; /* do nothing */
  }
  theBlankOX[CORNER1] = Blank;
  theBlankOX[CORNER2] = Blank;

  getchar();
  printf("\nNow, whose turn is it? [O/X] : ");
  scanf("%c",&c);
  if(c == 'x' || c == 'X')
    whoseTurn = x;
  else
    whoseTurn = o;

  POSITION p = BlankOXToPosition(theBlankOX ,whoseTurn);
  return p;
}


/************************************************************************
**
** NAME:        PrintComputersMove
**
** DESCRIPTION: Nicely format the computers move.
**
** INPUTS:      MOVE    computersMove : The computer's move.
**              STRING  computersName : The computer's name.
**
************************************************************************/

void PrintComputersMove(MOVE computersMove,STRING computersName)
{
	SLOT fromSlot,toSlot; //raw indeces
	MoveToSlots(computersMove,&fromSlot,&toSlot);
	printf("%8s's move              : %d %d\n",computersName,
	       ConvertIndexToOutput(fromSlot), ConvertIndexToOutput(toSlot));
}

/************************************************************************
**
** NAME:        Primitive
**
** DESCRIPTION: Return the value of a position if it fulfills certain
**              'primitive' constraints. Some examples of this is having
**              three-in-a-row with TicTacToe. TicTacToe has two
**              primitives it can immediately check for, when the board
**              is filled but nobody has one = primitive tie. Three in
**              a row is a primitive lose, because the player who faces
**              this board has just lost. I.e. the player before him
**              created the board and won. Otherwise undecided.
**
** INPUTS:      POSITION position : The position to inspect.
**
** OUTPUTS:     (VALUE) an enum which is oneof: (win,lose,tie,undecided)
**
** CALLS:       PositionToBlankOX()
**
************************************************************************/

VALUE Primitive(POSITION position)
{
	BlankOX theBlankOX[BOARDSIZE], whoseturn;

	PositionToBlankOX(position,theBlankOX,&whoseturn);

	if(FullSlots(theBlankOX))
		return(gStandardGame ? lose : win);
	else if(Trapped(position))
		return(gToTrapIsToWin ? lose : win);
	else
		return(undecided);
}

BOOLEAN FullSlots(theBlankOX)
BlankOX theBlankOX[];
{
  BOOLEAN oh = TRUE;
  BOOLEAN ex = TRUE;
  int i;
  for (int i = 0; i < WIDTH; i++) {
    if (theBlankOX[i] != x) ex = FALSE;
  }
  for (int i = CORNER2+1; i <= BOARDSIZE-2; i++) {
    if (theBlankOX[i] != o) oh = FALSE;
  }
  return oh || ex;
}

BOOLEAN Trapped(POSITION position)
{
	MOVELIST *ptr;
	BOOLEAN trapped;

	ptr = GenerateMoves(position);
	trapped = (ptr == NULL);
	FreeMoveList(ptr);
	return(trapped);
}

/************************************************************************
**
** NAME:        PrintPosition
**
** DESCRIPTION: Print the position in a pretty format, including the
**              prediction of the game's outcome.
**
** INPUTS:      POSITION position   : The position to pretty print.
**              STRING   playerName : The name of the player.
**              BOOLEAN  usersTurn  : TRUE <==> it's a user's turn.
**
** CALLS:       PositionToBlankOX()
**              GetValueOfPosition()
**              GetPrediction()
**
************************************************************************/

void PrintPosition(POSITION position, STRING playerName, BOOLEAN usersTurn)
{
  printf("\nReference Board!\n");
  PrintReferenceBoard();
  printf("\nYour Actual Board!\n");
  if (usersTurn)
    printf("\nIt is %s's Turn.", playerName);
  else
    printf("\nIt is Computer's Turn.\n");
  PrintActualBoard(position);
  printf("\nPrediction: %s\n\n", GetPrediction(position,playerName,usersTurn));
}

/************************************************************************
**
** NAME:        GenerateMoves
**
** DESCRIPTION: Create a linked list of every move that can be reached
**              from this position. Return a pointer to the head of the
**              linked list.
**
** INPUTS:      POSITION position : The position to branch off of.
**
** OUTPUTS:     (MOVELIST *), a pointer that points to the first item
**              in the linked list of moves that can be generated.
**
** CALLS:       MOVELIST *CreateMovelistNode(MOVE,MOVELIST)
**
*************************************************************************/


MOVELIST *GenerateMoves(position)
POSITION position;
{
	MOVELIST *head = NULL;
	BlankOX theBlankOX[BOARDSIZE], whoseturn;
	SLOT i, toSlot;

	int j,k; /* i is which FROM slot to start from
	          * j is direction
	          * k is number of slots to slide
	          * values for j: 0="left", 1="right"
	          * 0   1     O
	          *  \ /     / \
	          *   X     1   0
	          */

	PositionToBlankOX(position, theBlankOX, &whoseturn);

	for(i = 0; i < BOARDSIZE; i++) { /*enumerate over all FROM slots*/
		for(j = 0; j < 2; j++) { /* enumerate over all directions */
			k = 1;
			while((k < 4) && OkMove(theBlankOX, whoseturn, i, j, k, &toSlot)) {
				head = CreateMovelistNode(SlotsToMove(i, toSlot),head);
				k++;
			}
		}
	}
	return(head);
}


BOOLEAN OkMove(theBlankOx, whoseturn, fromSlot, direction, slide, toSlot)
BlankOX *theBlankOx, whoseturn;
SLOT fromSlot;
int direction, slide;
SLOT *toSlot;
{
	*toSlot = GetToSlot(whoseturn, fromSlot, direction, slide);
  BOOLEAN ok = (theBlankOx[fromSlot] == whoseturn) && (*toSlot != BADSLOT) && (theBlankOx[*toSlot] == Blank);
  // printf("\nfrom %d to %d on %d's turn is %d.", fromSlot, *toSlot, whoseturn, ok);
	return ok;

}


SLOT GetToSlot(BlankOX whoseturn, SLOT fromSlot, int direction, int slide)
{
//here, a bunch of else ifs where you return the toSlot slot

  int row = IndexToRow(fromSlot);
  int col = IndexToCol(fromSlot);
  int corner1 = WIDTH;
  int corner2 = BOARDSIZE-WIDTH-1;
  int toslot;
  // fromSlot is an integer 1 to BOARDSIZE
  if(whoseturn == o) {
     row += slide;
     if (direction == 1)
      col += slide;
    if (row > WIDTH+1 || col > HEIGHT+2)
      return BADSLOT;
    if (PairToIndex(row, col) >= BOARDSIZE || PairToIndex(row, col) == corner1 || PairToIndex(row, col) == corner2)
      return BADSLOT;
    toslot = PairToIndex(row, col);
    return toslot;
  } //ends if(whoseturn == o) !!!!!
  else if(whoseturn == x) {
    row -= slide;
    if (direction == 0)
     col -= slide;
   if (row < 0 || col < 0)
     return -1;
   if (PairToIndex(row, col) == corner1 || PairToIndex(row, col) == corner2)
     return BADSLOT;
     toslot = PairToIndex(row, col);
     return toslot;
  }
	BadElse("GetToSlot"); //if it doesn't return anything, there's a PROBLEM

	//should never reach here
	return 0;
}

/************************************************************************
**
** NAME:        GetAndPrintPlayersMove
**
** DESCRIPTION: This finds out if the player wanted an undo or abort or not.
**              If so, return Undo or Abort and don't change theMove.
**              Otherwise get the new theMove and fill the pointer up.
**
** INPUTS:      POSITION *thePosition : The position the user is at.
**              MOVE *theMove         : The move to fill with user's move.
**              STRING playerName     : The name of the player whose turn it is
**
** OUTPUTS:     USERINPUT             : Oneof( Undo, Abort, Continue )
**
** CALLS:       ValidMove(MOVE, POSITION)
**              BOOLEAN PrintPossibleMoves(POSITION) ...Always True!
**
************************************************************************/

USERINPUT GetAndPrintPlayersMove(thePosition, theMove, playerName)
POSITION thePosition;
MOVE *theMove;
STRING playerName;
{
	USERINPUT ret;

	do {
		printf("%8s's move [(u)ndo or 1-%d  1-%d] :  ",  playerName, BOARDSIZE-2, BOARDSIZE-2);

		ret = HandleDefaultTextInput(thePosition, theMove, playerName);
		if(ret != Continue)
			return(ret);

	}
	while (TRUE);
	return(Continue); /* this is never reached, but lint is now happy */
}

/************************************************************************
**
** NAME:        ValidTextInput
**
** DESCRIPTION: Return TRUE iff the string input is of the right 'form'.
**              For example, if the user is allowed to select one slot
**              from the numbers 1-9, and the user chooses 0, it's not
**              valid, but anything from 1-9 IS, regardless if the slot
**              is filled or not. Whether the slot is filled is left up
**              to another routine.
**
** INPUTS:      STRING input : The string input the user typed.
**
** OUTPUTS:     BOOLEAN : TRUE iff the input is a valid text input.
**
************************************************************************/

BOOLEAN ValidTextInput(input)
STRING input;
{
	SLOT fromSlot, toSlot;
	int text;
	text = sscanf(input, "%d %d", &fromSlot, &toSlot);
	return(text == 2 && fromSlot <= BOARDSIZE-2 && fromSlot >= 1 && toSlot <= BOARDSIZE-2 &&
	       toSlot >= 1);
}

/************************************************************************
**
** NAME:        ConvertTextInputToMove
**
** DESCRIPTION: Convert the string input to the internal move representation.
**
** INPUTS:      STRING input : The string input the user typed.
**
** OUTPUTS:     MOVE : The move corresponding to the user's input.
**
************************************************************************/


MOVE ConvertTextInputToMove(input)
STRING input;
{
  SLOT fromSlot, toSlot;
	int text;
	text = sscanf(input, "%d %d", &fromSlot, &toSlot);
  fromSlot = ConvertOutputToIndex(fromSlot);
  toSlot = ConvertOutputToIndex(toSlot);

	return(SlotsToMove(fromSlot,toSlot));
}

/************************************************************************
**
** NAME:        PrintMove
**
** DESCRIPTION: Print the move in a nice format.
**
** INPUTS:      MOVE *theMove         : The move to print.
**
************************************************************************/

void PrintMove(theMove)
MOVE theMove;
{
	printf( "%s", MoveToString(theMove) );
}

/************************************************************************
**
** NAME:        MoveToString
**
** DESCRIPTION: Returns the move as a STRING
**
** INPUTS:      MOVE *theMove         : The move to put into a string.
**
************************************************************************/

STRING MoveToString (theMove)
MOVE theMove;
{
	STRING move = (STRING) SafeMalloc(6);

	SLOT fromSlot, toSlot;
	MoveToSlots(theMove,&fromSlot,&toSlot);
/* The plus 1 is because the user thinks it's 1-9, but MOVE is 0-8 */

	sprintf(move, "[%d %d]", ConvertIndexToOutput(fromSlot), ConvertIndexToOutput(toSlot));

	return move;
}


int NumberOfOptions() {
	return 2*2;
}


int getOption() {
	int option = 1;
	option += gStandardGame ? 0 : 1;
	option += 2*(gToTrapIsToWin ? 1 : 0);
	return option;
}


void setOption(int option) {
	option--;
	gStandardGame = (option%2==0);
	gToTrapIsToWin = (option/2%2==1);
}


/************************************************************************
*************************************************************************
**         EVERYTHING BELOW THESE LINES IS LOCAL TO THIS FILE
*************************************************************************
************************************************************************/

/************************************************************************
**
** NAME:        PositionToBlankOX
**
** DESCRIPTION: convert an internal position to that of a BlankOX.
**
** INPUTS:      POSITION thePos     : The position input.
**              BlankOX *theBlankOx : The converted BlankOX output array.
**
** CALLS:       BadElse()
**
************************************************************************/

void PositionToBlankOX(thePos,theBlankOX,whoseturn)
POSITION thePos;
BlankOX *theBlankOX, *whoseturn;
{
	int i;

	if(thePos >= POSITION_OFFSET) {
		*whoseturn = x;
		thePos -= POSITION_OFFSET;
	}
	else *whoseturn = o;

	for(i = (BOARDSIZE - 1); i >= 0; i--) {
		if(thePos >= ((int)x * g3Array[i])) {
			theBlankOX[i] = x;
			thePos -= (int)x * g3Array[i];
		}
		else if(thePos >= ((int)o * g3Array[i])) {
			theBlankOX[i] = o;
			thePos -= (int)o * g3Array[i];
		}
		else if(thePos >= ((int)Blank * g3Array[i])) {
			theBlankOX[i] = Blank;
			thePos -= (int)Blank * g3Array[i];
		}
		else
			BadElse("PositionToBlankOX");
	}
}

/************************************************************************
**
** NAME:        BlankOXToPosition
**
** DESCRIPTION: convert a BlankOX to that of an internal position.
**
** INPUTS:      BlankOX *theBlankOx : The converted BlankOX output array.
**
** OUTPUTS:     POSITION: The equivalent position given the BlankOX.
**
************************************************************************/

POSITION BlankOXToPosition(theBlankOX, whoseturn)
BlankOX *theBlankOX, whoseturn;
{
	int i;
	POSITION position = 0;

	for(i = 0; i < BOARDSIZE; i++)
		position += g3Array[i] * (int)theBlankOX[i];

	if(whoseturn == x)
		position += POSITION_OFFSET;

	return(position);
}

// actually ouputting the matrix or array indeces as slots
void MoveToSlots(theMove, fromSlot, toSlot)
MOVE theMove;
SLOT *fromSlot, *toSlot;
{
	*fromSlot = theMove % (BOARDSIZE+1);
	*toSlot = theMove / (BOARDSIZE+1);
}

// these are actually getting the matrix or array indeces as slots
MOVE SlotsToMove(fromSlot, toSlot)
SLOT fromSlot, toSlot;
{
	return((MOVE)toSlot*(BOARDSIZE+1) + fromSlot);
}


POSITION StringToPosition(char* board) {
	// FIXME: this is just a stub
	return atoi(board);
}


char* PositionToString(POSITION pos) {
	// FIXME: this is just a stub
	return "Implement Me";
}

//GM_DEFINE_BLANKOX_ENUM_BOARDSTRINGS()

char * PositionToEndData(POSITION pos) {
	return NULL;
}

