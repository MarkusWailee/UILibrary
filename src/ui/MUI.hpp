#pragma once


/*
margin-(left, right, top, bottom)
padding-(left, right, top, bottom)

*/


namespace UI
{


    //All measurements are based on this Unit
    struct Unit
    {
        enum class Type : unsigned char 
        {
            //Unit Type 1
            PIXEL,
            MM,
            CM,
            INCH,

            //Unit Type 2
            PARENT_PERCENT,     //Based on the size of the parent div
            ROOT_PERCENT,       //Based on the root div size

            //Unit Type 3
            // width/height only
            CONTENT_PERCENT,    //Based on the size of the inner content(child divs)
            AVAILABLE_PERCENT   //Based on the amount of space available
        };

        float value = 0;
        Unit::Type unit = Type::PIXEL;
    };
    struct Color
    {
        unsigned char r = 0, g = 0, b = 0, a = 0;
    };

    enum class Layout: unsigned char {FLOW, GRID};
    enum class Positioning: unsigned char {RELATIVE, ABSOLUTE};
    struct Flow
    {
        enum class Axis : unsigned char {HORIZONTAL, VERTICAL};
        enum class Alignment: unsigned char {START, END, CENTERED, SPACE_AROUND, SPACE_BETWEEN};
        Axis axis = Axis::VERTICAL;
        Alignment vertical_alignment = Alignment::START;
        Alignment horizontal_alignment = Alignment::START;
        bool wrap = false;
    };

    struct Grid
    {
        unsigned char row_max = 0, column_max = 0;
        unsigned char row_start = 0, column_start = 0; 
        unsigned char row_end = 0, column_end = 0; 
        Unit cell_width;
        Unit cell_height;
    };
    struct Spacing
    {
        unsigned char left = 0;
        unsigned char right = 0;
        unsigned char top = 0;
        unsigned char bottom = 0;
    };
    struct StyleSheet
    {
        //container
        Positioning positioning = Positioning::RELATIVE;
        Layout layout = Layout::FLOW;
        Flow flow;
        Grid grid;

        Unit gap_row;
        Unit gap_column;

        Unit width  = Unit{0, Unit::Type::PIXEL};
        Unit height = Unit{0, Unit::Type::PIXEL};

        Unit min_width;
        Unit max_width = Unit{9999, Unit::Type::PIXEL};
        Unit min_height;
        Unit max_height = Unit{9999, Unit::Type::PIXEL};

        Unit x = Unit{0, Unit::Type::PIXEL};
        Unit y = Unit{0, Unit::Type::PIXEL};


        Color background_color = UI::Color{0, 0, 0, 0};
        Color border_color = UI::Color{0, 0, 0, 0};

        Spacing padding;
        Spacing margin;

        //Potentially performance heavy
        bool scissor = false;


        //PIXEL VALUES
        unsigned char corner_radius = 0; //255 sets to circle
        unsigned char border_width = 0;  
    };
}




//Main UI Functions
namespace UI
{
    struct MouseInfo
    {
        bool on_mouse_hover = 0;
        bool on_mouse_down = 0;
        bool on_mouse_click = 0;
        bool on_mouse_release = 0;
    };
    void BeginBox(const UI::StyleSheet* div_style_sheet, const char* label = nullptr, UI::MouseInfo* get_info = nullptr);
    void EndBox();
    void Draw();
}


//IMPLEMENT THESE FUNCTIONS
namespace UI
{
    struct TextPrimitive
    {
        float x, y;
        float font_size;
        UI::Color color;
    };

    //Error handling
    void LogError_impl(const char* text);
    void LogError_impl(int num);

    //Rendering
    void DrawText_impl(const char* text, const TextPrimitive& p);
    void DrawRectangle_impl(float x, float y, float width, float height, float corner_radius, float border_size, Color border_color, Color background_color);
    float MeasureText_impl(const char* text, const TextPrimitive& p);
    void BeginScissorMode_impl(float x, float y, float width, float height);
    void EndScissorMode_impl();
}