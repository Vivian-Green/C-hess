#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>

int charCodeForCapitalA = 65;
int charCodeFor1 = 49;

//knights are horses now
char chessPieceTypes[6] = "RHBQKP";
wchar_t chessDisplayPiecesB[6] = {0x2656, 0x2658, 0x2657, 0x2655, 0x2654, 0x2659};
wchar_t chessDisplayPiecesW[6] = {0x265C, 0x265E, 0x265D, 0x265B, 0x265A, 0x265F};

//char chessDisplayPiecesB[6][6] = {"♖", "♘", "♗", "♕", "♔", "♙"};
//char chessDisplayPiecesW[6][6] = {"♜", "♞", "♝", "♛", "♚", "🕴"};

wchar_t wBoardColor[5] = {0x2593};
wchar_t bBoardColor[5] = {0x2591};

wchar_t defaultFormatting[] = L"\033[0m";
wchar_t highlightedFormatting[] = L"\033[0;31m\033[45m";

int turnCounter = 0;

char board[8][16] = {
  "BRBHBBBQBKBBBHBR",
  "BPBPBPBPBPBPBPBP",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "0000000000000000",
  "WPWPWPWPWPWPWPWP",
  "WRWHWBWQWKWBWHWR"
};

struct square{
  int row;
  int column;
};
typedef struct square square;

int possibleMoves[50][2];

int isPossibleMove(struct square moveToCheck, int moveCount){
  //no possible moves, give up
  if(moveCount == 0){
    return -1;
  }

  //check if possibleMoves contains moveToCheck
  for(int i = 0; i < moveCount; i++){
    //printf("possibleMoves i: %d %d\n", possibleMoves[i][0], possibleMoves[i][1]);
    if(moveToCheck.column == possibleMoves[i][0] && moveToCheck.row == possibleMoves[i][1]){
      return 1;
    }
  }
  return -1;
}

int indexCheck(int index){
  //returns the index if it is on the board, else -1
  if(index >= 0 && index <= 7){
    return index;
  }
  return -1;
}

int isInBounds(struct square mySquare){
  //2D indexCheck that takes a square lol
  if(indexCheck(mySquare.row) == -1 || indexCheck(mySquare.column) == -1){
    return -1;
  }
  return 1;
}

int getIndexFromChar(char character){
  //converts a char to an index, if valid, else -1
  int charCode = (int)toupper(character);
  int index;
  
  if(charCode > 60){
    index = charCode - charCodeForCapitalA;
  }else{
    index = charCode - charCodeFor1;
  }
  
  return indexCheck(index);
}

char getSquareColor(int index1, int index2){
  //returns default color of empty squares
  if((index1 + index2)%2 == 0){
    return 'W';
  }
  return 'B';
}

