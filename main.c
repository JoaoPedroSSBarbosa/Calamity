#include "raylib.h"
#include "game.h"

int main(void)
{
    GameState game = {0};

    InitWindow(20 * 32, 15 * 32 + 80, "Calamity - Dungeon Crawler");
    SetTargetFPS(60);

    if (!InitGame(&game)) {
        CloseWindow();
        return 1;
    }

    while (!WindowShouldClose()) {
        UpdateGame(&game);

        BeginDrawing();
        ClearBackground(BLACK);
        DrawGame(&game);

        if (game.gameOver) {
            const char *message = game.win ? "VOCE VENCEU! PRESSIONE R PARA REINICIAR" : "GAME OVER! PRESSIONE R PARA REINICIAR";
            DrawRectangle(0, game.height * game.tileSize / 2 - 24, game.width * game.tileSize, 48, Fade(BLACK, 0.5f));
            DrawText(message, 20, game.height * game.tileSize / 2 - 10, 20, RAYWHITE);
            if (IsKeyPressed(KEY_R)) {
                UnloadGame(&game);
                InitGame(&game);
            }
        }

        EndDrawing();
    }

    UnloadGame(&game);
    CloseWindow();
    return 0;
}
