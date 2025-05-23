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
    //SetTargetFPS();



    
    //Version 1




    std::cout<<sizeof(UI::StyleSheet)<<'\n';

    UI::StyleSheet base;
    base.padding = {10,10,10,10};
    base.width = UI::Unit{100, UI::Unit::Type::PARENT_WIDTH_PERCENT};
    base.height = UI::Unit{100, UI::Unit::Type::PARENT_HEIGHT_PERCENT};
    base.flow.horizontal_alignment = UI::Flow::Alignment::CENTERED;
    base.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;

    UI::StyleSheet horizontal_container;
    horizontal_container.flow.axis = UI::Flow::Axis::VERTICAL;
    horizontal_container.margin = {10, 10, 10, 10};
    horizontal_container.width = UI::Unit{100, UI::Unit::Type::PARENT_WIDTH_PERCENT};
    horizontal_container.height = UI::Unit{100, UI::Unit::Type::PARENT_HEIGHT_PERCENT};
    horizontal_container.flow.horizontal_alignment = UI::Flow::Alignment::CENTERED;
    horizontal_container.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
    horizontal_container.background_color = UI::Color{43, 41, 50, 255};

    UI::StyleSheet button1;
    button1.width = UI::Unit{23, UI::Unit::Type::MM};
    button1.height = UI::Unit{11, UI::Unit::Type::MM};
    button1.background_color = UI::Color{50, 50, 50, 255};
    button1.border_color = UI::Color{20, 20, 20, 255};
    button1.border_width = 1;

    UI::StyleSheet button2;
    //button2.margin = {4, 4, 0, 0};
    button2.width = UI::Unit{10, UI::Unit::Type::MM};
    button2.height = UI::Unit{10, UI::Unit::Type::MM};
    button2.background_color = UI::Color{50, 50, 50, 255};
    button2.border_color = UI::Color{20, 20, 20, 255};
    button2.border_width = 1;



    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        BeginDrawing();
        ClearBackground(Color{83, 81, 100, 255});
        if(IsKeyDown(KEY_F))
        {
            horizontal_container.flow.axis = UI::Flow::Axis::HORIZONTAL;
        }
        else
            horizontal_container.flow.axis = UI::Flow::Axis::VERTICAL;
        //UI LIBRARY TEST
        UI::BeginRoot(GetScreenWidth(), GetScreenHeight(), GetMouseX(), GetMouseY());
        UI::BeginBox(&base);
            UI::BeginBox(&horizontal_container);
            for(int i = 0; i<10; i++)
            {
                UI::BeginBox(&button2);
                UI::EndBox();
            }
            UI::BeginBox(&button1);
            UI::EndBox();
            UI::BeginBox(&button1);
            UI::EndBox();
            UI::EndBox();
        UI::EndBox();
        UI::EndRoot();



        UI::Draw();

        

        DrawText(TextFormat("%i", GetFPS()), GetScreenWidth() - 80, 10, 30, WHITE);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    CloseWindow();        // Close window and OpenGL context

    return 0;
}