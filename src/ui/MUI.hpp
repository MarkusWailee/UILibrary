#pragma once
/*
    The library useses the css box model, but border-wdith does not extend the box. It is only effects rendering
*/


//it is recommended to double the memory when debug is enabled
#define UI_ENABLE_DEBUG 1

#if UI_ENABLE_DEBUG
    #if __cplusplus >= 202002L
        #include <source_location>
        #define UI_DEBUG {(int)std::source_location::current().line(), std::source_location::current().file_name()}
    #else
        #define UI_DEBUG {__LINE__, __FILE__}
    #endif
#else
    #define UI_DEBUG {-1, nullptr}
#endif

//Only used for StringFormat
#include <stdarg.h>
#include <stdio.h>


namespace UI
{
    class Context;
    struct Error;
    struct BoxStyle;
    struct Grid;
    struct Flow;
    struct Color;
    struct Unit;
    struct Spacing;
    struct BoxInfo;
    struct Rect;
}
namespace UI::Internal
{
    struct Box;
    struct TreeNode;
}

namespace UI
{

    //String Helper
    const char *StringFormat(const char *text, ...);

    //Math helpers
    float MillimeterToPixels();
    float CentimeterToPixels(float cm);
    float InchToPixels(float inches);
    template<typename T>
    inline T Min(T a, T b) {return a < b? a: b;}
    template<typename T>
    inline T Max(T a, T b) {return a >= b? a: b;}
    template<typename T>
    inline T Clamp(T value, T minimum, T maximum) { return Max(Min(value, maximum), minimum); }

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
    enum class Layout: unsigned char { FLOW, GRID };
    enum class Positioning: unsigned char { RELATIVE, ABSOLUTE };
    struct Flow
    {
        enum Axis : unsigned char {HORIZONTAL, VERTICAL};
        enum Alignment: unsigned char {START, END, CENTERED, SPACE_AROUND, SPACE_BETWEEN};
        Axis axis = Axis::HORIZONTAL;
        Alignment vertical_alignment = Alignment::START;
        Alignment horizontal_alignment = Alignment::START;
    };
    struct Grid
    {
        unsigned char row_max = 0,      column_max = 0;
        unsigned char row_start = 0,    column_start = 0; 
        unsigned char row_end = 0,      column_end = 0; 
        Unit cell_width;
        Unit cell_height;
    };
    struct Color { unsigned char r = 0, g = 0, b = 0, a = 0; };
    struct Spacing { unsigned char left = 0, right = 0, top = 0, bottom = 0; };

    // ========== Main Box Styling ========== 
    struct BoxStyle
    {
        //container
        Positioning positioning = Positioning::RELATIVE;
        Layout layout = Layout::FLOW;
        Flow flow;
        Grid grid;

        int gap_row = 0;
        int gap_column = 0;

        Unit width =    Unit{50, Unit::Type::PIXEL};
        Unit height =   Unit{50, Unit::Type::PIXEL};

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
    struct DebugInfo
    {
        int line = -1;
        const char* file = nullptr;
    };

    // ========== Main Functions ==========
    BoxInfo GetBoxInfo(const char* label);
    void SetContext(UI::Context* context);
    void BeginRoot(unsigned int screen_width, unsigned int screen_height, int mouse_x, int mouse_y);
    void EndRoot();
    void BeginBox(const BoxStyle& box_style, const char* label = nullptr, DebugInfo debug_info = UI_DEBUG);
    void InsertText(const char* text, bool copy_text = true);
    void EndBox();
    void Draw();


    struct TextPrimitive
    {
        const char* text = nullptr;
        int x = 0;
        int y = 0;

        // A cursor is used to determine where the next character is rendered.
        // This is simply used to initialize a cursor position so we can render where we left off.
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
    //Internal namespace is just used for seperation in public api
    namespace Internal
    {

        struct Box
        {
            // ========= Only used when debugging is enabled
            #if UI_ENABLE_DEBUG
                const char* debug_file = nullptr;
                int         debug_line = -1;
                BoxStyle    debug_style;
            #endif
            // =============================================

            //By the end of UI::Draw(), all final measurements are placed back into box
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
        struct TreeNode
        {
            Box box;
            ArenaLL<TreeNode> children;
        };
    }

    //Error handling
    #define ERROR_MSG_SIZE 512
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
    };




    class Context
    {
    public:
        Context(uint64_t arena_bytes);
        BoxInfo GetBoxInfo(const char* label);
        void BeginRoot(unsigned int screen_width, unsigned int screen_height, int mouse_x, int mouse_y);
        void EndRoot();
        void BeginBox(const UI::BoxStyle& box_style, const char* label = nullptr, DebugInfo debug_info = UI_DEBUG);
        void InsertText(const char* text, bool copy_text = true);
        void EndBox();
        void Draw();
        Internal::TreeNode* GetInternalTree();
    private:

        //These functions are for internals only
        using Box = Internal::Box;
        using TreeNode = Internal::TreeNode;
        bool HasInternalError();
        //Returns false and does nothing if no error
        //Returns true, sets internal error, and displays error if true
        bool HandleInternalError(const Error& error);
        //Width
        void WidthPass(TreeNode* node);
        void WidthPass_Flow(Internal::ArenaLL<TreeNode>::Node* child, const Box& parent_box); //Recurse Helper
        //Height
        void HeightContentPercentPass_Flow(TreeNode* node);
        void HeightContentPercentPass(TreeNode* node);

        void HeightPass(TreeNode* node);
        void HeightPass_Flow(Internal::ArenaLL<TreeNode>::Node* child, const Box& parent_box); //Recurse Helper

        //Computes position and draws.
        void DrawPass_FlowNoWrap(Internal::ArenaLL<TreeNode>::Node* child, const Box& parent_box, int x, int y, Rect parent_aabb);
        void DrawPass(TreeNode* node, int x, int y, const Box& parent_box, Rect parent_aabb);
    private:
        uint64_t directly_hovered_element_key = 0;
        Error internal_error;
        int mouse_x = 0, mouse_y = 0;
        Internal::MemoryArena arena;
        Internal::DoubleBufferMap<BoxInfo> double_buffer_map;
        Internal::ArenaLL<TreeNode*> deferred_elements;
        TreeNode* root_node = nullptr;
        Internal::FixedStack<TreeNode*, 100> stack; //elements should never nest over 100 layers deep

        #if UI_ENABLE_DEBUG
            Rect debug_hover;
        #endif
    };
}