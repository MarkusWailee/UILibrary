#include <raylib/raylib.h>
#include "ui/ui.hpp"
#include <math/vec.h>
#include "UI_Demo.hpp"
#include "ui_inspector.hpp"

int main(void)
{
    float screenWidth = 960;
    float screenHeight = 600;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetExitKey(0);
    //SetTargetFPS(60);

    UI::Init_impl();
    UI::Context ui_context(32768 *2);
    UI::SetContext(&ui_context);


    float time = 0;
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        BeginDrawing();
        ClearBackground(Color{0, 0, 0, 255});
        SpotifyExample();

        UI::ToolKit::Inspector(UI::GetContext());
        //DrawText(TextFormat("fps = %d", GetFPS()), 10, 10, 20, WHITE);
        EndDrawing();
    }
    CloseWindow();        // Close window and OpenGL context

    return 0;
}