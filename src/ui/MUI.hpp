#pragma once
/*
    The library useses the css box model, but border-wdith does not extend the box. It is only effects rendering
*/



#define UI_DEBUG 1

namespace UI
{
    //Math helpers
    template<typename T>
    inline T Min(T a, T b) {return a < b? a: b;}
    template<typename T>
    inline T Max(T a, T b) {return a >= b? a: b;}
    template<typename T>
    inline T Clamp(T value, T minimum, T maximum)
    {
        return Max(Min(value, maximum), minimum);
    }

    //All measurements are based on this Unit
    struct Unit
    {
        enum Type : unsigned char 
        {

            PIXEL,
            MM,
            CM,
            INCH,
            //Limited to width/height
            PARENT_PERCENT,
            ROOT_PERCENT,
            CONTENT_PERCENT,   
            AVAILABLE_PERCENT,

            WIDTH_PERCENT //only be applied to height
        };

        float value = 0;
        Unit::Type unit = Type::PIXEL;
    };
    struct Color
    {
        unsigned char r = 0, g = 0, b = 0, a = 0;
    };

    enum class Layout: unsigned char 
    {
        FLOW,
        GRID
    };
    enum class Positioning: unsigned char 
    {
        RELATIVE,
        ABSOLUTE
    };
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
    struct BoxStyle
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
        bool detach = false;


        //PIXEL VALUES
        unsigned char corner_radius = 0; //255 sets to circle
        unsigned char border_width = 0;  
    };


    //Main UI Functions
    struct BoxInfo
    {
        int draw_x =             0;
        int draw_y =             0;
        int draw_width =         0;
        int draw_height =        0;
        int content_width =     0;
        int content_height =    0;
        bool valid =            false;
        bool is_hover =         false;
        bool is_direct_hover =  false;
        int MaxScrollX() const { return Max(0, content_width - draw_width);}
        int MaxScrollY() const { return Max(0, content_height - draw_height);}
    };
    BoxInfo GetBoxInfo(const char* label);
    void BeginRoot(unsigned int screen_width, unsigned int screen_height, int mouse_x, int mouse_y);
    void EndRoot();
    void BeginBox(const UI::BoxStyle& box_style, const char* label = nullptr);
    void InsertText(const char* text, bool copy_text = true);
    void EndBox();
    void Draw();


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

    //Implement these functions
    void LogError_impl(const char* text);
    void LogError_impl(int num);
    void Init_impl();
    void DrawRectangle_impl(float x, float y, float width, float height, float corner_radius, float border_size, Color border_color, Color background_color);
    void SetFont_impl(const char* file_path);
    void DrawText_impl(TextPrimitive draw_command);
    int MeasureChar_impl(char c, int font_size, int spacing);
    int MeasureText_impl(const char* text, int size, int spacing);
    void BeginScissorMode_impl(float x, float y, float width, float height);
    void EndScissorMode_impl();
}


