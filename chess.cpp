#include <cstdlib>
#include<iostream>
#include<string>
#include <ctype.h>
#include<cmath>

void initializeboard();
void print_board();
void square_to_index(std::string square, int &row, int &col);
std::string index_to_square(int row, int col);
void request_move();
bool king_in_check(std::string board[8][8], char color);
void horizontal_moves(std::string board[8][8], std::string *movelist, int &index, int row, int col, std::string piece);
void vertical_moves(std::string board[8][8], std::string *movelist, int &index, int row, int col, std::string piece);
void diagonal_moves(std::string board[8][8], std::string *movelist, int &index, int row, int col, std::string piece);
void knight_moves(std::string board[8][8], std::string *movelist, int &index, int row, int col, std::string piece);
void piece_simple_moves(std::string board[8][8], std::string *movelist, int &index, int row, int col, std::string piece);
void color_simple_moves(std::string board[8][8], std::string *movelist, char color);
void color_legal_moves(std::string board[8][8], std::string *legal_moves, char color);
void move_piece(std::string board[8][8], std::string start_square, std::string end_square, bool test);
bool parse_move(std::string move, char color, std::string &piece, std::string &move_square);
bool test_move(std::string piece, std::string from_square, std::string to_square);
bool make_move(std::string move, char color);

std::string board[8][8] = {""};
std::string test_board[8][8] = {""};
std::string en_passantable_pawn = "";
int en_passant_reset_timer = 1;

char state_color = 'W';
char state_opponent = 'B';

bool white_castling_rights[2] = {true, true};
bool black_castling_rights[2] = {true, true};

