#include"raylib.h"
#include <assert.h>
#include<stdlib.h>
#include<stdbool.h>
#include<time.h>
#include<math.h>
#include<stdio.h>

#define BOARD_SIZE 8
#define TILE_SIZE 42
#define TILE_TYPES 5
#define SCORE_FONT_SIZE 32
#define MAX_SCORE_POPUPS 32

const char tile_chars[TILE_TYPES] = {'#', '@', '&', '%', '$'};
char board[BOARD_SIZE][BOARD_SIZE];
int matches[BOARD_SIZE][BOARD_SIZE] = {0};
float fall_offset[BOARD_SIZE][BOARD_SIZE] = {0};
float fall_vel[BOARD_SIZE][BOARD_SIZE] = {0};
int score = 0;
float INITIAL_VELOCITY = 150.0f;
const float GRAVITY = 800.0f;
float match_delay_timer = 0.0f;
const float MATCH_DELAY_DURATION = 0.3f; 
int total_matches_count = 0;

float score_scale = 1.0f;
float score_scale_velocity = 0.0f;
bool score_animating = false;

float timer = 10.0f;
const float timer_3tiles = 2.0f;
const float timer_4tiles = 3.0f;
const float timer_many_tiles = 6.0f;
bool time_is_up = false;

Vector2 grid_origin;
Texture2D background;
Font score_font;
Vector2 selected_tile = {-1, -1};
Music background_music;
Sound match_sound;
Sound kaboom_sound;
Sound rampage;

typedef enum{
    STATE_IDLE,
    STATE_ANIMATING,
    STATE_MATCH_DELAY,
    STATE_GAME_OVER
}TileState;

TileState tile_state;

typedef struct{
    Vector2 position;
    int amount;
    float lifetime;
    float alpha;
    bool active;
}ScorePopup;

ScorePopup score_popups[MAX_SCORE_POPUPS] = {0};

char random_tile(){
    return tile_chars[rand()%TILE_TYPES];
}

bool find_matches();
void resolve_matches();
bool tile_if_any_match();
void detect_matches();
void clear_matches();
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

   if(tile_if_any_match()){
        clear_matches();
        detect_matches();
        tile_state = STATE_MATCH_DELAY;
        match_delay_timer = MATCH_DELAY_DURATION;
   }
   else{
        tile_state = STATE_IDLE;
   }
}

void clear_matches(){
    //filling matches board with dummy false values
    for(int i=0;i<BOARD_SIZE;i++){
        for(int j = 0;j<BOARD_SIZE;j++){
            matches[i][j] = 0;
        }
    }
}

