#include <raylib/raylib.h>
#include "ui/ui.hpp"
#include <math/vec.h>
#include "UI_Demo.hpp"
#include "ui_inspector.hpp"

#include <vector>
int main(void)
{
    float screenWidth = 960;
    float screenHeight = 600;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetExitKey(0);
    //SetTargetFPS(256);

    //UI::Init_impl("assets/fonts/Raleway-Regular.ttf");
    UI::Init_impl("assets/fonts/Archivo-Regular.ttf");
    UI::Context ui_context(128 * UI::KB);
    UI::DebugInspector ui_inspector1(UI::MB);
    UI::DebugInspector ui_inspector2(2 * UI::MB);
    ui_inspector2.theme.base_color = {100, 100, 120, 255};
    ui_inspector2.theme.title_bar_color = {200, 200, 255, 255};



    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        BeginDrawing();
        ClearBackground(Color{0, 0, 0, 255});
        
        ui_inspector1.GetContext()->SetInspector(IsKeyPressed(KEY_F2), &ui_inspector2);
        ui_context.SetInspector(UI::IsKeyPressed(UI::KEY_F1), &ui_inspector1);
        SpotifyExample(&ui_context);

        UI::BoxStyle root = 
        {
            .width = GetScreenWidth(),
            .height = GetScreenHeight(),
            .padding = {10, 10, 10, 10},
            .margin = {10, 10, 10, 10},
            .background_color = {40, 40, 45, 255}
        };
        UI::BoxStyle left_panel =
        {
            .width = {50, UI::Unit::PARENT_PERCENT},
            .height = {100, UI::Unit::PARENT_PERCENT},
            .background_color = {60, 60, 60, 255},
            .corner_radius = 10
        };

        //UI::Builder ui;
        //ui.SetContext(UI::GetContext());
        //ui.Root(root, [&]
        //{
        //    ui.Box()
        //    .Style(left_panel)
        //    .Run([&]{

        //    });
        //});
        //UI::Draw();
    
        //ui_context.Draw();

        //DrawText(TextFormat("fps = %d", GetFPS()), 9, 10, 20, WHITE);
        EndDrawing();
    }
    CloseWindow();        // Close window and OpenGL context

    return 0;
}