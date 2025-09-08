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
//#include <stdio.h>
//#include <uchar.h>

#include <iostream>
#include "Memory.hpp"


namespace UI
{
    template<typename char_type> struct BaseString;
    using StringAsci = BaseString<const char>;
    using StringU8 = BaseString<const char8_t>; //UTF8 not supported right now
    using StringU32 = BaseString<const char32_t>;
    class Context;
    class Builder;
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
namespace UI
{
    namespace Internal
    {
        struct BoxCore;
        template<typename T>
        struct TreeNode;
    }
}

namespace UI
{
    enum Key //copied from glfw or raylib
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

    template<typename char_type>
    struct BaseString : public Internal::ArrayView<char_type>
    {
        BaseString() = default;
        BaseString(char_type* str, uint64_t size): Internal::ArrayView<char_type>{str, size} {}
        template<int N>
        constexpr BaseString(char_type (&str)[N]): BaseString(str, N > 0 ? N - 1: 0){}

        BaseString SubStr(int start, int size) const
        {
            if(size == -1)
                size = (int)this->Size() - start;
            assert(start >= 0 && start <= (int)this->Size() && size <= (int)this->Size() - start);
            return {this->data + start, (uint64_t)size};
        }
        char_type* Cstr(){return this->data; }
    };

    constexpr uint64_t KB = 1024;
    constexpr uint64_t MB = KB * KB;

    /*
       Uses regular printf specifiers
       %d => int
       %u => unsigned int
       %lld => 64bit signed
       %llu => 64bit unsigned
       %f => float
       %.2f => float x.xx
       %c => char
       %s => c-string
    */
    StringAsci Fmt(const char *text, ...);
    StringU32 AsciToStrU32(const StringAsci& str);
    //StringU32 FmtU32(const char *text, ...);

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
    template<typename T>
    inline T Mix(T a, T b, float amount) { return a + Clamp(amount, 0.0f, 1.0f) * (b - a); }

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

    struct Color { unsigned char r = 0, g = 0, b = 0, a = 0; };
    inline Color Mix(Color c1, Color c2, float amount)
    {
        return Color
        {
            Mix(c1.r, c2.r, amount),
            Mix(c1.g, c2.g, amount),
            Mix(c1.b, c2.b, amount),
            Mix(c1.a, c2.a, amount),
        };
    }

    // ========== Hashing functions ==========
    // ========================================

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
        // parent
        uint8_t row_count = 1;
        uint8_t column_count = 1;

        //child
        uint8_t x = 0;
        uint8_t y = 0;
        uint8_t span_x = 1;
        uint8_t span_y = 1;
    };

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
    struct TextureRect
    {
        void* texture =     nullptr;
        uint16_t x =        0;
        uint16_t y =        0;
        uint16_t width =    0;
        uint16_t height =   0;
        inline bool HasTexture() const { return texture; }
    };
    struct BoxStyle
    {
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

        Color color = UI::Color{0, 0, 0, 0};
        Color border_color = UI::Color{0, 0, 0, 0};

        TextureRect texture;

        int gap_row = 0;
        int gap_column = 0;

        int scroll_x = 0;
        int scroll_y = 0;

        unsigned char corner_radius = 0; //255 sets to circle
        unsigned char border_width = 0;

        //Potentially performance heavy
        bool scissor = false;
        Detach detach = Detach::NONE;
    };
    struct TextStyle
    {
        TextStyle& FontSize(int size);
        TextStyle& LineSpacing(int spacing);
        TextStyle& FgColor(const Color& color);
        TextStyle& BgColor(const Color& color);
        int GetFontSize() const;
        int GetFontSpacing() const;
        int GetLineSpacing() const;
        Color GetFgColor() const;
        Color GetBgColor() const;
        // bool operator==(const TextStyle& t) const;
    private:
        Color fg_color = {255, 255, 255, 255};
        Color bg_color;
        uint8_t font_size = 32;
        uint8_t font_spacing = 0;
        uint8_t line_spacing = 0;
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

    struct DebugInfo
    {
        const char* name = nullptr;
        const char* file = nullptr;
        int line = -1;
    };

