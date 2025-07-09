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
            .width = {100, UI::Unit::AVAILABLE_PERCENT},
            .height = {100, UI::Unit::AVAILABLE_PERCENT},
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
            .background_color = {50, 40, 40, 255},
            .corner_radius = 10
        };

        ui_inspector1.GetContext()->SetInspector(UI::IsKeyPressed(UI::KEY_F2), &ui_inspector2);
        context.SetInspector(UI::IsKeyPressed(UI::KEY_F1), &ui_inspector1);
        UI::Root(&context, root, [&]
        {
            UI::Box()
            .Style(left_panel)
            .Run([&]
            {
                for(int x = 0; x < 3; x++)
                    for(int y = 0; y < 3; y ++)
                    {
                        UI::Box(UI::Fmt("Box %d %d", x, y)).Style(box1).PreRun([&]
                        {
                            UI::Style().grid.x = x;
                            UI::Style().grid.y = y;
                            UI::Style().background_color.r = 50 + x * 10;
                            UI::Style().background_color.g = 50 + x * 10;
                            UI::Style().background_color.b = 50 + x * 10;
                        })
                        .OnHover([&]
                        {
                            UI::Style().background_color = {100, 100, 100, 255};
                        }).Run();
                    }

            });
        });

        //SpotifyExample(&context);

        BeginDrawing();
        ClearBackground(Color{0, 0, 0, 255});


        UI::Draw();
        //DrawText(TextFormat("Fps = %f", avg_fps), 10, 10, 20, WHITE);
        EndDrawing();
    }
    CloseWindow();        // Close window and OpenGL context

    return 0;
}