#pragma once
/*
    The library useses the css box model, but border-wdith does not extend the box. It is only effects rendering
*/


//it is recommended to double the memory when debug is enabled
#define UI_ENABLE_DEBUG 1

#if UI_ENABLE_DEBUG
    #if __cplusplus >= 202002L
        #include <source_location>
        #define UI_DEBUG(name) {name, std::source_location::current().file_name(), (int)std::source_location::current().line()}
    #else
        #define UI_DEBUG(name) {name, __FILE__, __LINE__} //This does not work
    #endif
#else
    #define UI_DEBUG(name) {name, nullptr, -1}
#endif

//Only used for Fmt
#include <stdarg.h>
#include <stdio.h>

#include <iostream>
#include "Memory.hpp"


namespace UI
{
    class Context;
    class Builder;
    class DebugInspector;
    struct Error;
    struct BoxStyle;
    struct Grid;
    struct Flow;
    struct Color;
    struct HexColor;
    struct Unit;
    struct Spacing;
    struct BoxInfo;
    struct Rect;
}
namespace UI::Internal
{
    struct BoxInternal;
    struct TreeNode;
}

namespace UI
{
    enum Key 
    {
        KEY_NULL            = 0,        // Key: NULL, used for no key pressed
        KEY_SPACE           = 32,       // Key: Space
        KEY_APOSTROPHE      = 39,       // Key: '
        KEY_COMMA           = 44,       // Key: ,
        KEY_MINUS           = 45,       // Key: -
        KEY_PERIOD          = 46,       // Key: .
        KEY_SLASH           = 47,       // Key: /
        KEY_ZERO            = 48,       // Key: 0
        KEY_ONE             = 49,       // Key: 1
        KEY_TWO             = 50,       // Key: 2
        KEY_THREE           = 51,       // Key: 3
        KEY_FOUR            = 52,       // Key: 4
        KEY_FIVE            = 53,       // Key: 5
        KEY_SIX             = 54,       // Key: 6
        KEY_SEVEN           = 55,       // Key: 7
        KEY_EIGHT           = 56,       // Key: 8
        KEY_NINE            = 57,       // Key: 9
        KEY_SEMICOLON       = 59,       // Key: ;
        KEY_EQUAL           = 61,       // Key: =
        KEY_A               = 65,       // Key: A | a
        KEY_B               = 66,       // Key: B | b
        KEY_C               = 67,       // Key: C | c
        KEY_D               = 68,       // Key: D | d
        KEY_E               = 69,       // Key: E | e
        KEY_F               = 70,       // Key: F | f
        KEY_G               = 71,       // Key: G | g
        KEY_H               = 72,       // Key: H | h
        KEY_I               = 73,       // Key: I | i
        KEY_J               = 74,       // Key: J | j
        KEY_K               = 75,       // Key: K | k
        KEY_L               = 76,       // Key: L | l
        KEY_M               = 77,       // Key: M | m
        KEY_N               = 78,       // Key: N | n
        KEY_O               = 79,       // Key: O | o
        KEY_P               = 80,       // Key: P | p
        KEY_Q               = 81,       // Key: Q | q
        KEY_R               = 82,       // Key: R | r
        KEY_S               = 83,       // Key: S | s
        KEY_T               = 84,       // Key: T | t
        KEY_U               = 85,       // Key: U | u
        KEY_V               = 86,       // Key: V | v
        KEY_W               = 87,       // Key: W | w
        KEY_X               = 88,       // Key: X | x
        KEY_Y               = 89,       // Key: Y | y
        KEY_Z               = 90,       // Key: Z | z
        KEY_LEFT_BRACKET    = 91,       // Key: [
        KEY_BACKSLASH       = 92,       // Key: '\'
        KEY_RIGHT_BRACKET   = 93,       // Key: ]
        KEY_GRAVE           = 96,       // Key: `
        KEY_ESCAPE          = 256,      // Key: Esc
        KEY_ENTER           = 257,      // Key: Enter
        KEY_TAB             = 258,      // Key: Tab
        KEY_BACKSPACE       = 259,      // Key: Backspace
        KEY_INSERT          = 260,      // Key: Ins
        KEY_DELETE          = 261,      // Key: Del
        KEY_RIGHT           = 262,      // Key: Cursor right
        KEY_LEFT            = 263,      // Key: Cursor left
        KEY_DOWN            = 264,      // Key: Cursor down
        KEY_UP              = 265,      // Key: Cursor up
        KEY_PAGE_UP         = 266,      // Key: Page up
        KEY_PAGE_DOWN       = 267,      // Key: Page down
        KEY_HOME            = 268,      // Key: Home
        KEY_END             = 269,      // Key: End
        KEY_CAPS_LOCK       = 280,      // Key: Caps lock
        KEY_SCROLL_LOCK     = 281,      // Key: Scroll down
        KEY_NUM_LOCK        = 282,      // Key: Num lock
        KEY_PRINT_SCREEN    = 283,      // Key: Print screen
        KEY_PAUSE           = 284,      // Key: Pause
        KEY_F1              = 290,      // Key: F1
        KEY_F2              = 291,      // Key: F2
        KEY_F3              = 292,      // Key: F3
        KEY_F4              = 293,      // Key: F4
        KEY_F5              = 294,      // Key: F5
        KEY_F6              = 295,      // Key: F6
        KEY_F7              = 296,      // Key: F7
        KEY_F8              = 297,      // Key: F8
        KEY_F9              = 298,      // Key: F9
        KEY_F10             = 299,      // Key: F10
        KEY_F11             = 300,      // Key: F11
        KEY_F12             = 301,      // Key: F12
        KEY_LEFT_SHIFT      = 340,      // Key: Shift left
        KEY_LEFT_CONTROL    = 341,      // Key: Control left
        KEY_LEFT_ALT        = 342,      // Key: Alt left
        KEY_LEFT_SUPER      = 343,      // Key: Super left
        KEY_RIGHT_SHIFT     = 344,      // Key: Shift right
        KEY_RIGHT_CONTROL   = 345,      // Key: Control right
        KEY_RIGHT_ALT       = 346,      // Key: Alt right
        KEY_RIGHT_SUPER     = 347,      // Key: Super right
        KEY_KB_MENU         = 348,      // Key: KB menu
        KEY_KP_0            = 320,      // Key: Keypad 0
        KEY_KP_1            = 321,      // Key: Keypad 1
        KEY_KP_2            = 322,      // Key: Keypad 2
        KEY_KP_3            = 323,      // Key: Keypad 3
        KEY_KP_4            = 324,      // Key: Keypad 4
        KEY_KP_5            = 325,      // Key: Keypad 5
        KEY_KP_6            = 326,      // Key: Keypad 6
        KEY_KP_7            = 327,      // Key: Keypad 7
        KEY_KP_8            = 328,      // Key: Keypad 8
        KEY_KP_9            = 329,      // Key: Keypad 9
        KEY_KP_DECIMAL      = 330,      // Key: Keypad .
        KEY_KP_DIVIDE       = 331,      // Key: Keypad /
        KEY_KP_MULTIPLY     = 332,      // Key: Keypad *
        KEY_KP_SUBTRACT     = 333,      // Key: Keypad -
        KEY_KP_ADD          = 334,      // Key: Keypad +
        KEY_KP_ENTER        = 335,      // Key: Keypad Enter
        KEY_KP_EQUAL        = 336,      // Key: Keypad =
        KEY_BACK            = 4,        // Key: Android back button
        KEY_MENU            = 5,        // Key: Android menu button
    };
    enum MouseButton : unsigned char
    {
        MOUSE_LEFT    = 0,       // Mouse button left
        MOUSE_RIGHT   = 1,       // Mouse button right
        MOUSE_MIDDLE  = 2,       // Mouse button middle (pressed wheel)
        MOUSE_SIDE    = 3,       // Mouse button side (advanced mouse device)
        MOUSE_EXTRA   = 4,       // Mouse button extra (advanced mouse device)
        MOUSE_FORWARD = 5,       // Mouse button forward (advanced mouse device)
        MOUSE_BACK    = 6,       // Mouse button back (advanced mouse device)
    };

