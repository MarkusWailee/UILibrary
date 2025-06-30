#include <raylib/raylib.h>
#include "ui/ui.hpp"
#include <math/vec.h>
#include "UI_Demo.hpp"
#include "ui_inspector.hpp"

void ExampleDemo()
{
    UI::BeginRoot(0, 0, GetScreenWidth(), GetScreenHeight(), GetMouseX(), GetMouseY());
    UI::Draw();
}
int main(void)
{
    float screenWidth = 960;
    float screenHeight = 600;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetExitKey(0);
    //SetTargetFPS(256);

    UI::Init_impl();
    UI::Context ui_context(128 * UI::KB);
    UI::DebugInspector ui_inspector(UI::MB);

    UI::SetContext(&ui_context);


    float time = 0;
    bool flip = false;
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        BeginDrawing();
        ClearBackground(Color{0, 0, 0, 255});

        //UI::SetDebugInput(IsMouseButtonPressed(0), IsMouseButtonReleased(0), GetMouseWheelMove(),IsKeyPressed(KEY_F));
        ui_context.SetInspector(IsMouseButtonPressed(0), IsMouseButtonReleased(0), GetMouseWheelMove(), IsKeyPressed(KEY_F), &ui_inspector);
        SpotifyExample();

        DrawText(TextFormat("fps = %d", GetFPS()), 10, 10, 20, WHITE);
        EndDrawing();
    }
    CloseWindow();        // Close window and OpenGL context

    return 0;
}