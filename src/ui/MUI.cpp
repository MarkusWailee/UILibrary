#include "MUI.hpp"
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
}


//GLOBALS
namespace UI
{
    float dpi = 96.0f;
    Context* context = nullptr;
}



//IMPLEMENTATION
namespace UI
{
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
        if(context)
            return context->GetBoxInfo(label);
        return BoxInfo();
    }
    void BeginRoot(unsigned int screen_width, unsigned int screen_height, int mouse_x, int mouse_y)
    {
        if(context)
            context->BeginRoot(screen_width, screen_height, mouse_x, mouse_y);
    }
    void EndRoot()
    {
        if(context)
            context->EndRoot();
    }
    void BeginBox(const UI::BoxStyle& box_style, const char* label, DebugInfo debug_info)
    {
        if(context)
            context->BeginBox(box_style, label, debug_info);
    }
    void InsertText(const char* text, bool copy_text)
    {
        if(context)
            context->InsertText(text, copy_text);
    }
    void EndBox()
    {
        if(context)
            context->EndBox();
    }
    void Draw()
    {
        if(context)
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
        constexpr uint32_t MAX_BUFFERS = 4;
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
            else if(should_ignore)
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
            if(c == '\0')
                break;
            else if(should_ignore)
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
            else if(c == ' ')
                break;
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
}










namespace UI
{
    Context::Context(uint64_t arena_bytes) :
        arena(arena_bytes)
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
    void Context::BeginRoot(unsigned int screen_width, unsigned int screen_height, int mouse_x, int mouse_y)
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
            capacity = capacity? capacity: 64;
            double_buffer_map.AllocateBufferCapacity(capacity, &arena);
        }

        assert(stack.IsEmpty());
        Box root_box;
        root_box.width = screen_width;
        root_box.height = screen_height;

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
            assert(double_buffer_map.Insert(label_hash, BoxInfo()));
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
            if(HandleInternalError(CheckLeafNodeContradictions(parent_box)))
                return;
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

                            if(parent_box.width_unit == Unit::Type::CONTENT_PERCENT && !box.IsDetached())
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
                            if(parent_box.width_unit == Unit::Type::CONTENT_PERCENT && !box.IsDetached())
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



    void Context::InsertText(const char* text, bool should_copy)
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
        DrawPass(root_node, 0, 0, Box(), Rect{0, 0, root_box.width, root_box.height});

        while(!deferred_elements.IsEmpty())
        {
            TreeNode* node = deferred_elements.GetHead()->value;
            DrawPass(node, 0, 0, Box(), UI::Rect{0, 0, INT_MAX, INT_MAX});
            deferred_elements.PopHead();
        }
        //DrawRectangle_impl(debug_hover.x, debug_hover.y, debug_hover.width, debug_hover.height, 0, 2, UI::Color{255, 0, 0, 255}, UI::Color{0, 0, 0, 0});
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
                    assert(growing_elements.Add(GrowBox{&box, 0}, &arena) && "Arena out of memory");
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
                    assert(growing_elements.Add(GrowBox{&box, 0}, &arena) && "Arena out of memory");
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

        //#if UI_ENABLE_DEBUG
        //    if(Rect::Contains(Rect::Intersection(parent_aabb, Rect{x, y, render_width, render_height}), mouse_x, mouse_y))
        //    {
        //        debug_hover = Rect{render_x, render_y, render_width, render_height};
        //    }
        //#endif
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
            parent_aabb = Rect::Intersection(parent_aabb, Rect{x + parent_box.padding.left, y + parent_box.padding.bottom, parent_box.width, parent_box.height});
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