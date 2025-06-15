#include"raylib.h"
#include<stdlib.h>
#include<stdbool.h>
#include<time.h>
#include<math.h>

#define BOARD_SIZE 8
#define TILE_SIZE 42
#define TILE_TYPES 5

const char tile_chars[TILE_TYPES] = {'#', '@', '&', '%', '$'};
char board[BOARD_SIZE][BOARD_SIZE];

char random_tile(){
    return tile_chars[rand()%TILE_TYPES];
}

void init_board(){
    for(int i=0;i<BOARD_SIZE;i++){
        for(int j=0;j<BOARD_SIZE;j++){
            board[i][j] = random_tile();
        }
    }
}

int main(void){
    const int screen_width = 800;
    const int screen_height = 450;

    InitWindow(screen_width, screen_height, "Raylib 2D ASCII MATCH"); //init the window
    SetTargetFPS(60); 
    srand(time(NULL));

    init_board();
    while(!WindowShouldClose()){ //if user closes the window it breaks 
        BeginDrawing();
        ClearBackground(BLACK);


        for(int i=0;i<BOARD_SIZE;i++){  //making rectangles for the grid
            for(int j=0;j<BOARD_SIZE;j++){
                Rectangle rect = {
                    i * TILE_SIZE,
                    j * TILE_SIZE,
                    TILE_SIZE,
                    TILE_SIZE
                };
 
                DrawRectangleLinesEx(rect, 1, DARKGRAY); //setting the size and color of borders

                DrawTextEx(
                    GetFontDefault(),
                    TextFormat("%c", board[i][j]),  //converting single char to string
                    (Vector2){rect.x+12, rect.y+8},  //position of the character
                    20,   //font size
                    1,    //spacing 
                    WHITE
                );
            }
        }


        EndDrawing();
    }

    CloseWindow();
    return 0;
}
