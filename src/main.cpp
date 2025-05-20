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
    base.gap_column = UI::Unit{10, UI::Unit::Type::MM};
    //base.padding = {10,10,10,10};


    UI::StyleSheet red_div;
    red_div.padding = {20, 20, 20, 20};
    red_div.margin = {50, 50, 50, 50};
    red_div.border_color = {0, 0, 0, 255};
    red_div.border_width = 4;
    red_div.flow.axis = UI::Flow::Axis::HORIZONTAL;
    red_div.width = UI::Unit{100, UI::Unit::Type::AVAILABLE_PERCENT};
    red_div.height = UI::Unit{100, UI::Unit::Type::PARENT_PERCENT};
    red_div.background_color = UI::Color{255, 0, 0, 255};
    red_div.flow.wrap = true;
    //red_div.gap_column = UI::Unit{5, UI::Unit::Type::MM};

    UI::StyleSheet gray_div;
    gray_div.margin = {10, 10, 10, 10};
    gray_div.padding = {50, 50, 50, 50};
    gray_div.flow.axis = UI::Flow::Axis::HORIZONTAL;
    gray_div.background_color = UI::Color{100, 100, 100, 255};
    gray_div.width = UI::Unit{100, UI::Unit::Type::AVAILABLE_PERCENT};
    gray_div.height = UI::Unit{100, UI::Unit::Type::PIXEL};
    gray_div.border_color = UI::Color{0, 0, 0, 255};
    gray_div.border_width = 2;


    UI::StyleSheet blue_div;
    blue_div.background_color = UI::Color{0, 0, 255, 255};
    blue_div.border_color = {0, 0, 0, 255};
    blue_div.border_width = 4;
    blue_div.width = UI::Unit{100, UI::Unit::Type::PIXEL};
    blue_div.height = UI::Unit{100, UI::Unit::Type::PIXEL};

    UI::StyleSheet green_div;
    green_div.padding = {5, 5, 5, 5};
    green_div.margin = {5, 5, 5, 5};
    green_div.background_color = UI::Color{0, 255, 0, 255};
    green_div.border_color = {0, 0, 0, 255};
    green_div.border_width = 4;
    green_div.width = UI::Unit{100, UI::Unit::Type::PIXEL};
    green_div.height = UI::Unit{100, UI::Unit::Type::PIXEL};


    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        base.width = UI::Unit{(float)GetScreenWidth(), UI::Unit::Type::PIXEL};
        base.height = UI::Unit{(float)GetScreenHeight(), UI::Unit::Type::PIXEL};


        BeginDrawing();
        ClearBackground(Color{83, 81, 100, 255});

        //UI LIBRARY TEST
        UI::BeginBox(&base);
            UI::BeginBox(&red_div);
                UI::BeginBox(&gray_div);
                UI::EndBox();
                UI::BeginBox(&blue_div);
                UI::EndBox();
                UI::BeginBox(&green_div);
                UI::EndBox();
                UI::BeginBox(&gray_div);
                UI::EndBox();
            UI::EndBox();
        UI::EndBox();


        UI::Draw();

        

        DrawText(TextFormat("%i", GetFPS()), GetScreenWidth() - 50, 10, 30, WHITE);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    CloseWindow();        // Close window and OpenGL context

    return 0;
}