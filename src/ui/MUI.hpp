#pragma once
/*
    The library useses the css box model, but border-wdith does not extend the box. It is only effects rendering
*/


#define UI_DEBUG 0

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


#include "Memory.hpp"
//Internal
namespace UI
{
    struct Box
    {
        uint64_t label_hash =       0; 
        const char* text = nullptr;
        Color background_color = UI::Color{0, 0, 0, 0}; //used for debugging
        Color border_color = UI::Color{0, 0, 0, 0};
        //type 3
        int scroll_x =              0;
        int scroll_y =              0;
        uint16_t width =            0;
        uint16_t height =           0;
        uint16_t gap_row =          0;
        uint16_t gap_column =       0;
        uint16_t min_width =        0;
        uint16_t max_width =        UINT16_MAX;
        uint16_t min_height =       0;
        uint16_t max_height =       UINT16_MAX;
        int16_t x =                 0;
        int16_t y =                 0;
        uint16_t grid_cell_width =  0;
        uint16_t grid_cell_height = 0;

        Unit::Type width_unit =              Unit::Type::PIXEL;
        Unit::Type height_unit =             Unit::Type::PIXEL;
        Unit::Type gap_row_unit =            Unit::Type::PIXEL; //might not need
        Unit::Type gap_column_unit =         Unit::Type::PIXEL; //might not need
        Unit::Type min_width_unit =          Unit::Type::PIXEL;
        Unit::Type max_width_unit =          Unit::Type::PIXEL;
        Unit::Type min_height_unit =         Unit::Type::PIXEL;
        Unit::Type max_height_unit =         Unit::Type::PIXEL;
        Unit::Type x_unit =                  Unit::Type::PIXEL;
        Unit::Type y_unit =                  Unit::Type::PIXEL;
        Unit::Type grid_cell_width_unit =    Unit::Type::PIXEL;
        Unit::Type grid_cell_height_unit =   Unit::Type::PIXEL;

        uint8_t grid_row_max = 0;
        uint8_t grid_column_max = 0;
        uint8_t grid_row_start = 0;
        uint8_t grid_column_start = 0; 
        uint8_t grid_row_end = 0;
        uint8_t grid_column_end = 0; 

        Flow::Alignment flow_vertical_alignment = Flow::Alignment::START;
        Flow::Alignment flow_horizontal_alignment = Flow::Alignment::START;
        //PIXEL VALUES
        uint8_t corner_radius = 0; //255 sets to circle
        uint8_t border_width = 0;  
        Spacing padding;
        Spacing margin;
        Layout layout = Layout::FLOW;
    private:
        //Values that can potentially use bit array
        Positioning positioning = Positioning::RELATIVE;
        Flow::Axis flow_axis = Flow::Axis::HORIZONTAL;
        bool scissor = false;
        bool detach = false;
    public:
        void SetPositioning(Positioning p);
        void SetFlowAxis(Flow::Axis axis);
        void SetScissor(bool flag);
        void SetDetached(bool flag);
        Layout GetLayout() const;
        Flow::Axis GetFlowAxis() const;
        Positioning GetPositioning() const;
        bool IsScissor() const;
        bool IsDetached() const;
        int GetBoxExpansionWidth() const;
        int GetBoxExpansionHeight() const;
        int GetBoxModelWidth() const;
        int GetBoxModelHeight() const;
        int GetRenderingWidth() const;
        int GetRenderingHeight() const;

    };
    template<typename T>
    struct TreeNode
    {
        T val;
        ArenaLL<TreeNode> children;
    };

    //Error handling
    #define ERROR_MSG_SIZE 128
    struct Error
    {
        enum class Type : unsigned char
        {
            NO_ERROR,
            INCORRENT_UNIT_TYPE,
            NODE_CONTRADICTION,
            LEAF_NODE_CONTRADICTION,
            ROOT_NODE_CONTRADICTION,
            MISSING_END,
            MISSING_BEGIN,
            TEXT_NODE_CONTRADICTION,
            TEXT_UNKOWN_ESCAPE_CODE
        };
        Type type = Type::NO_ERROR;
        char msg[ERROR_MSG_SIZE]{};
        inline static int node_number = 0;
    };
    void DisplayError(const Error& error);
    Error CheckUnitErrors(const BoxStyle& style);
    Error CheckLeafNodeContradictions(const Box& leaf);
    Error CheckRootNodeConflicts(const BoxStyle& root);
    Error CheckNodeContradictions(const Box& child, const Box& parent);

    //Math helpers
    struct Rect
    {
        static bool Overlap(const Rect& r1, const Rect& r2);
        static bool Contains(const Rect& r ,int x, int y);
        static Rect Intersection(const Rect& r1, const Rect& r2);