    // ========== Main Functions ==========
    BoxInfo Info(const StringAsci& id);
    Context* GetContext();
    bool IsContextActive();
    void BeginRoot(Context* context, const BoxStyle& style, DebugInfo debug_info = UI_DEBUG("Root"));
    void EndRoot();
    void BeginBox(const BoxStyle& box_style, const StringAsci& id = StringAsci(), DebugInfo debug_info = UI_DEBUG("Box"));
    void EndBox();
    //void InsertText(const char16_t* text, const char* id = nullptr, bool copy_text = true, DebugInfo debug_info = UI_DEBUG("Text"));
    void Draw();
    // ====================================
    // ========== Builder Notation ==========
    template<typename Func>
    void Root(Context* context, const BoxStyle& style, Func&& func, DebugInfo debug_info = UI_DEBUG("Root"));

    // ===== Text Overloads ====
    void Text(const TextStyle& style, const StringU32& string, bool copy_text = true, DebugInfo debug_info = UI_DEBUG("Text"));
    inline void Text(const TextStyle& style, const StringAsci& string, DebugInfo debug_info = UI_DEBUG("Text"))
    {
        Text(style, AsciToStrU32(string), true, debug_info);
    }

    template<int N>
    inline void Text(const TextStyle& style, const char32_t(&str)[N], DebugInfo debug_info = UI_DEBUG("Text"))
    {
        Text(style, str, false, debug_info);
    }
    // =========================
    Builder& Box(const BoxStyle& style = BoxStyle(), const StringAsci& id = StringAsci(), DebugInfo debug_info = UI_DEBUG("Box"));
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
    void DrawTexturedRectangle_impl(int x, int y, int width, int height, const TextureRect& texture);
    void DrawText_impl(TextPrimitive draw_command);
    void DrawText_impl(TextStyle style, int x, int y, const char32_t* text, int size);
    int MeasureChar_impl(char32_t c, int font_size, int spacing);
    void BeginScissorMode_impl(float x, float y, float width, float height);
    void EndScissorMode_impl();

    //Input
    //These don't need to be implementd, but are great for backend agnostic widgets
    bool IsKeyPressed(Key key);
    bool IsKeyReleased(Key key);
    bool IsKeyDown(Key key);
    bool IsKeyRepeat(Key key);
    char GetPressedChar();
    ////Mouse
    bool IsMousePressed(MouseButton button);
    bool IsMouseReleased(MouseButton button);
    bool IsMouseDown(MouseButton button);
    float GetMouseScroll();
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

        struct TextSpan : public StringU32
        {
            TextStyle style;
        };
        struct TextSpans : public ArenaDLL<TextSpan>
        {
            /*
                This is only used in ComputeTextLines so I can
                iterate all characters easily for wrapping
            */
            struct Iterator
            {
                TextSpans::Node* node = nullptr;
                int string_index = 0;
                Iterator Next() const;
                Iterator Prev() const;
                char32_t GetChar() const;
                TextStyle GetStyle() const;
                bool IsValid() const;

                /*
                   End is inclusive,
                   End should be within the same node as Start or else
                   Start will return its entire string
                */
            };
            static StringU32 GetString(Iterator start, Iterator end);
            static TextSpan GetTextSpan(Iterator start, Iterator end);
            Iterator Begin();
            //The last character in the TextSpan list
            Iterator End();
        };

        //The same as a text span with positions
        struct TextLine : public TextSpan
        {
            int x = 0;
            int y = 0;
            int width = 0;
        };

        struct BoxCore
        {

            // ========= Only used when debugging is enabled
            #if UI_ENABLE_DEBUG
                DebugInfo debug_info;
            #endif
            // =============================================

            enum Type : unsigned char
            {
                BOX,
                IMAGE,
                TEXT,
                NONE,
            };

            //A doubly linked list of styled text spans
            TextSpans text_style_spans;

            TextureRect texture;
            uint64_t id_key =       0;

            Color background_color =    UI::Color{0, 0, 0, 0};
            Color border_color =        UI::Color{0, 0, 0, 0};


