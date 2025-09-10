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
    SetTargetFPS(60);

    UI::Init_impl("assets/fonts/RobotoMonoNerdFont-Regular.ttf");
    UI::Context context(128 * UI::KB, 128 * UI::KB);
    UI::DebugInspector inspector(128 * UI::KB);
    context.SetDebugInspector(&inspector, UI::KEY_F1);
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        //LayoutTest(&context);
        //TextLayoutTest(&context);

        UI::BoxStyle root =
        {
            .flow = {.vertical_alignment = UI::Flow::CENTERED, .horizontal_alignment = UI::Flow::CENTERED },
            .width = {GetScreenWidth()},
            .height = {GetScreenHeight()},
            .color = {40, 40, 40,255},
            .scissor = true
        };
        UI::BoxStyle box0 =
        {
            .width = {50},
            .height = {50},
            .color = {255, 255,255,255},
        };
        UI::BoxStyle box1 = box0;
        box1.detach = UI::Detach::RIGHT;

        UI::Root(&context, root, [&]
        {
            UI::Box(box0)
            .Run([&]
            {
                UI::Box(box1)
                .PreRun([&]
                {
                    UI::Style().color = {200, 200, 200, 255};
                })
                .Run([&]
                {
                    UI::Box(box1)
                    .PreRun([&]
                    {
                        UI::Style().color = {150, 150, 150, 255};
                    })
                    .Run([&]
                    {
                    });
                });
            });
        });

        BeginDrawing();
        ClearBackground(Color{0, 0, 0, 255});
        UI::Draw();
        //DrawText(TextFormat("Fps = %d", GetFPS()), 10, 10, 20, WHITE);
        EndDrawing();
    }

    CloseWindow(); // Close window and OpenGL context

    return 0;
}
