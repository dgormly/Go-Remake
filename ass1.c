/*
 * CSSE2310 Assignment 1 - NoGo
 * Description: Atari Go remake.
 * Initial inputs: 
 *  p1type p2type ('h' for human or 'c' for computer)[height width | filename]
 * Last modified: 09/08/2016
 * Author: Daniel Gormly
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Player Struct: Holds the player variables.
typedef struct Player {
    int num;
    char type;
    char symbol;
} Player;

// File Statistics
typedef struct FileIn {
    int newLineCount;
    int spaceCount;
    int count;
    int xPlayerCount;
    int oPlayerCount;
    int dotCount;
} FileIn;

// Game Struct: Holds main game variables.
typedef struct Game {
    int numRows;
    int numCols;
    bool gameOver;
    int** movePos;
    long settings[9];
    int oCounter;
    int xCounter;
    FileIn fileDetails;

    Player player[2];
    int playersTurn;
} Game;

// Initalises game struct with default variables.
void game_init(Game* gameDetails) {
    gameDetails->numRows = 0;
    gameDetails->numCols = 0;
    gameDetails->gameOver = false;
    gameDetails->playersTurn = 0;
    gameDetails->oCounter = 0;
    gameDetails->xCounter = 0;
}

// Initalises file counts to zero.
void file_init(FileIn* fileDetails) {
    fileDetails->newLineCount = 0;
    fileDetails->spaceCount = 0;
    fileDetails->count = 0;
    fileDetails->xPlayerCount = 0;
    fileDetails->oPlayerCount = 0;
    fileDetails->dotCount = 0;
}

/* Prototypes */
bool is_stone(Game* game, int* x, int* y);
void set_game_xy(Game* game, int x, int y);
bool valid_start_input(Game* game, int* numArguments, char** arguments);
void print_board(Game* game);
void set_players(Game* game, int playerNum, char* type);
bool get_human_input(Game* game, int playerNum);
bool valid_move_input(Game* game, int x, int y);
void add_stone(Game* game, int x, int y);
void next_player(Game* game);
void read_file_settings(Game* gameDetails, char* fileName);
bool validate_file(Game* gameDetails, char* fileName);
void read_board(Game* gameDetails, char* fileName);
void update_settings(Game* gameDetails);
void generate_initial_computer_move(Game* gameDetails, char* symbol);
bool check_board_dimensions(int x, int y);
bool check_board(Game* gameDetails, int player);
bool check_adjacent(Game* gameDetails, int x, int y, int player);

// Main loop.
int main(int argc, char** argv) {	
	// Game Struct setup.
    Game* gameDetails = malloc(sizeof(Game));	
    game_init(gameDetails);
    int* playersTurn = &gameDetails->playersTurn;
    // Check inputs
    valid_start_input(gameDetails, &argc, argv);
    print_board(gameDetails);    
    set_players(gameDetails, 0, argv[1]);
    set_players(gameDetails, 1, argv[2]);
    // Start game.
    while (1) {
        // If input human.
        if (gameDetails->player[*playersTurn].type == 'h') {
            get_human_input(gameDetails, *playersTurn);
        }
    }
    return 0;
}

// Validates argv input.
bool valid_start_input(Game* gameDetails, int* numArgs, char** args) {
    // Check numeber of inputs
    if (*numArgs == 4 || *numArgs == 5) {
        // Check first input.
        if(*args[1] == 'c' && *(args[1] + 1) == '\0') {
    	    // first input is =computer.
        } else if (*args[1] == 'h' && *(args[1] + 1) == '\0') {
            // first input is human.
        } else {
            fprintf(stderr, "Invalid player type\n");
            exit(2);
        }
        // Check second input.
        if(*args[2] == 'c' && *(args[2] + 1) == '\0') {
            // second input is computer.
        } else if (*args[2] == 'h' && *(args[2] + 1) == '\0') {
            // second input is human.
        } else {
            fprintf(stderr, "Invalid player type\n");
            exit(2);
        }
        // Start new game.
        if (*numArgs == 5) {
            char* pointer3;
            char* pointer4;
            long arg3 = strtol(args[3], &pointer3, 10);
            long arg4 = strtol(args[4], &pointer4, 10);
            if (check_board_dimensions(arg3, arg4)) {
                fprintf(stderr, "Invalid board dimension\n");
                exit(3);
            }
            set_game_xy(gameDetails, arg4, arg3);    
        }

        // Read game from file.
        if (*numArgs == 4 && validate_file(gameDetails, args[3])) {
            read_file_settings(gameDetails, args[3]);
            set_game_xy(gameDetails, gameDetails->numCols,
                    gameDetails->numRows);
            read_board(gameDetails, args[3]);
        }
        
        return true;
    } else {
        // argv format incorrect.
        fprintf(stderr, 
                "Usage: nogo p1type p2type [height width | filename]\n");
        exit(1);
    }
}
 
