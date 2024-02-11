#include "Tetris.h"
#include <time.h>
#include <math.h>
#include "Data.h"


int main(int argc, char** argv, char** environ)
{
    const int windowWidth = 350; 
    const int windowHeight = 600; 

    const int startOffsetX = (windowWidth / 2) - ((STAGE_WIDTH * TILE_SIZE) / 2);
    const int startOffsetY = (windowHeight / 2) - ((STAGE_HEIGHT * TILE_SIZE) / 2);

    const int tetrominoStartX = STAGE_WIDTH / 2;
    const int tetrominoStartY = 0;

    int currentTetrominoX = tetrominoStartX;
    int currentTetrominoY = tetrominoStartY;

    time_t unixTime;

    time(&unixTime);

    SetRandomSeed(unixTime);

    int currentTetrominoType = GetRandomValue(0, 6);
    int currentRotation = 0;

    float initialMoveDownTimer = 1.0f;
    float moveTetrominoDownTimer = initialMoveDownTimer;
    float maxMoveDownTimer = 0.1f;
    float speedIncreaseInterval = 30.0f;
    float elapsedTimeSinceSpeedIncrease = 0.0f;

    int currentColor = GetRandomValue(1, 7);

    InitWindow(windowWidth, windowHeight, "TETRIS");

    InitAudioDevice();
    Music music = LoadMusicStream("Assets/coffin_dance.wav");
    PlayMusicStream(music);

    SetTargetFPS(60);

    while(!WindowShouldClose())
    {
        initialMoveDownTimer -= GetFrameTime();
        elapsedTimeSinceSpeedIncrease += GetFrameTime();

        if (elapsedTimeSinceSpeedIncrease >= speedIncreaseInterval)
        {
            moveTetrominoDownTimer = fmax(moveTetrominoDownTimer * 0.9f, maxMoveDownTimer);
            elapsedTimeSinceSpeedIncrease = 0.0f;
        }

        UpdateMusicStream(music);

        if (IsKeyPressed(KEY_SPACE))
        {
            const int lastRotation = currentRotation;

            currentRotation++;

            if (currentRotation > 3)
            {
                currentRotation = 0;
            }

            if (CheckCollision(currentTetrominoX,currentTetrominoY,tetrominoTypes[currentTetrominoType][currentRotation]))
            {
                currentRotation = lastRotation;
            }
        }

        if (IsKeyPressed(KEY_RIGHT))
        {
            // No need to check overflow, wall is your protector
            if (!CheckCollision(currentTetrominoX+1,currentTetrominoY,tetrominoTypes[currentTetrominoType][currentRotation]))
            {
                currentTetrominoX++;
            }
        }
        if (IsKeyPressed(KEY_LEFT))
        {
            // No need to check overflow, wall is your protector
            if (!CheckCollision(currentTetrominoX-1,currentTetrominoY,tetrominoTypes[currentTetrominoType][currentRotation]))
            {
                currentTetrominoX--;
            }
        }


        if (initialMoveDownTimer <= 0 || IsKeyPressed(KEY_DOWN))
        {
            if (!CheckCollision(currentTetrominoX, currentTetrominoY + 1, tetrominoTypes[currentTetrominoType][currentRotation]))
            {
            currentTetrominoY++;
            initialMoveDownTimer = moveTetrominoDownTimer;
            }
            else
            {
                for(int y = 0; y < TETROMINO_SIZE; y++)
                {
                    for(int x = 0; x < TETROMINO_SIZE; x++)
                    {
                        const int offset = y * TETROMINO_SIZE + x;

                        const int *tetromino = tetrominoTypes[currentTetrominoType][currentRotation];

                        if(tetromino[offset] == 1)
                        {
                            const int offset_stage = (y + currentTetrominoY) * STAGE_WIDTH + (x + currentTetrominoX);

                            stage[offset_stage] = currentColor+1;

                        }
                    }
                }

                score += BASICPOINT;

                // Ensure the timer doesn't go below the maximum value
                if (moveTetrominoDownTimer < maxMoveDownTimer)
                {
                    moveTetrominoDownTimer = maxMoveDownTimer;
                }

                DeleteLines();

                currentTetrominoX = tetrominoStartX;
                currentTetrominoY = tetrominoStartY;

                currentTetrominoType = GetRandomValue(0, 6);
                currentRotation = 0;
                currentColor = GetRandomValue(1, 7);
            }
        }

        BeginDrawing();
        ClearBackground(GRAY);

        DrawText(TextFormat("SCORE: %d", score), 10, 10, 20, BLACK);

        for(int y = 0; y < STAGE_HEIGHT; y++)
        {
            for(int x = 0; x < STAGE_WIDTH; x++)
            {
                const int offset = y * STAGE_WIDTH + x;
                const int color = stage[offset];

                if(stage[offset] != 0)
                {
                    DrawRectangle(x * TILE_SIZE + startOffsetX, y * TILE_SIZE + startOffsetY, TILE_SIZE, TILE_SIZE, colorTypes[color-1]);
                }

                DrawRectangleLines(x * TILE_SIZE + startOffsetX, y * TILE_SIZE + startOffsetY, TILE_SIZE, TILE_SIZE, BLACK);
            }
        }
        
        drawTetromino(colorTypes[currentColor],startOffsetX, startOffsetY, currentTetrominoX, currentTetrominoY, tetrominoTypes[currentTetrominoType][currentRotation]);

        EndDrawing();
    }

    UnloadMusicStream(music);
    CloseAudioDevice(); 

    return 0;
}