void initializeboard() {
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

void print_board() {
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

void request_move() {
    std::string move;

    bool found_move = false;

    while(!found_move) {
        print_board();
        if(state_color == 'W') {
            std::cout << "Enter a move for white: ";
        }
        else {
            std::cout << "Enter a move for black: ";
        }

        std::cin >> move;

        found_move = make_move(move, state_color);
    }

    return;
}

void random_bot() {
    int index = 0;
    int row, col;
    int randomIndex;

    std::string possible_moves[128] = {""};
    color_legal_moves(board, possible_moves, state_color);

    if(possible_moves[0] == "")
        return;
    for(index = 0; index < 128; index++) {
        if(possible_moves[index] == "") {
            break;
        }
    }
    randomIndex = rand()%index; 

    square_to_index(possible_moves[randomIndex].substr(0,2), row, col);
    std::cout << "BOT: I played " << board[row][col] << " to " << possible_moves[randomIndex].substr(2,2) << std::endl;

    move_piece(board, possible_moves[randomIndex].substr(0,2), possible_moves[randomIndex].substr(2,2), false);

    if(en_passant_reset_timer > 1)
        en_passant_reset_timer--;
    if(en_passant_reset_timer == 1) {
        en_passantable_pawn = "";
        en_passant_reset_timer = 0;
    }

    return;
}

void color_legal_moves(std::string board[8][8], std::string *legal_moves, char color) {
    int row, col;

    int legal_index = 0;

    std::string piece;

    std::string potential_moves[256] = {""};

    color_simple_moves(board, potential_moves, color);

    for(int j = 0; j < 256; j++) {
        if(legal_moves[j] == "")
            break;
        legal_moves[j] = "";
    }

    for(int i = 0; i < 256; i++) {
        if(potential_moves[i] == "") {
            break;
        }

        square_to_index(potential_moves[i].substr(0,2), row, col);
        piece = board[row][col];

        if(test_move(piece, potential_moves[i].substr(0,2), potential_moves[i].substr(2,2))) {
            legal_moves[legal_index] = potential_moves[i];
            legal_index++;
        }
    }

    return;
}

void color_simple_moves(std::string board[8][8], std::string *movelist, char color) {
    int row, col;

    int index = 0;

    for(row = 0; row < 8; row++) {
        for(col = 0; col < 8; col++) {
            if(board[row][col] == "[]")
                continue;

            if(board[row][col].at(0) != color)
                continue;

            piece_simple_moves(board, movelist, index, row, col, board[row][col]);
        }
    }

    return;
}

bool king_in_check(std::string board[8][8], char color) {
    int index = 0;

    int row, col;

    char enemy_color;

    if(color == 'W')
        enemy_color = 'B';
    else enemy_color = 'W';

    std::string potential_moves[256] = {""};
    color_simple_moves(board, potential_moves, enemy_color);

    std::string king_piece = " K";
    king_piece.at(0) = color;

    std::string king_square;

    for(row = 0; row < 8; row++) {
        for(col = 0; col < 8; col++) {
            if(board[row][col] == king_piece) {
                king_square = index_to_square(row, col);
                goto OUTSIDELOOP;
            }
        }
    }
    OUTSIDELOOP:
    
    int i = 0;
    while(potential_moves[i] != "") {
        if(potential_moves[i].substr(2, 2) == king_square) {
            return true;
        }
        i++;
        if(i > 256)
            break;
    }

    return false;
}

void horizontal_moves(std::string board[8][8], std::string *movelist, int &index, int row, int col, std::string piece) {
    bool legal_move = true;

    char color = piece.at(0);

    int rowIndex = 0;
    int colIndex = 0;

    std::string move;

    rowIndex = row;
    colIndex = col;

    //Horizontal moves from current square to the right, not including the square the queen is on.
    rowIndex = row;
    colIndex = col;
    for(colIndex = col + 1; colIndex <= 7; colIndex++) {
        //If the color of the piece on the square to be moved to is the same, don't add the move.
        if(board[rowIndex][colIndex].at(0) == color)
            break;
            
        //If the color of the piece on the square to be moved to is different add the move and break.
        if(isalpha(board[rowIndex][colIndex].at(0)) && board[rowIndex][colIndex].at(0) != color) {
            movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex, colIndex));
            index++;
            break;
        }

        movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex, colIndex));
        index++;
    }

    //Horizontal moves from current square to the left, not including the square the queen is on.
    rowIndex = row;
    colIndex = col;
    for(colIndex = col - 1; colIndex >= 0; colIndex--) {
        //If the color of the piece on the square to be moved to is the same, don't add the move.
        if(board[rowIndex][colIndex].at(0) == color)
            break;
            
        //If the color of the piece on the square to be moved to is different add the move and break.
        if(isalpha(board[rowIndex][colIndex].at(0)) && board[rowIndex][colIndex].at(0) != color) {
            movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex, colIndex));
            index++;
            break;
        }

        movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex, colIndex));
        index++;
    }

    return;
}

void vertical_moves(std::string board[8][8], std::string *movelist, int &index, int row, int col, std::string piece) {
    bool legal_move = true;

    char color = piece.at(0);

    int rowIndex = 0;
    int colIndex = 0;

    std::string move;

    rowIndex = row;
    colIndex = col;

    //Vertical moves from current square to the top, not including the square the queen is on.
    rowIndex = row;
    colIndex = col;
    for(rowIndex = row + 1; rowIndex <= 7; rowIndex++) {
        //If the color of the piece on the square to be moved to is the same, don't add the move.
        if(board[rowIndex][colIndex].at(0) == color)
            break;
            
        //If the color of the piece on the square to be moved to is different add the move and break.
        if(isalpha(board[rowIndex][colIndex].at(0)) && board[rowIndex][colIndex].at(0) != color) {
            movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex, colIndex));
            index++;
            break;
        }

        movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex, colIndex));
        index++;
    }

    //Vertical moves from current square to the bottom, not including the square the queen is on.
    rowIndex = row;
    colIndex = col;
    for(rowIndex = row - 1; rowIndex >= 0; rowIndex--) {
        //If the color of the piece on the square to be moved to is the same, don't add the move.
        if(board[rowIndex][colIndex].at(0) == color)
            break;
            
        //If the color of the piece on the square to be moved to is different add the move and break.
        if(isalpha(board[rowIndex][colIndex].at(0)) && board[rowIndex][colIndex].at(0) != color) {
            movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex, colIndex));
            index++;
            break;
        }

        movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex, colIndex));
        index++;
    }

    return;
}

