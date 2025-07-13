#include <raylib/raylib.h>
#include "ui/ui.hpp"
#include <math/vec.h>
#include "UI_Demo.hpp"
#include "ui/ui_widgets.hpp"


#include <vector>
int main(void)
{
    float screenWidth = 960;
    float screenHeight = 600;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetExitKey(0);

    UI::Init_impl("assets/fonts/Roboto-Regular.ttf");
    UI::Context context(128 * UI::KB);
    UI::Context context2(128 * UI::KB);
    UI::DebugInspector ui_inspector1(2 * UI::MB);
    UI::DebugInspector ui_inspector2(2 * UI::MB);
    ui_inspector2.theme.base_color = {100, 100, 120, 255};
    ui_inspector2.theme.title_bar_color = {200, 200, 255, 255};

    Texture tex = LoadTexture("assets/textures/FinnSprite.png");
    UI::TextureRect texture =
    {
        .texture = &tex,
        .x = 0,
        .y = 0,
        .width = 32,
        .height = 32
    };

    int frames = 0;
    double time = 0;
    double avg_fps = 0;

    int selection = 0;
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
        
        //SpotifyExample(&ui_context);

        UI::BoxStyle root = 
        {
            .width = GetScreenWidth(),
            .height = GetScreenHeight(),
            .padding = {10, 10, 10, 10},
            .margin = {10, 10, 10, 10},
            .background_color = {50, 50, 60, 255}
        };
        UI::BoxStyle left_panel =
        {
            .layout = UI::Layout::GRID,
            .grid = 
            {
                .row_count = 3,
                .column_count = 3,
            },
            .width = {100, UI::Unit::PARENT_PERCENT},
            .height = {100, UI::Unit::PARENT_PERCENT},
            .padding = {10, 10, 10, 10},
            .background_color = {30, 30, 35, 255},
            .gap_row = 4,
            .gap_column = 4,
            .corner_radius = 10
        };
        UI::BoxStyle box1 = 
        {
            .width = {100, UI::Unit::PARENT_PERCENT},
            .height = {100, UI::Unit::PARENT_PERCENT},
            .background_color = {50, 80, 40, 255},
            .corner_radius = 10
        };

        UI::BoxStyle box2 = 
        {
            .width = {100, UI::Unit::PIXEL},
            .height = {100, UI::Unit::PIXEL},
            .texture = texture
        };

        ui_inspector1.GetContext()->SetInspector(UI::IsKeyPressed(UI::KEY_F2), &ui_inspector2);
        context.SetInspector(UI::IsKeyPressed(UI::KEY_F1), &ui_inspector1);

        SpotifyExample(&context);
        //UI::Root(&context, root, [&]
        //{
        //    UI::Box()
        //    .Style(left_panel)
        //    .Run([&]
        //    {
        //        UI::Box().Style(box1)
        //        .PreRun([&]
        //        {
        //            UI::Style().grid.x = 0;
        //            UI::Style().grid.y = 0;
        //            UI::Style().background_color = {200, 100, 100, 255};
        //        }).Run();
        //        UI::Box().Style(box1)
        //        .PreRun([&]
        //        {
        //            UI::Style().grid.x = 1;
        //            UI::Style().grid.y = 0;
        //            UI::Style().background_color = {200, 200, 100, 255};
        //        }).Run();

        //        UI::Box().Style(box1)
        //        .PreRun([&]
        //        {
        //            UI::Style().grid.x = 1;
        //            UI::Style().grid.y = 2;
        //            UI::Style().background_color = {100, 200, 100, 255};
        //        }).Run();

        //        UI::Box().Style(box1)
        //        .PreRun([&]
        //        {
        //            UI::Style().grid.x = 0;
        //            UI::Style().grid.y = 2;
        //            UI::Style().background_color = {100, 150, 150, 255};
        //        }).Run();

        //        UI::Box().Style(box1)
        //        .PreRun([&]
        //        {
        //            UI::Style().grid.x = 0;
        //            UI::Style().grid.y = 1;
        //            UI::Style().grid.span_x = 2;
        //            UI::Style().background_color = {100, 200, 200, 255};
        //        }).Run();

        //        UI::Box().Style(box1)
        //        .PreRun([&]
        //        {
        //            UI::Style().grid.x = 2;
        //            UI::Style().grid.y = 0;
        //            UI::Style().grid.span_x = 0;
        //            UI::Style().grid.span_y = 3;
        //            UI::Style().background_color = {150, 100, 150, 255};
        //        }).Run();
        //    });
        //});

        BeginDrawing();
        ClearBackground(Color{0, 0, 0, 255});


        UI::Draw();
        //DrawText(TextFormat("Fps = %f", avg_fps), 10, 10, 20, WHITE);
        EndDrawing();
    }

    UnloadTexture(tex); 
    CloseWindow();        // Close window and OpenGL context

    return 0;
}