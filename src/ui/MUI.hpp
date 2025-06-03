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
            //Considering using PARENT_WIDTH_PERCENT/PARENT_HEIGHT_PERCENT instead
            PARENT_WIDTH_PERCENT,     //Based on the size of the parent div
            PARENT_HEIGHT_PERCENT,     //Based on the size of the parent div
            ROOT_WIDTH_PERCENT,       //Based on the root div size
            ROOT_HEIGHT_PERCENT,       //Based on the root div size

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
        Axis axis = Axis::HORIZONTAL;
        Alignment vertical_alignment = Alignment::START;
        Alignment horizontal_alignment = Alignment::START;
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

        Unit width  = Unit{50, Unit::Type::PIXEL};
        Unit height = Unit{50, Unit::Type::PIXEL};

        Unit min_width;
        Unit max_width = Unit{9999, Unit::Type::PIXEL};
        Unit min_height;
        Unit max_height = Unit{9999, Unit::Type::PIXEL};

        Unit x = Unit{0, Unit::Type::PIXEL};
        Unit y = Unit{0, Unit::Type::PIXEL};

        int scroll_x = 0;
        int scroll_y = 0;


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
        int pos_x = 0;
        int pos_y = 0;
        int mouse_rel_x = 0;
        int mouse_rel_y = 0;
        bool on_mouse_hover = 0;
    };
    MouseInfo GetMouseInfo(const char* label);
    void BeginRoot(unsigned int screen_width, unsigned int screen_height, int mouse_x, int mouse_y);
    void EndRoot();
    void BeginBox(const UI::StyleSheet* div_style_sheet, const char* label = nullptr, UI::MouseInfo* get_info = nullptr);
    void InsertText(const char* text, bool copy_text = true);
    void EndBox();
    void Draw();
}


//IMPLEMENT THESE FUNCTIONS
namespace UI
{
    struct TextPrimitive
    {
        const char* text = nullptr;
        int x = 0;
        int y = 0;

        //This relative position is where the last rendered text left off.
        int cursor_x = 0;
        int cursor_y = 0;

        int font_size = 0;
        int line_spacing = 0;
        int font_spacing = 0;

        Color font_color;
    };

    //Error handling
    void LogError_impl(const char* text);
    void LogError_impl(int num);

    //Rendering
    void DrawRectangle_impl(float x, float y, float width, float height, float corner_radius, float border_size, Color border_color, Color background_color);

    //Text Rendering
    //Set internal font. Should load/cache fonts
    void Init_impl();
    void SetFont_impl(const char* file_path);
    void DrawText_impl(TextPrimitive draw_command);
    int MeasureChar_impl(char c, int font_size, int spacing);
    int MeasureText_impl(const char* text, int size, int spacing);

    void BeginScissorMode_impl(float x, float y, float width, float height);
    void EndScissorMode_impl();
}