#include "ui.hpp"
#include "Memory.hpp"

namespace UI
{
    using namespace Internal;
    void DisplayError(const Error& error);
    Error CheckUnitErrors(const Box& style);
    Error CheckLeafNodeContradictions(const Box& leaf);
    Error CheckRootNodeConflicts(const Box& root);
    Error CheckNodeContradictions(const Box& child, const Box& parent);

    //Used during tree descending
    int FixedUnitToPx(Unit unit, int root_size);
    Box ComputeStyleSheet(const BoxStyle& style, const Box& root);

    //Size should include '\0'
    void StringCopy(char* dst, const char* src, uint32_t size);
    bool StringCompare(const char* s1, const char* s2);
    char ToLower(char c);

    //Does not count '\0'
    int StringLength(const char* text);
    uint32_t StrToU32(const char* text, bool* error = nullptr);
    uint32_t HexToU32(const char* text, bool* error = nullptr);
    Color HexToRGBA(const char* text, bool* error = nullptr);

    constexpr char U4ToHexDigit(uint8_t n);
    //Should copy data

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
        bool disable_markdown = false;
        FixedString<128> code;
        FixedString<FLUSH_CAP> text;
    };


    //Draws text based on custom markup
    void DrawTextNode(const char* text, int max_width, int max_height, int x, int y);

    //Computing PARENT_PERCENT
    int ParentPercentToPx(int value , Unit::Type unit_type, int parent_width);
    void ComputeParentWidthPercent(Box& box, int parent_width);
    void ComputeParentHeightPercent(Box& box, int parent_width);


    //Debugger
    class DebugView
    {
    public:
        struct Theme
        {
            Color base;
            Color left_panel;
            Color right_panel;
            Color button;
            Color button_hover;
            Color title_bar;
            Color text_color;
            Color text_color_hover;
            Color faded_button;
            Color inserted_text_color;
            Color id_text_color;
            unsigned char corner_radius = 1;
        };
        constexpr static Theme light_theme = 
        {
            {253, 253, 253, 50}, //base
            {253, 253, 253, 255}, //left_panel
            {38, 38, 38, 255}, //right_panel
            {253, 253, 253, 255}, //button
            {38, 38, 38, 255},      //button_hover
            {200, 200, 200, 255},   //titel_bar
            {38, 38, 38, 255},      //text_color
            {253, 253, 253, 255},   //text_color_hover
            {253, 211, 211, 255},   //faded_button
            {70, 188, 70, 255},    //inserted_text_color
            {216, 169, 60, 255},   //id_text_color
            16                      //corner radius
        };
        DebugView(uint64_t memory);
        bool IsActive() const;
        void SetActive(bool flag);
        void GetUserInput(bool is_mouse_pressed, bool is_mouse_released);
        void RunDebugView(TreeNode* root_node, int x, int y, int screen_width, int screen_height, int mouse_x, int mouse_y);
        void Reset();
    private:
        void ConstructDubLayout(TreeNode* node, int& count);
        void ConstructInspector(TreeNode* node);
        void InjectTree(TreeNode* node, int depth);
        bool SearchNodeAndOpenTree(TreeNode* node);
        Context ui;
        TreeNode* root_node = nullptr;

        // ========== Debug Window state ==========
        Theme theme = light_theme;
        Rect window_dim = Rect{10, 10, 400, 400};
        int drag_mouse_x = 0;
        int drag_mouse_y = 0;
        bool window_pos_drag = false;
        bool window_size_drag = false;

        int left_panel_width = 125;
        bool left_panel_size_drag = false;
        bool left_panel_scroll_drag = false;
        Map<bool> tree_state;
        TreeNode* selected_node = nullptr;
        Rect hover_node;
        // ==============================================
        int mouse_x = 0;
        int mouse_y = 0;
        bool mouse_pressed = false;
        bool mouse_released = false;
        bool active = false;
    };
}


//GLOBALS
namespace UI
{
    bool should_activate_debug_view = false;
    DebugView debug_view(2097152); //2 MB
    static float dpi = 96.0f;
    static Context* context = nullptr;
}



//IMPLEMENTATION
namespace UI
{
    void SetDebugInput(bool mouse_pressed, bool mouse_released, bool activate_pressed)
    {
        debug_view.GetUserInput(mouse_pressed, mouse_released);
        if(activate_pressed)
            should_activate_debug_view = !should_activate_debug_view;
    }
    void SetContext(UI::Context* context)
    {
        UI::context = context;
    }
    Context* GetContext()
    {
        return context;
    }
    BoxInfo GetBoxInfo(const char* label)
    {
        if(context && !debug_view.IsActive())
            return context->GetBoxInfo(label);
        return BoxInfo();
    }
    void BeginRoot(int x, int y, unsigned int screen_width, unsigned int screen_height, int mouse_x, int mouse_y)
    {
        debug_view.SetActive(should_activate_debug_view);
        if(context)
        {
            if(!debug_view.IsActive())
            {
                context->BeginRoot(x, y, screen_width, screen_height, mouse_x, mouse_y);
            }
            else
            {
                debug_view.RunDebugView(context->GetInternalTree(), x, y, screen_width, screen_height, mouse_x, mouse_y);
            }
        }
    }
    void EndRoot()
    {
        if(context && !debug_view.IsActive())
            context->EndRoot();
    }
    void BeginBox(const UI::BoxStyle& box_style, const char* label, DebugInfo debug_info)
    {
        if(context && !debug_view.IsActive())
            context->BeginBox(box_style, label, debug_info);
    }
    void InsertText(const char* text, bool copy_text)
    {
        if(context && !debug_view.IsActive())
            context->InsertText(text, copy_text);
    }
    void EndBox()
    {
        if(context && !debug_view.IsActive())
            context->EndBox();
    }
    void Draw()
    {
        if(context && !debug_view.IsActive())
            context->Draw();
    }
}


//Box
namespace UI
{
    inline void Box::SetPositioning(Positioning p){positioning = p;}
    inline void Box::SetFlowAxis(Flow::Axis axis){flow_axis = axis;}
    inline void Box::SetScissor(bool flag){scissor = flag;}
    inline void Box::SetDetached(bool flag){detach = flag;}
    inline Layout Box::GetLayout() const
    {
        return layout;
    }
    inline Flow::Axis Box::GetFlowAxis() const
    {
        return flow_axis;
    }
    inline Positioning Box::GetPositioning() const
    {
        return positioning;
    }
    inline bool Box::IsScissor() const
    {
        return scissor;
    }
    inline bool Box::IsDetached() const
    {
        return detach;
    }
    inline int Box::GetBoxExpansionWidth() const
    {
        return margin.left + margin.right + padding.left + padding.right;
    }
    inline int Box::GetBoxExpansionHeight() const
    {
        return margin.top + margin.bottom + padding.top + padding.bottom;
    }
    inline int Box::GetBoxModelWidth() const
    {
        //internal box model
        return margin.left + padding.left + width + padding.right + margin.right;
    }
    inline int Box::GetBoxModelHeight() const
    {
        return margin.top + padding.top + height + padding.bottom + margin.bottom;
    }
    inline int Box::GetRenderingWidth() const
    {
        return padding.left + width + padding.right;
    }
    inline int Box::GetRenderingHeight() const
    {
        return padding.top + height + padding.bottom;
    }


    //Common helpers and error checking
    void DisplayError(const Error& error)
    {
        LogError_impl(error.msg);
    }



