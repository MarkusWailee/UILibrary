#include <raylib/raylib.h>
#include "ui/MUI.hpp"

#include <math/vec.h>



int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetExitKey(0);
    //SetTargetFPS(100);

    UI::Init_impl();

    
    //Version 1




    std::cout<<sizeof(UI::StyleSheet)<<'\n';

    UI::StyleSheet base;
    base.background_color = {80, 80, 80, 255};
    base.flow.axis = UI::Flow::Axis::HORIZONTAL;
    base.flow.horizontal_alignment = UI::Flow::Alignment::CENTERED;
    base.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
    base.width = UI::Unit{100, UI::Unit::Type::PARENT_WIDTH_PERCENT};
    base.height = UI::Unit{100, UI::Unit::Type::PARENT_HEIGHT_PERCENT};
    base.padding = {20, 20, 20, 20};
    base.background_color = {80, 80, 100, 255};

    UI::StyleSheet h_container;
    h_container.padding = {20, 20, 10, 10};
    h_container.flow.axis = UI::Flow::Axis::VERTICAL;
    h_container.flow.horizontal_alignment = UI::Flow::Alignment::START;
    h_container.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
    h_container.gap_row = UI::Unit{1, UI::Unit::Type::PIXEL};
    //h_container.gap_column = UI::Unit{10, UI::Unit::Type::PIXEL};
    h_container.background_color = {40, 40, 50, 255};
    h_container.width = UI::Unit{100 ,UI::Unit::Type::PARENT_WIDTH_PERCENT};
    h_container.height = UI::Unit{100 ,UI::Unit::Type::PARENT_HEIGHT_PERCENT};
    h_container.border_width = 2;
    h_container.corner_radius = 10;
    h_container.border_color = {25, 25, 30, 255};
    h_container.scissor = true;


    UI::StyleSheet button;
    //button.margin = {20, 20, 20, 20};
    button.background_color = {40, 255, 50, 255};
    button.width = UI::Unit{100, UI::Unit::Type::PIXEL};
    button.height = UI::Unit{100, UI::Unit::Type::PIXEL};
    button.min_height = UI::Unit{100 ,UI::Unit::Type::PIXEL};
    button.border_width = 4;
    button.border_color = {0,0,0,255};

    UI::StyleSheet button2;
    button2.background_color = {255, 50, 50, 255};
    button2.width = UI::Unit{100 ,UI::Unit::Type::PIXEL};
    button2.height = UI::Unit{100 ,UI::Unit::Type::PIXEL};
    button2.max_height = UI::Unit{100 ,UI::Unit::Type::PIXEL};
    button2.border_width = 4;
    button2.border_color = {0,0,0,255};
    //button2.padding = {5, 5, 5, 5};
    //button2.margin = {5, 5, 5, 5};

    UI::StyleSheet button3;
    button3.margin = {5,5,5,5};
    button3.background_color = {50, 50, 70, 255};
    button3.border_color = {25, 25, 30, 255};
    button3.width = UI::Unit{20 ,UI::Unit::Type::MM};
    button3.height = UI::Unit{50,UI::Unit::Type::AVAILABLE_PERCENT};
    button3.border_width = 0;
    button3.corner_radius = 20;

    UI::StyleSheet button4;
    button4.flow.axis = UI::Flow::Axis::HORIZONTAL;
    button4.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
    button4.flow.horizontal_alignment = UI::Flow::Alignment::CENTERED;
    //button4.margin = {5,5,5,5};
    button4.background_color = {50, 50, 70, 255};
    button4.border_color = {25, 25, 30, 255};
    button4.width = UI::Unit{100,UI::Unit::Type::AVAILABLE_PERCENT};
    button4.height = UI::Unit{16,UI::Unit::Type::PIXEL};
    button4.border_width = 0;
    button4.corner_radius = 10;

    UI::StyleSheet button5;
    button5.margin = {5,5,5,5};
    button5.background_color = {0, 0, 0, 255};
    button5.border_color = {25, 25, 30, 255};
    button5.width = UI::Unit{100 ,UI::Unit::Type::PIXEL};
    button5.height = UI::Unit{100,UI::Unit::Type::PIXEL};
    button5.border_width = 0;
    button5.corner_radius = 20;
    SetTextLineSpacing(0);
    UI::StyleSheet style;
    float time = 0;
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        time += GetFrameTime() * 20;
        BeginDrawing();
        ClearBackground(Color{0, 0, 0, 255});
        h_container.scroll_y += GetMouseWheelMove() * 40;
        //UI LIBRARY TEST



        const char text1[] = "[C:228BE6]A [C:F06292]B[C:4CAF50] C[C:FFC107] D[C:795548] E[C:9C27B0] F[C:2196F3] G[C:FF5722] H[C:3F51B5] I[C:CDDC39] J";
        const char text2[] ="[C:228BE6] Sky Blue [C:F06292]Pink Rose [C:4CAF50]Leaf Green [C:FFC107]Golden Amber [C:795548]Cocoa Brown [C:9C27B0]Vivid Purple [C:2196F3]Bright Azure [C:FF5722]Flame Orange [C:3F51B5]Indigo Blue [C:CDDC39]Lime Yellow";
        const char text3[] = "\\[C:FF0000]Red  [C:FF7F00]Orange	[C:FFFF00]Yellow[C:00FF00] Green\n[C:0000FF]Blue [C:4B0082]Indigo   [C:8B00FF]Violet [C:FF0000] R	 e  d [C:FF7F00]O r\ta	n g  e";
        const char text4[] = "[S:64][C:FF0000]R[C:FF7F00]a[C:FFFF00]i[C:00FF00]n[C:0000FF]b[C:4B0082]o[C:8B00FF]w [C:FF0000]T[C:FF7F00]e[C:FFFF00]x[C:00FF00]t [C:0000FF]T[C:4B0082]E[C:8B00FF]S[C:FF0000]T!";



        UI::BeginRoot(GetScreenWidth(), GetScreenHeight(), GetMouseX(), GetMouseY());
        UI::BeginBox(&base);
            UI::BeginBox(&h_container);
                UI::BeginBox(&button4);
                UI::EndBox();
                UI::BeginBox(&button4);
                UI::EndBox();
                UI::InsertText(text1);
                UI::InsertText(text2);
                UI::InsertText(text3);
                UI::InsertText(text4);
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