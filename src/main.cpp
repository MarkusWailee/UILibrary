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
    SetTargetFPS(256);

    UI::Init_impl();
    UI::Context ui_context(32768 * 4);
    UI::SetContext(&ui_context);

    UI::Builder ui;
    ui.SetContext(&ui_context);

    float time = 0;
    bool flip = false;
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        BeginDrawing();
        ClearBackground(Color{0, 0, 0, 255});

        //UI::SetDebugInput(IsMouseButtonPressed(0), IsMouseButtonReleased(0), GetMouseWheelMove(),IsKeyPressed(KEY_F));
        //SpotifyExample();
        ui.Root(0, 0, GetScreenWidth(), GetScreenHeight(), GetMouseX(), GetMouseY(), [&]
        {
            UI::BoxStyle style;
            style.width = {100};
            style.height = {100};
            style.background_color = {255, 0, 0, 255};

            ui.Box("Box Name")
                .OnHover([&]
                {
                    style.background_color = {0, 255, 0, 255};
                    if(IsMouseButtonPressed(0))
                        std::cout<<"Pressed\n";
                })
                .Style(style)
                .Run([&]
                {
                    UI::BoxStyle style;
                    style.width = {60};
                    style.height = {60};
                    style.background_color = {0, 0, 255, 255};
                    ui.Box()
                        .Style(style)
                        .Run([&]
                        {
                            ui.Text("[S:20]Inserted Text").Run();
                        });
                });
            });

        ui_context.Draw();


        //DrawText(TextFormat("fps = %d", GetFPS()), 10, 10, 20, WHITE);
        EndDrawing();
    }
    CloseWindow();        // Close window and OpenGL context

    return 0;
}