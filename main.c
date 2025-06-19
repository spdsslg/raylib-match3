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
float fall_offset[BOARD_SIZE][BOARD_SIZE] = {0};
float fall_vel[BOARD_SIZE][BOARD_SIZE] = {0};
int score = 0;
float fall_speed = 7.0f;
const float GRAVITY = 1000.0f;
float match_delay_timer = 0.0f;
const float MATCH_DELAY_DURATION = 0.3f; 

Vector2 grid_origin;
Texture2D background;
Font score_font;
Vector2 selected_tile = {-1, -1};
Music background_music;
Sound match_sound;

typedef enum{
    STATE_IDLE,
    STATE_ANIMATING,
    STATE_MATCH_DELAY  
}TileState;

TileState tile_state;

char random_tile(){
    return tile_chars[rand()%TILE_TYPES];
}

bool find_matches();
void resolve_matches();
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

   if(find_matches()){
        resolve_matches();
   }
   else{
        tile_state = STATE_IDLE;
   }
}

bool find_matches(){
    bool found = false;
    //filling matches board with dummy false values
    for(int i=0;i<BOARD_SIZE;i++){
        for(int j = 0;j<BOARD_SIZE;j++){
            matches[i][j] = false;
        }
    }

    //--checking matches in rows
    for(int y=0;y<BOARD_SIZE;y++){
        //bool flag_consecutive = false;
        for(int x=0;x<BOARD_SIZE - 2;x++){
            if(board[y][x] == board[y][x+1] && board[y][x] == board[y][x+2]){
                matches[y][x] = matches[y][x+1] = matches[y][x+2] = true;
                //if(!flag_consecutive){
                    score+=10;
                    //flag_consecutive = true;
                //}
                found = true;
                PlaySound(match_sound);
            } 
            // else{
            //     flag_consecutive = false;
            // }
        }
    }

    // --checking matches in columns
    for(int x=0;x<BOARD_SIZE;x++){
        //bool flag_consecutive = false;
        for(int y=0;y<BOARD_SIZE - 2;y++){
            if(board[y][x] == board[y+1][x] && board[y][x] == board[y+2][x]){
                matches[y][x] = matches[y+1][x] = matches[y+2][x] = true;
                //if(!flag_consecutive){
                    score+=10;
                    //flag_consecutive = true;
                //}
                found = true;
                PlaySound(match_sound);
            }
            // else{
            //     flag_consecutive = false;
            // }
        }
    }

    return found;
}

void resolve_matches(){
    //checking columns and substituting the values with the ones that are above
    //+ calculating the number of matches in a column
    for(int x=0;x<BOARD_SIZE;x++){ 
        int write_y = BOARD_SIZE-1;
        for(int y=BOARD_SIZE-1;y>=0;y--){
            if(!matches[y][x]){
                if(y!=write_y){
                    board[write_y][x] = board[y][x];
                    fall_offset[write_y][x] = (write_y - y)*TILE_SIZE;
                    fall_vel[write_y][x] = 0.0f;
                    board[y][x] = ' ';
                }
                write_y--;
            }
        }

        //filling the upper tiles with random values
        while(write_y>=0){
            board[write_y][x] = random_tile();
            fall_offset[write_y][x] = (write_y + 1)*TILE_SIZE;
            fall_vel[write_y][x] = 0.0f;
            write_y--;
        }
    }

    tile_state = STATE_ANIMATING;
}

void detect_matches(){  //checks for matches but do not add scores, used to find matches to highlight 
                        //after resolve_matches_if_any to update matches count
                        //!!THIS FUNCTION IS CURRENTLY NOT USED IN THE GAME LOGIC!!
    for(int y = 0;y<BOARD_SIZE;y++){
        for(int x = 0;x<BOARD_SIZE;x++){
            matches[y][x] = false;
        }
    }

    for(int y=0;y<BOARD_SIZE;y++){
        for(int x=0;x<BOARD_SIZE-2;x++){
            if(board[y][x] == board[y][x+1] && board[y][x] == board[y][x+2]){
                matches[y][x] = matches[y][x+1] = matches[y][x+2] = true;
            }
        }
    }

    for(int x=0;x<BOARD_SIZE;x++){
        for(int y=0;y<BOARD_SIZE - 2;y++){
            if(board[y][x] == board[y+1][x] && board[y][x] == board[y+2][x]){
                matches[y][x] = matches[y+1][x] = matches[y+2][x] = true;
            }
        }
    }
}

bool tile_has_match_at(int x,int y){  //cheks for match3 in the consecutive cells to the one that is provided
                                      //used to prevent swapping tiles that don't form match3
    int cnt = 1;
    char curr_tile_val = board[y][x];
    for(int dx = 1; (x-dx)>=0 && board[y][x-dx]==curr_tile_val;dx++) cnt++;
    for(int dx = 1; (x+dx)<BOARD_SIZE && board[y][x+dx]==curr_tile_val;dx++) cnt++;
    if (cnt>=3) return true;
    cnt=1;
    for(int dy = 1; (y-dy)>=0 && board[y-dy][x] == curr_tile_val;dy++) cnt++;
    for(int dy = 1; (y+dy)<BOARD_SIZE && board[y+dy][x] == curr_tile_val;dy++) cnt++;
    return (cnt>=3);
}

