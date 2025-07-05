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
    UI::Init_impl("assets/fonts/Roboto-Regular.ttf");
    UI::Context context(128 * UI::KB);
    UI::Context context2(128 * UI::KB);
    UI::DebugInspector ui_inspector1(2 * UI::MB);
    UI::DebugInspector ui_inspector2(2 * UI::MB);
    ui_inspector2.theme.base_color = {100, 100, 120, 255};
    ui_inspector2.theme.title_bar_color = {200, 200, 255, 255};


    int frames = 0;
    double time = 0;
    double avg_fps = 0;
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        time += GetFrameTime();
        frames++;
        if(time > 3)
        {
            avg_fps = (double)frames / time; 
            frames = 0;
            time = 0;
        }
        BeginDrawing();
        ClearBackground(Color{0, 0, 0, 255});
        
        //SpotifyExample(&ui_context);

        UI::BoxStyle root = 
        {
            .width = GetScreenWidth(),
            .height = GetScreenHeight(),
            .background_color = {100, 40, 45, 255}
        };
        UI::BoxStyle box =
        {
            .width = 200,
            .height = 200,
            //.margin = {10, 10, 10, 10},
            .background_color = {100, 60, 60, 255},
            .corner_radius = 10
        };
        UI::BoxStyle box2 =
        {
            .width = 100,
            .height = 100,
            .margin = {10, 10, 10, 10},
            .background_color = {60, 100, 60, 255},
            .corner_radius = 10
        };

        //UI::Root(&context, root, [&]
        //{
        //    UI::Box("Box")
        //    .Style(box)
        //    .OnDirectHover([&]
        //    {
        //        UI::Style().background_color = {255, 0,0,255};
        //        std::cout<<UI::Info().DrawHeight()<<'\n';
        //    })
        //    .Run([&]
        //    {
        //        UI::Box().Style(box2).Run(); 
        //    });
        //});

        context.SetInspector(UI::IsKeyPressed(UI::KEY_F1), &ui_inspector1);
        SpotifyExample(&context);



        UI::Draw();
        DrawText(TextFormat("Fps = %f", avg_fps), 10, 10, 20, WHITE);
        EndDrawing();
    }
    CloseWindow();        // Close window and OpenGL context

    return 0;
}