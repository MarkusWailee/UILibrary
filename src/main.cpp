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


    UI::StyleSheet vertical_bar_left;
    vertical_bar_left.flow.axis = UI::Flow::Axis::VERTICAL;
    vertical_bar_left.flow.horizontal_alignment = UI::Flow::Alignment::CENTERED;
    vertical_bar_left.flow.vertical_alignment = UI::Flow::Alignment::END;
    vertical_bar_left.width = UI::Unit{200, UI::Unit::Type::PIXEL};
    vertical_bar_left.height = UI::Unit{100, UI::Unit::Type::PARENT_HEIGHT_PERCENT};
    vertical_bar_left.background_color = UI::Color{43, 41, 50, 255};
    vertical_bar_left.border_color = UI::Color{23, 21, 25, 255};
    vertical_bar_left.border_width = 1;

    UI::StyleSheet vertical_container;
    vertical_container.flow.axis = UI::Flow::Axis::VERTICAL;
    vertical_container.flow.horizontal_alignment = UI::Flow::Alignment::CENTERED;
    vertical_container.flow.vertical_alignment = UI::Flow::Alignment::SPACE_AROUND;
    vertical_container.width = UI::Unit{100, UI::Unit::Type::AVAILABLE_PERCENT};
    vertical_container.height = UI::Unit{100, UI::Unit::Type::PARENT_HEIGHT_PERCENT};
    vertical_container.background_color = UI::Color{43, 41, 50, 255};
    vertical_container.border_color = UI::Color{23, 21, 25, 255};
    vertical_container.border_width = 1;


    UI::StyleSheet bottom_container;
    bottom_container.gap_column = UI::Unit{20, UI::Unit::Type::PIXEL};
    bottom_container.padding = {20, 20, 20, 20};
    bottom_container.flow.axis = UI::Flow::Axis::HORIZONTAL;
    bottom_container.flow.horizontal_alignment = UI::Flow::Alignment::SPACE_BETWEEN;
    bottom_container.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
    bottom_container.width = UI::Unit{100, UI::Unit::Type::AVAILABLE_PERCENT};
    bottom_container.height = UI::Unit{100, UI::Unit::Type::PIXEL};
    bottom_container.background_color = UI::Color{43, 41, 50, 255};
    bottom_container.border_color = UI::Color{23, 21, 25, 255};
    bottom_container.border_width = 1;
    bottom_container.gap_row = UI::Unit{2, UI::Unit::Type::PIXEL};

    UI::StyleSheet main_container;
    main_container.margin = {10,10,10,10};
    main_container.flow.axis = UI::Flow::Axis::HORIZONTAL;
    main_container.flow.horizontal_alignment = UI::Flow::Alignment::CENTERED;
    main_container.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
    main_container.width = UI::Unit{100, UI::Unit::Type::AVAILABLE_PERCENT};
    main_container.height = UI::Unit{100, UI::Unit::Type::AVAILABLE_PERCENT};
    main_container.background_color = UI::Color{100, 100, 125, 255};
    main_container.border_color = UI::Color{23, 21, 25, 255};
    main_container.border_width = 1;
    main_container.gap_row = UI::Unit{2, UI::Unit::Type::PIXEL};

    UI::StyleSheet button;
    button.width = UI::Unit{100, UI::Unit::Type::PIXEL};
    button.height = UI::Unit{25, UI::Unit::Type::PIXEL};
    button.background_color = UI::Color{80, 50, 50, 255};
    button.border_color = UI::Color{30, 20, 20, 255};
    button.border_width = 1;

    UI::StyleSheet button2;
    //button2.margin = {4, 4, 0, 0};
    button2.width = UI::Unit{100, UI::Unit::Type::AVAILABLE_PERCENT};
    button2.height = UI::Unit{100, UI::Unit::Type::AVAILABLE_PERCENT};
    button2.background_color = UI::Color{50, 50, 50, 255};
    button2.border_color = UI::Color{20, 20, 20, 255};
    button2.border_width = 1;


    UI::StyleSheet progress_bar;
    //progress_bar.margin = {10,10,10,10};
    progress_bar.width = UI::Unit{100, UI::Unit::Type::AVAILABLE_PERCENT};
    progress_bar.height = UI::Unit{10, UI::Unit::Type::PIXEL};
    progress_bar.background_color = UI::Color{50, 100, 50, 255};
    progress_bar.border_color = UI::Color{20, 20, 20, 255};
    progress_bar.border_width = 1;



    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        BeginDrawing();
        ClearBackground(Color{83, 81, 100, 255});

        //UI LIBRARY TEST
        UI::BeginRoot(GetScreenWidth(), GetScreenHeight(), GetMouseX(), GetMouseY());
        UI::BeginBox(&base);
            UI::BeginBox(&vertical_bar_left);
            for(int i = 0; i<20; i++)
            {
                UI::BeginBox(&button2);
                UI::EndBox();
            }
            UI::EndBox();
            UI::BeginBox(&vertical_container);
                UI::BeginBox(&main_container);
                    for(int i = 0; i<40; i++)
                    {
                        UI::BeginBox(&button2);
                        UI::EndBox();
                    }

                UI::EndBox();
                UI::BeginBox(&bottom_container);
                    UI::BeginBox(&button);
                    UI::EndBox();
                    UI::BeginBox(&progress_bar);
                    UI::EndBox();
                    UI::BeginBox(&button);
                    UI::EndBox();
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