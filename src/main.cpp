#include <raylib/raylib.h>
#include "ui/ui.hpp"
#include <math/vec.h>
#include "UI_Demo.hpp"
#include "ui/ui_widgets.hpp"


#include <vector>
int main(void)
{
    float screenWidth = 960;
    float screenHeight = 600;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetExitKey(0);

    UI::Init_impl("assets/fonts/Roboto-Regular.ttf");
    UI::Context context(128 * UI::KB);

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {

        SpotifyExample(&context);

        BeginDrawing();
        ClearBackground(Color{0, 0, 0, 255});


        UI::Draw();
        //DrawText(TextFormat("Fps = %f", avg_fps), 10, 10, 20, WHITE);
        EndDrawing();
    }

    CloseWindow();        // Close window and OpenGL context

    return 0;
}