    constexpr uint64_t KB = 1024;
    constexpr uint64_t MB = KB * KB;
    //String Helper
    const char *Fmt(const char *text, ...);

    //Math helpers
    constexpr float DPI = 96.0f;
    inline int MmToPx(float mm) { return int(mm * DPI / 25.4f); } 
    inline int CmToPx(float cm) { return int(cm * DPI / 2.54f); }
    inline int InchToPx(float inches) { return int(inches * DPI); }
    template<typename T>
    inline T Min(T a, T b) {return a < b? a: b;}
    template<typename T>
    inline T Max(T a, T b) {return a > b? a: b;}
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
    uint64_t Hash(const char* str);

    //All measurements are based on this Unit
    struct Unit
    {
        enum Type : unsigned char 
        {

            PIXEL,
            //Limited to width/height
            PARENT_PERCENT,
            ROOT_PERCENT,
            CONTENT_PERCENT,   
            AVAILABLE_PERCENT,

            WIDTH_PERCENT, //only be applied to height
        };

        int value = 0;
        Unit::Type unit = Type::PIXEL;
    };
    enum class Layout: unsigned char { FLOW, GRID };
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

    struct HexColor 
    { 
        char text[9] = "FFFFFFFF";
    };

    constexpr HexColor RGBAToHex(Color color)
    {
        auto U4ToHexDigit = [](uint8_t n) constexpr -> char
        {
            if(n <= 9)
                return n + '0';
            else if(n >= 10 && n <= 15)
                return (n - 10) + 'a';
            return '0';
        };
        HexColor result;
        result.text[0] = U4ToHexDigit((color.r >> 4) & 0xF);
        result.text[1] = U4ToHexDigit(color.r & 0xF);
        result.text[2] = U4ToHexDigit((color.g >> 4) & 0xF);
        result.text[3] = U4ToHexDigit(color.g & 0xF);
        result.text[4] = U4ToHexDigit((color.b >> 4) & 0xF);
        result.text[5] = U4ToHexDigit(color.b & 0xF);
        result.text[6] = U4ToHexDigit((color.a >> 4) & 0xF);
        result.text[7] = U4ToHexDigit(color.a & 0xF);
        result.text[8] = '\0';
        return result;
    }