            int scroll_x =              0;
            int scroll_y =              0;
            uint16_t width =            0;
            uint16_t height =           0;
            uint16_t min_width =        0;
            uint16_t max_width =        UINT16_MAX;
            uint16_t min_height =       0;
            uint16_t max_height =       UINT16_MAX;
            uint16_t gap_row =          0;
            uint16_t gap_column =       0;
            int16_t x =                 0;
            int16_t y =                 0;

            /*
                By the end of UI::Draw(), final measurements are sent to BoxResult.
                These variables are the only things saved by layout computation.
                Porperties like color, texture, and positioning can be passed by reference
                straight through.
                Properteis like width/height are progressively resolved from layout algorithms
                at the end, width/height should hold their final measurements and BoxResult can then use
                GetRenderWidth() to store the final computation. See void SetComputedResults(BoxCore& node);
                to see what variable get passed down for caching into the next arena.
            */
            //Info sent to BoxResult
            ArenaDLL<TextLine> result_text_lines;
            int16_t result_rel_x = 0;
            int16_t result_rel_y = 0;
            uint16_t result_content_width = 0;
            uint16_t result_content_height = 0;
            //======================

            Unit::Type width_unit =              Unit::Type::PIXEL;
            Unit::Type height_unit =             Unit::Type::PIXEL;
            Unit::Type min_width_unit =          Unit::Type::PIXEL;
            Unit::Type max_width_unit =          Unit::Type::PIXEL;
            Unit::Type min_height_unit =         Unit::Type::PIXEL;
            Unit::Type max_height_unit =         Unit::Type::PIXEL;

            uint8_t grid_row_count =    0;
            uint8_t grid_column_count = 0;
            uint8_t grid_x =                    0;
            uint8_t grid_y =                    0;
            uint8_t grid_span_x =               1;
            uint8_t grid_span_y =               1;

            Flow::Alignment flow_vertical_alignment = Flow::Alignment::START;
            Flow::Alignment flow_horizontal_alignment = Flow::Alignment::START;
            uint8_t corner_radius = 0; //255 sets to circle
            uint8_t border_width = 0;
            Spacing padding;
            Spacing margin;
            Layout layout = Layout::FLOW;
            Detach detach = Detach::NONE;
            Type type = Type::BOX;
        private:
            //Values that can potentially use bit array
            Flow::Axis flow_axis = Flow::Axis::HORIZONTAL;
            bool scissor = false;
        public:
            Type GetElementType() const;
            void SetFlowAxis(Flow::Axis axis);
            void SetScissor(bool flag);
            Layout GetLayout() const;
            Flow::Axis GetFlowAxis() const;
            bool IsScissor() const;
            bool IsDetached() const;
            bool IsTextElement() const;
            int GetBoxExpansionWidth() const;
            int GetBoxExpansionHeight() const;
            int GetBoxModelWidth() const;
            int GetBoxModelHeight() const;
            int GetRenderingWidth() const;
            int GetRenderingHeight() const;
            int GetGridCellWidth() const;
            int GetGridCellHeight() const;
        };

        struct BoxResult
        {
            BoxCore* core = nullptr;
            ArenaDLL<TextLine> text_lines;
            int16_t rel_x = 0;
            int16_t rel_y = 0;
            uint16_t draw_width = 0;
            uint16_t draw_height = 0;
            uint16_t content_width = 0;
            uint16_t content_height = 0;
            void UpdatePointer(BoxCore& node);
            void SetComputedResults(BoxCore& node);
        };

        template<typename T>
        struct TreeNode
        {
            T val;
            ArenaLL<TreeNode> children;
        };
        template<>
        struct TreeNode<BoxCore>
        {
            BoxCore box;
            ArenaLL<TreeNode> children;
        };
        template<>
        struct TreeNode<BoxResult>
        {
            BoxResult box;
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
        using BoxCore = Internal::BoxCore;
        using BoxResult = Internal::BoxResult;
        template<typename T>
        using TreeNode = Internal::TreeNode<T>;
        template<typename T>
        using ArenaLL = Internal::ArenaLL<T>;
        using BoxType = Internal::BoxCore::Type;