void diagonal_moves(std::string board[8][8], std::string *movelist, int &index, int row, int col, std::string piece) {
    bool legal_move = true;

    char color = piece.at(0);

    int rowIndex = 0;
    int colIndex = 0;

    std::string move;

    rowIndex = row;
    colIndex = col;

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
            
        //If the color of the piece on the square to be moved to is different add the move and break.
        if(isalpha(board[rowIndex][colIndex].at(0)) && board[rowIndex][colIndex].at(0) != color) {
            movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex, colIndex));
            index++;
            break;
        }

        movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex, colIndex));
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

        //If the color of the piece on the square to be moved to is different add the move and break.
        if(isalpha(board[rowIndex][colIndex].at(0)) && board[rowIndex][colIndex].at(0) != color) {
            movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex, colIndex));
            index++;
            break;
        }

        movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex, colIndex));
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

        //If the color of the piece on the square to be moved to is the same, don't add the move and break.
        if(board[rowIndex][colIndex].at(0) == color)
            break;

        //If the color of the piece on the square to be moved to is different add the move and break.
        if(isalpha(board[rowIndex][colIndex].at(0)) && board[rowIndex][colIndex].at(0) != color) {
            movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex, colIndex));
            index++;
            break;
        }
            
        movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex, colIndex));
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
            
        //If the color of the piece on the square to be moved to is different add the move and break.
        if(isalpha(board[rowIndex][colIndex].at(0)) && board[rowIndex][colIndex].at(0) != color) {
            movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex, colIndex));
            index++;
            break;
        }

        movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex, colIndex));
        index++;
    }

    return;
}

void knight_moves(std::string board[8][8], std::string *movelist, int &index, int row, int col, std::string piece) {
    bool legal_move = true;

    char color = piece.at(0);

    int rowIndex = 0;
    int colIndex = 0;

    std::string move;

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
        movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex, colIndex));
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
        movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex, colIndex));
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
        movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex, colIndex));
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
        movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex, colIndex));
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
        movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex, colIndex));
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
        movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex, colIndex));
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
        movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex, colIndex));
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
        movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex, colIndex));
        index++;
    }

    return;
}