    struct Spacing { unsigned char left = 0, right = 0, top = 0, bottom = 0; };

    enum class Detach : unsigned char
    {
        NONE,
        ABSOLUTE,
        RELATIVE,
        LEFT,
        RIGHT,
        TOP,
        BOTTOM,
        LEFT_CENTER,
        RIGHT_CENTER,
        TOP_CENTER,
        BOTTOM_CENTER,
        LEFT_END,
        RIGHT_END,
        TOP_END,
        BOTTOM_END
    };
    // ========== Box Styling ========== 
    struct BoxStyle
    {
        //container
        Layout layout = Layout::FLOW;
        Flow flow;
        Grid grid;

        int x = 0;
        int y = 0;

        Unit width =    Unit{50, Unit::Type::PIXEL};
        Unit height =   Unit{50, Unit::Type::PIXEL};

        Unit min_width;
        Unit max_width = Unit{9999, Unit::Type::PIXEL};
        Unit min_height;
        Unit max_height = Unit{9999, Unit::Type::PIXEL};

        Spacing padding;
        Spacing margin;

        Color background_color = UI::Color{0, 0, 0, 0};
        Color border_color = UI::Color{0, 0, 0, 0};

        int gap_row = 0;
        int gap_column = 0;

        int scroll_x = 0;
        int scroll_y = 0;

        //PIXEL VALUES
        unsigned char corner_radius = 0; //255 sets to circle
        unsigned char border_width = 0;  

        //Potentially performance heavy
        bool scissor = false;
        Detach detach = Detach::NONE;
    };

    struct Vertex
    {
        float x, y, r, g, b, a; 
    };

    struct BoxInfo
    {
        uint64_t key = 0;
        int x = 0;
        int y = 0;
        int width = 0;
        int height = 0;
        Spacing padding;
        Spacing margin;

        int content_width =     0;
        int content_height =    0;
        bool valid =            false; // mainly used when you want to verify sizings as they are default to 0
        bool is_hover =         false;
        bool is_direct_hover =  false;
        bool is_rendered =      false;
        bool IsValid() const { return key != 0; }
        bool IsDirectHover() const {return is_direct_hover; }
        bool IsHover() const {return is_hover; }
        bool IsRendered() const { return is_rendered; }
        int DrawX() const { return x + margin.left; }
        int DrawY() const { return y + margin.top; }
        int DrawWidth() const { return width + padding.left + padding.right; }
        int DrawHeight() const { return height + padding.top + padding.bottom; }
        int MaxScrollX() const { return Max(0, content_width - width);}
        int MaxScrollY() const { return Max(0, content_height - height);}
        uint64_t GetKey() const { return key; }
    };

    struct Vec2
    {
        float x = 0;
        float y = 0;
    };
    struct Triangle
    {
        Vec2 position;
    };
    struct DebugInfo
    {
        const char* name = nullptr;
        const char* file = nullptr;
        int line = -1;
    };

