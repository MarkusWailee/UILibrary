#include <raylib/raylib.h>
#include "ui/ui.hpp"
#include "UI_Demo.hpp"


int main(void)
{
    float screenWidth = 960;
    float screenHeight = 600;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    SetExitKey(0);
    SetTargetFPS(60);

    UI::Init_impl("assets/fonts/RobotoMonoNerdFont-Regular.ttf");
    UI::Context context(128 * UI::KB, 128 * UI::KB);
    UI::DebugInspector inspector(128 * UI::KB);
    context.SetDebugInspector(&inspector, UI::KEY_F1);
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        //LayoutTest(&context);
        TextLayoutTest(&context);

        // UI::BoxStyle root =
        // {
        //     .width = {GetScreenWidth()},
        //     .height = {GetScreenHeight()},
        //     .color = {255, 0,0,50},
        //     .scissor = true
        // };
        // UI::BoxStyle box0 =
        // {
        //     .width = {50, UI::Unit::PARENT_PERCENT},
        //     .height = {50, UI::Unit::PARENT_PERCENT},
        //     .color = {10, 10,100,255}
        // };
        // UI::Root(&context, root, [&]
        // {
        //     UI::Box(box0)
        //     .Run([&]
        //     {
        //     });
        // });

        BeginDrawing();
        ClearBackground(Color{0, 0, 0, 255});
        UI::Draw();
        //DrawText(TextFormat("Fps = %d", GetFPS()), 10, 10, 20, WHITE);
        EndDrawing();
    }

    CloseWindow(); // Close window and OpenGL context

    return 0;
}