/* Sets initial game X and Y values of board. */
void set_game_xy(Game* gameDetails, int x, int y) {
    gameDetails->numRows = y;
    gameDetails->numCols = x; 
    gameDetails->movePos = malloc(sizeof(int*) * y);
     // Set up grid with -1 ('.')
    for (int i = 0; i < y; ++i) {
        gameDetails->movePos[i] = malloc(sizeof(int) * x);
    }
    for (int boardY = 0; boardY < y; boardY++) {
     	for (int boardX = 0; boardX < x; boardX++) {
            gameDetails->movePos[boardY][boardX] = -1;
     	}
    }
}

/* Print current board setup. */
void print_board(Game* gameDetails) {
    // Print top of board.
    printf("/");
    for (int i = 0; i < gameDetails->numCols; i++) {
        printf("-");
    }  
    printf("\\\n");
    for (int row = 0; row < gameDetails->numRows; row++) {
        printf("|");
        // Print contents.
        for (int cols = 0; cols < gameDetails->numCols; cols++) {
            if (gameDetails->movePos[row][cols] == 0) {
                printf("O");
                continue;
            } else if (gameDetails->movePos[row][cols] == 1) {
                printf("X");
                continue;
            } else {
                printf(".");
            }
        }
        printf("|\n");
    }
    // Print base.
    printf("\\");
    for (int i = 0; i < gameDetails->numCols; i++) {
        printf("-");
    }
    printf("/\n");
}

/* Sets player profiles up. (Human or Computer). */
void set_players(Game* gameDetails, int playerNum, char* type) {
    if (*type == 'c') {
        gameDetails->player[playerNum].type = *type;
    } else if (*type == 'h') {
        gameDetails->player[playerNum].type = *type;
    }
    if (playerNum == 0) {
        gameDetails->player[playerNum].symbol = 'O';
    } else {
        gameDetails->player[playerNum].symbol = 'X';
    }
}

/* Gets the user's input and returns false if thereis a winner. */
bool get_human_input(Game* gameDetails, int playerNum) {
    int x;
    int y;
    int count = 0;
    char input[10] = {'\0'};
    char c;
    char* err1;
    char* err2;	
    // Get user input.
    printf("Player %c> ", gameDetails->player[playerNum].symbol);
    while ((c = fgetc(stdin)) != '\n') {
        if (feof(stdin) && count == 0) {
            fprintf(stderr, "End of input from user\n");
            exit(6);
        } else if (feof(stdin)) {
            printf("\n");
            return false;
        }
        if (count > 8) {
            continue;
        } else {
            input[count] = c;
        }
        count++;
    }
    x = strtol(input, &err1, 10);
    y = strtol(err1 + 1, &err2, 10);
    if (x == 0 && input[0] != '0') {
        return false;
    }
    if (y == 0 && input[2] != '0') {
        return false;
    }
    if (*err1 != ' ' || *err2 != '\0' || count > 8) {
        return false;
    }
    // If valid, add it to the board
    if (valid_move_input(gameDetails, x, y)) {
        add_stone(gameDetails, y, x);
        if (check_board(gameDetails, gameDetails->playersTurn) == false) {
            next_player(gameDetails);
            printf("Player %c wins\n", 
                    gameDetails->player[gameDetails->playersTurn].symbol);
            exit(0);
        }
        return true;
    }
    return false;
}

/* Returns true if input is a valid move. */   
bool valid_move_input(Game* gameDetails, int y, int x) {
    if (y < gameDetails->numRows && y >= 0 
            && x >= 0 && x < gameDetails->numCols) {
        if (gameDetails->movePos[y][x] == -1) {
            return true;
        }
    }
    return false;
}

/* Updates game struct board details */
void add_stone(Game* gameDetails, int y, int x) {
    if (gameDetails->playersTurn == 0) {
        gameDetails->oCounter++;
    } else {
        gameDetails->xCounter++;
    }
    gameDetails->movePos[x][y] = gameDetails->playersTurn;
    next_player(gameDetails);
    print_board(gameDetails);
}

/* Updates game struct to current players turn. */
void next_player(Game* gameDetails) {
    if (gameDetails->playersTurn == 0) {
        gameDetails->playersTurn = 1;
    } else {
        gameDetails->playersTurn = 0;
    }
}

