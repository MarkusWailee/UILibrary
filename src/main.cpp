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
    SetTargetFPS(30);



    
    //Version 1




    std::cout<<sizeof(UI::StyleSheet)<<'\n';

    UI::StyleSheet base;
    base.flow.horizontal_alignment = UI::Flow::Alignment::CENTERED;
    base.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;


    UI::StyleSheet box1;
    box1.flow.axis = UI::Flow::Axis::VERTICAL;
    box1.flow.horizontal_alignment = UI::Flow::Alignment::START;
    box1.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
    box1.width = UI::Unit{80, UI::Unit::Type::PARENT_WIDTH_PERCENT};
    box1.height = UI::Unit{80, UI::Unit::Type::PARENT_HEIGHT_PERCENT};
    box1.background_color = UI::Color{43, 41, 50, 255};
    box1.border_color = UI::Color{23, 21, 25, 255};
    box1.border_width = 2;
    box1.gap_row = UI::Unit{2, UI::Unit::Type::PIXEL};
    //box1.gap_column = UI::Unit{2, UI::Unit::Type::PIXEL};


    UI::StyleSheet box2;
    box2.margin = {20,2,2,2};
    box2.width = UI::Unit{100, UI::Unit::Type::PARENT_HEIGHT_PERCENT};
    box2.max_width = UI::Unit{90, UI::Unit::Type::PARENT_WIDTH_PERCENT};
    box2.height = UI::Unit{100, UI::Unit::Type::AVAILABLE_PERCENT};
    box2.background_color = UI::Color{33, 31, 40, 255};
    box2.border_color = UI::Color{23, 21, 25, 255};
    box2.border_width = 2;


    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        base.width = UI::Unit{(float)GetScreenWidth(), UI::Unit::Type::PIXEL};
        base.height = UI::Unit{(float)GetScreenHeight(), UI::Unit::Type::PIXEL};


        BeginDrawing();
        ClearBackground(Color{83, 81, 100, 255});

        //UI LIBRARY TEST
        UI::BeginBox(&base);
            UI::BeginBox(&box1);
                UI::BeginBox(&box2);
                UI::EndBox();
                UI::BeginBox(&box2);
                UI::EndBox();
                UI::BeginBox(&box2);
                UI::EndBox();
                UI::BeginBox(&box2);
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