void printBoard(int selectedColumn, int selectedRow, int moveCount){
  wchar_t displayBoard[8][240];
  /*240 is larger than should ever be needed: 96 for 8 pieces that are each 
  12 charachers long, plus the highlightedFormatting & defaultFormatting lengths for each, 
  if a rook or queen is selected & has hella room*/

  for(int row = 0; row < 8; row+= 1){
    //wprintf(L"row %d\n", row+1);
    //vertical, store 1 line
    wchar_t displayRow[240];
    wchar_t emptyWideString[1] = L"";
    wcscpy(displayRow, emptyWideString);
    
    for(int column = 0; column < 16; column+= 2){
      //horizontal, store 1 square
      wchar_t displayPiece[30];
      wcscpy(displayPiece, emptyWideString);
      
      wchar_t pieceTeam = board[row][column];
      wchar_t pieceType = board[row][column+1];

      struct square thisSquare;
      thisSquare.column = column/2;
      thisSquare.row = row;

      //set backgroundColor to getSquareColor, getting default color for this square
      wchar_t backgroundColor[5];
      //wcscpy(backgroundColor, emptyWideString);
      if(getSquareColor(row, column/2) == 'B'){
        wcscpy(backgroundColor, bBoardColor);
      }else{
        wcscpy(backgroundColor, wBoardColor);
      }
      
      if(pieceTeam == '0'){
        //if empty square
        wchar_t temp[30];
        wcscpy(temp, emptyWideString);
        if(isPossibleMove(thisSquare, moveCount) == 1){
          //if (empty) square is a possible move, store it highlighted
          wcscpy(temp, highlightedFormatting);
          wcscat(temp, backgroundColor);
          wcscat(temp, backgroundColor);
          wcscat(temp, defaultFormatting);
          
          wcscat(displayRow, temp);
        }else{
          //if (empty) square is NOT a possible move, store it in black & white
          wcscpy(temp, backgroundColor);
          wcscat(displayRow, temp);
          wcscat(displayRow, temp);
        }
      }else{
        //get index of piece type to print, as it's stored.
        //this is to get the unicode for it from chessDisplayPiecesB/W
        int displayPieceIndex;
        for(int i = 0; i < strlen(chessPieceTypes); i++){
          //compare piece type against chessPiecesB
          if(pieceType == chessPieceTypes[i]){
            displayPieceIndex = i;
            break;
          }
        }

        //if black, get the black piece at displayPieceIndexB, else displayPieceIndexW
        //wchar_t displayPieceStr = chessDisplayPiecesB[displayPieceIndex];
        if(pieceTeam == 'B'){
          wcsncpy(displayPiece, &chessDisplayPiecesB[displayPieceIndex], 1);
        }else{
          wcsncpy(displayPiece, &chessDisplayPiecesW[displayPieceIndex], 1);
        }

        
        if((isPossibleMove(thisSquare, moveCount) == 1) || (selectedColumn == column/2 && selectedRow == row)){
          //highlight display piece if it's a possible move
          //append to displayRow
          wcscat(displayRow, highlightedFormatting);
          wcscpy(displayRow, wcscat(wcscat(displayRow, displayPiece), L" "));
          wcscat(displayRow, defaultFormatting);
        }else{
          //otherwise, don't highlight it
          //append to displayRow
          wcscpy(displayRow, wcscat(wcscat(displayRow, displayPiece), L" "));
        }
      }
    }
    //add row to board
    wcscpy(displayBoard[row], displayRow);
  }

  //print header, then every line of displayBoard, formatted
  wprintf(L"  |A B C D E F G H |\n");
  for(int i = 0; i < 8; i++){
    wprintf(L"%d |%ls%ls| \n", i+1, displayBoard[i], defaultFormatting);
  }
}

//actually move the pieces and handle taking them
void executeMove(struct square from, struct square to) {
  wprintf(L"executing move\n");
  //get index of first character in piece, not piece location
  from.column *= 2;
  to.column *= 2;
  
  char pieceColor = board[from.row][from.column];
  char pieceType = board[from.row][from.column+1];
  char pieceColorLong[6];
  char toPieceColor = board[to.row][to.column];
  char toPieceType = board[to.row][to.column+1];

  char capturedPiece;
  char capturedPieceLong[8];
  int didCapturePiece = 0;

  //store piece color
  if(pieceColor == 'B'){
    strncpy(pieceColorLong, "Black", 5);
  }else{
    strncpy(pieceColorLong, "White", 5);
  }

  //check if a piece was captured, and store captured piece
  if(pieceType == 'P'){
    //pawns
    if(to.column != from.column){
      if(toPieceColor == '0'){
        //if en passant (only time a pawn moves horizontally)
        capturedPiece = board[from.row][to.column+1];
        didCapturePiece = 1;
  
        //handle en passant captures here
        board[from.row][to.column] = '0';
        board[from.row][to.column+1] = '0';
      }else{
        //regular diagonal capture
        capturedPiece = board[to.row][to.column+1];
        didCapturePiece = 1;
      }

    }
  }else{
    //non-pawns
    if (board[to.row][to.column] != '0') {
      capturedPiece = board[to.row][to.column+1];
      didCapturePiece = 1;
    }
  }

  if(didCapturePiece == 1){
    //store long captured piece name if one was captured
    switch(capturedPiece) {
        case 'R'://rook
          strcpy(capturedPieceLong, "Rook");
          break;
        case 'H'://knight
          strcpy(capturedPieceLong, "Knight");
          break;
        case 'B'://bishop
          strcpy(capturedPieceLong, "Bishop");
          break;
        case 'Q'://queen
          strcpy(capturedPieceLong, "Queen");
          break;
        case 'K'://king (handling sort of checkmates here)
          strcpy(capturedPieceLong, "King");
          
          turnCounter = -10;
          if(pieceColor == 'B'){
            wprintf(L"\n\n\n\n\n\n\nBlack is win, wow!!!!!!!!\n");
          }else{
            wprintf(L"\n\n\n\n\n\n\nWhite is win, wow!!!!!!!!\n");
          }
          break;
        case 'P'://pawn
          strcpy(capturedPieceLong, "Pawn");
          break;
        default:
          strcpy(capturedPieceLong, "Nothing");//shouldn't happen
      }

      //print message with stored captured piece
      wprintf(L"%s captured %s!\n", pieceColorLong, capturedPieceLong);
  }

  // Update the board with the new move
  board[to.row][to.column] = pieceColor;
  board[to.row][to.column+1] = pieceType;
  board[from.row][from.column] = '0';
  board[from.row][from.column+1] = '0';
}

