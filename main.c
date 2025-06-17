#include"raylib.h"
#include <assert.h>
#include<stdlib.h>
#include<stdbool.h>
#include<time.h>
#include<math.h>

#define BOARD_SIZE 8
#define TILE_SIZE 42
#define TILE_TYPES 5
#define SCORE_FONT_SIZE 32

const char tile_chars[TILE_TYPES] = {'#', '@', '&', '%', '$'};
char board[BOARD_SIZE][BOARD_SIZE];
char matches[BOARD_SIZE][BOARD_SIZE] = {0};
int score = 200;

Vector2 grid_origin;
Texture2D background;
Font score_font;
Vector2 selected_tile = {-1, -1};

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

bool find_matches(){
    bool found = false;
    //filling matches board with dummy false values
    for(int i=0;i<BOARD_SIZE;i++){
        for(int j = 0;j<BOARD_SIZE;j++){
            matches[i][j] = false;
        }
    }

    //checking matches in rows
    for(int y=0;y<BOARD_SIZE;y++){
        for(int x=0;x<BOARD_SIZE - 2;x++){
            if(board[y][x] == board[y][x+1] && board[y][x] == board[y][x+2]){
                matches[y][x] = matches[y][x+1] = matches[y][x+2] = true;
            } 
            score+=10;
            found = true;
        }
    }

    //checking matches in columns
    for(int x=0;x<BOARD_SIZE;x++){
        for(int y=0;y<BOARD_SIZE - 2;y++){
            if(board[y][x] == board[y+1][x] && board[y][x] == board[y+2][x]){
                matches[y][x] = matches[y+1][x] = matches[y+2][x] = true;
            }
            score+=10;
            found = true;
        }
    }

    return found;
}

int main(void){
    const int screen_width = 736;
    const int screen_height = 460;

    InitWindow(screen_width, screen_height, "Raylib 2D ASCII MATCH"); //init the window
    SetTargetFPS(60); 
    srand(time(NULL));

    background = LoadTexture("assets/Canti.jpeg"); //load a background
    score_font = LoadFontEx("src/04b03.ttf", SCORE_FONT_SIZE, NULL, 0);  //load a font

    init_board();
    Vector2 mouse = {0, 0 };

    while(!WindowShouldClose()){ //if user closes the window it breaks 
        mouse = GetMousePosition();
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            int x = (mouse.x - grid_origin.x)/TILE_SIZE;
            int y = (mouse.y - grid_origin.y)/TILE_SIZE;
            if((mouse.x - grid_origin.x)>=0 && x<BOARD_SIZE && (mouse.y - grid_origin.y)>=0 && y<BOARD_SIZE){  //consider mouse clicks only if they are inside the grid
                selected_tile.x = x;
                selected_tile.y = y;
            }
        }

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

        find_matches();

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
                    matches[j][i] ? GREEN : WHITE
                );

                //DrawText(TextFormat("SCORE: %d", score), 20, 20, 24, YELLOW);
            }
        }
        if(selected_tile.x >= 0){
            DrawRectangleLinesEx((Rectangle){
                grid_origin.x + TILE_SIZE*selected_tile.x,
                grid_origin.y + TILE_SIZE*selected_tile.y,
                TILE_SIZE,
                TILE_SIZE
            }, 
            2, 
            YELLOW);
        }

        DrawTextEx(score_font, TextFormat("SCORE: %d", score), (Vector2){20, 20}, SCORE_FONT_SIZE, 1.0f, YELLOW);

        EndDrawing();
    }

    UnloadTexture(background);
    UnloadFont(score_font);

    CloseWindow();
    return 0;
}

