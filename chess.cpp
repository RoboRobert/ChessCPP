#include<iostream>
#include<string>
#include <ctype.h>

void initializeboard(std::string board[8][8]);
void print_board(std::string board[8][8]);
void square_to_index(std::string square, int &row, int &col);
std::string index_to_square(int row, int col);
void request_move(std::string board[8][8], bool &color);
void legal_moves(std::string movelist[64], std::string board[8][8], int row, int col, std::string piece);
void move_piece(std::string board[8][8], std::string start_square, std::string end_square);
bool parse_move(std::string move, bool color, std::string &piece, std::string &move_square);
bool make_move(std::string board[8][8], std::string move, bool color);

void initializeboard(std::string board[8][8]) {
    //Black rooks
    board[0][0] = "BR";
    board[0][7] = "BR";

    //White rooks
    board[7][0] = "WR";
    board[7][7] = "WR";

    //Black knights
    board[0][1] = "BN";
    board[0][6] = "BN";

    //White knights
    board[7][1] = "WN";
    board[7][6] = "WN";

    //Black bishops
    board[0][2] = "BB";
    board[0][5] = "BB";

    //White bishops
    board[7][2] = "WB";
    board[7][5] = "WB";

    //Black king and queen
    board[0][4] = "BK";
    board[0][3] = "BQ";

    //White king and queen
    board[7][4] = "WK";
    board[7][3] = "WQ";

    //Pawns and empty squares
    for(int i = 1; i < 7; i++) {
        for(int j = 0; j < 8; j++) {
            if(i == 1)
                board[i][j] = "BP";

            else if(i == 6)
                board[i][j] = "WP"; 

            else
                board[i][j] = "[]";
        }
    }
}