//handles user input (square coordinates) AFTER being prompted elsewhere
struct square handleMoveInput(){
  //get input, 2 chars
  char moveStr[2];
  scanf("%s", moveStr);
  
  //set moveOut to the board indeces corresponding to the given input
  struct square moveOut;
  moveOut.row = getIndexFromChar(moveStr[1]);
  moveOut.column = getIndexFromChar(moveStr[0]);

  //printf("%s, %d, %d\n\n", moveStr, moveOut.column, moveOut.row);
  //if moveOut is out of bounds, return square {-1, -1}
  if(isInBounds(moveOut) == -1){
    wprintf(L"INVALID MOVE\n");
    moveOut.row = -1;
    moveOut.column = -1;
  }
  
  return moveOut;
}

void getPossibleMovesInDirection(struct square piecePosition, char pieceColor, int possibleMoves[50][2], int* pMoveCount, struct square offset){
  //travels in a line, makin sure things are gucci
  //returns valid tiles on that line
  
  //printf("get possible moves in direction: %d %d\n", offset.column, offset.row);
  
  struct square checkingPosition = piecePosition;
  char landingSquareColor = '0';
  
  while(isInBounds(checkingPosition) == 1){
    //move (offset) square(s)
    checkingPosition.row += offset.row;
    checkingPosition.column += offset.column;;
    
    //if on board
    if(isInBounds(checkingPosition) == 1){
      //get the color of the piece to land on
      landingSquareColor = board[checkingPosition.row][checkingPosition.column*2];
      
      if(landingSquareColor == pieceColor){
        //can't capture your own piece, stop
        break;
      }

      //add square to list of possible moves (not own piece)
      possibleMoves[*pMoveCount][0] = checkingPosition.column;
      possibleMoves[*pMoveCount][1] = checkingPosition.row;
      *pMoveCount += 1;

      //if not an empty square (if it's an enemy piece)
      if(landingSquareColor != '0'){
        //allow landing on it, but not going through it
        break;
      }
    }
  }
}
int getPossibleMovesInMultipleDirections(struct square piecePosition, char pieceColor, int possibleMoves[50][2], int movesPattern[8][2], int movesLength){
  int moveCount = 0;

  //for each direction, get moves in that direction
  for(int i = 0; i < movesLength; i++){
    struct square offset = {movesPattern[i][0], movesPattern[i][1]};
    getPossibleMovesInDirection(piecePosition, pieceColor, possibleMoves, &moveCount, offset);
  }
  return moveCount;
}

int getPossibleRookMoves(struct square piecePosition, char pieceColor, int possibleMoves[50][2]){
  int moveCount = 0;
  //list of directions
  int movesPattern[4][2] = {
    {0, -1}, {0, 1}, {-1, 0}, {1, 0}  //cardinals
  };
  return getPossibleMovesInMultipleDirections(piecePosition, pieceColor, possibleMoves, movesPattern, 4);
}

int getPossibleBishopMoves(struct square piecePosition, char pieceColor, int possibleMoves[50][2]){
  int moveCount = 0;
  //list of directions
  int movesPattern[4][2] = {
    {-1, -1}, {-1, 1}, {1, -1}, {1, 1} //diagonals
  };
  return getPossibleMovesInMultipleDirections(piecePosition, pieceColor, possibleMoves, movesPattern, 4);
}

int getPossibleQueenMoves(struct square piecePosition, char pieceColor, int possibleMoves[50][2]){
  int moveCount = 0;
  //list of directions
  int movesPattern[8][2] = {
    {-1, -1}, {-1, 1}, {1, -1}, {1, 1}, //diagonals
    {0, -1}, {0, 1}, {-1, 0}, {1, 0}  //cardinals
  };
  return getPossibleMovesInMultipleDirections(piecePosition, pieceColor, possibleMoves, movesPattern, 8);
}

