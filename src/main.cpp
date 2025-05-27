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

    UI::Init_impl();

    
    //Version 1




    std::cout<<sizeof(UI::StyleSheet)<<'\n';

    UI::StyleSheet base;
    base.background_color = {80, 80, 80, 255};
    base.flow.axis = UI::Flow::Axis::HORIZONTAL;
    base.flow.horizontal_alignment = UI::Flow::Alignment::START;
    base.flow.vertical_alignment = UI::Flow::Alignment::START;
    base.width = UI::Unit{100, UI::Unit::Type::PARENT_WIDTH_PERCENT};
    base.height = UI::Unit{100, UI::Unit::Type::PARENT_HEIGHT_PERCENT};
    base.padding = {20, 20, 20, 20};

    UI::StyleSheet h_container;
    h_container.flow.axis = UI::Flow::Axis::VERTICAL;
    h_container.flow.horizontal_alignment = UI::Flow::Alignment::CENTERED;
    h_container.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
    h_container.gap_row = UI::Unit{10, UI::Unit::Type::PIXEL};
    h_container.gap_column = UI::Unit{10, UI::Unit::Type::PIXEL};
    h_container.background_color = {40, 40, 50, 255};
    h_container.width = UI::Unit{100 ,UI::Unit::Type::PARENT_WIDTH_PERCENT};
    h_container.height = UI::Unit{100 ,UI::Unit::Type::PARENT_HEIGHT_PERCENT};
    h_container.border_width = 4;
    h_container.border_color = {0,0,0,255};


    UI::StyleSheet button;
    button.background_color = {40, 255, 50, 255};
    button.width = UI::Unit{100 ,UI::Unit::Type::AVAILABLE_PERCENT};
    button.height = UI::Unit{100 ,UI::Unit::Type::AVAILABLE_PERCENT};
    button.min_height = UI::Unit{100 ,UI::Unit::Type::PIXEL};
    button.border_width = 4;
    button.border_color = {0,0,0,255};

    UI::StyleSheet button2;
    button2.background_color = {255, 50, 50, 255};
    button2.width = UI::Unit{100 ,UI::Unit::Type::AVAILABLE_PERCENT};
    button2.height = UI::Unit{100 ,UI::Unit::Type::AVAILABLE_PERCENT};
    button2.max_height = UI::Unit{100 ,UI::Unit::Type::PIXEL};
    button2.border_width = 4;
    button2.border_color = {0,0,0,255};
    button2.padding = {5, 5, 5, 5};
    button2.margin = {5, 5, 5, 5};

    UI::StyleSheet button3;
    button3.margin = {10, 10, 10, 10};
    button3.padding = {10, 10, 10, 10};
    button3.background_color = {50, 50, 255, 255};
    button3.width = UI::Unit{100 ,UI::Unit::Type::AVAILABLE_PERCENT};
    button3.height = UI::Unit{100,UI::Unit::Type::AVAILABLE_PERCENT};
    button3.border_width = 4;
    button3.border_color = {0,0,0,255};


    UI::StyleSheet style;
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        BeginDrawing();
        ClearBackground(Color{83, 81, 100, 255});

        //UI LIBRARY TEST
        UI::BeginRoot(GetScreenWidth(), GetScreenHeight(), GetMouseX(), GetMouseY());
        UI::BeginBox(&base);
            UI::BeginBox(&h_container);
                UI::BeginBox(&button);
                UI::EndBox();
                UI::BeginBox(&button);
                UI::EndBox();
                UI::BeginBox(&button2);
                UI::EndBox();
                UI::BeginBox(&button3);
                UI::EndBox();
                UI::BeginBox(&button3);
                UI::EndBox();
            UI::EndBox();
        UI::EndBox();
        UI::EndRoot();


        UI::Draw();

        //UI::DrawText_impl("THIS     TEST", 100, 100, 32, 1, UI::Color{255 ,255,255,255}); 
        //int width = UI::MeasureText_impl("THIS     TEST", 32, 1);
        //UI::DrawText_impl("hello world", 100 + width, 100, 32, 1, UI::Color{255 ,255,255,255}); 

        DrawText(TextFormat("%i", GetFPS()), GetScreenWidth() - 80, 10, 30, WHITE);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    CloseWindow();        // Close window and OpenGL context

    return 0;
}