    #define UNIT_CONFLICT(value, illegal_unit, error_type)\
        if(value == illegal_unit)\
        {\
            error.type = error_type;\
            StringCopy(error.msg, StringFormat(#error_type"\n"#value" = " #illegal_unit"\nFile: %s\nLine: %d\n", file, line), ERROR_MSG_SIZE);\
        }
    Error CheckUnitErrors(const Box& style)
    {
        //The following units cannot equal the specified Unit Types 

        //Content%
        Error error;
        #if UI_ENABLE_DEBUG
            const char* file = style.debug_file;
            int line = style.debug_line;
            UNIT_CONFLICT(style.x_unit,                     Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
            UNIT_CONFLICT(style.y_unit,                     Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
            UNIT_CONFLICT(style.grid_cell_width_unit,       Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
            UNIT_CONFLICT(style.grid_cell_height_unit,      Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
            UNIT_CONFLICT(style.x_unit,                     Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
            UNIT_CONFLICT(style.y_unit,                     Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
            UNIT_CONFLICT(style.grid_cell_width_unit,       Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
            UNIT_CONFLICT(style.grid_cell_height_unit,      Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
            UNIT_CONFLICT(style.min_width_unit,             Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
            UNIT_CONFLICT(style.min_height_unit,            Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
            UNIT_CONFLICT(style.max_width_unit,             Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
            UNIT_CONFLICT(style.max_height_unit,            Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        #endif
        return error;
    }



    Error CheckLeafNodeContradictions(const Box& leaf)
    {
        //The Following erros are contradictions
        Error error;
        #if UI_ENABLE_DEBUG
            const char* file = leaf.debug_file;
            int line = leaf.debug_line;
            if(leaf.width_unit == Unit::Type::CONTENT_PERCENT)
            {
                error.type = Error::Type::LEAF_NODE_CONTRADICTION;
                StringCopy(error.msg, StringFormat("LEAF_NODE_CONTRADICTION\nbox.width_unit = Unit::CONTENT_PERCENT with 0 children\nFile:%s\nLine%d", file, line), ERROR_MSG_SIZE);
            }
            if(leaf.height_unit == Unit::Type::CONTENT_PERCENT)
            {
                error.type = Error::Type::LEAF_NODE_CONTRADICTION;
                StringCopy(error.msg, StringFormat("LEAF_NODE_CONTRADICTION\nbox.height_unit = Unit::CONTENT_PERCENT with 0 children\nFile:%s\nLine%d", file, line), ERROR_MSG_SIZE);
            }
        #endif
        return error;
    }

    Error CheckRootNodeConflicts(const Box& root)
    {
        //Root node style sheet cannot equal any of these Unit types
        //The following errors are contradictions
        //Parent%
        Error error;
        #if UI_ENABLE_DEBUG
            const char* file = root.debug_file;
            int line = root.debug_line;
            UNIT_CONFLICT(root.width_unit,      Unit::Type::PARENT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
            UNIT_CONFLICT(root.height_unit,     Unit::Type::PARENT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
            UNIT_CONFLICT(root.min_width_unit,  Unit::Type::PARENT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
            UNIT_CONFLICT(root.min_height_unit, Unit::Type::PARENT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
            UNIT_CONFLICT(root.max_width_unit,  Unit::Type::PARENT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
            UNIT_CONFLICT(root.max_height_unit, Unit::Type::PARENT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
            UNIT_CONFLICT(root.x_unit,          Unit::Type::PARENT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
            UNIT_CONFLICT(root.y_unit,          Unit::Type::PARENT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
            //Available%
            UNIT_CONFLICT(root.width_unit,      Unit::Type::AVAILABLE_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
            UNIT_CONFLICT(root.height_unit,     Unit::Type::AVAILABLE_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
            UNIT_CONFLICT(root.min_width_unit,  Unit::Type::AVAILABLE_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
            UNIT_CONFLICT(root.min_height_unit, Unit::Type::AVAILABLE_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
            UNIT_CONFLICT(root.max_width_unit,  Unit::Type::AVAILABLE_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
            UNIT_CONFLICT(root.max_height_unit, Unit::Type::AVAILABLE_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
            UNIT_CONFLICT(root.x_unit,          Unit::Type::AVAILABLE_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
            UNIT_CONFLICT(root.y_unit,          Unit::Type::AVAILABLE_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
            //Root%
            UNIT_CONFLICT(root.width_unit,      Unit::Type::ROOT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
            UNIT_CONFLICT(root.height_unit,     Unit::Type::ROOT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
            UNIT_CONFLICT(root.min_width_unit,  Unit::Type::ROOT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
            UNIT_CONFLICT(root.min_height_unit, Unit::Type::ROOT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
            UNIT_CONFLICT(root.max_width_unit,  Unit::Type::ROOT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
            UNIT_CONFLICT(root.max_height_unit, Unit::Type::ROOT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
            UNIT_CONFLICT(root.x_unit,          Unit::Type::ROOT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
            UNIT_CONFLICT(root.y_unit,          Unit::Type::ROOT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        #endif
        return error;
    }
        
    Error CheckNodeContradictions(const Box& child, const Box& parent)
    {
        //The following errors are contradictions between parent and child
        Error error;
        #if UI_ENABLE_DEBUG

            #define CHILD_PARENT_CONFLICT(child_unit, illegal_unit, parent_unit, error_type)\
                if(child_unit == illegal_unit && parent_unit == Unit::CONTENT_PERCENT)\
                {\
                    error.type = error_type;\
                    StringCopy(error.msg, StringFormat(#error_type"\n"#child_unit " = " #illegal_unit" and "#parent_unit " = Unit::CONTENT_PERCENT\nFile: %s\nLine: %d\n", file, line), ERROR_MSG_SIZE);\
                }

            const char* file = child.debug_file;
            int line = child.debug_line;
            CHILD_PARENT_CONFLICT(child.width_unit, Unit::PARENT_PERCENT, parent.width_unit, Error::Type::NODE_CONTRADICTION);
            CHILD_PARENT_CONFLICT(child.width_unit, Unit::AVAILABLE_PERCENT, parent.width_unit, Error::Type::NODE_CONTRADICTION);
            CHILD_PARENT_CONFLICT(child.height_unit, Unit::PARENT_PERCENT, parent.height_unit, Error::Type::NODE_CONTRADICTION);
            CHILD_PARENT_CONFLICT(child.height_unit, Unit::AVAILABLE_PERCENT, parent.height_unit, Error::Type::NODE_CONTRADICTION);
            CHILD_PARENT_CONFLICT(child.min_width_unit, Unit::PARENT_PERCENT, parent.width_unit, Error::Type::NODE_CONTRADICTION);
            CHILD_PARENT_CONFLICT(child.min_width_unit, Unit::AVAILABLE_PERCENT, parent.width_unit, Error::Type::NODE_CONTRADICTION);
            CHILD_PARENT_CONFLICT(child.max_width_unit, Unit::PARENT_PERCENT, parent.width_unit, Error::Type::NODE_CONTRADICTION);
            CHILD_PARENT_CONFLICT(child.max_width_unit, Unit::AVAILABLE_PERCENT, parent.width_unit, Error::Type::NODE_CONTRADICTION);
            CHILD_PARENT_CONFLICT(child.min_height_unit, Unit::PARENT_PERCENT, parent.height_unit, Error::Type::NODE_CONTRADICTION);
            CHILD_PARENT_CONFLICT(child.min_height_unit, Unit::AVAILABLE_PERCENT, parent.height_unit, Error::Type::NODE_CONTRADICTION);
            CHILD_PARENT_CONFLICT(child.max_height_unit, Unit::PARENT_PERCENT, parent.height_unit, Error::Type::NODE_CONTRADICTION);
            CHILD_PARENT_CONFLICT(child.max_height_unit, Unit::AVAILABLE_PERCENT, parent.height_unit, Error::Type::NODE_CONTRADICTION);
        #endif

        return error;
    }

    bool Rect::Overlap(const Rect& r1, const Rect& r2)
    {
        return (r1.x < r2.x + r2.width && r1.x + r1.width > r2.x &&
                r1.y < r2.y + r2.height && r1.y + r1.height > r2.y);
    }
    bool Rect::Contains(const Rect& r ,int x, int y)
    {
        return (x >= r.x && x <= r.x + r.width &&
                y >= r.y && y <= r.y + r.height);
    }
    Rect Rect::Intersection(const Rect& r1, const Rect& r2)
    {
        Rect r;
        if(Overlap(r1, r2))
        {
            r.x = Max(r1.x, r2.x);
            r.y = Max(r1.y, r2.y);
            int outer_x = Min(r1.x + r1.width, r2.x + r2.width);
            int outer_y = Min(r1.y + r1.height, r2.y + r2.height);
            r.width = abs(outer_x - r.x);
            r.height = abs(outer_y  - r.y);
        }
        return r;
    }
    inline float MillimeterToPixels(float mm)
    {
        return mm * dpi / 25.4f;
    } inline float CentimeterToPixels(float cm)
    {
        return cm * dpi / 2.54f;
    }
    inline float InchToPixels(float inches)
    {
        return inches * dpi;
    }


//Pass 1
//Compute unit CM, MM, INCH, ROOT_PERCENT
    int FixedUnitToPx(Unit unit, int root_size)
    {
        switch(unit.unit)
        {
            case Unit::Type::PIXEL:
                return unit.value;
            case Unit::Type::MM:
                return MillimeterToPixels((float)unit.value);
            case Unit::Type::CM:
                return CentimeterToPixels((float)unit.value);
            case Unit::Type::ROOT_PERCENT: 
                return unit.value * root_size / 100;
            default:
                return unit.value; //Only meant for width/height
        }
    }
    Box ComputeStyleSheet(const BoxStyle& style, const Box& root)
    {
        int root_width = root.width - style.margin.left - style.margin.right - style.padding.left - style.padding.right;
        int root_height = root.height - style.margin.top - style.margin.bottom - style.padding.top - style.padding.bottom;
        root_width = Max(0, root_width);
        root_height = Max(0, root_height);

        Box box;
        box.background_color =          style.background_color;
        box.border_color =              style.border_color;
        //type 3

        box.scroll_x =                  style.scroll_x;
        box.scroll_y =                  style.scroll_y;
        
        box.width =                     (uint16_t)Max(0, FixedUnitToPx(style.width, root_width));
        box.height =                    (uint16_t)Max(0, FixedUnitToPx(style.height, root_height));
        box.gap_row =                    style.gap_row;
        box.gap_column =                 style.gap_column;
        box.min_width =                 (uint16_t)Max(0, FixedUnitToPx(style.min_width, root_width));
        box.max_width =                 (uint16_t)Max(0, FixedUnitToPx(style.max_width, root_width));
        box.min_height =                (uint16_t)Max(0, FixedUnitToPx(style.min_height, root_height));
        box.max_height =                (uint16_t)Max(0, FixedUnitToPx(style.max_height, root_height));
        box.x =                         (int16_t)FixedUnitToPx(style.x, root_width);
        box.y =                         (int16_t)FixedUnitToPx(style.y, root_height);
        box.grid_cell_width =           (uint16_t)Max(0, FixedUnitToPx(style.grid.cell_width, root_width));
        box.grid_cell_height =          (uint16_t)Max(0, FixedUnitToPx(style.grid.cell_height, root_width));

        box.width_unit =                style.width.unit;
        box.height_unit =               style.height.unit;
        box.min_width_unit =            style.min_width.unit;
        box.max_width_unit =            style.max_width.unit;
        box.min_height_unit =           style.min_height.unit;
        box.max_height_unit =           style.max_height.unit;
        box.x_unit =                    style.x.unit;
        box.y_unit =                    style.y.unit;
        box.grid_cell_width_unit =      style.grid.cell_width.unit;
        box.grid_cell_height_unit =     style.grid.cell_height.unit;

        box.grid_row_max =              style.grid.row_max;
        box.grid_column_max =           style.grid.column_max;
        box.grid_row_start =            style.grid.row_start;
        box.grid_column_start =         style.grid.column_start; 
        box.grid_row_end =              style.grid.row_end;
        box.grid_column_end =           style.grid.column_end; 

        box.flow_vertical_alignment =   style.flow.vertical_alignment;
        box.flow_horizontal_alignment = style.flow.horizontal_alignment;
        //PIXEL VALUES
        box.corner_radius =             style.corner_radius; //255 sets to circle
        box.border_width =              style.border_width;  
        box.padding =                   style.padding;
        box.margin =                    style.margin;
        box.layout =                    style.layout;

        box.SetPositioning(style.positioning);
        box.SetFlowAxis(style.flow.axis);
        box.SetScissor(style.scissor);
        box.SetDetached(style.detach);
        return box;
    }


    inline uint64_t Hash(const char* str)
    {
        if(str == nullptr)
            return 0;
        uint64_t hash = 14695981039346656037ULL;
        while (*str) 
        {
            hash ^= static_cast<uint64_t>(*str++);
            hash *= 1099511628211ULL;
        }
        //avoiding returning 0
        return !hash? 1: hash;
    }
    void StringCopy(char* dst, const char* src, uint32_t size)
    {
        if(!size || !src || !dst) return;
        uint32_t i;
        for(i = 0; i<size-1 && src[i] != '\0'; i++)
            dst[i] = src[i];
        dst[i] = '\0';
    }
    bool StringCompare(const char* s1, const char* s2)
    {
        if(s1 == nullptr || s2 == nullptr)
            return false;
        while(*s1 && *s2)
        {
            if(*s1 != *s2)
                return false;
            s1++;
            s2++;
        }
        return *s1 == *s2;
    }

    //TEXT RENDERING
    const char *StringFormat(const char *text, ...)
    {
        static int index = 0;
        constexpr uint32_t MAX_LENGTH = 512;
        constexpr uint32_t MAX_BUFFERS = 6;
        static char buffer[MAX_BUFFERS][MAX_LENGTH];  // Fixed-size static buffer
        index = (index + 1) % MAX_BUFFERS;

        va_list args;
        va_start(args, text);
        int count = vsnprintf(buffer[index], MAX_LENGTH, text, args);
        if(count >= MAX_LENGTH) //just for sanity
            buffer[index][MAX_LENGTH-1] = '\0';
        va_end(args);
        return buffer[index];
    }
    inline char ToLower(char c)
    {
        return (c >= 'A' && c <= 'Z')? c + 32: c;
    }
    int StringLength(const char* text)
    {
        if(!text)
            return -1;
        int i;
        for(i = 0; text[i]; i++);
        return i;
    }
    inline uint32_t StrToU32(const char* text, bool* error)
    {
        if(!text)
        {
            if(error)
                *error = true;
            return 0;
        }
        uint32_t result = 0;
        for(;*text; text++)
        {
            char c = *text;
            if(c >= '0' && c <= '9')
            {
                uint32_t digit = c - '0';
                if(result > (0xFFFFFFFF - digit)/10)
                {
                    if(error)
                        *error = true;
                    return 0;
                }
                result = result * 10 + digit;
            }
            else
            {
                if(error)
                    *error = true;
                return 0;
            }
        }
        return result;
    }
    inline uint32_t HexToU32(const char* text, bool* error)
    {
        if(!text)
        {
            if(error)
                *error = true;
            return 0;
        }
        uint32_t result = 0;
        for(; *text; text++)
        {
            result <<= 4;
            char c = *text;
            c = ToLower(c);
            if(c >= '0' && c <= '9')
                result |= c - '0';
            else if(c >= 'a' && c<= 'f')
                result |= c - 87;
            else
            {
                if(error)
                    *error = true;
                return 0;
            }
        }
        return result;
    }
    inline constexpr char U4ToHexDigit(uint8_t n)
    {
        if(n <= 9)
            return n + '0';
        else if(n >= 10 && n <= 15)
            return (n - 10) + 'a';
        return '0';
    }
    inline constexpr HexColor RGBAToHex(Color color)
    {
        //constexpr uint32_t MAX_BUFFERS = 16;
        //static int index = 0;
        //static char data[MAX_BUFFERS][9]{};
        //index = (index + 1) % MAX_BUFFERS;
        //data[index][0] = U4ToHexDigit((color.r >> 4) & 0xF);
        //data[index][1] = U4ToHexDigit(color.r & 0xF);
        //data[index][2] = U4ToHexDigit((color.g >> 4) & 0xF);
        //data[index][3] = U4ToHexDigit(color.g & 0xF);
        //data[index][4] = U4ToHexDigit((color.b >> 4) & 0xF);
        //data[index][5] = U4ToHexDigit(color.b & 0xF);
        //data[index][6] = U4ToHexDigit((color.a >> 4) & 0xF);
        //data[index][7] = U4ToHexDigit(color.a & 0xF);
        //data[index][8] = '\0';
        //return data[index];
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
    inline Color HexToRGBA(const char* text, bool* error)
    {
        bool err = false;
        if(!text)
            err = true;
        char hex[3]{};
        Color result = {0, 0, 0, 255};
        for(int i = 0; i<6; i++)
        {
            if(text[i] == '\0')
                err = true;
        }
        hex[0] = text[0]; hex[1] = text[1]; hex[2] = '\0';
        result.r = HexToU32(hex, &err);
        hex[0] = text[2]; hex[1] = text[3]; hex[2] = '\0';
        result.g = HexToU32(hex, &err);
        hex[0] = text[4]; hex[1] = text[5]; hex[2] = '\0';
        result.b = HexToU32(hex, &err);
        if(text[6] == '\0')
        {
            if(error)
                *error = err;
            return err? Color() : result;
        }
        if(text[7] == '\0')
        {
            if(error)
                *error = true;
            return Color();
        }
        hex[0] = text[6]; hex[1] = text[7]; hex[2] = '\0';
        result.a = HexToU32(hex, &err);
        if(text[8] != '\0')
            err = true;
        if(error)
            *error = err;
        return err? Color(): result;
    }
    //FixedString
    template<uint32_t CAPACITY>
    const char* FixedString<CAPACITY>::Data() const
    {
        return data;
    }
    template<uint32_t CAPACITY>
    uint32_t FixedString<CAPACITY>::Size() const
    {
        return size;
    }
    template<uint32_t CAPACITY>
    bool FixedString<CAPACITY>::IsEmpty() const
    {
        return size == 0;
    }
    template<uint32_t CAPACITY>
    bool FixedString<CAPACITY>::IsFull() const
    {
        return size >= CAPACITY;
    }
    template<uint32_t CAPACITY>
    bool FixedString<CAPACITY>::Push(char c)
    {
        if(size >= CAPACITY)
            return false;
        data[size++] = c;
        data[size] = '\0';
        return true;
    }
    template<uint32_t CAPACITY>
    bool FixedString<CAPACITY>::Pop()
    {
        if(size > 0)
        {
            size--;
            data[size] = '\0';
            return true;
        }
        return false;
    }
    template<uint32_t CAPACITY>
    void FixedString<CAPACITY>::Clear()
    {
        data[0] = '\0';
        size = 0;
    }
    template<uint32_t CAPACITY>
    char& FixedString<CAPACITY>::operator[](uint32_t index)
    {
        assert(index < size && "FixedString operator[] out of scope");
        return data[index];
    }
    void Markdown::SetInput(const char* source, int max_width, int max_height)
    {
        assert(source);
        this->source = source; 
        this->max_width = max_width;
    }
    bool Markdown::Done()
    {
        if(source == nullptr)
            return true;
        return *source == '\0';
    }
    bool Markdown::ComputeEscapeCode()
    {
        if(code.IsEmpty())
            return false;

        switch(code[0])
        {
            case 'C':
            {
                if(code[1] != ':')
                    return false;
                bool err = false;
                state.font_color = HexToRGBA(&code[2], &err);
                return !err;
            }
            case 'S':
            {
                if(code[1] != ':')
                    return false;
                bool err = false;
                state.font_size = StrToU32(&code[2], &err);
                return !err;
            }
            case 'O':
            {
                if(code[1] != 'F')
                    return false;
                if(code[2]!= 'F')
                    return false;
                disable_markdown = true; 
                return true;
            }
            default:
            {
                return false;
                break;
            }
        }
    }
    void Markdown::HandleWrap()
    {
        if(text.Size() <= 1)
            return;
        int index = text.Size() - 1;
        int offset = 0;
        for(;index >= 0; index--)
        {
            if(text[index] == ' ')
            {
                text[index] = '\n';
                cursor_x = 0;
                cursor_y += state.font_size + state.line_spacing;
                break;
            }
            offset += MeasureChar_impl(text[index], state.font_size, state.font_spacing);
        }
        cursor_x += offset;
    }
    void Markdown::PushAndMeasureChar(char c)
    {
        text.Push(c);
        if(c == '\n')
        {
            cursor_x = 0;
            cursor_y += state.font_size + state.line_spacing;
        }
        else
        {
            cursor_x += MeasureChar_impl(c, state.font_size, state.font_spacing);
        }
        if(cursor_x >= max_width)
        {
            HandleWrap();
        }
        if(cursor_x >= measured_width)
            measured_width = cursor_x;
    }
    bool Markdown::ComputeNextTextRun()
    {
        if(!source || *source == '\0')
            return false;

        ClearBuffers();
        p.text = text.Data();
        p.cursor_x = cursor_x;
        p.cursor_y = cursor_y;
        p.font_size = state.font_size;
        p.line_spacing = state.line_spacing;
        p.font_spacing = state.font_spacing;
        p.font_color = state.font_color;
        bool should_ignore = false;
        for(;;source++)
        {
            char c = *source; 
            if(c == '\0')
            {
                break;
            }
            else if(should_ignore || disable_markdown)
            {
                //Inserting text
                if(text.IsFull()) 
                    break;
                PushAndMeasureChar(c);
                should_ignore = false;
            }
            else if(c == '\\' && !escape)
            {
                should_ignore = true;
            }
            else if(c == '[')
            {
                escape = true;
            }
            else if(c == ']' && escape)
            {
                if(!ComputeEscapeCode())
                {
                    assert(0 && "unknown escape code");
                }
                escape = false;
                source++;
                break;
            }
            else if(escape)
            {
                if(c == ' ')
                    continue;
                code.Push(c);
            }
            else
            {
                //Inserting text
                if(text.IsFull())
                    break;
                PushAndMeasureChar(c);
            }
        }
        //Fixing edge case
        bool esc = false;
        int c_x = cursor_x;
        for(int i = 0;; i++)
        {
            char c = source[i];
            if(c == '\0' || c == ' ')
            {
                break;
            }
            else if(should_ignore || disable_markdown)
            {
                c_x += MeasureChar_impl(c, state.font_size, state.font_spacing);
                should_ignore = false;
            }
            else if(c == '\\' && !esc)
                should_ignore = true;
            else if(c == '[')
                esc = true;
            else if(c == ']' && esc)
                esc = false;
            else if(!esc)
                c_x += MeasureChar_impl(c, state.font_size, state.font_spacing);
        }
        if(c_x >= max_width)
            HandleWrap();
        return true;
    }
    TextPrimitive Markdown::GetTextPrimitive(int x, int y)
    {
        p.x = x;
        p.y = y;
        return p;
    }
    int Markdown::GetMeasuredWidth() const
    {
        return measured_width;
    }
    int Markdown::GetMeasuredHeight() const
    {
        return cursor_y + state.font_size;
    }
    void Markdown::ClearBuffers()
    {
        text.Clear();
        code.Clear();
    }

    void DrawTextNode(const char* text, int max_width, int max_height, int x, int y)
    {
        if(!text)
            return;
        Markdown md;

        md.SetInput(text, max_width, max_height);

        while(md.ComputeNextTextRun())
        {
            TextPrimitive p = md.GetTextPrimitive(x, y);
            DrawText_impl(p);
        }
    }




    //Width
    int ParentPercentToPx(int value , Unit::Type unit_type, int parent_size)
    {
        return unit_type == Unit::Type::PARENT_PERCENT? value * parent_size / 100: value;
    }
    void ComputeParentWidthPercent(Box& box, int parent_width)
    {
        parent_width -= box.padding.left + box.padding.right + box.margin.left + box.margin.right;
        parent_width = Max(0, parent_width);
        box.width =                     (uint16_t)Max(0, ParentPercentToPx(box.width,            box.width_unit,             parent_width)); 
        box.min_width =                 (uint16_t)Max(0, ParentPercentToPx(box.min_width,        box.min_width_unit,         parent_width)); 
        box.max_width =                 (uint16_t)Max(0, ParentPercentToPx(box.max_width,        box.max_width_unit,         parent_width)); 
        box.x =                                 (int16_t)ParentPercentToPx(box.x,                box.x_unit,                 parent_width); 
        box.grid_cell_width =           (uint16_t)Max(0, ParentPercentToPx(box.grid_cell_width,  box.grid_cell_width_unit,   parent_width)); 
    }

    //Height
    void ComputeParentHeightPercent(Box& box, int parent_height)
    {
        parent_height -= box.padding.top + box.padding.bottom + box.margin.top + box.margin.bottom;
        parent_height = Max(0, parent_height);
        box.height =                    (uint16_t)Max(0, ParentPercentToPx(box.height,           box.height_unit,            parent_height)); 
        box.gap_row =                   (uint16_t)Max(0, ParentPercentToPx(box.gap_row,          box.gap_row_unit,           parent_height)); 
        box.min_height =                (uint16_t)Max(0, ParentPercentToPx(box.min_height,       box.min_height_unit,        parent_height)); 
        box.max_height =                (uint16_t)Max(0, ParentPercentToPx(box.max_height,       box.max_height_unit,        parent_height)); 
        box.y =                                 (int16_t)ParentPercentToPx(box.y,                box.y_unit,                 parent_height); 
        box.grid_cell_height =          (uint16_t)Max(0, ParentPercentToPx(box.grid_cell_height, box.grid_cell_height_unit,  parent_height)); 
    }
    void ComputeWidthPercentForHeight(Box& box)
    {
        if(box.height_unit == Unit::Type::WIDTH_PERCENT)
            box.height = box.width * box.height / 100;
        if(box.min_height_unit == Unit::Type::WIDTH_PERCENT)
            box.min_height = box.width * box.min_height / 100;
        if(box.max_height_unit == Unit::Type::WIDTH_PERCENT)
            box.max_height = box.width * box.max_height / 100;
    }


    DebugView::DebugView(uint64_t memory): ui(memory)
    {

    }
    bool DebugView::IsActive() const
    {
        return active;
    }
    void DebugView::SetActive(bool flag) 
    {
        active = flag;
        if(!active)
        {
            Reset();
        }
    }
    void DebugView::GetUserInput(bool is_mouse_pressed, bool is_mouse_released)
    {
        mouse_pressed = is_mouse_pressed;
        mouse_released = is_mouse_released;
    }
    void DebugView::RunDebugView(TreeNode* root_node, int x, int y, int screen_width, int screen_height, int mouse_x, int mouse_y)
    {
        this->mouse_x = mouse_x;
        this->mouse_y = mouse_y;
        this->root_node = root_node;
        ui.BeginRoot(x, y, screen_width, screen_height, mouse_x, mouse_y);
        int count = 0;
        ConstructDubLayout(root_node, count);
        ConstructInspector(root_node);
        ui.EndRoot();
        ui.Draw();
    }
    void DebugView::Reset()
    {
        selected_node = nullptr;
        root_node = nullptr;
        hover_node = Rect();
    }
    void DebugView::ConstructDubLayout(TreeNode* node, int& count)
    {
        if(node == nullptr)
            return;
        
        //hover_outline.width = 
        Box& box = node->box;
        const char* element_id = StringFormat("element_id_%d", count);

        BoxInfo info = ui.GetBoxInfo(element_id);
        if(info.valid)
        {
            if(info.is_direct_hover)
            {
                hover_node.width = info.draw_width;
                hover_node.height = info.draw_height;
                hover_node.x = info.draw_x;
                hover_node.y = info.draw_y;
                if(mouse_pressed)
                {
                    this->selected_node = node;
                    SearchNodeAndOpenTree(this->root_node);
                }
            }
        }

        count++;
        if(box.text)
        {
            ui.InsertText(box.text);
        }
        else
        {
            ui.BeginBox(box.debug_style, element_id);
            for(auto temp = node->children.GetHead(); temp != nullptr; temp = temp->next)
                ConstructDubLayout(&temp->value, count);
            ui.EndBox();
        }
    }
    void DebugView::ConstructInspector(TreeNode* node)
    {
        // ===== Red outline =====
        if(selected_node && selected_node->box.debug_valid)
        {
            Box& b = selected_node->box;
            BoxStyle select_outline;
            select_outline.detach = true;
            select_outline.x = {(float)b.x};
            select_outline.y = {(float)b.y};
            select_outline.width = {(float)b.GetRenderingWidth()};
            select_outline.height = {(float)b.GetRenderingHeight()};
            select_outline.border_width = 1;
            select_outline.border_color = {255, 0, 0, 255};
            ui.BeginBox(select_outline); ui.EndBox();
        }
        if(hover_node.width > 0)
        {
            BoxStyle hover_outline;
            hover_outline.detach = true;
            hover_outline.x = {(float)hover_node.x};
            hover_outline.y = {(float)hover_node.y};
            hover_outline.width = {(float)hover_node.width};
            hover_outline.height = {(float)hover_node.height};
            hover_outline.border_width = 1;
            hover_outline.border_color = {255, 255, 255, 255};
            ui.BeginBox(hover_outline); ui.EndBox();
        }
        hover_node = Rect();


        // ========== Window Dragging =========

        BoxInfo title_bar_info = ui.GetBoxInfo("title_bar");
        BoxInfo resize_button_info = ui.GetBoxInfo("resize_button");
        Rect base_dim = window_dim;
        if(mouse_pressed)
        {
            drag_mouse_x = mouse_x;
            drag_mouse_y = mouse_y;
        }
        int mouse_delta_x = mouse_x - drag_mouse_x;
        int mouse_delta_y = mouse_y - drag_mouse_y;

        if(window_pos_drag)
        {
            base_dim.x += mouse_delta_x;
            base_dim.y += mouse_delta_y;
        }
        if(window_size_drag)
        {
            base_dim.width += mouse_delta_x;
            base_dim.height += mouse_delta_y;
        }
        base_dim.width = Clamp(base_dim.width, 300, INT_MAX);
        base_dim.height = Clamp(base_dim.height, 300, INT_MAX);


        if(title_bar_info.valid && title_bar_info.is_direct_hover)
        {
            if(mouse_pressed)
                window_pos_drag = true;
        }
        // ===== End Window Dragging =====

        HexColor text_color = RGBAToHex(theme.text_color);
        HexColor text_color_hover = RGBAToHex(theme.text_color_hover);

        BoxStyle base; 
        base.flow.axis = Flow::VERTICAL;
        base.background_color = theme.base;
        base.x = {(float)base_dim.x};
        base.y = {(float)base_dim.y};
        base.width = {(float)base_dim.width};
        base.height = {(float)base_dim.height};
        base.corner_radius = theme.corner_radius;
        base.detach = true;

        BoxStyle resize_button;
        resize_button.width = {16};
        resize_button.height = {16};
        resize_button.background_color = theme.title_bar;
        resize_button.detach = true;
        resize_button.x = {(float)base_dim.x + base_dim.width - 16};
        resize_button.y = {(float)base_dim.y + base_dim.height - 16};
        resize_button.corner_radius = 4;
        HexColor resize_button_text_color = text_color;

        if(resize_button_info.valid && resize_button_info.is_direct_hover)
        {
            resize_button_text_color = text_color_hover;
            resize_button.background_color = theme.button_hover;
            if(mouse_pressed)
                window_size_drag = true;
        }
        
        // ====================================

        BoxStyle title_bar;
        title_bar.padding = {10, 10, 10, 10};
        title_bar.corner_radius = theme.corner_radius/2;
        title_bar.background_color = theme.title_bar;
        title_bar.width = {100, Unit::PARENT_PERCENT};
        title_bar.height = {100, Unit::CONTENT_PERCENT};


        BoxStyle h_container;
        h_container.gap_column = {1};
        h_container.flow.vertical_alignment = Flow::CENTERED;
        h_container.padding = {10, 10, 10, 10};
        h_container.flow.axis = Flow::HORIZONTAL;
        h_container.width = {100, Unit::AVAILABLE_PERCENT};
        h_container.height = {100, Unit::AVAILABLE_PERCENT};


        BoxStyle panel_divider_button;
        panel_divider_button.flow.horizontal_alignment = Flow::CENTERED;
        panel_divider_button.flow.vertical_alignment = Flow::CENTERED;
        panel_divider_button.width = {8};
        panel_divider_button.background_color = theme.title_bar;
        panel_divider_button.height = {70};
        panel_divider_button.corner_radius = 3;
        HexColor panel_divider_button_text_color = text_color;

        // ======== Panel drag sizing logic ===============
        BoxInfo panel_divider_button_info = ui.GetBoxInfo("panel_divider_button");
        if(panel_divider_button_info.valid && panel_divider_button_info.is_direct_hover)
        {
            panel_divider_button.background_color = theme.button_hover;
            panel_divider_button_text_color = text_color_hover;
            if(mouse_pressed)
                left_panel_size_drag = true;
        }
        int left_panel_new_width = left_panel_width;
        if(left_panel_size_drag)
            left_panel_new_width += mouse_delta_x;
        left_panel_new_width = Clamp(left_panel_new_width, 100, base_dim.width - 100);
        // =============================================
        
        //Holds "Navigation" and "Details"
        BoxStyle panel_title;
        panel_title.padding = {10, 10, 10, 10};
        panel_title.flow.vertical_alignment = Flow::CENTERED;
        panel_title.width = {100, Unit::PARENT_PERCENT};
        panel_title.height = {100, Unit::CONTENT_PERCENT};

        BoxStyle left_panel;
        left_panel.flow.axis = Flow::VERTICAL;
        left_panel.width = {(float)left_panel_new_width};
        left_panel.height = {100, Unit::AVAILABLE_PERCENT};
        left_panel.background_color = theme.left_panel;
        left_panel.corner_radius = theme.corner_radius;

        BoxStyle left_panel_tree;
        left_panel_tree.flow.axis = Flow::VERTICAL;
        left_panel_tree.width = {100, Unit::PARENT_PERCENT};
        left_panel_tree.height = {100, Unit::AVAILABLE_PERCENT};
        left_panel_tree.background_color = theme.left_panel;
        left_panel_tree.corner_radius = theme.corner_radius;
        left_panel_tree.scissor = true;
        left_panel_tree.padding = {0, 0, 0, theme.corner_radius};





        BoxStyle right_panel;
        right_panel.flow.axis = Flow::VERTICAL;
        right_panel.width = {100, Unit::AVAILABLE_PERCENT};
        right_panel.height = {100, Unit::AVAILABLE_PERCENT};
        right_panel.background_color = theme.right_panel;
        right_panel.corner_radius = theme.corner_radius;


        ui.BeginBox(base, "debug_view_base");

            // ===== Title bar =====
            ui.BeginBox(title_bar, "title_bar");
                ui.InsertText(StringFormat("[S:21][C:%s]Debug View", text_color), true);
            ui.EndBox();
            // =====================

            // ===== Just under the title bar =====
            ui.BeginBox(h_container);

                // ======= Left Panel =====
                ui.BeginBox(left_panel);
                    ui.BeginBox(panel_title);
                        ui.InsertText(StringFormat("[S:21][C:%s]Navigate", text_color));
                    ui.EndBox();
                    ui.BeginBox(left_panel_tree);
                        InjectTree(node, 0);
                    ui.EndBox();
                ui.EndBox();
                // ========================

                ui.BeginBox(panel_divider_button, "panel_divider_button");
                    ui.InsertText(StringFormat("[S:14][C:%s]||", panel_divider_button_text_color));
                ui.EndBox();

                // ==== Right Panel ====
                ui.BeginBox(right_panel);
                    ui.BeginBox(panel_title);
                        ui.InsertText(StringFormat("[S:21][C:%s]Details", text_color_hover), true);
                    ui.EndBox();
                ui.EndBox();
                // =====================
            ui.EndBox();
            // ====================================

        ui.EndBox();

        ui.BeginBox(resize_button, "resize_button");
        ui.EndBox();

        if(mouse_released)
        {
            window_size_drag = false;
            window_pos_drag = false;
            window_dim = base_dim;

            left_panel_scroll_drag = false;
            left_panel_size_drag = false;
            left_panel_width = left_panel_new_width;
        }
    }
    void DebugView::InjectTree(TreeNode* node, int depth)
    {
        if(node == nullptr)
            return;


        HexColor text_color = RGBAToHex(theme.text_color);
        HexColor text_color_hover = RGBAToHex(theme.text_color_hover);


        // ===== Button Properties =====
        BoxStyle button;
        button.flow.vertical_alignment = Flow::CENTERED;
        button.width = {9999};
        button.height = {100, Unit::CONTENT_PERCENT};
        button.background_color = theme.button;
        HexColor button_text_color = text_color;

        BoxStyle filler; 
        filler.width = {8};
        filler.height = {0};

        BoxStyle line;
        line.width = {1};
        line.height = {20};
        line.background_color = theme.button_hover;

        BoxStyle open_button;
        open_button.flow.horizontal_alignment = Flow::CENTERED;
        open_button.flow.vertical_alignment = Flow::CENTERED;
        open_button.width = {18};
        open_button.height = {18};
        open_button.border_width = 1;
        open_button.border_color = theme.text_color;
        open_button.corner_radius = 2;
        HexColor open_button_text_color = text_color;

        // ========== Faded Out Buttons =============== not rendered nodes
        if(!node->box.debug_valid)
        {
            button.background_color = theme.faded_button;
            line.background_color = theme.button;
            open_button.border_color = theme.text_color_hover;
            open_button_text_color = text_color_hover;
            button_text_color = text_color_hover;
        }
        // ============================================

        // ====== Highlight button of selected Node In Tree =====
        if(selected_node == node)
        {
            button.background_color = theme.button_hover;
            line.background_color = theme.button;
            open_button.border_color = theme.text_color_hover;
            open_button_text_color = text_color_hover;
            button_text_color = text_color_hover;
        }
        // ============================================

        // ===== Button Hover Logic =====
        const char* button_id = StringFormat("button_id: %d", (uintptr_t)node);
        BoxInfo button_info = ui.GetBoxInfo(button_id);
        if(button_info.valid)
        {
            if(button_info.is_direct_hover)
            {
                button.background_color = theme.button_hover;
                line.background_color = theme.button;
                open_button.border_color = theme.text_color_hover;
                open_button_text_color = text_color_hover;
                button_text_color = text_color_hover;
                // ========= Select Node ==========
                if(mouse_pressed)
                    selected_node = node;
                // ================================
            }
            if(button_info.is_hover)
            {
                // ========== Outline Node ==========
                if(node->box.debug_valid)
                {
                    hover_node.width = node->box.GetRenderingWidth();
                    hover_node.height = node->box.GetRenderingHeight();
                    hover_node.x = node->box.x;
                    hover_node.y = node->box.y;
                }
                // ===================================
            }
        }
        // =============================

        // ========== Open Button Logic ===========
        const char* open_button_id = StringFormat("open_button_id: %d", (uintptr_t)node);
        uint64_t open_button_id_key = Hash(open_button_id);
        bool* is_open = tree_state.GetValue(open_button_id_key);
        if(is_open == nullptr)
            is_open = tree_state.Insert(open_button_id_key, false);
        BoxInfo open_button_info = ui.GetBoxInfo(open_button_id);
        if(open_button_info.valid && open_button_info.is_direct_hover)
        {
            open_button.border_color = theme.text_color_hover;
            open_button_text_color = text_color_hover;
            open_button.background_color = theme.button_hover;
            open_button.border_color = theme.button_hover;
            if(is_open && mouse_pressed)
                *is_open = !*is_open;
        }

        //Button Icon
        char open_icon = '+';
        if(is_open && *is_open)
        {
            open_icon = '-';
            open_button.background_color = {130, 255, 130, 255};
        }
        // ========================================

        // ========== Button Text ==========
        HexColor id_text_color = RGBAToHex(theme.id_text_color);
        HexColor inserted_text_color = RGBAToHex(theme.inserted_text_color);
        const Box& box = node->box;
        const char* button_text = nullptr;
        if(box.text)
        {
            button_text = StringFormat("[S:20][C:%s]Text [C:%s][OFF]\"%s\"", button_text_color, inserted_text_color, box.text);
        }
        else if(box.debug_label)
        {
            button_text = StringFormat("[S:20][C:%s]Box id:[C:%s][OFF] \"%s\"", button_text_color, id_text_color, box.debug_label);
        }
        else
        {
            button_text = StringFormat("[S:20][C:%s]Box", button_text_color);
        }
        // =================================


        ui.BeginBox(button, button_id);
        for(int i = 0; i < depth; i++)
        {
            ui.BeginBox(filler); ui.EndBox();
            ui.BeginBox(line); ui.EndBox();
            ui.BeginBox(filler); ui.EndBox();
        }
        // ========== Open [+] Button =========
        if(!node->children.IsEmpty())
        {
            ui.BeginBox(open_button, open_button_id);
                ui.InsertText(StringFormat("[S:20][C:%s]%c", open_button_text_color, open_icon), true);
            ui.EndBox();
        }// ===================================

            ui.InsertText(button_text, true);
        ui.EndBox();

        if(is_open && *is_open)
        {
            for(auto temp = node->children.GetHead(); temp != nullptr; temp = temp->next)
            {
                InjectTree(&temp->value, depth + 1);
            }
        }

    }
    bool DebugView::SearchNodeAndOpenTree(TreeNode* root)
    {
        if(root == nullptr)
            return false;
        if(root == selected_node)
            return true;
        bool has_node = false;
        for(auto temp = root->children.GetHead(); temp != nullptr; temp = temp->next)
        {
            bool result = SearchNodeAndOpenTree(&temp->value);
            if(result)
                has_node = true;
        }


        const char* open_button_id = StringFormat("open_button_id: %d", (uintptr_t)root);
        uint64_t open_button_id_key = Hash(open_button_id);
        if(has_node)
        {
            tree_state.Insert(open_button_id_key, true);
            return true;
        }
        else
        {
            tree_state.Insert(open_button_id_key, false);
            return false;
        }
    }

}










namespace UI
{
    Context::Context(uint64_t arena_bytes) :
        arena(arena_bytes)
    {
    }
    void Context::SetFreeze(bool freeze_ui)
    {


    }
    Internal::TreeNode* Context::GetInternalTree()
    {
        if(!stack.IsEmpty())
            return nullptr;
        return root_node;
    }
    uint32_t Context::GetElementCount() const
    {
        return element_count;
    }
    bool Context::HasInternalError()
    {
        return internal_error.type != Error::Type::NO_ERROR;
    }
    bool Context::HandleInternalError(const Error& error)
    {
        if(error.type != Error::Type::NO_ERROR)
        {
            internal_error = error;
            DisplayError(error);
            return true;
        }
        return false;
    }
    BoxInfo Context::GetBoxInfo(const char* label)
    {
        uint64_t key = Hash(label);
        BoxInfo* info = double_buffer_map.FrontValue(key);
        if(info)
        {
            info->valid = true;
            if(directly_hovered_element_key == key)
                info->is_direct_hover = true;
            return *info;
        }
        return BoxInfo();
    }
    void Context::BeginRoot(int x, int y, unsigned int screen_width, unsigned int screen_height, int mouse_x, int mouse_y)
    {
        this->mouse_x = mouse_x;
        this->mouse_y = mouse_y;
        if(HasInternalError())
            return;

        double_buffer_map.SwapBuffer();
        arena.Rewind(root_node);
        stack.Clear();
        root_node = nullptr;
        element_count = 1;
        if(double_buffer_map.ShouldResize())
        {
            arena.Reset();
            //Double the capacity for hash map
            uint32_t capacity = double_buffer_map.Capacity() * 2;
            capacity = capacity? capacity: 512;
            double_buffer_map.AllocateBufferCapacity(capacity, &arena);
        }

        assert(stack.IsEmpty());
        Box root_box;
        root_box.width = screen_width;
        root_box.height = screen_height;
        root_box.x = x;
        root_box.y = y;
        
        // ========== Debug Mode Only ==========
        #if UI_ENABLE_DEBUG
            BoxStyle style;
            style.width = {(float)screen_width};
            style.height = {(float)screen_height};
            style.x = {(float)x};
            style.y = {(float)y};
            root_box.debug_style = style;
        #endif

        if(stack.IsEmpty())//Root Node
        {
            //Checking errors unique to root node
            root_node = arena.New<TreeNode>();
            assert(root_node && "Arena out of space");

            //compute 
            root_node->box = root_box;
            stack.Push(root_node);
        }
        else
        {
            assert(0);
            HandleInternalError(Error{Error::Type::ROOT_NODE_CONTRADICTION, "There can only be 1 Root node"});
            return;
        }
    }
    void Context::EndRoot()
    {
        if(HasInternalError())
            return;
        if(stack.Size() == 1)
        {
            stack.Pop();
        }
        else if(stack.Size() < 1)
        {
            HandleInternalError(Error{Error::Type::ROOT_NODE_CONTRADICTION, "More than one RootEnd() function"});
            return;
        }
        else
        {
            HandleInternalError(Error{Error::Type::MISSING_END, "Missing EndBox()"});
            return;
        }
    }

    void Context::BeginBox(const UI::BoxStyle& style, const char* label, DebugInfo debug_info)
    {
        if(HasInternalError())
            return;
        element_count++;

        //Check for unit type errors
        //Could be moved into creating style sheets for more performance, but this is good enough for now

        //Input Handling
        uint64_t label_hash = Hash(label);
        if(label)
        {
            //BoxInfo will be filled in next frame 
            bool err = double_buffer_map.Insert(label_hash, BoxInfo());
            assert(err && "HashMap out of memory, go to BeginRoot and change it");
        }

        if(!stack.IsEmpty())  // should add to parent
        {
            TreeNode* parent_node = stack.Peek();
            assert(parent_node);
            assert(root_node);

            TreeNode child_node;
            child_node.box = ComputeStyleSheet(style, root_node->box);
            child_node.box.label_hash = label_hash;

            // ========= Only enabled with DEBUG mode ==========
            #if UI_ENABLE_DEBUG
                child_node.box.debug_file = debug_info.file;
                child_node.box.debug_line = debug_info.line;
                child_node.box.debug_style = style;
                if(label)
                {
                    int label_length = StringLength(label) + 1;
                    char* debug_label = arena.New<char>(label_length);
                    assert(debug_label && "Arena out of memory");
                    StringCopy(debug_label, label, label_length);
                    child_node.box.debug_label = debug_label;
                }
            #endif
            //Might bundle this with macro
            if(HandleInternalError(CheckUnitErrors(child_node.box)))
                return;

            TreeNode* child_ptr = parent_node->children.Add(child_node, &arena); 
            assert(child_ptr && "Arena out of memory");
            stack.Push(child_ptr);
        }
        else
        {
            if(HandleInternalError(Error{Error::Type::ROOT_NODE_CONTRADICTION, "Missing BeginRoot()"}))
                return;
        }
    }

    void Context::EndBox()
    {
        if(HasInternalError())
            return;
        if(stack.Size() <= 1)
        {
            HandleInternalError(Error{Error::Type::MISSING_BEGIN, "Missing BeginBox()"});
            return;
        }
        TreeNode* node = stack.Peek();
        assert(node);
        Box& parent_box = node->box;
        if(node->children.IsEmpty())
        {
            //if(HandleInternalError(CheckLeafNodeContradictions(parent_box)))
            //    return;
            if(parent_box.width_unit == Unit::CONTENT_PERCENT)
            {
                parent_box.width = 1;
                parent_box.width_unit = Unit::PIXEL;
            }
            if(parent_box.height_unit == Unit::CONTENT_PERCENT)
            {
                parent_box.height = 1;
                parent_box.height_unit = Unit::PIXEL;
            }
        }
        else //Calculate all CONTENT_PERCENT
        {
            ArenaLL<TreeNode>::Node* child = node->children.GetHead();  
            int content_width = 0;
            //only handles content_percent for width
            //This is done for dynamic text wrapping with different Unit types
            if(parent_box.width_unit == Unit::Type::CONTENT_PERCENT)
            {
                if(parent_box.GetLayout() == Layout::FLOW)
                {
                    if(parent_box.GetFlowAxis() == Flow::Axis::HORIZONTAL)
                    {
                        for(;child != nullptr; child = child->next)
                        {
                            Box& box = child->value.box;

                            if(!box.IsDetached())
                            {
                                box.width = Clamp(box.width, box.min_width, box.max_width);
                                content_width += box.GetBoxModelWidth() + parent_box.gap_column;
                            }
                        }
                        content_width -= parent_box.gap_column;
                    }
                    else //VERTICAL
                    {
                        int largest_width = 0;
                        for(;child != nullptr; child = child->next)
                        {
                            Box& box = child->value.box;
                            if(!box.IsDetached())
                            {
                                int width = box.GetBoxModelWidth();
                                if(largest_width < width)
                                    largest_width = width;
                            }
                        }
                        content_width = largest_width;
                    }
                    if(parent_box.width_unit == Unit::Type::CONTENT_PERCENT)
                        parent_box.width = content_width * parent_box.width / 100;
                    if(parent_box.min_width_unit == Unit::Type::CONTENT_PERCENT)
                        parent_box.min_width = content_width * parent_box.min_width / 100;
                    if(parent_box.max_width_unit == Unit::Type::CONTENT_PERCENT)
                        parent_box.max_width = content_width * parent_box.max_width / 100;
                }
                else //Grid 
                {
                    assert(0 && "Have not added grid");
                }
            }
        }
        stack.Pop();
        if(!stack.IsEmpty())
        {
            TreeNode* grand_parent = stack.Peek();
            assert(grand_parent);
            if(HandleInternalError(CheckNodeContradictions(parent_box, grand_parent->box)))
                return;
        }

    }



    void Context::InsertText(const char* text, bool should_copy, DebugInfo debug_info)
    {
        if(HasInternalError())
            return;
        if(stack.IsEmpty())
        {
            HandleInternalError(Error{Error::Type::TEXT_NODE_CONTRADICTION, "Text node needs a container"});
            return;
        }
        TreeNode* parent_node = stack.Peek();
        assert(parent_node);
        const Box& parent_box = parent_node->box;
        TreeNode text_node;
        Box box;

        //DEBUGGING TEXT
        //box.background_color = {0, 0, 0, 50};

        #if UI_ENABLE_DEBUG
            box.debug_file = debug_info.file;
            box.debug_line = debug_info.line;
        #endif

        if(should_copy && text)
        {
            int len = StringLength(text);
            char* text_copy = (char*)arena.Allocate(len + 1);
            if(!text_copy)
                assert(0 && "Arena out of memory");
            StringCopy(text_copy, text, len + 1); 
            text = text_copy;
        }
        box.text = text;


        Markdown md;
        md.SetInput(text, 9999, 9999);
        while(md.ComputeNextTextRun()){}

        if(parent_box.width_unit == Unit::Type::CONTENT_PERCENT)
        {
            box.width = md.GetMeasuredWidth() + 1;
        }
        else
        {
            box.width_unit = Unit::Type::AVAILABLE_PERCENT;
            box.width = 100;
            box.max_width = md.GetMeasuredWidth() + 1;
        }



        text_node.box = box;
        TreeNode* addr = parent_node->children.Add(text_node, &arena);
        assert(addr && "Arena out of memory");
    }



    void Context::Draw()
    {
        if(HasInternalError())
            return;
        
        if(!stack.IsEmpty())
        {
            HandleInternalError(Error{Error::Type::ROOT_NODE_CONTRADICTION, "Missing EndRoot()"});
            return;
        }
        const Box& root_box = root_node->box;


        //Layout pipeline
        //WidthContentPercentPass happens during EndBox()

        WidthPass(root_node);
        HeightContentPercentPass(root_node);
        HeightPass(root_node);
        DrawPass(root_node, 0, 0, Box(), Rect{root_box.x, root_box.y, root_box.width, root_box.height});

        while(!deferred_elements.IsEmpty())
        {
            TreeNode* node = deferred_elements.GetHead()->value;
            DrawPass(node, 0, 0, Box(), UI::Rect{0, 0, INT_MAX, INT_MAX});
            deferred_elements.PopHead();
        }
    }
    void Context::WidthPass(TreeNode* node)
    {
        if(node == nullptr || node->children.IsEmpty())
            return;
        Box& box = node->box;

        //Might aswell compute this here since width is all calculated
        ComputeWidthPercentForHeight(box);

        if(box.GetLayout() == Layout::FLOW)
        {
            WidthPass_Flow(node->children.GetHead(), box);
        }
        else
        {
            assert("have not added grid");
        }
    }

    void Context::WidthPass_Flow(ArenaLL<TreeNode>::Node* child, const Box& parent_box)
    {
        assert(child);
        ArenaLL<TreeNode>::Node* temp;

        struct GrowBox {
            Box* box = nullptr;
            float result = 0;
        };
        ArenaLL<GrowBox> growing_elements;

        if(parent_box.GetFlowAxis() == Flow::Axis::HORIZONTAL)
        {
            float available_width = parent_box.width;
            float total_percent = 0;
            for(temp = child; temp != nullptr; temp = temp->next)
            {
                Box& box = temp->value.box;
                ComputeParentWidthPercent(box, parent_box.width);

                if(box.width_unit != Unit::Type::AVAILABLE_PERCENT)
                {
                    box.width = Clamp(box.width, box.min_width, box.max_width);

                    //detached boxes cannot be AVAILABLE_PERCENT
                    if(!box.IsDetached()) //Ignore detached boxes
                        available_width -= box.GetBoxModelWidth() + parent_box.gap_column;
                }
                else
                {
                    bool err = (bool)growing_elements.Add(GrowBox{&box, 0}, &arena);
                    assert(err && "Arena out of memory");
                    available_width -= box.GetBoxExpansionWidth() + parent_box.gap_column;
                    total_percent += box.width;
                }
            }
            available_width += parent_box.gap_column;

            # if 1
            float remaining_space = available_width;
            if(total_percent < 100.0f)
                remaining_space = remaining_space * (total_percent / 99.99f);

            for(auto node = growing_elements.GetHead(); node != nullptr; node = node->next)
                remaining_space -= node->value.box->min_width;

            while(remaining_space > 0)
            {
                int total_percent_below_min = 0;
                int total_percent_in_bounds = 0;
                for(auto node = growing_elements.GetHead(); node != nullptr; node = node->next)
                {
                    GrowBox& b = node->value;
                    if(b.result >= b.box->min_width && b.result < b.box->max_width) 
                        total_percent_in_bounds += b.box->width;
                    if(b.result < b.box->min_width)
                        total_percent_below_min += b.box->width;
                }
                int total_p = total_percent_in_bounds? total_percent_in_bounds: total_percent_below_min;
                float min_p = 1.0f;
                for(auto node = growing_elements.GetHead(); node != nullptr; node = node->next)
                {
                    GrowBox& b = node->value;
                    float normalized_weight = (float)total_p / (remaining_space * b.box->width);
                    float p1 = ((float)b.box->min_width - b.result) * normalized_weight;
                    float p2 = ((float)b.box->max_width - b.result) * normalized_weight;
                    min_p = b.result < b.box->min_width? Min(p1, min_p): min_p;
                    min_p = b.result < b.box->max_width? Min(p2, min_p): min_p;
                }
                if(!total_percent_in_bounds)
                {
                    for(auto node = growing_elements.GetHead(); node != nullptr; node = node->next)
                        if(node->value.result < node->value.box->min_width)
                            node->value.result += min_p * remaining_space * ((float)node->value.box->width / total_p);
                }
                else
                {
                    float total_delta = 0;
                    for(auto node = growing_elements.GetHead(); node != nullptr; node = node->next)
                    {
                        if(node->value.result < node->value.box->max_width)
                        {
                            float normalized_weight = (float)node->value.box->width / total_p;
                            float delta = min_p * remaining_space * normalized_weight;
                            if(node->value.result >= node->value.box->min_width)
                                total_delta += delta;
                            node->value.result += delta;
                        }
                    }
                    remaining_space -= total_delta;
                }
                if(total_p == 0)
                    break;
            }

            #endif

            #if 0 //Available Percent version only with MAX values working
            while(true) 
            {
                int total_p = 0;
                for(auto node = growing_elements.GetHead(); node != nullptr; node = node->next)
                {
                    GrowBox& b = node->value;
                    if(b.result < b.box->max_width)
                    {
                        total_p += b.box->width;
                    }
                }
                //available_width = total_p < 100? available_width * total_p / 100: available_width; //this did not work
                float min_p = 1;
                for(auto node = growing_elements.GetHead(); node != nullptr; node = node->next)
                {
                    GrowBox& b = node->value;
                    if(b.result < b.box->max_width)
                    {
                        float p = (float)(b.box->max_width - b.result) * total_p / (available_width * b.box->width);
                        min_p = Min(p, min_p);
                    }
                }
                float total_delta = 0;
                for(auto node = growing_elements.GetHead(); node != nullptr; node = node->next)
                {
                    GrowBox& b = node->value;
                    if(b.result < b.box->max_width)
                    {
                        float delta = min_p * available_width * b.box->width / total_p;
                        b.result += delta;
                        total_delta += delta;
                    }
                }
                available_width -= total_delta;
                if(total_delta == 0)
                    break;
            }
            #endif
            for(auto node = growing_elements.GetHead(); node != nullptr; node = node->next)
                node->value.box->width = Max(node->value.box->min_width, (uint16_t)node->value.result);
            arena.Rewind(growing_elements.GetHead());
            growing_elements.Clear();

            //Sets all final sizes
            for(temp = child; temp != nullptr; temp = temp->next)
            {
                WidthPass(&temp->value);
            }

        } //End Horizontal
        else // Compute Vertical layout in height pass
        {
            for(temp = child; temp != nullptr; temp = temp->next)
            {
                Box& box = temp->value.box;
                if(box.width_unit == Unit::Type::AVAILABLE_PERCENT)
                    box.width_unit = Unit::Type::PARENT_PERCENT;
                ComputeParentWidthPercent(box, parent_box.width);
                box.width = Clamp(box.width, box.min_width, box.max_width);
                WidthPass(&temp->value);
            }
        } //End vertical
    }



    void Context::HeightPass(TreeNode* node)
    {
        if(node == nullptr || node->children.IsEmpty())
            return;
        const Box& box = node->box;
        if(box.GetLayout() == Layout::FLOW)
        {
            HeightPass_Flow(node->children.GetHead(), box);
        }
        else
        {
            assert("have not added grid");
        }
    }
    void Context::HeightPass_Flow(ArenaLL<TreeNode>::Node* child, const Box& parent_box)
    {
        assert(child);
        ArenaLL<TreeNode>::Node* temp;

        struct GrowBox {
            Box* box = nullptr;
            float result = 0;
        };
        ArenaLL<GrowBox> growing_elements;

        if(parent_box.GetFlowAxis() == Flow::Axis::VERTICAL)
        {
            float available_height = parent_box.height;
            float total_percent = 0;
            for(temp = child; temp != nullptr; temp = temp->next)
            {
                Box& box = temp->value.box;
                ComputeParentHeightPercent(box, parent_box.height);

                if(box.height_unit != Unit::Type::AVAILABLE_PERCENT)
                {
                    box.height = Clamp(box.height, box.min_height, box.max_height);

                    //detached boxes cannot be AVAILABLE_PERCENT
                    if(!box.IsDetached()) //Ignore detached boxes
                        available_height -= box.GetBoxModelHeight() + parent_box.gap_row;
                }
                else
                {
                    bool err = growing_elements.Add(GrowBox{&box, 0}, &arena);
                    assert(err && "Arena out of memory");
                    available_height -= box.GetBoxExpansionHeight() + parent_box.gap_row;
                    total_percent += box.height;
                }
            }
            available_height += parent_box.gap_row;

            float remaining_space = available_height;
            if(total_percent < 100.0f)
                remaining_space = remaining_space * (total_percent / 99.99f);

            for(auto node = growing_elements.GetHead(); node != nullptr; node = node->next)
                remaining_space -= node->value.box->min_height;

            while(remaining_space > 0)
            {
                int total_percent_below_min = 0;
                int total_percent_in_bounds = 0;
                for(auto node = growing_elements.GetHead(); node != nullptr; node = node->next)
                {
                    GrowBox& b = node->value;
                    if(b.result >= b.box->min_height && b.result < b.box->max_height) 
                        total_percent_in_bounds += b.box->height;
                    if(b.result < b.box->min_height)
                        total_percent_below_min += b.box->height;
                }
                int total_p = total_percent_in_bounds? total_percent_in_bounds: total_percent_below_min;
                float min_p = 1.0f;
                for(auto node = growing_elements.GetHead(); node != nullptr; node = node->next)
                {
                    GrowBox& b = node->value;
                    float normalized_weight = (float)total_p / (remaining_space * b.box->height);
                    float p1 = ((float)b.box->min_height - b.result) * normalized_weight;
                    float p2 = ((float)b.box->max_height - b.result) * normalized_weight;
                    min_p = b.result < b.box->min_height? Min(p1, min_p): min_p;
                    min_p = b.result < b.box->max_height? Min(p2, min_p): min_p;
                }
                if(!total_percent_in_bounds)
                {
                    for(auto node = growing_elements.GetHead(); node != nullptr; node = node->next)
                        if(node->value.result < node->value.box->min_height)
                            node->value.result += min_p * remaining_space * ((float)node->value.box->height / total_p);
                }
                else
                {
                    float total_delta = 0;
                    for(auto node = growing_elements.GetHead(); node != nullptr; node = node->next)
                    {
                        if(node->value.result < node->value.box->max_height)
                        {
                            float normalized_weight = (float)node->value.box->height / total_p;
                            float delta = min_p * remaining_space * normalized_weight;
                            if(node->value.result >= node->value.box->min_height)
                                total_delta += delta;
                            node->value.result += delta;
                        }
                    }
                    remaining_space -= total_delta;
                }
                if(total_p == 0)
                    break;
            }
            for(auto node = growing_elements.GetHead(); node != nullptr; node = node->next)
                node->value.box->height = Max(node->value.box->min_height, (uint16_t)node->value.result);
            arena.Rewind(growing_elements.GetHead());
            growing_elements.Clear();

            //Sets all final sizes
            for(temp = child; temp != nullptr; temp = temp->next)
            {
                HeightPass(&temp->value);
            }

        } //End Vertical
        else // Horizontal
        {
            for(temp = child; temp != nullptr; temp = temp->next)
            {
                Box& box = temp->value.box;
                if(box.height_unit == Unit::Type::AVAILABLE_PERCENT)
                    box.height_unit = Unit::Type::PARENT_PERCENT;
                ComputeParentHeightPercent(box, parent_box.height);
                box.height = Clamp(box.height, box.min_height, box.max_height);
                HeightPass(&temp->value);
            }
        }
    }


    void Context::HeightContentPercentPass_Flow(TreeNode* node)
    {
        assert(node);
        Box& parent_box = node->box;
        ArenaLL<TreeNode>::Node* child = node->children.GetHead();
        int content_height = 0;
        if(parent_box.GetFlowAxis() == Flow::Axis::HORIZONTAL)
        {
            int largest_height = 0;
            for(ArenaLL<TreeNode>::Node* temp = child; temp != nullptr; temp = temp->next)
            {
                HeightContentPercentPass(&temp->value);
                Box& box = temp->value.box;
                
                if(box.IsDetached()) //Ignore layout for detached boxes
                    continue;

                if(box.text)
                {
                    Markdown md;
                    //Width should be computed by this point
                    box.width = Min(parent_box.width, box.width);
                    md.SetInput(box.text, box.width, INT_MAX);
                    while(md.ComputeNextTextRun()){}
                    box.height = md.GetMeasuredHeight();
                }
                int height = box.GetBoxModelHeight();
                if(largest_height < height)
                    largest_height = height;
            }
            content_height = largest_height;
        }
        else //Vertical
        {
            for(ArenaLL<TreeNode>::Node* temp = child; temp != nullptr; temp = temp->next)
            {
                HeightContentPercentPass(&temp->value);
                Box& box = temp->value.box;

                if(box.IsDetached()) //Ignore layout for detached boxes
                    continue;

                if(box.text)
                {
                    Markdown md;
                    //Width should be computed by this point
                    box.width = Min(parent_box.width, box.width);
                    md.SetInput(box.text, box.width, INT_MAX);
                    while(md.ComputeNextTextRun()){}
                    box.height = md.GetMeasuredHeight();
                }
                content_height += box.GetBoxModelHeight() + parent_box.gap_row;
            }
            content_height -= parent_box.gap_row;
        }
        if(parent_box.height_unit == Unit::Type::CONTENT_PERCENT)
            parent_box.height = parent_box.height * content_height / 100;
        if(parent_box.min_height_unit == Unit::Type::CONTENT_PERCENT)
            parent_box.min_height = parent_box.min_height * content_height / 100;
        if(parent_box.max_height_unit == Unit::Type::CONTENT_PERCENT)
            parent_box.max_height = parent_box.max_height * content_height / 100;
    }

    void Context::HeightContentPercentPass(TreeNode* node)
    {
        if(!node)    
            return;
        assert(node);
        const Box& box = node->box;
        if(box.GetLayout() == Layout::FLOW)
        {
            HeightContentPercentPass_Flow(node);
        }
        else
        {
            assert(0 && "Have not added grid yet");
        }
    }



    void Context::DrawPass(TreeNode* node, int x, int y, const Box& parent_box, Rect parent_aabb)
    {
        if(node == nullptr)
            return;
        Box& box = node->box;

        //Render
        int render_x = box.GetPositioning() == UI::Positioning::ABSOLUTE? box.x : box.x + x;
        int render_y = box.GetPositioning() == UI::Positioning::ABSOLUTE? box.y : box.y + y;
        int render_width =    box.GetRenderingWidth();
        int render_height =   box.GetRenderingHeight();
        int corner_radius =   box.corner_radius;
        int border_size =     box.border_width;
        Color border_c =        box.border_color;
        Color bg_c =            box.background_color;
        if(parent_box.IsScissor())
        {
            if(!Rect::Overlap(parent_aabb, Rect{render_x, render_y, render_width, render_height}))
            {
                return;
            }
            else
            {
                BeginScissorMode_impl(parent_aabb.x, parent_aabb.y, parent_aabb.width, parent_aabb.height);
            }
        }
        DrawRectangle_impl(render_x, render_y, render_width, render_height, corner_radius, border_size, border_c, bg_c);
        box.x = render_x;
        box.y = render_y;
        box.SetPositioning(Positioning::ABSOLUTE);

        #if UI_ENABLE_DEBUG
            box.debug_valid = true;
        #endif
        if(box.text)
        {
            DrawTextNode(box.text, box.width, box.height, render_x, render_y);
        }

        //Handling Next Frame info
        if(box.label_hash != 0)
        {
            BoxInfo* info = double_buffer_map.BackValue(box.label_hash);
            if(info)
            {
                info->draw_width = render_width;
                info->draw_height = render_height;
                info->draw_x = render_x;
                info->draw_y = render_y;
                //Handling mouse hover next frame
                if(Rect::Contains(Rect::Intersection(parent_aabb, Rect{render_x, render_y, render_width, render_height}), mouse_x, mouse_y))
                {
                    directly_hovered_element_key = box.label_hash;
                    info->is_hover = true;
                }
                else if(directly_hovered_element_key == box.label_hash)
                {
                    directly_hovered_element_key = 0;
                }
            }
        }

        if(node->children.IsEmpty())
            return;
        //Next Recurse
        if(box.GetLayout() == Layout::FLOW)
        {
            DrawPass_FlowNoWrap(node->children.GetHead(), box, render_x, render_y, parent_aabb);
        }
        else
        {
            assert("Grid has not been added yet");
        }

    } //end of DrawPass()

    void Context::DrawPass_FlowNoWrap(ArenaLL<TreeNode>::Node* child, const Box& parent_box, int x, int y, Rect parent_aabb)
    {
        ArenaLL<TreeNode>::Node* temp = child;
        assert(temp);
        if(parent_box.IsScissor())
        {
            //initially is grandparent_aabb
            parent_aabb = Rect::Intersection(parent_aabb, Rect{x + parent_box.padding.left, y + parent_box.padding.top, parent_box.width, parent_box.height});
        }
        //Horizontal
        if(parent_box.GetFlowAxis() == Flow::Axis::HORIZONTAL)
        {
            int count = 0;
            int content_width = 0;
            for(temp = child; temp!=nullptr; temp = temp->next)
            {
                const Box& box = temp->value.box;

                if(box.IsDetached()) //Ignore layout for detached boxes
                    continue;

                count++;
                content_width += box.GetBoxModelWidth();
            }
            content_width = count? content_width + (count-1) * parent_box.gap_column: content_width;

            int cursor_x = 0;
            int offset_x = 0;
            int available_width = parent_box.width - content_width;
            //handling alignment 
            switch(parent_box.flow_horizontal_alignment) //START, END, CENTERED, SPACE_AROUND, SPACE_BETWEEN
            {
                case Flow::Alignment::START:
                    cursor_x = 0;
                    break;
                case Flow::Alignment::END:
                    cursor_x = parent_box.width - content_width;
                    break;
                case Flow::Alignment::CENTERED:
                    cursor_x = available_width/2;
                    break;
                case Flow::Alignment::SPACE_AROUND:
                    offset_x = available_width/(count + 1);
                    cursor_x = offset_x;
                    break;
                case Flow::Alignment::SPACE_BETWEEN:
                    if(count > 1)
                        offset_x = available_width/(count - 1);
                    break;
            }
            int content_height = 0;
            for(temp = child; temp!=nullptr; temp = temp->next)
            {
                const Box& box = temp->value.box;

                if(box.IsDetached()) //Ignore layout for detached boxes
                {
                    deferred_elements.Add(&temp->value, &arena);
                    continue;
                }

                int cursor_y = 0;

                //Computing content_height
                int box_model_height = box.GetBoxModelHeight();
                if(content_height < box_model_height)
                    content_height = box_model_height;

                //Handling alignment
                switch(parent_box.flow_vertical_alignment) //START, END, CENTERED, SPACE_AROUND, SPACE_BETWEEN
                {
                    case Flow::Alignment::START:
                        cursor_y = 0;
                        break;
                    case Flow::Alignment::END:
                        cursor_y = parent_box.height - box_model_height;
                        break;
                    default:
                        cursor_y = parent_box.height/2 - box_model_height/2;
                        break;
                }
                int layout_x =        x + cursor_x - parent_box.scroll_x + box.margin.left + parent_box.padding.left;
                int layout_y =        y + cursor_y - parent_box.scroll_y + box.margin.top + parent_box.padding.top;
                DrawPass(&temp->value, layout_x, layout_y, parent_box, parent_aabb);
                cursor_x += box.GetBoxModelWidth() + parent_box.gap_column + offset_x;
            }
            if(parent_box.label_hash != 0) //next frame info
            {
                BoxInfo* info = double_buffer_map.BackValue(parent_box.label_hash);
                if(info)
                {
                    info->content_width = content_width;
                    info->content_height = content_height;
                }
            }
        }
        else //Vertical
        {
            int count = 0;
            int content_height = 0;
            for(temp = child; temp!=nullptr; temp = temp->next)
            {
                const Box& box = temp->value.box;

                if(box.IsDetached()) //Ignore layout for detached boxes
                    continue;

                count++;
                content_height += box.GetBoxModelHeight();
            }
            content_height = count? content_height + (count-1) * parent_box.gap_row: content_height;

            int cursor_y = 0;
            int offset_y = 0;
            int available_height = parent_box.height - content_height;
            switch(parent_box.flow_vertical_alignment) //START, END, CENTERED, SPACE_AROUND, SPACE_BETWEEN
            {
                case Flow::Alignment::START:
                    cursor_y = 0;
                    break;
                case Flow::Alignment::END:
                    cursor_y = parent_box.height - content_height;
                    break;
                case Flow::Alignment::CENTERED:
                    cursor_y = available_height/2;
                    break;
                case Flow::Alignment::SPACE_AROUND:
                    offset_y = available_height/(count + 1);
                    cursor_y = offset_y;
                    break;
                case Flow::Alignment::SPACE_BETWEEN:
                    if(count > 1)
                        offset_y = available_height/(count - 1);
                    break;
            }
            int content_width = 0;
            for(temp = child; temp!=nullptr; temp = temp->next)
            {
                const Box& box = temp->value.box;

                if(box.IsDetached()) //Ignore layout for detached boxes
                {
                    deferred_elements.Add(&temp->value, &arena);
                    continue;
                }

                int cursor_x = 0;


                //Computing content_width
                int box_model_width = box.GetBoxModelWidth();
                if(content_width < box_model_width)
                    content_width = box_model_width;
                
                //Handling alignment
                switch(parent_box.flow_horizontal_alignment) //START, END, CENTERED, SPACE_AROUND, SPACE_BETWEEN
                {
                    case Flow::Alignment::START:
                        cursor_x = 0;
                        break;
                    case Flow::Alignment::END:
                        cursor_x = parent_box.width - box_model_width;
                        break;
                    default:
                        cursor_x = parent_box.width/2 - box_model_width/2;
                        break;
                }
                int layout_x =        x + cursor_x - parent_box.scroll_x + box.margin.left + parent_box.padding.left;
                int layout_y =        y + cursor_y - parent_box.scroll_y + box.margin.top + parent_box.padding.top;
                DrawPass(&temp->value, layout_x, layout_y, parent_box, parent_aabb);
                cursor_y += box.GetBoxModelHeight() + parent_box.gap_row + offset_y;
            }
            if(parent_box.label_hash != 0) //next frame info
            {
                BoxInfo* info = double_buffer_map.BackValue(parent_box.label_hash);
                if(info)
                {
                    info->content_width = content_width;
                    info->content_height = content_height;
                }
            }
        }
        if(parent_box.IsScissor())
            EndScissorMode_impl();
    } //End of DrawPass_FlowNoWrap()



}