    // ========== Main Functions ==========
    BoxInfo Info(const char* label);
    Context* GetContext();
    bool IsContextActive();
    void BeginRoot(Context* context, const BoxStyle& style, DebugInfo debug_info = UI_DEBUG("Root"));
    void EndRoot();
    void BeginBox(const BoxStyle& box_style, const char* label = nullptr, DebugInfo debug_info = UI_DEBUG("Box"));
    void EndBox();
    void InsertText(const char* text, const char* label = nullptr, bool copy_text = true, DebugInfo debug_info = UI_DEBUG("Text"));
    void Draw();
    // ====================================

    // ========== Builder Notation ==========
    template<typename Func>
    void Root(Context* context, const BoxStyle& style, Func&& func, DebugInfo debug_info = UI_DEBUG("Root"));
    Builder& Text(const char* text, const char* id = nullptr, bool should_copy = true, DebugInfo debug_info = UI_DEBUG("Text"));
    Builder& Box(const char* id = nullptr, DebugInfo debug_info = UI_DEBUG("Box"));
    BoxInfo Info();
    BoxStyle& Style();
    bool IsHover();
    bool IsDirectHover();
    // ======================================



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


    //Backend function to implement
    void Init_impl(const char* font_path);
    void DrawRectangle_impl(float x, float y, float width, float height, float corner_radius, float border_size, Color border_color, Color background_color);
    void DrawText_impl(TextPrimitive draw_command);
    int MeasureChar_impl(char c, int font_size, int spacing);
    void BeginScissorMode_impl(float x, float y, float width, float height);
    void EndScissorMode_impl();

    //Input
    //These don't need to be implementd, but are great for backend agnostic widgets
    bool IsKeyPressed(Key key);
    bool IsKeyReleased(Key key);
    bool IsKeyDown(Key key);
    bool IsKeyRepeat(Key key);
    char GetPressedChar();
    //Mouse
    bool IsMousePressed(MouseButton button);
    bool IsMouseReleased(MouseButton button);
    bool IsMouseDown(MouseButton button);
    int GetMouseScroll();
    int GetMouseX();
    int GetMouseY();

    //These are actually only for the inspector
    int GetScreenWidth();
    int GetScreenHeight();



}


//Internal
namespace UI
{
    //Internal namespace is just used for seperation in public api
    namespace Internal
    {
        struct BoxInternal
        {
            // ========= Only used when debugging is enabled
            #if UI_ENABLE_DEBUG
                DebugInfo debug_info;
            #endif
            // =============================================
            uint64_t label_hash =       0; 
            const char* text = nullptr;

            Color background_color = UI::Color{0, 0, 0, 0}; //used for debugging
            Color border_color = UI::Color{0, 0, 0, 0};
            //By the end of UI::Draw(), all final measurements are placed back into box
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
            uint8_t corner_radius = 0; //255 sets to circle
            uint8_t border_width = 0;  
            Spacing padding;
            Spacing margin;
            Layout layout = Layout::FLOW;
            Detach detach = Detach::NONE;
        private:
            //Values that can potentially use bit array
            Flow::Axis flow_axis = Flow::Axis::HORIZONTAL;
            bool scissor = false;
        public:
            void SetFlowAxis(Flow::Axis axis);
            void SetScissor(bool flag);
            Layout GetLayout() const;
            Flow::Axis GetFlowAxis() const;
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
            BoxInternal box;
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
            BoxInfo Info(const char* label);
            BoxInfo Info(uint64_t key);
            void SetMousePos(int x, int y);
            void BeginRoot(BoxStyle style, DebugInfo debug_info = UI_DEBUG("Root"));
            void EndRoot();
            void BeginBox(const UI::BoxStyle& box_style, const char* label = nullptr, DebugInfo debug_info = UI_DEBUG("Box"));
            void InsertText(const char* text, const char* label = nullptr, bool copy_text = true, DebugInfo info = UI_DEBUG("Text"));
            void EndBox();
            void Draw();
            uint32_t GetElementCount() const;

            void SetInspector(bool activate_pressed, DebugInspector* inspector_context); 
            //For Advanced Purposes

            //Might not even use this
            void ResetAllStates();
        private:
            void ClearPreviousFrame();
            //These functions are for internals only
        using Box = Internal::BoxInternal;
        using TreeNode = Internal::TreeNode;
        bool HasInternalError();
        //Returns false and does nothing if no error
        //Returns true, sets internal error, and displays error if true
        bool HandleInternalError(const Error& error);

        // ========== Layout ===============