void swap_tiles(int x1, int y1, int x2, int y2){  //function for swapping tiles
    char temp = board[y1][x1];
    board[y1][x1] = board[y2][x2];
    board[y2][x2] = temp;
}

bool are_tiles_adjacent(Vector2 a, Vector2 b){  //check if the tiles are adjacent
    return (abs((int)a.x - (int)b.x) + abs((int)a.y - (int)b.y)) == 1;
}

int main(void){
    const int screen_width = 736;
    const int screen_height = 460;

    InitWindow(screen_width, screen_height, "Raylib 2D ASCII MATCH"); //init the window
    SetTargetFPS(60); 
    srand(time(NULL));
    InitAudioDevice();

    background = LoadTexture("assets/Canti.jpeg"); //load a background
    score_font = LoadFontEx("src/04b03.ttf", SCORE_FONT_SIZE, NULL, 0);  //load a font
    background_music = LoadMusicStream("assets/bgm_old.mp3");
    match_sound = LoadSound("assets/match_old.mp3");

   SetMasterVolume(0.3f);
    PlayMusicStream(background_music);

    init_board();
    Vector2 mouse = {0, 0 };

    while(!WindowShouldClose()){ //if user closes the window it breaks 
        UpdateMusicStream(background_music);

        mouse = GetMousePosition();
        if(tile_state == STATE_IDLE && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            int x = (mouse.x - grid_origin.x)/TILE_SIZE;
            int y = (mouse.y - grid_origin.y)/TILE_SIZE;
            if((mouse.x - grid_origin.x)>=0 && x<BOARD_SIZE && (mouse.y - grid_origin.y)>=0 && y<BOARD_SIZE){  //consider mouse clicks only if they are inside the grid
                Vector2 current_tile = (Vector2){x,y}; 
                if(selected_tile.x<0 && selected_tile.y<0){ //if the tile that we clicked on is the first chosen
                                                            //we just select it
                    selected_tile = current_tile;
                }
                else{                                       //otherwise, we try to swap tiles, as we know that some tile was already chosen
                    if(are_tiles_adjacent(selected_tile, current_tile)){
                        swap_tiles(selected_tile.x, selected_tile.y, current_tile.x, current_tile.y);
                        if(tile_has_match_at(selected_tile.x, selected_tile.y) || tile_has_match_at(current_tile.x, current_tile.y)){
                            if(find_matches()){
                                resolve_matches();
                            }
                        }
                        else{  //if no matches occured, swap back
                            swap_tiles(selected_tile.x, selected_tile.y, current_tile.x, current_tile.y);
                        }
                    }
                    selected_tile = (Vector2){-1, -1};
                }
            }
        }

        float dt = GetFrameTime(); // getting the time from the previous frame update
                                   // to properly render the acceleration

        //changing the offset for each tile, that has it
        if(tile_state == STATE_ANIMATING){
            bool still_animating = false;
            for(int y=0;y<BOARD_SIZE;y++){
                for(int x=0;x<BOARD_SIZE;x++){
                    if(fall_offset[y][x]>0){
                        fall_vel[y][x] += GRAVITY*dt; //current velocity of the tile
                        fall_offset[y][x] -= fall_vel[y][x]*dt; //current offset from the final position
                        if(fall_offset[y][x]<0){  //if it is <0 ,tile is already at the final position => offset=0
                            fall_offset[y][x] = 0;
                        }
                        else{
                            still_animating = true;
                        }
                    }
                }
            }

            if(!still_animating){
                tile_state = STATE_MATCH_DELAY;
                match_delay_timer = MATCH_DELAY_DURATION;
            }
        }

        if(tile_state == STATE_MATCH_DELAY){
            match_delay_timer -= GetFrameTime();
            if(match_delay_timer<=0.0f){
                if(find_matches()){
                    resolve_matches();
                }
                else{
                    tile_state = STATE_IDLE;
                }
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

        DrawRectangle(grid_origin.x, grid_origin.y, TILE_SIZE*BOARD_SIZE, TILE_SIZE*BOARD_SIZE, Fade(DARKGRAY, 0.6));

        for(int i=0;i<BOARD_SIZE;i++){  //making rectangles for the grid
            for(int j=0;j<BOARD_SIZE;j++){
                Rectangle rect = {
                    grid_origin.x + (i * TILE_SIZE),
                    grid_origin.y + (j * TILE_SIZE),
                    TILE_SIZE,
                    TILE_SIZE
                };
 
                DrawRectangleLinesEx(rect, 1, DARKGRAY); //setting the size and color of borders

                if(board[j][i]!=' '){
                    DrawTextEx( //drawing text in the cells
                        GetFontDefault(),
                        TextFormat("%c", board[j][i]),  //converting single char to string
                        (Vector2){rect.x+13, rect.y+10 - fall_offset[j][i]},  //position of the character
                        20,   //font size
                        1,    //spacing 
                        matches[j][i] ? GREEN : WHITE
                    );
                }

                //DrawText(TextFormat("SCORE: %d", score), 20, 20, 24, YELLOW);
            }
        }

        //highlighting selected tile with a yellow border
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

    StopMusicStream(background_music);
    UnloadTexture(background);
    UnloadFont(score_font);
    UnloadMusicStream(background_music);
    UnloadSound(match_sound);

    CloseWindow();
    return 0;
}