        int x = 0;
        int y = 0;
        int width = 0;
        int height = 0;
    };
    float MillimeterToPixels(float mm);
    float CentimeterToPixels(float cm);
    float InchToPixels(float inches);

    //Used during tree descending
    int FixedUnitToPx(Unit unit, int root_size);
    Box ComputeStyleSheet(const BoxStyle& style, const Box& root);


    //UserInput
    //Returns 0 if str is nullptr. Otherwise it will never return 0
    uint64_t Hash(const char* str);
    void StringCopy(char* dst, const char* src, uint32_t size);
    bool StringCompare(const char* s1, const char* s2);
    char ToLower(char c);

    //Does not count '\0'
    int StringLength(const char* text);
    uint32_t StrToU32(const char* text, bool* error = nullptr);
    uint32_t HexToU32(const char* text, bool* error = nullptr);
    Color HexToRGBA(const char* text, bool* error = nullptr);


    //Stack allocated string for convenience
    template<uint32_t CAPACITY>
    class FixedString
    {
        uint32_t size = 0;
        char data[CAPACITY + 1]{};
    public:
        const char* Data() const;
        uint32_t Size() const; 
        bool IsEmpty() const;
        bool IsFull() const;
        bool Push(char c);
        bool Pop();
        void Clear();
        char& operator[](uint32_t index);
    };

    //Custom markdown language
    #define FLUSH_CAP 512
    class Markdown
    {
        struct Attributes
        {
            //All the variables the markdown can change
            int font_size = 32;
            int line_spacing = 0;
            int font_spacing = 0;
            Color font_color = {255, 255, 255, 255};
        };
    public:
        void SetInput(const char* source, int max_width, int max_height);
        bool ComputeNextTextRun();
        bool Done();
        int GetMeasuredWidth() const;
        int GetMeasuredHeight() const;
        TextPrimitive GetTextPrimitive(int x, int y);
    private:
        void HandleWrap();
        void PushAndMeasureChar(char c);

        //Go to this function to add more markdown features
        bool ComputeEscapeCode();

        void ClearBuffers();

        const char* source = nullptr;
        TextPrimitive p;
        Attributes state;
        int cursor_x = 0;
        int cursor_y = 0;
        int max_width = INT_MAX;
        int max_height = INT_MAX;
        int measured_width = 0;
        bool escape = false;
        FixedString<128> code;
        FixedString<FLUSH_CAP> text;
    };
    //Draws text based on custom markup
    void DrawTextNode(const char* text, int max_width, int max_height, int x, int y);

    //Computing PARENT_PERCENT
    int ParentPercentToPx(int value , Unit::Type unit_type, int parent_width);
    void ComputeParentWidthPercent(Box& box, int parent_width);
    void ComputeParentHeightPercent(Box& box, int parent_width);

    class Context
    {
    public:
        Context(uint64_t arena_bytes);
        BoxInfo GetBoxInfo(const char* label);
        void BeginRoot(unsigned int screen_width, unsigned int screen_height, int mouse_x, int mouse_y);
        void EndRoot();
        void BeginBox(const UI::BoxStyle& box_style, const char* label = nullptr);
        void InsertText(const char* text, bool copy_text = true);
        void EndBox();
        void Draw();
    private:
        bool HasInternalError();
        //Returns false and does nothing if no error
        //Returns true, sets internal error, and displays error if true
        bool HandleInternalError(const Error& error);
        //Width
        void WidthPass(TreeNode<Box>* node);
        void WidthPass_Flow(ArenaLL<TreeNode<Box>>::Node* child, const Box& parent_box); //Recurse Helper
        //Height
        void HeightContentPercentPass_Flow(TreeNode<Box>* node);
        void HeightContentPercentPass(TreeNode<Box>* node);

        void HeightPass(TreeNode<Box>* node);
        void HeightPass_Flow(ArenaLL<TreeNode<Box>>::Node* child, const Box& parent_box); //Recurse Helper

        //Computes position and draws.
        void DrawPass_FlowNoWrap(ArenaLL<TreeNode<Box>>::Node* child, const Box& parent_box, int x, int y, Rect parent_aabb);
        void DrawPass(TreeNode<Box>* node, int x, int y, const Box& parent_box, Rect parent_aabb);
    private:
        uint64_t directly_hovered_element_key = 0;
        Error internal_error;
        int mouse_x = 0, mouse_y = 0;
        MemoryArena arena;
        DoubleBufferMap<BoxInfo> double_buffer_map;
        ArenaLL<TreeNode<Box>*> deferred_elements;
        TreeNode<Box>* root_node = nullptr;
        FixedStack<TreeNode<Box>*, 100> stack; //elements should never nest over 100 layers deep
    };
}