        //Width
        void WidthContentPercentPass_Flow(TreeNode* node);
        void WidthContentPercentPass(TreeNode* node);
        void WidthPass(TreeNode* node);
        void WidthPass_Flow(Internal::ArenaLL<TreeNode>::Node* child, const Box& parent_box); //Recurse Helper
        //Height
        void HeightContentPercentPass_Flow(TreeNode* node);
        void HeightContentPercentPass(TreeNode* node);
        void HeightPass(TreeNode* node);
        void HeightPass_Flow(Internal::ArenaLL<TreeNode>::Node* child, const Box& parent_box); //Recurse Helper

        //Computes position and draws.
        void PositionPass_FlowNoWrap(Internal::ArenaLL<TreeNode>::Node* child, const Box& parent_box);
        void PositionPass(TreeNode* node, const Box& parent_box);
        // ================================
        void DrawPass(TreeNode* node, const Box& parent_box, Rect parent_aabb);

    private:

        DebugInspector* debug_inspector = nullptr;
        bool is_inspecting = false;
        bool copy_tree = false;

        uint64_t directly_hovered_element_key = 0;
        Error internal_error;
        Internal::MemoryArena arena;
        Internal::ArenaDoubleBufferMap<BoxInfo> double_buffer_map;
        Internal::ArenaLL<TreeNode*> deferred_elements;
        TreeNode* root_node = nullptr;
        Internal::FixedStack<TreeNode*, 64> stack; //elements should never nest over 100 layers deep

        uint32_t element_count = 0;
    };

    struct DebugBox
    {
        BoxStyle    style;
        DebugInfo   debug_info;
        const char* label = nullptr;
        const char* text = nullptr;
        Rect dim;
        bool        is_rendered = false;
        bool        is_open = false;
    };
    class DebugInspector
    {
    private:
        friend class Context;
        struct TreeNodeDebug
        {
            Internal::ArenaLL<TreeNodeDebug> children;
            DebugBox box;
        };
    public:
        struct Theme
        {
            Color base_color;
            Color left_panel_color;
            Color right_panel_color;
            Color button_color;
            Color button_color_hover;
            Color info_box_color;
            Color invalid_button;
            Color title_bar_color;
            HexColor text_color;
            HexColor text_color_hover;
            HexColor string_color;
            HexColor id_text_color;
            HexColor info_text_color;
            Spacing base_padding;
            uint8_t base_corner_radius;
            uint8_t icon_corner_radius;
            uint8_t button_corner_radius;
        };
        constexpr static Theme light_theme = 
        {
            {253, 253, 253, 50}, //base_color
            {253, 253, 253, 255}, //left_panel_color
            {38, 38, 38, 255},  //right_panel_color;
            {253, 253, 253, 255},   //button_color;
            {38, 38, 38, 255},  //button_color_hover;
            {59, 59, 59, 255}, //info_box_color
            {255, 230, 230, 255}, //invalid_button
            {200, 200, 200, 255}, //title_bar_color;
            RGBAToHex({38, 38, 38, 255}),//text_color;
            RGBAToHex({253, 253, 253, 255}),//text_color_hover;
            RGBAToHex({70, 188, 70, 255}), //string_color;
            RGBAToHex({220, 173, 48, 255}), //id_text_color;
            RGBAToHex({200, 200, 200, 255}), //info_text_color
            {10, 10, 10, 10}, //base_padding
            16, //base_corner_radius;
            3,  //icon_corner_radius;
            8,  //button_corner_radius;
        };

        DebugInspector(uint64_t memory);
        Theme theme = light_theme;
        Context* GetContext();
    private:

        const char* CopyStringToArena(const char* str);
        void Reset();
        //Only used for copying ui tree
        void PushNode(const DebugBox& box);
        void PopNode();
        void RunDebugInspector(int x, int y, int screen_width, int screen_height, int mouse_x, int mouse_y);

        bool IsTreeEmpty();

        void ConstructMockUI(TreeNodeDebug* node);
        void ConstructInspector(int mouse_x, int mouse_y);
        void ConstructEditor();
        void ConstructTree(TreeNodeDebug* node, int depth);
        bool SearchNodeAndOpenTree(TreeNodeDebug* node);

        //Widgets
        void CustomComboList(const char * id, int& selected, const char** options, uint64_t valid);
        void CustomDigitInput(const char* id, int& value);
        void UnitEditBox(const char* name, Unit& unit, uint64_t valid);



        Internal::MemoryArena arena; //only public to copy labels and strings. I dont feel like making a getter function

