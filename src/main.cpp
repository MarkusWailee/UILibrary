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
    UI::Context context(128 * UI::KB);
    UI::TextStyle style;
    style.FgColor({255, 255, 255, 255}).FontSize(32);
    int count = 0;
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        count++;
        LayoutTest(&context);
        //TextLayoutTest(&context);

        BeginDrawing();
        ClearBackground(Color{0, 0, 0, 255});
        UI::Draw();
        //DrawText(TextFormat("Fps = %d", GetFPS()), 10, 10, 20, WHITE);
        EndDrawing();
    }

    CloseWindow(); // Close window and OpenGL context

    return 0;
}