void piece_simple_moves(std::string board[8][8], std::string *movelist, int &index, int row, int col, std::string piece) {
    bool legal_move = true;

    char color = piece.at(0);

    int rowIndex = 0;
    int colIndex = 0;

    std::string move;
    //Find moves for the king
    if(piece.at(1) == 'K') {
        for(rowIndex = row - 1; rowIndex <= row + 1; rowIndex++) {
            if(rowIndex < 0 || rowIndex > 7) {
                continue;
            }
            for(colIndex = col - 1; colIndex <= col + 1; colIndex++) {
                //Makes sure the indices are within the correct bounds
                if((rowIndex == row && colIndex == col) || colIndex < 0 || colIndex > 7) {
                    continue;
                }

                //If the color of the piece on the square to be moved to is the same, don't add the move.
                if(board[rowIndex][colIndex].at(0) == color)
                    continue;
                
                movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex, colIndex));
                index++;
            }
        }

        return;
    }

    //Find moves for queens
    if(piece.at(1) == 'Q') {
        horizontal_moves(board, movelist, index, row, col, piece);
        vertical_moves(board, movelist, index, row, col, piece);
        diagonal_moves(board, movelist, index, row, col, piece);

        return;
    }

    //Find moves for knights
    if(piece.at(1) == 'N') {
        knight_moves(board, movelist, index, row, col, piece);

        return;
    }

    //Find moves for bishops
    if(piece.at(1) == 'B') {
        diagonal_moves(board, movelist, index, row, col, piece);

        return;
    }

    //Find moves for rooks
    if(piece.at(1) == 'R') {
        horizontal_moves(board, movelist, index, row, col, piece);
        vertical_moves(board, movelist, index, row, col, piece);
        
        return;
    }

    //Find moves for pawns
    if(piece.at(1) == 'P') {
        //Moves for white pawns
        if(color == 'W') {
            rowIndex = row;
            colIndex = col;

            if(board[rowIndex - 1][colIndex] == "[]") {
                movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex - 1, colIndex));
                index++;
            }

            //Capturing pieces
            if(legal_move && (colIndex-1 >= 0) && (board[rowIndex - 1][colIndex - 1].at(0) == 'B')) {
                movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex - 1, colIndex - 1));
                index++;
            }
            if(legal_move && (colIndex + 1 <= 7) && (board[rowIndex - 1][colIndex + 1].at(0) == 'B')) {
                movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex - 1, colIndex + 1));
                index++;
            }

            //En passant
            if((colIndex-1 >= 0) && (board[rowIndex][colIndex - 1].at(0) == 'B') && (index_to_square(rowIndex, (colIndex-1)) == en_passantable_pawn)) {
                if(board[rowIndex - 1][colIndex - 1] == "[]") {
                    movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex - 1, colIndex - 1));
                    index++;
                }
            }
            if((colIndex + 1 <= 7) && (board[rowIndex][colIndex + 1].at(0) == 'B') && (index_to_square(rowIndex, (colIndex+1)) == en_passantable_pawn)) {
                if(board[rowIndex - 1][colIndex + 1] == "[]") {
                    movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex - 1, colIndex + 1));
                    index++;
                }
            }

            if(board[rowIndex - 1][colIndex] == "[]" && board[rowIndex - 2][colIndex] == "[]" && rowIndex == 6) {
                movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex - 2, colIndex));
                index++;
            }

            return;
        }

        //Moves for black pawns
        if(color == 'B') {
            rowIndex = row;
            colIndex = col;

            //Capturing pieces
            if(legal_move && (colIndex-1 >= 0) && (board[rowIndex + 1][colIndex - 1].at(0) == 'W')) {
                movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex + 1, colIndex - 1));
                index++;
            }

            if(legal_move && (colIndex + 1 <= 7) && (board[rowIndex + 1][colIndex + 1].at(0) == 'W')) {
                movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex + 1, colIndex + 1));
                index++;
            }

            if(board[rowIndex + 1][colIndex] == "[]") {
                movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex + 1, colIndex));
                index++;
            }

            //En passant
            if(legal_move && (colIndex-1 >= 0) && (board[rowIndex][colIndex - 1].at(0) == 'W') && (index_to_square(rowIndex, (colIndex-1)) == en_passantable_pawn)) {
                if(board[rowIndex + 1][colIndex - 1] == "[]") {
                    movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex - 1, colIndex - 1));
                    index++;
                }
            }
            if(legal_move && (colIndex + 1 <= 7) && (board[rowIndex][colIndex + 1].at(0) == 'W') && (index_to_square(rowIndex, colIndex+1) == en_passantable_pawn)) {
                if(board[rowIndex + 1][colIndex + 1] == "[]") {
                    movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex + 1, colIndex + 1));
                    index++;
                }
            }
                
            if(board[rowIndex + 1][colIndex] == "[]" && board[rowIndex + 2][colIndex] == "[]" && rowIndex == 1) {
                movelist[index].append(index_to_square(row, col)).append(index_to_square(rowIndex+2, colIndex));
                    index++;
            }

            return;
        }

        return;
    }

    return;
}