void print_board(std::string board[8][8]) {
    std::cout << std::endl;
    std::cout << std::endl;
    for(int i = 0; i < 8; i++) {
        std::cout << 8-i << " ";
        for(int j = 0; j < 8; j++) {
            std::cout << board[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "  A  B  C  D  E  F  G  H" << std::endl;
}

void square_to_index(std::string square, int &row, int &col) {
    char letter = std::tolower(square.at(0));
    int number = std::stoi(square.substr(1,1));

    col = int(letter - 97); //Converts lowercase character to 0-25
    row = 8 - number;

    return;
}

std::string index_to_square(int row, int col) {
    std::string square = " ";

    square.at(0) = char(col + 65); //Converts column to uppercase letter
    square.append(std::to_string(8 - row));

    return square;
}

void request_move(std::string board[8][8], bool &color) {
    std::string move;

    bool found_move = false;

    while(!found_move) {
        print_board(board);
        if(color == 1) {
            std::cout << "Enter a move for white: ";
        }
        else {
            std::cout << "Enter a move for black: ";
        }

        std::cin >> move;

        found_move = make_move(board, move, color);
    }

    color = !color;

    std::cout << "Your move is " << move << std::endl;

    return;
}

void legal_moves(std::string movelist[64], std::string board[8][8], int row, int col, std::string piece) {
    int index = 0;

    bool legal_move = true;

    char color = piece.at(0);

    int rowIndex = 0;
    int colIndex = 0;

    std::string move;
    //Find moves for the king
    if(piece.at(1) == 'K') {
        for(rowIndex = row - 1; rowIndex <= row + 1; rowIndex++) {
            std::cout << "Row: " << rowIndex << std::endl;
            if(rowIndex < 0 || rowIndex > 7) {
                continue;
            }
            for(colIndex = col - 1; colIndex <= col + 1; colIndex++) {
                std::cout << "Col: " << colIndex << std::endl;

                //Makes sure the indices are within the correct bounds
                if((rowIndex == row && colIndex == col) || colIndex < 0 || colIndex > 7) {
                    continue;
                }

                //If the color of the piece on the square to be moved to is the same, don't add the move.
                if(board[rowIndex][colIndex].at(0) == color)
                    continue;
                
                movelist[index] = index_to_square(rowIndex, colIndex);
                index++;
            }
        }

        return;
    }

    //Find moves for queens
    if(piece.at(1) == 'Q') {
        //Vertical moves from current square to the top, not including the square the queen is on.
        rowIndex = row;
        colIndex = col;
        for(rowIndex = row + 1; rowIndex <= 7; rowIndex++) {
            //If the color of the piece on the square to be moved to is the same, don't add the move.
            if(board[rowIndex][colIndex].at(0) == color)
                break;

            movelist[index] = index_to_square(rowIndex, colIndex);
            index++;
        }

        //Vertical moves from current square to the bottom, not including the square the queen is on.
        rowIndex = row;
        colIndex = col;
        for(rowIndex = row - 1; rowIndex >= 0; rowIndex--) {
            //If the color of the piece on the square to be moved to is the same, don't add the move.
            if(board[rowIndex][colIndex].at(0) == color)
                break;

            movelist[index] = index_to_square(rowIndex, colIndex);
            index++;
        }

        //Horizontal moves from current square to the right, not including the square the queen is on.
        rowIndex = row;
        colIndex = col;
        for(colIndex = col + 1; colIndex <= 7; colIndex++) {
            //If the color of the piece on the square to be moved to is the same, don't add the move.
            if(board[rowIndex][colIndex].at(0) == color)
                break;

            movelist[index] = index_to_square(rowIndex, colIndex);
            index++;
        }

        //Horizontal moves from current square to the left, not including the square the queen is on.
        rowIndex = row;
        colIndex = col;
        for(colIndex = col - 1; colIndex >= 0; colIndex--) {
            //If the color of the piece on the square to be moved to is the same, don't add the move.
            if(board[rowIndex][colIndex].at(0) == color)
                break;

            movelist[index] = index_to_square(rowIndex, colIndex);
            index++;
        }

        //Diagonal moves from current square to top left.
        rowIndex = row;
        colIndex = col;
        for(colIndex = col - 1; colIndex >= 0; colIndex--) {
            rowIndex--;
            if(colIndex < 0 || colIndex > 7)
                break;
            if(rowIndex < 0 || rowIndex > 7)
                break;

            //If the color of the piece on the square to be moved to is the same, don't add the move.
            if(board[rowIndex][colIndex].at(0) == color)
                break;

            movelist[index] = index_to_square(rowIndex, colIndex);
            index++;
        }

        //Diagonal moves from current square to bottom right.
        rowIndex = row;
        colIndex = col;
        for(colIndex = col + 1; colIndex < 8; colIndex++) {
            rowIndex++;

            if(colIndex < 0 || colIndex > 7)
                break;
            if(rowIndex < 0 || rowIndex > 7)
                break;

            //If the color of the piece on the square to be moved to is the same, don't add the move.
            if(board[rowIndex][colIndex].at(0) == color)
                break;

            movelist[index] = index_to_square(rowIndex, colIndex);
            index++;
        }

        //Diagonal moves from current square to top right.
        rowIndex = row;
        colIndex = col;
        for(colIndex = col + 1; colIndex < 8; colIndex++) {
            rowIndex--;

            if(colIndex < 0 || colIndex > 7)
                break;
            if(rowIndex < 0 || rowIndex > 7)
                break;

            //If the color of the piece on the square to be moved to is the same, don't add the move.
            if(board[rowIndex][colIndex].at(0) == color)
                break;

            movelist[index] = index_to_square(rowIndex, colIndex);
            index++;
        }

        //Diagonal moves from current square to bottom left.
        rowIndex = row;
        colIndex = col;
        for(colIndex = col - 1; colIndex >= 0; colIndex--) {
            rowIndex++;

            if(colIndex < 0 || colIndex > 7)
                break;
            if(rowIndex < 0 || rowIndex > 7)
                break;

            //If the color of the piece on the square to be moved to is the same, don't add the move.
            if(board[rowIndex][colIndex].at(0) == color)
                break;

            movelist[index] = index_to_square(rowIndex, colIndex);
            index++;
        }

        return;
    }

    //Find moves for knights
    if(piece.at(1) == 'N') {
        rowIndex = row;
        colIndex = col;

        //Above and to the right
        legal_move = true;
        rowIndex = row - 2;
        colIndex = col + 1;
        if((colIndex < 0 || colIndex > 7) || (rowIndex < 0 || rowIndex > 7))
            legal_move = false;
        //If the color of the piece on the square to be moved to is the same, don't add the move.
        if(legal_move && board[rowIndex][colIndex].at(0) == color)
            legal_move = false;
        if(legal_move) {
            movelist[index] = index_to_square(rowIndex, colIndex);
            index++;
        }
        //Above and to the left
        legal_move = true;
        rowIndex = row - 2;
        colIndex = col - 1;
        if((colIndex < 0 || colIndex > 7) || (rowIndex < 0 || rowIndex > 7))
            legal_move = false;
        //If the color of the piece on the square to be moved to is the same, don't add the move.
        if(legal_move && board[rowIndex][colIndex].at(0) == color)
            legal_move = false;
        if(legal_move) {
            movelist[index] = index_to_square(rowIndex, colIndex);
            index++;
        }
        //Down and to the right
        legal_move = true;
        rowIndex = row + 2;
        colIndex = col + 1;
        if((colIndex < 0 || colIndex > 7) || (rowIndex < 0 || rowIndex > 7))
            legal_move = false;
        //If the color of the piece on the square to be moved to is the same, don't add the move.
        if(legal_move && board[rowIndex][colIndex].at(0) == color)
            legal_move = false;
        if(legal_move) {
            movelist[index] = index_to_square(rowIndex, colIndex);
            index++;
        }
        //Down and to the left
        legal_move = true;
        rowIndex = row + 2;
        colIndex = col - 1;
        if((colIndex < 0 || colIndex > 7) || (rowIndex < 0 || rowIndex > 7))
            legal_move = false;
        //If the color of the piece on the square to be moved to is the same, don't add the move.
        if(legal_move && board[rowIndex][colIndex].at(0) == color)
            legal_move = false;
        if(legal_move) {
            movelist[index] = index_to_square(rowIndex, colIndex);
            index++;
        }
        //To the left and up
        legal_move = true;
        rowIndex = row - 1;
        colIndex = col - 2;
        if((colIndex < 0 || colIndex > 7) || (rowIndex < 0 || rowIndex > 7))
            legal_move = false;
        //If the color of the piece on the square to be moved to is the same, don't add the move.
        if(legal_move && board[rowIndex][colIndex].at(0) == color)
            legal_move = false;
        if(legal_move) {
            movelist[index] = index_to_square(rowIndex, colIndex);
            index++;
        }
        //To the left and down
        legal_move = true;
        rowIndex = row + 1;
        colIndex = col - 2;
        if((colIndex < 0 || colIndex > 7) || (rowIndex < 0 || rowIndex > 7))
            legal_move = false;
        //If the color of the piece on the square to be moved to is the same, don't add the move.
        if(legal_move && board[rowIndex][colIndex].at(0) == color)
            legal_move = false;
        if(legal_move) {
            movelist[index] = index_to_square(rowIndex, colIndex);
            index++;
        }
        //To the right and up
        legal_move = true;
        rowIndex = row - 1;
        colIndex = col + 2;
        if((colIndex < 0 || colIndex > 7) || (rowIndex < 0 || rowIndex > 7))
            legal_move = false;
        //If the color of the piece on the square to be moved to is the same, don't add the move.
        if(legal_move && board[rowIndex][colIndex].at(0) == color)
            legal_move = false;
        if(legal_move) {
            movelist[index] = index_to_square(rowIndex, colIndex);
            index++;
        }
        //To the right and down
        legal_move = true;
        rowIndex = row + 1;
        colIndex = col + 2;
        if((colIndex < 0 || colIndex > 7) || (rowIndex < 0 || rowIndex > 7))
            legal_move = false;
        //If the color of the piece on the square to be moved to is the same, don't add the move.
        if(legal_move && board[rowIndex][colIndex].at(0) == color)
            legal_move = false;
        if(legal_move) {
            movelist[index] = index_to_square(rowIndex, colIndex);
            index++;
        }

        return;
    }

    //Find moves for bishops
    if(piece.at(1) == 'B') {
        //Diagonal moves from current square to top left.
        rowIndex = row;
        colIndex = col;
        for(colIndex = col - 1; colIndex >= 0; colIndex--) {
            rowIndex--;
            if(colIndex < 0 || colIndex > 7)
                break;
            if(rowIndex < 0 || rowIndex > 7)
                break;

            if(board[rowIndex][colIndex].at(0) == color)
                break; 

            movelist[index] = index_to_square(rowIndex, colIndex);
            index++;
        }

        //Diagonal moves from current square to bottom right.
        rowIndex = row;
        colIndex = col;
        for(colIndex = col + 1; colIndex < 8; colIndex++) {
            rowIndex++;

            if(colIndex < 0 || colIndex > 7)
                break;
            if(rowIndex < 0 || rowIndex > 7)
                break;

            if(board[rowIndex][colIndex].at(0) == color)
                break;                 

            movelist[index] = index_to_square(rowIndex, colIndex);
            index++;
        }

        //Diagonal moves from current square to top right.
        rowIndex = row;
        colIndex = col;
        for(colIndex = col + 1; colIndex < 8; colIndex++) {
            rowIndex--;

            if(colIndex < 0 || colIndex > 7)
                break;
            if(rowIndex < 0 || rowIndex > 7)
                break;

            if(board[rowIndex][colIndex].at(0) == color)
                break;                 

            movelist[index] = index_to_square(rowIndex, colIndex);
            index++;
        }

        //Diagonal moves from current square to bottom left.
        rowIndex = row;
        colIndex = col;
        for(colIndex = col - 1; colIndex >= 0; colIndex--) {
            rowIndex++;

            if(colIndex < 0 || colIndex > 7)
                break;
            if(rowIndex < 0 || rowIndex > 7)
                break;

            if(board[rowIndex][colIndex].at(0) == color)
                break;                 

            movelist[index] = index_to_square(rowIndex, colIndex);
            index++;
        }
        return;
    }

    //Find moves for rooks
    if(piece.at(1) == 'R') {
        //Vertical moves from current square to the top, not including the square the queen is on.
        rowIndex = row;
        colIndex = col;
        for(rowIndex = row + 1; rowIndex <= 7; rowIndex++) {
            //If the color of the piece on the square to be moved to is the same, don't add the move.
            if(board[rowIndex][colIndex].at(0) == color)
                break;

            movelist[index] = index_to_square(rowIndex, colIndex);
            index++;
        }

        //Vertical moves from current square to the bottom, not including the square the queen is on.
        rowIndex = row;
        colIndex = col;
        for(rowIndex = row - 1; rowIndex >= 0; rowIndex--) {
            //If the color of the piece on the square to be moved to is the same, don't add the move.
            if(board[rowIndex][colIndex].at(0) == color)
                break;

            movelist[index] = index_to_square(rowIndex, colIndex);
            index++;
        }

        //Horizontal moves from current square to the right, not including the square the queen is on.
        rowIndex = row;
        colIndex = col;
        for(colIndex = col + 1; colIndex <= 7; colIndex++) {
            //If the color of the piece on the square to be moved to is the same, don't add the move.
            if(board[rowIndex][colIndex].at(0) == color)
                break;

            movelist[index] = index_to_square(rowIndex, colIndex);
            index++;
        }

        //Horizontal moves from current square to the left, not including the square the queen is on.
        rowIndex = row;
        colIndex = col;
        for(colIndex = col - 1; colIndex >= 0; colIndex--) {
            //If the color of the piece on the square to be moved to is the same, don't add the move.
            if(board[rowIndex][colIndex].at(0) == color)
                break;

            movelist[index] = index_to_square(rowIndex, colIndex);
            index++;
        }
        return;
    }

    if(piece.at(1) == 'P') {
        //Moves for white pawns
        if(color == 'W') {
            legal_move = true;
            rowIndex = row;
            colIndex = col;
            if(board[rowIndex - 1][colIndex] != "[]") {
                legal_move = false;
            }

            if(legal_move) {
                movelist[index] = index_to_square(rowIndex - 1, colIndex);
                index++;
            }

            if(legal_move && (colIndex-1 >= 0) && (board[rowIndex - 1][colIndex - 1].at(0) != 'W' && board[rowIndex - 1][colIndex - 1] != "[]")) {
                movelist[index] = index_to_square(rowIndex - 1, colIndex - 1);
                index++;
            }

            if(legal_move && (colIndex + 1 <= 7) && (board[rowIndex - 1][colIndex + 1].at(0) != 'W' && board[rowIndex + 1][colIndex + 1] != "[]")) {
                movelist[index] = index_to_square(rowIndex - 1, colIndex + 1);
                index++;
            }

            if(legal_move && rowIndex == 6) {
                if(board[rowIndex - 2][colIndex] != "[]") {
                    legal_move = false;
                }

                if(legal_move) {
                    movelist[index] = index_to_square(rowIndex - 2, colIndex);
                    index++;
                }
            }
        }

        //Moves for black pawns
        if(color == 'B') {
            legal_move = true;
            rowIndex = row;
            colIndex = col;
            if(board[rowIndex + 1][colIndex] != "[]") {
                legal_move = false;
            }

            if(legal_move && (colIndex-1 >= 0) && (board[rowIndex + 1][colIndex - 1].at(0) != 'B' && board[rowIndex + 1][colIndex - 1] != "[]")) {
                movelist[index] = index_to_square(rowIndex + 1, colIndex - 1);
                index++;
            }

            if(legal_move && (colIndex + 1 <= 7) && (board[rowIndex + 1][colIndex + 1].at(0) != 'B' && board[rowIndex + 1][colIndex + 1] != "[]")) {
                movelist[index] = index_to_square(rowIndex + 1, colIndex + 1);
                index++;
            }

            if(legal_move) {
                movelist[index] = index_to_square(rowIndex + 1, colIndex);
                index++;
            }
                
            if(legal_move && rowIndex == 1) {
                if(board[rowIndex + 2][colIndex] != "[]") {
                    legal_move = false;
                }
                
                if(legal_move) {
                    movelist[index] = index_to_square(rowIndex + 2, colIndex);
                    index++;
                }
                    
            }
        }

        return;
    }

    return;
}

void move_piece(std::string board[8][8], std::string start_square, std::string end_square) {
    int row1;
    int col1;

    int row2;
    int col2;

    square_to_index(start_square, row1, col1);
    square_to_index(end_square, row2, col2);

    board[row2][col2] = board[row1][col1];
    board[row1][col1] = "[]";

    return;
}

bool parse_move(std::string move, bool color, std::string &piece, std::string &move_square) {
    if(move.length() > 6) {
        return false;
    }
    if(move.length() < 2) {
        return false;
    }

    //This block sets the color of the piece, and then appends the type.
    if(color) 
        piece = "W ";
    else piece = "B ";

    if(isdigit(move.at(1))) {
        piece.at(1) = 'P';
        move_square.at(0) = toupper(move.at(0));
        move_square.at(1) = move.at(1);
    }
    else {
        piece.at(1) = toupper(move.at(0));
        move_square.at(0) = toupper(move.at(1));
        move_square.at(1) = move.at(2);
    }

    return true;
}

bool make_move(std::string board[8][8], std::string move, bool color) {
    std::string piece = "";
    std::string movesquare = "  ";

    bool move_found;

    if(!parse_move(move, color, piece, movesquare)) {
        std::cout << "Invalid move!" << std::endl;
        return false;
    }
        
    std::string movelist[64] = {""};

    int row = 0;
    int col = -1;
    move_found = false;
    while(!move_found) {
        col = col + 1;
        if(col == 8) {
            col = 0;
            row++;
        }
        for(row = row; row < 8; row++) {
            while(col < 7) {
                // std::cout << "Row: " << row << " Col: " << col << std::endl;
                if(board[row][col] == piece) {
                    break;
                }
                col++;
            }
            // std::cout << "Row: " << row << " Col: " << col << std::endl;
            if(board[row][col] == piece) {
                break;
            }
            col = 0;
        }

        std::cout << "Row: " << row << " Col: " << col << std::endl;

        if(row >= 8 || col >= 8) {
            std::cout << "Invalid move!" << std::endl;
            return false;
        }

        //Stores a list of legal moves in movelist
        legal_moves(movelist, board, row, col, piece);

        // Prints out the list of legal moves
        std::cout << "Legal moves: ";
        for(int i = 0; i < 64; i++) {
            if(movelist[i] == "") {
                std::cout << std::endl;
                break;
            }
            std::cout << movelist[i] << " ";
        }

        for(int i = 0; i < 64; i++) {
            if(movelist[i] == movesquare) {
                move_piece(board, index_to_square(row, col), movesquare);
                std::cout << "Valid move!" << std::endl;
                move_found = true;
                return true;
            }
        }
    }

    std::cout << "Invalid move!" << std::endl;
    return false;
}

int main() {
    int row = 0;
    int col = 0;

    std::string current_move;

    bool game_running = true;
    bool current_color = true;

    std::string chessboard[8][8];

    initializeboard(chessboard);

    while(game_running) {
        request_move(chessboard, current_color);
    }

    return 0;
}