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
    struct Box;
    struct TreeNode;
}

namespace UI
{

    constexpr uint64_t KB = 1024;
    constexpr uint64_t MB = KB * KB;
    //String Helper
    const char *Fmt(const char *text, ...);

    //Math helpers
    float MillimeterToPixels();
    float CentimeterToPixels(float cm);
    float InchToPixels(float inches);
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
            MM,
            CM,
            INCH,
            //Limited to width/height
            PARENT_PERCENT,
            ROOT_PERCENT,
            CONTENT_PERCENT,   
            AVAILABLE_PERCENT,

            WIDTH_PERCENT, //only be applied to height
        };

        float value = 0;
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

    enum Detach : unsigned char
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
    // ========== Main Box Styling ========== 
    struct BoxStyle
    {
        //container
        Layout layout = Layout::FLOW;
        Flow flow;
        Grid grid;

        Unit x = Unit{0, Unit::Type::PIXEL};
        Unit y = Unit{0, Unit::Type::PIXEL};

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
        bool IsValid() const { return valid; }
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
    BoxInfo GetBoxInfo(const char* label);
    void SetContext(UI::Context* context);
    Context* GetContext();
    void SetFreeze(bool state);
    void SetDebugInput(bool mouse_pressed, bool mouse_release, int mouse_scroll, bool activate_pressed);
    void BeginRoot(int x, int y, int screen_width, int screen_height, int mouse_x, int mouse_y);
    void EndRoot();
    void BeginBox(const BoxStyle& box_style, const char* label = nullptr, DebugInfo debug_info = UI_DEBUG("Box"));
    void InsertText(const char* text, const char* label = nullptr, bool copy_text = true, DebugInfo debug_info = UI_DEBUG("Text"));
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
            void SetDetached(bool flag);
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
            BoxInfo GetBoxInfo(uint64_t key);
            void SetMousePos(int x, int y);
            void BeginRoot(int x, int y, int screen_width, int screen_height, int mouse_x, int mouse_y, DebugInfo debug_info = UI_DEBUG("Root"));
            void BeginRoot(int x, int y, int screen_width, int screen_height, DebugInfo debug_info = UI_DEBUG("Root"));
            void EndRoot();
            void BeginBox(const UI::BoxStyle& box_style, const char* label = nullptr, DebugInfo debug_info = UI_DEBUG("Box"));
            void InsertText(const char* text, const char* label = nullptr, bool copy_text = true, DebugInfo info = UI_DEBUG("Text"));
            void EndBox();
            void Draw();
            uint32_t GetElementCount() const;

            void SetInspector(bool mouse_pressed, bool mouse_released, int mouse_scroll, bool activate_pressed, DebugInspector* inspector_context); 
            //For Advanced Purposes

            //Might not even use this
            void ResetAllStates();
        private:
            void ClearPreviousFrame();
            //These functions are for internals only
        using Box = Internal::Box;
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
        void DrawPass_FlowNoWrap(Internal::ArenaLL<TreeNode>::Node* child, const Box& parent_box, int x, int y, Rect parent_aabb);
        void DrawPass(TreeNode* node, int x, int y, const Box& parent_box, Rect parent_aabb);
        // ================================

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

        int mouse_x = 0, mouse_y = 0;
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
        void SetUserInput(bool mouse_pressed, bool mouse_released, int mouse_scroll); 
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
        void CustomDigitInput(int& value);
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
        BoxInfo GetBoxInfo() const;
        BoxStyle& GetStyle();

        //Main 3 options
        template<typename Func>
        void Root(int x, int y, int screen_width, int screen_height, int mouse_x, int mouse_y, Func&& func);
        Builder& Text(const char* text, const char* id = nullptr, bool should_copy = true, DebugInfo debug_info = UI_DEBUG("Text"));
        Builder& Box(const char* id = nullptr, DebugInfo debug_info = UI_DEBUG("Box"));

        //Parmeters
        Builder& Style(const BoxStyle& style);
        template<typename Func>
        Builder& OnHover(Func&& func);
        template<typename Func>
        Builder& OnDirectHover(Func&& func);

        bool IsHover() const;
        bool IsDirectHover() const;

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
    void Builder::Root(int x, int y, int screen_width, int screen_height, int mouse_x, int mouse_y, Func&& func)
    {
        if(HasContext())
        {
            context->BeginRoot(x, y, screen_width, screen_height, mouse_x, mouse_y);
            func();
            context->EndRoot();
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
        id = nullptr;
        text = nullptr;
        info = BoxInfo();
        style = BoxStyle();
        debug_info = DebugInfo();
        should_copy = true;
    }
    inline BoxInfo Builder::GetBoxInfo() const
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
    inline BoxStyle& Builder::GetStyle()
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