void add_score_popup(int x, int y, int amount, Vector2 grid_origin);
bool find_matches(){
    bool found = false;
    clear_matches();

    //--checking matches in rows
    for(int y=0;y<BOARD_SIZE;y++){
        int runLen = 1;
        int runStart = 0;
        for(int x=1;x<=BOARD_SIZE;x++){
            bool endOfRun = (x==BOARD_SIZE) || (board[y][x] != board[y][x-1]); //check if it is not the end of the board and 
                                                                               // if the previous symbol is the same as the current ome
            if(!endOfRun){
                runLen++;
            }
            else{
                if(runLen>=3){
                    for(int i=runStart;i<runStart+runLen;i++){ //filling matches array with true values for 
                                                               //the gap between runStart and till runLen+runStart
                        matches[y][i] = runLen-2;
                    }
                    int total = 10*(runLen - 2); // calculating total score for the sequence of tiles
                    score+=total;
                    found = true;
                    total_matches_count+=1;
                    PlaySound(match_sound);

                    if(runLen>=4){
                        score_animating = true;
                        if(runLen>=5){
                            score_scale = 2.0;
                        }
                        score_scale = 1.5f;
                        score_scale_velocity = -2.5f;
                        PlaySound(kaboom_sound);
                    }

                    //updating timer
                    if(runLen>=5){
                        timer+=timer_many_tiles;
                    }
                    else if(runLen == 4){
                        timer+=timer_4tiles;
                    }
                    else{
                        timer+=timer_3tiles;
                    }
            
                    add_score_popup(runStart+runLen/2, y, total, grid_origin);
                }
                runStart = x; //setting new values for the start of new sequence
                runLen = 1;
            }
        }
    }

    // --checking matches in columns
    for(int x=0;x<BOARD_SIZE;x++){
        int runLen = 1;
        int runStart = 0;
        for(int y=1;y<=BOARD_SIZE;y++){
            bool endOfRun = (y == BOARD_SIZE || (board[y][x] != board[y-1][x]));
            if(!endOfRun){
                runLen++;
            }
            else{
                if(runLen>=3){
                    for(int i=runStart;i<runStart+runLen;i++){
                        matches[i][x] = runLen-2;
                    }
                    int total = 10*(runLen - 2);
                    score+=total;
                    found = true;
                    total_matches_count+=1;
                    PlaySound(match_sound);

                    if(runLen>=4){
                        score_animating = true;
                        if(runLen>=5){
                            score_scale = 2.0;
                        }
                        score_scale = 1.5f;
                        score_scale_velocity = -2.5f;
                        PlaySound(kaboom_sound);
                    }

                    //updating timer
                    if(runLen == 3){
                        timer+=timer_3tiles;
                    }
                    else if(runLen == 4){
                        timer+=timer_4tiles;
                    }
                    else{
                        timer+=timer_many_tiles;
                    }

                    add_score_popup(x, runStart+runLen/2, total, grid_origin);
                }
                runLen = 1;
                runStart = y;
            }
        }
    }

    // //--checking matches in rows  !! THIS IS THE OL VERSION OF THE FUNCTION !!
    // for(int y=0;y<BOARD_SIZE;y++){
    //     for(int x=0;x<BOARD_SIZE - 2;x++){
    //         if(board[y][x] == board[y][x+1] && board[y][x] == board[y][x+2]){
    //             matches[y][x] = matches[y][x+1] = matches[y][x+2] = true;
    //             //if(flag_consecutive){
    //                 score+=10;
    //                 //flag_consecutive = true;
    //             //}
    //             found = true;
    //             PlaySound(match_sound);

    //             add_score_popup(x, y, 10, grid_origin);
    //         } 
    //     }
    // }

    // // --checking matches in columns
    // for(int x=0;x<BOARD_SIZE;x++){
    //     //bool flag_consecutive = false;
    //     for(int y=0;y<BOARD_SIZE - 2;y++){
    //         if(board[y][x] == board[y+1][x] && board[y][x] == board[y+2][x]){
    //             matches[y][x] = matches[y+1][x] = matches[y+2][x] = true;
    //             //if(!flag_consecutive){
    //                 score+=10;
    //                 //flag_consecutive = true;
    //             //}
    //             found = true;
    //             PlaySound(match_sound);
    //             add_score_popup(x, y, 10, grid_origin);
    //         }
    //         // else{
    //         //     flag_consecutive = false;
    //         // }
    //     }
    // }

    return found;
}