void move_piece(std::string board[8][8], std::string start_square, std::string end_square, bool test) {
    int row1, col1, row2, col2; 

    square_to_index(start_square, row1, col1);
    square_to_index(end_square, row2, col2);


    //When moving a pawn two squares up, makes it eligible for en passant
    if(test == false) {
        if(board[row1][col1].at(1) == 'P' && abs(row2 - row1) == 2) {
            en_passantable_pawn = index_to_square(row2, col1);
            en_passant_reset_timer = 3;
        }
    }

    //If the move is en passant, removes the en-passanted pawn.
    if(board[row1][col1] == "WP" && board[row2+1][col2] == "BP" && index_to_square(row2+1, col2) == en_passantable_pawn) {
        board[row2+1][col2] = "[]";
        if(test == false) {
            en_passantable_pawn = "";
        }
    }
    if(board[row1][col1] == "BP" && board[row2-1][col2] == "WP" && index_to_square(row2-1, col2) == en_passantable_pawn) {
        board[row2-1][col2] = "[]";
        if(test == false) {
            en_passantable_pawn = "";
        }
        
    }

    board[row2][col2] = board[row1][col1];
    board[row1][col1] = "[]";

    if((row2 == 7 || row2 == 0) && board[row2][col2].at(1) == 'P') {
        board[row2][col2].at(1) = 'Q';
    }

    return;
}

bool parse_move(std::string move, char color, std::string &piece, std::string &move_square) {
    if(move.length() > 7) {
        return false;
    }
    if(move.length() < 2) {
        return false;
    }

    //This block sets the color of the piece, and then appends the type.
    piece = "  ";
    piece.at(0) = color;

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

bool test_move(std::string piece, std::string from_square, std::string to_square) {
    char color = piece.at(0);

    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            test_board[i][j] = board[i][j];
        }
    }

    move_piece(test_board, from_square, to_square, true);
    if(king_in_check(test_board, color)) {
        // std::cout << "Invalid move because the " << color  << " king would be in check!" << std::endl;
        return false;
    }

    return true;
}

bool make_move(std::string move, char color) {
    int row, col, index = 0;

    std::string piece = "";
    std::string movesquare = "  ";

    if(!parse_move(move, color, piece, movesquare)) {
        std::cout << "Invalid move! Please use proper chess notation!" << std::endl;
        return false;
    }

    //Stores all the legal moves for the current color.
    std::string legal_moves[128] = {""};
    color_legal_moves(board, legal_moves, piece.at(0));

    if(legal_moves[0] == "")
        return false;

    for(int i = 0; i < 128; i++) {
        if(legal_moves[i] == "")
            break;
        square_to_index(legal_moves[i].substr(0,2), row, col);
        if(legal_moves[i].substr(2,2) == movesquare && board[row][col] == piece) {
            move_piece(board, legal_moves[i].substr(0,2), movesquare, false);

            //If it is a valid move, moves the en passant rolling window.
            if(en_passant_reset_timer > 1)
                en_passant_reset_timer--;
            if(en_passant_reset_timer == 1) {
                en_passantable_pawn = "";
                en_passant_reset_timer = 0;
            }
            return true;
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

    initializeboard();

    std::string legal_moves[128] = {""};

    while(game_running) {
        if(current_color == true) {
            state_color = 'W';
            state_opponent = 'B';

            // std::cout << "Colors: " << state_color << state_opponent << std::endl;
            request_move();
        }
        else {
            state_color = 'B';
            state_opponent = 'W';
            random_bot();
        }

        color_legal_moves(board, legal_moves, state_color);
        
        if(legal_moves[0] == "" && king_in_check(board, state_color)) {
            game_running = false;
            print_board();
            std::cout << "The game ended with " << state_opponent << " checkmating the " << state_color << " king." << std::endl;

            goto GAMEOVER;
        }
        if(legal_moves[0] == "" && !king_in_check(board, state_color)) {
            game_running = false;
            print_board();
            std::cout << "Stalemate!" << std::endl;

            goto GAMEOVER;
        }

        current_color = !current_color;
    }
    GAMEOVER:

    return 0;
}