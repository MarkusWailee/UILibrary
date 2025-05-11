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
            PIXEL,
            MM,
            CM,
            CONTENT_PERCENT,    //Based on the size of the inner content(child divs)
            PARENT_PERCENT,     //Based on the size of the parent div
            ROOT_PERCENT,       //Based on the root div size
            AVAILABLE_PERCENT   //Based on the amount of space available
        };

        short value = 0;
        Unit::Type unit = Type::PIXEL;
    };
    struct Color
    {
        unsigned char r = 0, g = 0, b = 0, a = 0;
    };

    enum class Layout: unsigned char {FLOW, GRID};
    enum class Positioning: unsigned char {RELATIVE, ABSOLUTE};
    enum class OverFlow : unsigned char {WRAP, SCROLL, HIDE, NONE};
    struct Flow
    {
        enum class Axis : unsigned char {HORIZONTAL, VERTICAL};
        enum class Spacing: unsigned char {START, END, CENTERED, AROUND, BETWEEN};
        Unit gap;
        Axis axis = Axis::VERTICAL;
        Spacing vertical_spacing = Spacing::START;
        Spacing horizontal_spacing = Spacing::START;
    };

    struct Grid
    {
        unsigned char row_max = 0, column_max = 0;
        unsigned char row_start = 0, column_start = 0; 
        unsigned char row_end = 0, column_end = 0; 
        Unit column_height;
        Unit row_width;
        Unit gap_row;
        Unit gap_column;
    };
    struct StyleSheet
    {
        //container
        Positioning positioning = Positioning::RELATIVE;
        OverFlow overflow = OverFlow::WRAP;
        Layout layout = Layout::FLOW;
        Flow flow;
        Grid grid;

        Unit width  = Unit{0, Unit::Type::PIXEL};
        Unit height = Unit{0, Unit::Type::PIXEL};

        Unit width_min;
        Unit width_max = Unit{100, Unit::Type::ROOT_PERCENT};
        Unit height_min;
        Unit height_max = Unit{100, Unit::Type::ROOT_PERCENT};

        Unit x = Unit{0, Unit::Type::PIXEL};
        Unit y = Unit{0, Unit::Type::PIXEL};

        Color background_color = UI::Color{0, 0, 0, 0};
        Color border_color = UI::Color{0, 0, 0, 0};

        //PIXEL VALUES
        unsigned char corner_radius = 0; //255 sets to circle
        unsigned char border_width = 0;  
        unsigned char padding_top = 0;
        unsigned char padding_right = 0;
        unsigned char padding_bottom = 0;
        unsigned char padding_left = 0;
        unsigned char margin_top = 0;
        unsigned char margin_right = 0;
        unsigned char margin_bottom = 0;
        unsigned char margin_left = 0;

        inline void set_padding(unsigned char value) {padding_top = padding_right = padding_bottom = padding_left = value;}
        inline void set_margin(unsigned char value){margin_top = margin_right = margin_bottom = margin_left = value;}
    };
}




//Main UI Functions
namespace UI
{
    struct DivMouseInfo
    {
        bool on_mouse_hover = 0;
        bool on_mouse_down = 0;
        bool on_mouse_click = 0;
        bool on_mouse_release = 0;
    };
    void BeginDiv(const UI::StyleSheet* style_sheet, UI::DivMouseInfo* get_info = nullptr);
    void EndDiv();
    void Draw();
}


//IMPLEMENT THESE FUNCTIONS
namespace UI
{
    struct RectanglePrimitive
    {
        float x, y;
        float width, height;
        float border_width;
        float corner_radius;
        Color background_color;
        Color border_color;
    };
    struct TextPrimitive
    {
        float x, y;
        float font_size;
        UI::Color color;
    };
    void DrawText_impl(const TextPrimitive& p);
    void DrawRectangle_impl(const char* text, const RectanglePrimitive& p);
    float MeasureText_impl(const char* text, const TextPrimitive& p);

    void BeginScissorMode_impl(float x, float y, float width, float height);
    void EndScissorMode_impl();
}