        // ===== Internal Tree =====
        TreeNodeDebug* root_node = nullptr;
        TreeNodeDebug* selected_node = nullptr;
        Internal::FixedStack<TreeNodeDebug*, 64> stack;
        // =========================

        // ===== UI state =====
        Context ui_context;
        Rect hovered_element;
        Rect window_dim = {10, 10, 400, 300};
        bool window_pos_drag = false;
        bool window_size_drag = false;
        bool panel_drag = false;
        int panel_width = 200;
        int left_panel_scroll = 0;
        int right_panel_scroll = 0;
        // ===== BoxStyle states =====


        // ====================

        // ===== User Input =====
        int mouse_scroll = 0;
        int mouse_drag_x = 0;
        int mouse_drag_y = 0;
        bool mouse_pressed = false;
        bool mouse_released = false;
        // ======================
    };

    //Builder Notation
    class Builder 
    {
    public: 
        void SetContext(Context* context);


        //Also Implemented as global functions
        Builder& Text(const char* text, const char* id = nullptr, bool should_copy = true, DebugInfo debug_info = UI_DEBUG("Text"));
        Builder& Box(const char* id = nullptr, DebugInfo debug_info = UI_DEBUG("Box"));
        BoxInfo Info() const;
        BoxStyle& Style();
        bool IsHover() const;
        bool IsDirectHover() const;

        //Parmeters
        Builder& Style(const BoxStyle& style);
        template<typename Func>
        Builder& OnHover(Func&& func);
        template<typename Func>
        Builder& OnDirectHover(Func&& func);
        template<typename Func>
        Builder& PreRun(Func&& func);


        //Executes begin/end
        void Run();
        void InsertText(const char* text, bool should_copy = true, DebugInfo debug_info = UI_DEBUG("Text"));

        //Executes begin/end with lambda
        template<typename Func>
        void Run(Func&& func);
    private:
        bool HasContext() const;
        void ClearStates();
        Context* context = nullptr;

        //States
        const char* id = nullptr;
        const char* text = nullptr;
        BoxInfo info;
        BoxStyle style;
        DebugInfo debug_info;
        bool should_copy = true;
    };

}


//Builder Implementation
namespace UI
{
    template<typename Func>
    inline void Root(Context* context, const BoxStyle& style, Func&& func, DebugInfo debug_info)
    {
        UI::BeginRoot(context, style, debug_info);
        func();
        UI::EndRoot();
    }


    inline void Builder::SetContext(Context* context)
    {
        this->context = context;
    }
    inline bool Builder::HasContext() const
    {
        return context != nullptr;
    }
    inline void Builder::ClearStates()
    {
        id = nullptr;
        text = nullptr;
        info = BoxInfo();
        style = BoxStyle();
        debug_info = DebugInfo();
        should_copy = true;
    }
    inline BoxInfo Builder::Info() const
    {
        return info;
    }
    inline bool Builder::IsHover() const 
    {
        return info.IsHover();
    }
    inline bool Builder::IsDirectHover() const
    {
        return info.IsDirectHover();
    }
    inline BoxStyle& Builder::Style()
    {
        return style;
    }
    inline Builder& Builder::Style(const BoxStyle& style)
    {
        this->style = style;
        return *this;
    }
    template<typename Func>
    Builder& Builder::OnHover(Func&& func)
    {
        if(info.IsValid() && info.IsHover()) 
        {
            func();
        }
        return *this;
    }
    template<typename Func>
    Builder& Builder::OnDirectHover(Func&& func)
    {
        if(info.IsValid() && info.IsDirectHover()) 
        {
            func();
        }
        return *this;
    }
    template<typename Func>
    Builder& Builder::PreRun(Func&& func)
    {
        func();
        return *this;
    }
    inline void Builder::Run()
    {
        if(HasContext())
        {
            if(text)
            {
                context->InsertText(text, id, should_copy, debug_info);
            }
            else
            {
                context->BeginBox(style, id, debug_info);
                context->EndBox();
            }
        }
    }
    template<typename Func>
    void Builder::Run(Func&& func)
    {
        if(HasContext())
        {
            if(text)
            {
                context->InsertText(text, id, should_copy, debug_info);
            }
            else
            {
                context->BeginBox(style, id, debug_info);
                func();
                context->EndBox();
            }
        }
    }
    inline void Builder::InsertText(const char* text, bool should_copy, DebugInfo debug_info)
    {
        if(context)
            context->InsertText(text, nullptr, should_copy, debug_info);
    }
}