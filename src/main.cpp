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
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        //LayoutTest(&context);
        //TextLayoutTest(&context);
        UI::BoxStyle root =
        {
            .flow =
            {
                .vertical_alignment = UI::Flow::CENTERED,
                .horizontal_alignment = UI::Flow::CENTERED,
            },
            .width = {GetScreenWidth()},
            .height = {GetScreenHeight()},
        };
        UI::BoxStyle box1 =
        {
            .width = {100},
            .height = {100},
            .background_color = {255, 255, 255, 255}
        };
        UI::BoxStyle box2 =
        {
            //.x = -10,
            //.y = -10,
            .width = {200},
            .height = {200},
            .background_color = {0, 255, 0, 80},
            .scissor = true
        };

        UI::Root(&context, root, [&]
        {
            UI::Box(box2)
            .Run([&]
            {
                UI::Box(box1)
                .Id("SomeBox")
                .OnHover([&]
                {
                    UI::Style().background_color = {255, 0, 0, 255};
                })
                .Run();
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