bool tile_has_match_at(int x, int y);
bool tile_if_any_match(){  //just checking if at least one match exists
    for(int y=0;y<BOARD_SIZE;y++){
        for(int x=0;x<BOARD_SIZE;x++){
            if(tile_has_match_at(x,y)){
                return true;
            }
        }
    }
    return false;
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
                    fall_vel[write_y][x] = INITIAL_VELOCITY;
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
    clear_matches();

    // for(int y=0;y<BOARD_SIZE;y++){
    //     for(int x=0;x<BOARD_SIZE-2;x++){
    //         if(board[y][x] == board[y][x+1] && board[y][x] == board[y][x+2]){
    //             matches[y][x] = matches[y][x+1] = matches[y][x+2] = true;
    //         }
    //     }
    // }

    // for(int x=0;x<BOARD_SIZE;x++){
    //     for(int y=0;y<BOARD_SIZE - 2;y++){
    //         if(board[y][x] == board[y+1][x] && board[y][x] == board[y+2][x]){
    //             matches[y][x] = matches[y+1][x] = matches[y+2][x] = true;
    //         }
    //     }
    // }

     //--checking matches in rows
     for(int y=0;y<BOARD_SIZE;y++){
        int runLen = 1;
        int runStart = 0;
        for(int x=1;x<=BOARD_SIZE;x++){
            bool endOfRun = (x==BOARD_SIZE) || (board[y][x] != board[y][x-1]); //check if it is not the end of the board and 
                                                                               // if the previous symbol is the same as the current ome
            if(!endOfRun){
                runLen++;
            }
            else{
                if(runLen>=3){
                    for(int i=runStart;i<runStart+runLen;i++){ //filling matches array with true values for 
                                                               //the gap between runStart and till runLen+runStart
                        matches[y][i] = runLen-2;
                    }
                }
                runStart = x; //setting new values for the start of new sequence
                runLen = 1;
            }
        }
    }

    // --checking matches in columns
    for(int x=0;x<BOARD_SIZE;x++){
        int runLen = 1;
        int runStart = 0;
        for(int y=1;y<=BOARD_SIZE;y++){
            bool endOfRun = (y == BOARD_SIZE || (board[y][x] != board[y-1][x]));
            if(!endOfRun){
                runLen++;
            }
            else{
                if(runLen>=3){
                    for(int i=runStart;i<runStart+runLen;i++){
                        matches[i][x] = runLen-2;
                    }
                }
                runLen = 1;
                runStart = y;
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

void add_score_popup(int x, int y, int amount, Vector2 grid_origin){  //updating the score array with a new value
//the logic behind it is that we have an array that can contain up to 32 values
//so at the same time the maximum number of popups is 32 and in this function we are updating one value of popup
    for(int i=0;i<MAX_SCORE_POPUPS;i++){
        if(!score_popups[i].active){
            score_popups[i].position = (Vector2){
                grid_origin.x + x * TILE_SIZE + TILE_SIZE/2,
                grid_origin.y + y * TILE_SIZE + TILE_SIZE/2
            };
            score_popups[i].amount = amount;
            score_popups[i].alpha = 1.0f;
            score_popups[i].lifetime = 1.0f;
            score_popups[i].active = true;
            break;
        }
    }
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
    kaboom_sound = LoadSound("assets/Kaboom_sound_effect.mp3");
    rampage = LoadSound("assets/Rampage_sound.mp3");

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
                            detect_matches();
                            tile_state = STATE_MATCH_DELAY;
                            match_delay_timer = MATCH_DELAY_DURATION;
                            
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

            //going into delay
            if(!still_animating){
                clear_matches();
                detect_matches();
                tile_state = STATE_MATCH_DELAY;
                match_delay_timer = MATCH_DELAY_DURATION;
            }
        }

        //decreasing delay, so hadling its duration
        if(tile_state == STATE_MATCH_DELAY){
            match_delay_timer -= GetFrameTime();
            if(match_delay_timer<=0.0f){
                if(find_matches()){
                    resolve_matches();
                    if(total_matches_count >= 5){
                        PlaySound(rampage);
                    }
                }
                else{
                    tile_state = STATE_IDLE;
                    total_matches_count = 0;
                }
            }
        }

        //updating score popups
        for(int i=0;i<MAX_SCORE_POPUPS;i++){
            if(score_popups[i].active){
                score_popups[i].lifetime -= GetFrameTime();
                score_popups[i].position.y -= 30*GetFrameTime();
                score_popups[i].alpha = score_popups[i].lifetime;

                if(score_popups[i].lifetime <= 0.0f){
                    score_popups[i].active = false; 
                }
            }
        }

        //score animation 
        if(score_animating){
            score_scale += GetFrameTime()*score_scale_velocity;
            if(score_scale<=1.0f){
                score_scale = 1.0f;
                score_animating = false;
            }
        }

        if(!time_is_up){
            timer-=GetFrameTime();
            if(timer<=0){
                time_is_up = true;
                timer = 0;
                tile_state = STATE_GAME_OVER;
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
                    Color dependent_color = WHITE;
                    if(tile_state == STATE_MATCH_DELAY){
                        if(matches[j][i] == 1){
                            dependent_color = GREEN;
                        }
                        else if(matches[j][i] == 2){
                            dependent_color = BLUE;
                        }
                        else if (matches[j][i]>=3) {
                            dependent_color = RED;
                        }
                    }
                    DrawTextEx( //drawing text in the cells
                        GetFontDefault(),
                        TextFormat("%c", board[j][i]),  //converting single char to string
                        (Vector2){rect.x+13, rect.y+10 - fall_offset[j][i]},  //position of the character
                        20,   //font size
                        1,    //spacing 
                        dependent_color
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

        DrawTextEx(score_font, TextFormat("SCORE: %d", score), (Vector2){20, 20}, SCORE_FONT_SIZE*score_scale, 1.0f, YELLOW);

        //drawing popups for scores
        for(int i=0;i<MAX_SCORE_POPUPS;i++){
            if(score_popups[i].active){
                Color clr = Fade(YELLOW, score_popups[i].alpha);
                DrawText(TextFormat("+%d", score_popups[i].amount), 
                        score_popups[i].position.x, 
                        score_popups[i].position.y, 
                        20, 
                        clr);
            }
            else{
                break;
            }
        }

        int minutes = (int)(timer/60);
        int seconds = (int)timer%60;
        int milliseconds = (int)((timer - floorf(timer))*10.0f);
        DrawText(TextFormat("%02d:%02d:%01d", minutes, seconds, milliseconds), screen_width - 100, 20, 24, YELLOW); 

        if(tile_state == STATE_GAME_OVER){
            DrawText(TextFormat("GAME OVER"), screen_width/2 - 100, screen_height/2 - 100, 48, RED);
            DrawText(TextFormat("Final score: %d", score), screen_width/2 - 120, screen_height/2, 32, WHITE);
        }

        EndDrawing();
    }

    StopMusicStream(background_music);
    UnloadTexture(background);
    UnloadFont(score_font);
    UnloadMusicStream(background_music);
    UnloadSound(match_sound);
    UnloadSound(kaboom_sound);
    UnloadSound(rampage);

    CloseWindow();
    return 0;
}

