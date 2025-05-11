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
    SetTargetFPS(10);



    
    //Version 1




    std::cout<<sizeof(UI::StyleSheet)<<'\n';

    UI::StyleSheet base;
    base.width = UI::Unit{(short)GetScreenWidth(), UI::Unit::Type::PIXEL};
    base.height = UI::Unit{(short)GetScreenHeight(), UI::Unit::Type::PIXEL};
    base.layout = UI::Layout::FLOW;  
    base.flow.axis = UI::Flow::Axis::HORIZONTAL;

    UI::StyleSheet vertical_panel;
    vertical_panel.width = UI::Unit{100, UI::Unit::Type::PIXEL};
    vertical_panel.height = UI::Unit{100, UI::Unit::Type::PARENT_PERCENT};
    vertical_panel.layout = UI::Layout::FLOW;
    vertical_panel.flow.axis = UI::Flow::Axis::VERTICAL;
    vertical_panel.flow.vertical_spacing = UI::Flow::Spacing::START;

    UI::StyleSheet grid1;
    grid1.width = UI::Unit{100, UI::Unit::Type::AVAILABLE_PERCENT};
    grid1.height = UI::Unit{100, UI::Unit::Type::AVAILABLE_PERCENT}; //PARENT_PERCENT should do the same thing
    grid1.layout = UI::Layout::GRID;
    grid1.grid.column_max = 2;
    grid1.grid.row_max = 2; //Wont add more if items are maxed. Setting to a large number will add rows(Might turn into a scrollable).
    grid1.overflow = UI::OverFlow::SCROLL;
    grid1.grid.column_height = UI::Unit{100, UI::Unit::Type::PIXEL};
    grid1.grid.row_width = UI::Unit{100, UI::Unit::Type::PIXEL};

    UI::StyleSheet items;
    items.width = UI::Unit{100, UI::Unit::Type::AVAILABLE_PERCENT};
    items.height = UI::Unit{100, UI::Unit::Type::AVAILABLE_PERCENT};

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {


        BeginDrawing();
        ClearBackground(Color{43, 41, 51, 255});

        //UI LIBRARY TEST
        UI::BeginDiv(nullptr);
            UI::BeginDiv(nullptr);
                UI::BeginDiv(nullptr);
                UI::EndDiv();

                UI::BeginDiv(nullptr);
                    UI::BeginDiv(nullptr);
                    UI::EndDiv();

                    UI::BeginDiv(nullptr);
                    UI::EndDiv();
                UI::EndDiv();

                UI::BeginDiv(nullptr);
                UI::EndDiv();

            UI::EndDiv();
                UI::BeginDiv(nullptr);
                UI::EndDiv();

                UI::BeginDiv(nullptr);
                UI::EndDiv();
        UI::EndDiv();


        UI::Draw();

        
        { { {} {} } { {} {} } }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    CloseWindow();        // Close window and OpenGL context

    return 0;
}