int getPossibleMovesGeneral(struct square piecePosition, char pieceColor, int possibleMoves[50][2], int moveset[100][2], int movesLength){
  /*general function for pieces that are magic, like the king & knight, and can't be blocked
  by anything. Well the king isn't magic, but it does use this, bc it can only move 1 anyway*/
  int moveCount = 0;

  //for each move in moveset, check if it's valid
  for(int i = 0; i < movesLength; i++) {
    //checkingPosition is moveset position relative to piece position
    struct square checkingPosition = {
      piecePosition.row + moveset[i][0], 
      piecePosition.column + moveset[i][1]
    };

    int isBlankSpace = board[checkingPosition.row][(checkingPosition.column)*2] == '0';//checkingPosition is a blank space
    int isOtherTeam = board[checkingPosition.row][(checkingPosition.column)*2] != pieceColor;//checkingPosition has piece from the other team
    //can land on empty squares, and squares of the opposite team.. if they're in bounds.
    int isValidMove = (isInBounds(checkingPosition) && (isBlankSpace == 1 || isOtherTeam == 1));
    
    if(isValidMove == 1){
      //add valid moves to possibleMoves
      possibleMoves[moveCount][0] = checkingPosition.column;
      possibleMoves[moveCount][1] = checkingPosition.row;
      moveCount++;
    }
  }

  return moveCount;
}

int getPossibleKnightMoves(struct square piecePosition, char pieceColor, int possibleMoves[50][2]){
  //wrapper for getPossibleMovesGeneral with hardcoded knight moveset
  int moveset[100][2] = {
    {-2,-1}, {-2,1}, 
    {-1,-2}, {-1,2}, 
    {1,-2}, {1,2}, 
    {2,-1}, {2,1}
  };
  return getPossibleMovesGeneral(piecePosition, pieceColor, possibleMoves, moveset, 8);
}

int getPossibleKingMoves(struct square piecePosition, char pieceColor, int possibleMoves[50][2]){
  //wrapper for getPossibleMovesGeneral with hardcoded king moveset
  int moveset[100][2] = {
    {-1, -1}, {-1, 1}, {1, -1}, {1, 1}, //diagonals
    {0, -1}, {0, 1}, {-1, 0}, {1, 0}  //cardinals
  };
  return getPossibleMovesGeneral(piecePosition, pieceColor, possibleMoves, moveset, 8);
}

int getPossiblePawnMoves(struct square piecePosition, char pieceColor, int possibleMoves[50][2]){
  //this should not be the hardest piece to code, and yet
  int moveset[100][2];
  int moveCount = 0;
  int direction;
  int doubleMove = 0;

  //figure out possible moves & direction
  if(pieceColor == 'B'){
    wprintf(L"black piece selected\n");
    direction = 1;
    if(piecePosition.row == 1){//if in starting position, can double move
      doubleMove = 1;
    }
  }else{
    wprintf(L"white piece selected\n");
    direction = -1;
    if(piecePosition.row == 6){//if in starting position, can double move
      doubleMove = 1;
    }
  }
  
  //single move, if tile not occupied
  if(board[piecePosition.row+direction][piecePosition.column*2] == '0'){
    moveset[moveCount][0] = direction;
    moveset[moveCount][1] = 0;
    moveCount += 1;
  }

  //double move is an option, & tile not occupied
  if(doubleMove == 1){
    if(board[piecePosition.row+(direction*2)][piecePosition.column*2] == '0'){
      moveset[moveCount][0] = 2*direction;
      moveset[moveCount][1] = 0;
      moveCount += 1;
    }
  }

  //en passant
  for(int columnOffset = -1; columnOffset <= 1; columnOffset += 2){
    //holy hell
    //forward & to either side
    struct square toSquare = {piecePosition.row+direction, piecePosition.column+columnOffset};
    struct square attackingSquare = {piecePosition.row, piecePosition.column+columnOffset};
    
    char toSquareColor = board[toSquare.row][toSquare.column*2];
    char attackingSquareColor = board[attackingSquare.row][attackingSquare.column*2];
    
    if(toSquareColor == '0' && attackingSquareColor != '0' && attackingSquareColor != pieceColor){
      //if moving to empty square, and "passing" square is an enemy, allow en passant
      moveset[moveCount][0] = toSquare.row-piecePosition.row;
      moveset[moveCount][1] = toSquare.column-piecePosition.column;
      //printf("adding move: %d %d\n", moveset[moveCount][0], moveset[moveCount][1]);
      moveCount += 1;
    }
    if(toSquareColor != pieceColor && toSquareColor != '0'){
      //if moving diagonally to enemy square, allow taking
      moveset[moveCount][0] = toSquare.row-piecePosition.row;
      moveset[moveCount][1] = toSquare.column-piecePosition.column;
      //printf("adding move: %d %d\n", moveset[moveCount][0], moveset[moveCount][1]);
      moveCount += 1;
    }
  }
  return getPossibleMovesGeneral(piecePosition, pieceColor, possibleMoves, moveset, moveCount);
}

