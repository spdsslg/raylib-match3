#include"raylib.h"
#include <assert.h>
#include<stdlib.h>
#include<stdbool.h>
#include<time.h>
#include<math.h>

#define BOARD_SIZE 8
#define TILE_SIZE 42
#define TILE_TYPES 5

const char tile_chars[TILE_TYPES] = {'#', '@', '&', '%', '$'};
char board[BOARD_SIZE][BOARD_SIZE];
int score = 200;

Vector2 grid_origin;
Texture2D background;

char random_tile(){
    return tile_chars[rand()%TILE_TYPES];
}

void init_board(){
    for(int i=0;i<BOARD_SIZE;i++){
        for(int j=0;j<BOARD_SIZE;j++){
            board[i][j] = random_tile();
        }
    }

   int grid_width = BOARD_SIZE * TILE_SIZE;
   int grid_height = BOARD_SIZE * TILE_SIZE;

   grid_origin = (Vector2){
    (GetScreenWidth() - grid_width)/2,
    (GetScreenHeight() - grid_height)/2
   };
}

int main(void){
    const int screen_width = 736;
    const int screen_height = 460;

    InitWindow(screen_width, screen_height, "Raylib 2D ASCII MATCH"); //init the window
    SetTargetFPS(60); 
    srand(time(NULL));

    background = LoadTexture("assets/Canti.jpeg");

    init_board();
    while(!WindowShouldClose()){ //if user closes the window it breaks 
        BeginDrawing();
        ClearBackground(BLACK);


        DrawTexturePro(   //resizing the image to the size of the screen
            background,
            (Rectangle){ 0, 0, background.width, background.height},
            (Rectangle){ 0, 0, GetScreenWidth(), GetScreenHeight()},
            (Vector2){ 0, 0},
            0.0,
            (Color){ 182, 198, 217, 200 }
        );

        for(int i=0;i<BOARD_SIZE;i++){  //making rectangles for the grid
            for(int j=0;j<BOARD_SIZE;j++){
                Rectangle rect = {
                    grid_origin.x + (i * TILE_SIZE),
                    grid_origin.y + (j * TILE_SIZE),
                    TILE_SIZE,
                    TILE_SIZE
                };
 
                DrawRectangleLinesEx(rect, 1, BLACK); //setting the size and color of borders

                DrawTextEx(
                    GetFontDefault(),
                    TextFormat("%c", board[i][j]),  //converting single char to string
                    (Vector2){rect.x+13, rect.y+10},  //position of the character
                    20,   //font size
                    1,    //spacing 
                    WHITE
                );

                DrawText(TextFormat("SCORE: %d", score), 20, 20, 24, YELLOW);
            }
        }

        EndDrawing();
    }

    UnloadTexture(background);

    CloseWindow();
    return 0;
}
