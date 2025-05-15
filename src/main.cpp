#include <raylib/raylib.h>
#include "ui/MUI.hpp"

#include <math/vec.h>


int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetExitKey(0);
    SetTargetFPS(100);



    
    //Version 1




    std::cout<<sizeof(UI::StyleSheet)<<'\n';

    UI::StyleSheet base;
    base.layout = UI::Layout::FLOW;  
    base.flow.axis = UI::Flow::Axis::HORIZONTAL;
    base.background_color = UI::Color{59, 58, 60, 255};

    UI::StyleSheet red_div;
    red_div.width = UI::Unit{50, UI::Unit::Type::PARENT_PERCENT};
    red_div.height = UI::Unit{50, UI::Unit::Type::PARENT_PERCENT};
    red_div.background_color = UI::Color{255, 0, 0, 255};

    UI::StyleSheet green_div = red_div;
    green_div.background_color = UI::Color{0, 255, 0, 255};
    green_div.width = UI::Unit{90, UI::Unit::Type::PARENT_PERCENT};
    green_div.flow.wrap = false;
    green_div.max_width = UI::Unit{30, UI::Unit::Type::PIXEL};
    green_div.flow.wrap = true;


    UI::StyleSheet blue_div;
    blue_div.background_color = UI::Color{0, 0, 255, 255};
    blue_div.width = UI::Unit{50, UI::Unit::Type::MM};
    blue_div.height = UI::Unit{50, UI::Unit::Type::AVAILABLE_PERCENT};


    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        base.width = UI::Unit{(short)GetScreenWidth(), UI::Unit::Type::PIXEL};
        base.height = UI::Unit{(short)GetScreenHeight(), UI::Unit::Type::PIXEL};


        BeginDrawing();
        ClearBackground(Color{43, 41, 51, 255});

        //UI LIBRARY TEST
        UI::BeginDiv(&base);
            UI::BeginDiv(&red_div);
                UI::BeginDiv(&green_div);
                    UI::BeginDiv(&blue_div);
                    UI::EndDiv();
                UI::EndDiv();
            UI::EndDiv();
        UI::EndDiv();


        UI::Draw();

        

        DrawText(TextFormat("%i", GetFPS()), GetScreenWidth() - 50, 10, 30, WHITE);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    CloseWindow();        // Close window and OpenGL context

    return 0;
}