int getPossibleMoves(struct square piecePosition, int possibleMoves[50][2]){
  //gets piece type, and figures out valid moves for it
  
  //wprintf(L"piecePosition: %d\n", piecePosition.row);
  char pieceColor = board[piecePosition.row][piecePosition.column*2];
  char pieceType = board[piecePosition.row][piecePosition.column*2+1];

  int moveCount = 0; 

  //clear possibleMoves memory
  for(int i = 0; i < 50; i++){
    possibleMoves[i][0] = -1;
    possibleMoves[i][1] = -1;
  }
  
  switch(pieceType) {
    case 'R'://rook
      moveCount = getPossibleRookMoves(piecePosition, pieceColor, possibleMoves);
      break;
    case 'H'://knight
      moveCount = getPossibleKnightMoves(piecePosition, pieceColor, possibleMoves);
      break;
    case 'B'://bishop
      moveCount = getPossibleBishopMoves(piecePosition, pieceColor, possibleMoves);
      break;
    case 'Q'://queen
      moveCount = getPossibleQueenMoves(piecePosition, pieceColor, possibleMoves);
      break;
    case 'K'://king
      moveCount = getPossibleKingMoves(piecePosition, pieceColor, possibleMoves);
      break;
    case 'P'://pawn
      moveCount = getPossiblePawnMoves(piecePosition, pieceColor, possibleMoves);
      break;
  }
  return moveCount;
}

void turn(int possibleMoves[50][2]){
  //handle one turn
  int moveCount = 0;
  char movingTeam;

  //print game state
  turnCounter += 1;
  wprintf(L"\n|||||||||||||||||||||||||||||||||||||turn %d|||||||||||||||||||||||||||||||||||||\n", turnCounter);
  printBoard(-1, -1, moveCount);

  if(turnCounter % 2 == 0){
    wprintf(L"\nBlack's turn\n");
    movingTeam = 'B';
  }else{
    wprintf(L"\nWhite's turn\n");
    movingTeam = 'W';
  }

  //handle input, if bad input, return
  wprintf(L"\nEnter a piece to move (Letter, Number, eg: C5): ");
  struct square myPiece = handleMoveInput();;
  if(myPiece.row == -1){
    wprintf(L"NOOOO!!! BAD!! THAT'S NOT A SQUARE YOU FOOL!!!!\n");
    turnCounter -= 1;
    return;
  }

  //if piece from input is of wrong team, return
  if(board[myPiece.row][myPiece.column*2] != movingTeam){
    wprintf(L"NOOOO!!! BAD!! UR NOT THAT COLOR!!!!!!!!\n");
    turnCounter -= 1;
    return;
  }

  //get possible moves
  moveCount = getPossibleMoves(myPiece, possibleMoves);

  //print board with possible moves highlighted, then query for input
  printBoard(myPiece.column, myPiece.row, moveCount);
  wprintf(L"\n\nEnter a square to move to (Letter, Number, eg: C5), or q to cancel: ");
  struct square myMove = handleMoveInput();
  if(myMove.row == -1){
    return;
  }
  //printf("%d %d\n\n", myMove.column, myMove.row);

  //execute valid moves, do not the invalid moves
  if(isPossibleMove(myMove, moveCount) == 1){
    executeMove(myPiece, myMove);
  }else{
    wprintf(L"NOOOO!!! BAD!!\n");
    turnCounter -= 1;
  }
}

int main(void) {
  setlocale(LC_CTYPE, "");
  
  wchar_t star1 = 0x2606;
  wchar_t star2 = 0x2605;
  
  wprintf(L"\n\n\nWelcome to C hess.\n\n\n");
  wprintf(L"Checkmate is just capturing the king. \nPawns will never grow up, they have no aspirations in life. \n");

  wprintf(L"\n\nBlack Star: %lc    ",star1);
  wprintf(L"White Star: %lc\n",star2);
  wprintf(L"If the above unicode doesn't render properly, you need to change some settings to render unicode in this terminal, likely your font.\n");
  wprintf(L"I recommend GNU unifont, which can be installed with `sudo apt install unifont`\n\n");
  
  


  turnCounter = 0;
  while(turnCounter > -1){
    turn(possibleMoves);
  }
  return 0;
}
