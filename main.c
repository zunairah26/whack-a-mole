#include "raylib.h"   // game window, drawing, textures, audio
#include <stdlib.h>   // for da random function
#include <time.h>     // time() for random generetion 

// thus is structure it bundles all data for 1 mole into single variable 
typedef struct Mole {
    Vector2 pos;    // screen position x y cordinates
    int active;     // 1 = mole  visible 0 = hidden
    float timer;    // counts DOWN to 0  then mole hide
} Mole;

int main() {

    // window and audio it runs atleast once
    InitWindow(800, 600, "Whack-a-Mole - PF Project (C Version)");
    InitAudioDevice();                  // must call before any audio functions so they work good
    SetTargetFPS(60);                   // lock the game to 60 frames per second  so charcater no moves at random speeds
    SetExitKey(KEY_NULL);               // ESC no longer closes the window
    srand(time(NULL));                  // seed RNG with current time so rand() differs every run and the current time to seed random numbers 
   
    // load aasets
    Music bgMusic = LoadMusicStream("song.mp3");   // musoc  file refernce
    PlayMusicStream(bgMusic);

    Texture2D moleTex = LoadTexture("mole.png");   //  the mole
    Texture2D holeTex = LoadTexture("hole.png");   //  the hole 

    // game variables 
    int score     = 0, highScore = 0, gameStarted = 0,  gameOver    = 0;  

    // dificult settings
    int difficulty = 1;
    float moleDuration = 1.0f;  // how many seconds a mole stays visible

    // creating six moles and setting their fixed grid position
    Mole moles[6];

    moles[0].pos = (Vector2){150, 200};   
    moles[1].pos = (Vector2){350, 200};  
    moles[2].pos = (Vector2){550, 200};   
    moles[3].pos = (Vector2){150, 350};   
    moles[4].pos = (Vector2){350, 350};   
    moles[5].pos = (Vector2){550, 350};   

    // start all moles as hidden with no timer run asw
    for (int i = 0; i < 6; i++) {
        moles[i].active = 0;
        moles[i].timer  = 0;
    }

    // when it hits spawnrate pop a mole 
    float spawnTimer = 0;
    float spawnRate  = 0.8f;   // seconds between mole spawns as i used GetFrameTime() so it will be same for every devoce 

    // achievement flags once set 1 they stay 1 
    int achievement10 = 0;   // at score >= 10
    int achievement25 = 0;   // at score >= 25
    float gameTime = 30.0f;  //seconds left in a round

        // game loop runs 60 times per second until window is closed
    while (!WindowShouldClose()) {

        UpdateMusicStream(bgMusic);   // must be called every frame to keep music streaming or its stops

        // menu wen player selcting difficulty
        if (!gameStarted) {
            if (IsKeyPressed(KEY_ONE)) {  //easy one
                difficulty   = 1;
                moleDuration = 1.2f;   // moles stay up longer
                spawnRate    = 1.0f;   // new mole every 1 second
                gameStarted  = 1;      // leave menu
            }
            if (IsKeyPressed(KEY_TWO)) {
                difficulty   = 2;
                moleDuration = 0.8f;
                spawnRate    = 0.7f;
                gameStarted  = 1;
            }
            if (IsKeyPressed(KEY_THREE)) {
                difficulty   = 3;
                moleDuration = 0.5f;   // moles go away very quickly on hard
                spawnRate    = 0.5f;   // new mole every 0.5 seconds
                gameStarted  = 1;
            }
        }

        // gameplay
        if (gameStarted && !gameOver) {
                        gameTime -= GetFrameTime();
            if (gameTime <= 0) {
                gameTime = 0;
                gameOver = 1;    // time is up, so show Game Over
            }

            // spawn add time mean when enough time passes pop mole 
            spawnTimer += GetFrameTime();   // GetFrameTime() = seconds since last frame   GetFrameTime() is the tiny amount of time (about 0.016 seconds if you are at 60 FPS)

            if (spawnTimer >= spawnRate) {
                spawnTimer = 0;                // reset stopwatch

                int index = rand() % 6;        // pick a random mole slot (0 to 5) meaning 6 moles
                moles[index].active = 1;       // make visible
                moles[index].timer  = moleDuration;  // tp give mole its countdown
            }

            // hiding mole count each active mole's timer down and hide it when it reaches 0 
            for (int i = 0; i < 6; i++) {
                if (moles[i].active) {
                    moles[i].timer -= GetFrameTime();   

                    if (moles[i].timer <= 0) {
                        moles[i].active = 0;   // timer ran out so mole gowms down
                    }
                }
            }

            //check if the mouse click landed on any mole 
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mouse = GetMousePosition();   // pixel coordsof da click

                for (int i = 0; i < 6; i++) {
                    if (moles[i].active) {

                        // build a bounding rectangle around this mole using its position and texture size
                        Rectangle moleRect = {
                            moles[i].pos.x,
                            moles[i].pos.y,
                            (float)moleTex.width,
                            (float)moleTex.height
                        };

                        // did the click land inside the mole's rectangle?
                        if (CheckCollisionPointRec(mouse, moleRect)) {
                            moles[i].active = 0;   // hide da mole cuz hit
                            score++;

                            // update high score 
                            if (score > highScore)
                                highScore = score;

                            // unlock achievements 
                            if (score >= 10) achievement10 = 1;
                            if (score >= 25) achievement25 = 1;
                        }
                    }
                }
            }

            // wen esc pressed ususlaly the window closed but beacsue of this itll go to the gameover screen 
            if (IsKeyPressed(KEY_ESCAPE)) {
                gameOver = 1;
            }
        }

        // R resets everything its with the game over screen
        if (gameOver && IsKeyPressed(KEY_R)) {
            score         = 0;             gameTime = 30.0f;
            gameOver      = 0;
            gameStarted   = 0;      
            achievement10 = 0;
            achievement25 = 0;

            // clear all mole and scoree so nothing is left over from last game
            for (int i = 0; i < 6; i++) {
                moles[i].active = 0;
                moles[i].timer  = 0;
            }
        }
      
        // everything between begin and end drawing shows on screen appears on screen
        
        

                        BeginDrawing();

        // pick the background color for the current screen
        Color bg;
        if (!gameStarted)    bg = (Color){ 255, 214, 224, 255 };   // pink strat
        else if (gameOver)   bg = (Color){ 60, 40, 50, 255 };      // dark game over
        else                 bg = DARKGREEN;                       // dark green game as yk moles live in grass

        ClearBackground(bg);

        // menu 
        if (!gameStarted) {
            ClearBackground((Color){ 255, 214, 224, 255 });
            DrawText("WHACK-A-MOLE", 230, 100, 45, DARKPURPLE);
            DrawText("Press 1 for EASY",   250, 250, 30, BLACK);
            DrawText("Press 2 for MEDIUM", 250, 300, 30, BLACK);
            DrawText("Press 3 for HARD",   250, 350, 30, BLACK);
        }
        // game over
        else if (gameOver) {
            ClearBackground((Color){ 60, 40, 50, 255 });
            DrawText("GAME OVER!", 260, 200, 50, RED);
            DrawText(TextFormat("Score: %d",      score),     320, 260, 30, WHITE);
            DrawText(TextFormat("High Score: %d", highScore), 285, 300, 30, WHITE);
            DrawText("Press R to Restart", 280, 360, 30, LIGHTGRAY);
        }
        
                // gameplay
        else {
            for (int i = 0; i < 6; i++) {
                DrawTexture(holeTex, moles[i].pos.x, moles[i].pos.y, WHITE);

                if (moles[i].active)
                    DrawTexture(moleTex, moles[i].pos.x, moles[i].pos.y - 30, WHITE);
            }

            DrawText(TextFormat("Score: %d", score), 20, 20, 30, WHITE);
            DrawText(TextFormat("Time: %d", (int)gameTime + 1), 650, 20, 30, YELLOW);

            if (achievement10)
                DrawText("Achievement: Mole Beginner (10)", 20, 70, 20, YELLOW);
            if (achievement25)
                DrawText("Achievement: Mole Master (25)",   20, 95, 20, ORANGE);
        }

        EndDrawing();
    }

    // cleanup and its in reverse ordeer
    UnloadTexture(moleTex);
    UnloadTexture(holeTex);
    UnloadMusicStream(bgMusic);
    CloseAudioDevice();
    CloseWindow();

    return 0;   // return 0  program finished successfully yay
}