        struct DeferredBox
        {
            TreeNode<BoxResult>* node = nullptr;
            int x = 0;
            int y = 0;
        };

    public:
        Context(uint64_t arena_bytes, uint64_t string_bytes);
        BoxInfo Info(const StringAsci& id);
        BoxInfo Info(uint64_t key);
        void BeginRoot(BoxStyle style, DebugInfo debug_info = UI_DEBUG("Root"));
        void EndRoot();
        void BeginBox(const UI::BoxStyle& style, const StringAsci& id, DebugInfo debug_info = UI_DEBUG("Box"));
        //void InsertText(const UI::TextStyle& style, const StringU8& string, const char* id = nullptr, bool copy_text = true, DebugInfo info = UI_DEBUG("Text"));
        void InsertText(const UI::TextStyle& style, const StringU32& string, const char* id = nullptr, bool copy_text = true, DebugInfo info = UI_DEBUG("Text"));
        void EndBox();
        void Draw();
        uint32_t GetElementCount() const;

        //For Advanced Purposes

        //Might not even use this
        void ResetAllStates();
    private:
        void ResetAtBeginRoot();
        void ResetArena1();
        void ResetArena2();
            //These functions are for internals only
        bool HasInternalError();
        //Returns false and does nothing if no error
        //Returns true, sets internal error, and displays error if true
        bool HandleInternalError(const Error& error);

        BoxType GetPreviousNodeBoxType() const;
        // ========== Layout ===============
        //Text
        void ComputeTextLinesAndHeight(BoxCore& box);
        //Width
        void WidthContentPercentPass_Flow(TreeNode<BoxCore>* node);
        void WidthContentPercentPass_Grid(TreeNode<BoxCore>* node);
        void WidthContentPercentPass(TreeNode<BoxCore>* node);
        void WidthPass(TreeNode<BoxCore>* node);
        void WidthPass_Flow(ArenaLL<TreeNode<BoxCore>>::Node* child, const BoxCore& parent_box); //Recurse Helper
        void WidthPass_Grid(ArenaLL<TreeNode<BoxCore>>::Node* child, const BoxCore& parent_box); //Recurse Helper
        //Height
        void HeightContentPercentPass_Flow(TreeNode<BoxCore>* node);
        void HeightContentPercentPass_Grid(TreeNode<BoxCore>* node);
        void HeightContentPercentPass(TreeNode<BoxCore>* node);
        void HeightPass(TreeNode<BoxCore>* node);
        void HeightPass_Flow(ArenaLL<TreeNode<BoxCore>>::Node* child, const BoxCore& parent_box); //Recurse Helper
        void HeightPass_Grid(ArenaLL<TreeNode<BoxCore>>::Node* child, const BoxCore& parent_box); //Recurse Helpe

        //Computes relative positions from parent
        void PositionPass_Flow(ArenaLL<TreeNode<BoxCore>>::Node* child, int x, int y, const BoxCore& parent_box);
        void PositionPass_Grid(ArenaLL<TreeNode<BoxCore>>::Node* child, int x, int y, const BoxCore& parent_box);
        void PositionPass(TreeNode<BoxCore>* node, int x, int y, const BoxCore& parent_box);

        void GenerateComputedTree();
        void GenerateComputedTree_h(TreeNode<BoxCore>* tree_core, TreeNode<BoxResult>* tree_result);
        // ================================
        void AddDetachedBoxToQueue(TreeNode<BoxResult>* node, const Rect& parent);
        void DrawPass(TreeNode<BoxResult>* node, int x, int y, Rect scissor_aabb);

    private:
        Error internal_error;
        uint32_t element_count = 0;


        Internal::ArenaDoubleBufferMap<BoxInfo> double_buffer_map;
        TreeNode<BoxCore>* tree_core = nullptr;
        TreeNode<BoxResult>* tree_result = nullptr;

        Internal::MemoryArena arena1; //Arena used for building the ui tree
        Internal::MemoryArena arena2; //Arena used for caching computed ui tree and computed text lines after measurements
        Internal::MemoryArena arena3; //Arena used for string allocation

