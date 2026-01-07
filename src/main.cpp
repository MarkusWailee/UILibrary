#include <functional>
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
    //SetTargetFPS(60);

    UI::Init_impl("assets/fonts/Roboto-Regular.ttf");
    UI::Context context(128 * UI::KB, 128 * UI::KB);
    UI::DebugInspector inspector(8 * UI::MB);
    context.SetDebugInspector(&inspector, UI::KEY_F1);
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // LayoutTest(&context);
        UI::Root(&context, {.color = {255, 255, 255, 255}}, [&]
        {
            UI::Box({.color = {255, 0, 0, 255}}).Run();
        });

        BeginDrawing();
        ClearBackground(Color{0, 0, 0, 255});
        UI::Draw();
        DrawText(TextFormat("Fps = %d", GetFPS()), 10, 10, 20, WHITE);
        EndDrawing();
    }

    CloseWindow(); // Close window and OpenGL context

    return 0;
}