/* Creates statistics of imported File. */
bool validate_file(Game* gameDetails, char* fileName) {
    char c;
    int newLineCount = 0;
    int spaceCount;
    int count = 0;
    int xPlayerCount = 0;
    int oPlayerCount = 0;
    int dotCount = 0;
    FILE* file;
    file = fopen(fileName, "r");
    if (file == NULL || feof(file)) {
        fprintf(stderr, "Unable to open file\n");
        exit(4);
    }
    // Process characters.
    while ((c = fgetc(file)) != EOF) {
        switch(c) {
            case '\n':
                newLineCount++;
                break;
            case ' ':
                spaceCount++;
                break;
            case 'O':
                oPlayerCount++;
                break;
            case 'X':
                xPlayerCount++;
                break;
            case '.':
                dotCount++;
        }
        count++;
    }
    // Store in FileIn Struct.
    gameDetails->fileDetails.newLineCount = newLineCount;
    gameDetails->fileDetails.spaceCount = spaceCount;
    gameDetails->fileDetails.xPlayerCount = xPlayerCount;
    gameDetails->fileDetails.oPlayerCount = oPlayerCount;
    gameDetails->fileDetails.dotCount = dotCount;
    gameDetails->fileDetails.count = count;
    fflush(file);
    fclose(file);
    return true;
}

// Reads the given line.
void read_file_settings(Game* gameDetails, char* fileName) {
    char* result = malloc(sizeof(char) * 50);
    FILE* file;
    char* err;
    long info[9];
    file = fopen(fileName, "r");
    int position = 0;
    int next = 0;
    char* pos;
    int oCount = 0;
    int xCount = 0;
    
    while(1) {
        next = fgetc(file);
        if (next == 'X') {
            xCount++;
        }
        if (next == 'O') {
            oCount++;
        }
        if (next == '\n') {
            result[position] = '\0';
            break;
        } else {
            result[position++] = (char)next;
        }
    }
    pos = result;
    for (int i = 0; i < 9; i++) {
        info[i] = strtol(pos, &err, 10);
        if ((err[0] != ' ' && i != 8) || (err[0] != '\0' && i == 8)) {
            printf("%d", err[0]);
            fprintf(stderr, "Incorrect file contents\n");
            exit(5);
        }
        pos = err + 1;
    }
    gameDetails->oCounter = oCount;
    gameDetails->xCounter = xCount;
    fclose(file);
    memcpy(gameDetails->settings, info, 9 * sizeof(long));
    update_settings(gameDetails);
}

/* Reads the board part of the file.check_adjacent */
void read_board(Game* gameDetails, char* fileName) {
    int col = 0;
    int row = 0;
    int count = 0;
    int symbol = -1;
    char c;
    FILE* file;
    file = fopen(fileName, "r"); 
    // Scan to second line.
    while ((c = fgetc(file)) != '\n');
    // Begin reading grid.
    while ((c = fgetc(file)) != EOF) {
        count++;
        switch(c) {
            case 'X':
                symbol = 1;
                break;
            case 'O':
                symbol = 0;
                break;
            case '.':
                symbol = -1;
        }
        // Check for errors on grid.
        if (c != 'X' && c != 'O' && c != '.' && c != '&' && c != '\n') {
            fprintf(stderr, "Incorrect file contents\n");
            exit(5);
        }
        if (c == '\n') {
            if (col != gameDetails->numCols) {
                fprintf(stderr, "Incorrect file contents\n");
                exit(5);
            }
            col = 0;
            row++;
        } else {
            gameDetails->movePos[row][col] = symbol;
            col++;
        }
    }
    if (row != gameDetails->numRows) {
        fprintf(stderr, "Incorrect file contents\n");
        exit(5);
    }
    
}

/* Updates the settings of the game. */
void update_settings(Game* gameDetails) {
    if (check_board_dimensions(gameDetails->settings[0], 
            gameDetails->settings[1])) {
        fprintf(stderr, "Incorrect file contents\n");
        exit(5);
    }
    gameDetails->numRows = gameDetails->settings[0];
    gameDetails->numCols = gameDetails->settings[1];
    gameDetails->playersTurn = gameDetails->settings[2];
}

/* Checks if board dimensions are valid. */
bool check_board_dimensions(int x, int y) {
    if (x < 4 || 1000 < x || y < 4 || 1000 < y) {
        return true;
    }
    return false;
}

/* Iterates through the board checking all posible positions and stones
   and returns false if game is over. */
bool check_board(Game* gameDetails, int player) {
    int rows = gameDetails->numRows;
    int cols = gameDetails->numCols;
    // Check ever row and column.
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            if (gameDetails->movePos[y][x] == player          
                    && check_adjacent(gameDetails, x, y, player) == false) {
                return false;
            }
        }
    }
    return true;
}

/* Returns true if all stones for a player are valid. */
bool check_adjacent(Game* gameDetails, int x, int y, int player) {
    if (y > 0) {
        if (gameDetails->movePos[(y - 1)][x] == -1) {
            return true;
        }
    }
    if (y < gameDetails->numRows - 1) {
        if (gameDetails->movePos[(y + 1)][x] == -1) {
            return true;
        }
    }
    if (x > 0) {
        if (gameDetails->movePos[y][(x - 1)] == -1) {
            return true;
        }
    }
    if (x < gameDetails->numCols - 1) {
        if (gameDetails->movePos[y][(x + 1)] == -1) {
            return true;
        }
    }
    return false;
}