        Internal::FixedStack<TreeNode<BoxCore>*, 64> stack; //elements should never nest over 100 layers deep
        BoxCore* prev_inserted_box = nullptr;
        Internal::ArenaLL<DeferredBox> deferred_elements;
        uint64_t directly_hovered_element_key = 0;

    };


    //Builder Notation
    class Builder
    {
    public:
        void SetContext(Context* context);


        //Also Implemented as global functions
        Builder& Box(const BoxStyle& style = BoxStyle(), const StringAsci& id = StringAsci(), DebugInfo debug_info = UI_DEBUG("Box"));
        void Text(const TextStyle& style, const StringU32& string, bool copy_text = true, DebugInfo debug_info = UI_DEBUG("Text"));
        BoxInfo Info() const;
        BoxStyle& Style();
        bool IsHover() const;
        bool IsDirectHover() const;

        //Parmeters
        Builder& Style(const BoxStyle& style);
        Builder& Id(const StringAsci& id);
        template<typename Func>
        Builder& OnHover(Func&& func);
        template<typename Func>
        Builder& OnDirectHover(Func&& func);
        template<typename Func>
        Builder& PreRun(Func&& func);


        //Executes begin/end
        void Run();

        //Executes begin/end with lambda
        template<typename Func>
        void Run(Func&& func);
    private:
        bool HasContext() const;
        void ClearStates();
        Context* context = nullptr;

        //States
        StringAsci id;
        BoxInfo info;
        BoxStyle style;
        DebugInfo debug_info;
        bool copy_text = true;
    };

}


//Implementation
namespace UI
{
    inline TextStyle& TextStyle::FontSize(int size)
    {
        font_size = size;
        return *this;
    }
    inline TextStyle& TextStyle::LineSpacing(int spacing)
    {
        line_spacing = spacing;
        return *this;
    }
    inline TextStyle& TextStyle::FgColor(const Color& color)
    {
        this->fg_color = color;
        return *this;
    }
    inline TextStyle& TextStyle::BgColor(const Color& color)
    {
        this->bg_color = color;
        return *this;
    }
    inline int TextStyle::GetFontSize() const
    {
        return font_size;
    }
    inline Color TextStyle::GetFgColor() const
    {
        return fg_color;
    }
    inline Color TextStyle::GetBgColor() const
    {
        return bg_color;
    }
    inline int TextStyle::GetFontSpacing() const
    {
        return font_spacing;
    }
    inline int TextStyle::GetLineSpacing() const
    {
        return line_spacing;
    }
    // inline bool TextStyle::operator==(const TextStyle& t) const
    // {
    //     return std::memcmp(this, &t, sizeof(TextStyle)) == 0;
    // }

    template<typename Func>
    inline void Root(Context* context, const BoxStyle& style, Func&& func, DebugInfo debug_info)
    {
        UI::BeginRoot(context, style, debug_info);
        func();
        UI::EndRoot();
    }


    //Builder Implementation
    inline Builder& Builder::Box(const BoxStyle& style, const StringAsci& id, DebugInfo debug_info)
    {
        ClearStates();
        if(HasContext())
        {
            this->style = style;
            this->debug_info = debug_info;
            this->id = id;
            info = context->Info(id);
        }
        return *this;
    }
    inline void Builder::Text(const TextStyle& style, const StringU32& string, bool copy_text, DebugInfo debug_info)
    {
        ClearStates();
        if(HasContext())
        {
            this->context->InsertText(style, string, nullptr, copy_text, debug_info);
        }
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
        id = StringAsci{};
        info = BoxInfo();
        style = BoxStyle();
        debug_info = DebugInfo();
        copy_text = true;
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
    inline Builder& Builder::Id(const StringAsci& id)
    {
        this->id = id;
        this->info = context->Info(id);
        return *this;
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
            context->BeginBox(style, id, debug_info);
            context->EndBox();
        }
    }
    template<typename Func>
    void Builder::Run(Func&& func)
    {
        if(HasContext())
        {
            context->BeginBox(style, id, debug_info);
            func();
            context->EndBox();
        }
    }
}
