#include "ui.hpp"
#include "Memory.hpp"

namespace UI
{
    using namespace Internal;
    void DisplayError(const Error& error);
    Error CheckUnitErrors(const BoxInternal& style);
    Error CheckLeafNodeContradictions(const BoxInternal& leaf);
    Error CheckNodeContradictions(const BoxInternal& child, const BoxInternal& parent);

    //Used during tree descending
    int FixedUnitToPx(Unit unit, int root_size);
    BoxInternal ComputeStyleSheet(const BoxStyle& style, const BoxInternal& root);

    //Size should include '\0'
    void StringCopy(char* dst, const char* src, uint32_t size);
    //returns pointer pointing to arena
    const char* ArenaCopyString(const char* text, MemoryArena* arena);
    bool StringCompare(const char* s1, const char* s2);
    char ToLower(char c);

    //Does not count '\0'
    int StringLength(const char* text);
    uint32_t StrToU32(const char* text, bool* error = nullptr);
    uint32_t HexToU32(const char* text, bool* error = nullptr);
    Color HexToRGBA(const char* text, bool* error = nullptr);

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
    void ComputeParentWidthPercent(BoxInternal& box, int parent_width);
    void ComputeParentHeightPercent(BoxInternal& box, int parent_width);
    void ComputeDetachPositions(BoxInternal& box,  const BoxInternal& parent);


    //Debugger

}


//GLOBALS
namespace UI
{
    Internal::FixedStack<Context*, 16> context_stack;
    Internal::FixedQueue<Context*, 16> context_queue;
    Builder builder;
    void PushContext(Context* context);
}



//IMPLEMENTATION
namespace UI
{
    bool IsContextActive()
    {
        return !context_stack.IsEmpty() && context_stack.Peek();
    }
    void PushContext(Context* context)
    {
        assert(context && "Context nullptr");
        assert(!context_stack.IsFull() && "Why are you using so many contexts");
        assert(!context_queue.IsFull() && "Why are you using so many contexts");
        context_stack.Push(context);
        context_queue.Push(context);
    }
    Context* GetContext()
    {
        assert(!context_stack.IsEmpty() && context_stack.Peek() && "No context has been pushed");
        return context_stack.Peek();
    }
    BoxInfo Info(const char* label)
    {
        if(IsContextActive())
            return GetContext()->Info(label);
        return BoxInfo();
    }
    void BeginRoot(Context* context, const BoxStyle& style, DebugInfo debug_info)
    {
        PushContext(context);
        builder.SetContext(GetContext());
        if(IsContextActive())
        {
            GetContext()->BeginRoot(style, debug_info);
        }
    }
    void EndRoot()
    {
        if(IsContextActive())
            GetContext()->EndRoot();

        assert(!context_stack.IsEmpty() && "No context has been pushed");
        context_stack.Pop();

        if(IsContextActive()) 
            builder.SetContext(GetContext());
    }
    void BeginBox(const UI::BoxStyle& box_style, const char* label, DebugInfo debug_info)
    {
        if(IsContextActive())
            GetContext()->BeginBox(box_style, label, debug_info);
    }
    void InsertText(const char* text, const char* label, bool copy_text, DebugInfo debug_info)
    {
        if(IsContextActive())
            GetContext()->InsertText(text, label, copy_text, debug_info);
    }
    void EndBox()
    {
        if(IsContextActive())
            GetContext()->EndBox();
    }
    void Draw()
    {
        while(!context_queue.IsEmpty())
        {
            assert(context_queue.Front() && "Context nullptr");
            context_queue.Front()->Draw();
            context_queue.Pop();
        }
    }


    // ========== Builder Notation ===========
    Builder& Text(const char* text, const char* id, bool should_copy, DebugInfo debug_info)
    {
        return builder.Text(text, id, should_copy, debug_info);
    }
    Builder& Box(const char* id, DebugInfo debug_info)
    {
        return builder.Box(id, debug_info);
    }
    BoxInfo Info()
    {
        return builder.Info();
    }
    BoxStyle& Style()
    {
        return builder.Style();
    }
    bool IsHover()
    {
        return builder.IsHover();
    }
    bool IsDirectHover()
    {
        return builder.IsDirectHover();
    }
}


//Box
namespace UI
{
    inline void BoxInternal::SetFlowAxis(Flow::Axis axis){flow_axis = axis;}
    inline void BoxInternal::SetScissor(bool flag){scissor = flag;}
    inline Layout BoxInternal::GetLayout() const
    {
        return layout;
    }
    inline Flow::Axis BoxInternal::GetFlowAxis() const
    {
        return flow_axis;
    }
    inline bool BoxInternal::IsScissor() const
    {
        return scissor;
    }
    inline bool BoxInternal::IsDetached() const
    {
        return detach != Detach::NONE;
    }
    inline int BoxInternal::GetBoxExpansionWidth() const
    {
        return margin.left + margin.right + padding.left + padding.right;
    }
    inline int BoxInternal::GetBoxExpansionHeight() const
    {
        return margin.top + margin.bottom + padding.top + padding.bottom;
    }
    inline int BoxInternal::GetBoxModelWidth() const
    {
        //internal box model
        return margin.left + padding.left + width + padding.right + margin.right;
    }
    inline int BoxInternal::GetBoxModelHeight() const
    {
        return margin.top + padding.top + height + padding.bottom + margin.bottom;
    }
    inline int BoxInternal::GetRenderingWidth() const
    {
        return padding.left + width + padding.right;
    }
    inline int BoxInternal::GetRenderingHeight() const
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
            StringCopy(error.msg, Fmt(#error_type"\n"#value" = " #illegal_unit"\nFile: %s\nLine: %d\n", debug_info.file, debug_info.line), ERROR_MSG_SIZE);\
        }
    Error CheckUnitErrors(const BoxInternal& style)
    {
        //The following units cannot equal the specified Unit Types 

        //Content%
        Error error;
        #if UI_ENABLE_DEBUG
            DebugInfo debug_info = style.debug_info;
            UNIT_CONFLICT(style.grid_cell_width_unit,       Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
            UNIT_CONFLICT(style.grid_cell_height_unit,      Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
            UNIT_CONFLICT(style.grid_cell_width_unit,       Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
            UNIT_CONFLICT(style.grid_cell_height_unit,      Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
            UNIT_CONFLICT(style.min_width_unit,             Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
            UNIT_CONFLICT(style.min_height_unit,            Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
            UNIT_CONFLICT(style.max_width_unit,             Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
            UNIT_CONFLICT(style.max_height_unit,            Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        #endif
        return error;
    }



    Error CheckLeafNodeContradictions(const BoxInternal& leaf)
    {
        //The Following erros are contradictions
        Error error;
        #if UI_ENABLE_DEBUG
            DebugInfo debug_info = leaf.debug_info;
            if(leaf.width_unit == Unit::Type::CONTENT_PERCENT)
            {
                error.type = Error::Type::LEAF_NODE_CONTRADICTION;
                StringCopy(error.msg, Fmt("LEAF_NODE_CONTRADICTION\nbox.width_unit = Unit::CONTENT_PERCENT with 0 children\nFile:%s\nLine%d", debug_info.file, debug_info.line), ERROR_MSG_SIZE);
            }
            if(leaf.height_unit == Unit::Type::CONTENT_PERCENT)
            {
                error.type = Error::Type::LEAF_NODE_CONTRADICTION;
                StringCopy(error.msg, Fmt("LEAF_NODE_CONTRADICTION\nbox.height_unit = Unit::CONTENT_PERCENT with 0 children\nFile:%s\nLine%d", debug_info.file, debug_info.line), ERROR_MSG_SIZE);
            }
        #endif
        return error;
    }

    Error CheckNodeContradictions(const BoxInternal& child, const BoxInternal& parent)
    {
        //The following errors are contradictions between parent and child
        Error error;
        #if UI_ENABLE_DEBUG

            #define CHILD_PARENT_CONFLICT(child_unit, illegal_unit, parent_unit, error_type)\
                if(child_unit == illegal_unit && parent_unit == Unit::CONTENT_PERCENT)\
                {\
                    error.type = error_type;\
                    StringCopy(error.msg, Fmt(#error_type"\n"#child_unit " = " #illegal_unit" and "#parent_unit " = Unit::CONTENT_PERCENT\nFile: %s\nLine: %d\n", file, line), ERROR_MSG_SIZE);\
                }

            //DebugInfo debug_info = child.debug_info;
            //CHILD_PARENT_CONFLICT(child.width_unit, Unit::PARENT_PERCENT, parent.width_unit, Error::Type::NODE_CONTRADICTION);
            //CHILD_PARENT_CONFLICT(child.width_unit, Unit::AVAILABLE_PERCENT, parent.width_unit, Error::Type::NODE_CONTRADICTION);
            //CHILD_PARENT_CONFLICT(child.height_unit, Unit::PARENT_PERCENT, parent.height_unit, Error::Type::NODE_CONTRADICTION);
            //CHILD_PARENT_CONFLICT(child.height_unit, Unit::AVAILABLE_PERCENT, parent.height_unit, Error::Type::NODE_CONTRADICTION);
            //CHILD_PARENT_CONFLICT(child.min_width_unit, Unit::PARENT_PERCENT, parent.width_unit, Error::Type::NODE_CONTRADICTION);
            //CHILD_PARENT_CONFLICT(child.min_width_unit, Unit::AVAILABLE_PERCENT, parent.width_unit, Error::Type::NODE_CONTRADICTION);
            //CHILD_PARENT_CONFLICT(child.max_width_unit, Unit::PARENT_PERCENT, parent.width_unit, Error::Type::NODE_CONTRADICTION);
            //CHILD_PARENT_CONFLICT(child.max_width_unit, Unit::AVAILABLE_PERCENT, parent.width_unit, Error::Type::NODE_CONTRADICTION);
            //CHILD_PARENT_CONFLICT(child.min_height_unit, Unit::PARENT_PERCENT, parent.height_unit, Error::Type::NODE_CONTRADICTION);
            //CHILD_PARENT_CONFLICT(child.min_height_unit, Unit::AVAILABLE_PERCENT, parent.height_unit, Error::Type::NODE_CONTRADICTION);
            //CHILD_PARENT_CONFLICT(child.max_height_unit, Unit::PARENT_PERCENT, parent.height_unit, Error::Type::NODE_CONTRADICTION);
            //CHILD_PARENT_CONFLICT(child.max_height_unit, Unit::AVAILABLE_PERCENT, parent.height_unit, Error::Type::NODE_CONTRADICTION);
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


//Pass 1
//Compute unit CM, MM, INCH, ROOT_PERCENT
    int FixedUnitToPx(Unit unit, int root_size)
    {
        switch(unit.unit)
        {
            case Unit::Type::PIXEL:
                return (int)unit.value;
            case Unit::Type::ROOT_PERCENT: 
                return (int)unit.value * root_size / 100;
            default:
                return (int)unit.value; //Only meant for width/height
        }
    }
    BoxInternal ComputeStyleSheet(const BoxStyle& style, const BoxInternal& root)
    {
        int root_width = root.width - style.margin.left - style.margin.right - style.padding.left - style.padding.right;
        int root_height = root.height - style.margin.top - style.margin.bottom - style.padding.top - style.padding.bottom;
        root_width = Max(0, root_width);
        root_height = Max(0, root_height);

        BoxInternal box;
        box.background_color =          style.background_color;
        box.border_color =              style.border_color;
        //type 3

        box.scroll_x =                  style.scroll_x;
        box.scroll_y =                  style.scroll_y;
        
        box.x =                          style.x;
        box.y =                          style.y;
        box.width =                     (uint16_t)Max(0, FixedUnitToPx(style.width, root_width));
        box.height =                    (uint16_t)Max(0, FixedUnitToPx(style.height, root_height));
        box.gap_row =                    style.gap_row;
        box.gap_column =                 style.gap_column;
        box.min_width =                 (uint16_t)Max(0, FixedUnitToPx(style.min_width, root_width));
        box.max_width =                 (uint16_t)Max(0, FixedUnitToPx(style.max_width, root_width));
        box.min_height =                (uint16_t)Max(0, FixedUnitToPx(style.min_height, root_height));
        box.max_height =                (uint16_t)Max(0, FixedUnitToPx(style.max_height, root_height));
        box.grid_cell_width =           (uint16_t)Max(0, FixedUnitToPx(style.grid.cell_width, root_width));
        box.grid_cell_height =          (uint16_t)Max(0, FixedUnitToPx(style.grid.cell_height, root_width));

        box.width_unit =                style.width.unit;
        box.height_unit =               style.height.unit;
        box.min_width_unit =            style.min_width.unit;
        box.max_width_unit =            style.max_width.unit;
        box.min_height_unit =           style.min_height.unit;
        box.max_height_unit =           style.max_height.unit;
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
        box.detach =                    style.detach;

        box.SetFlowAxis(style.flow.axis);
        box.SetScissor(style.scissor);
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
    const char* ArenaCopyString(const char* text, MemoryArena* arena)
    {
        assert(text && "no text padded");
        assert(arena && "no arena passed");
        int n = StringLength(text) + 1;
        char* temp = (char*)arena->Allocate(n);
        if(temp == nullptr)
            return nullptr;
        StringCopy(temp, text, n);
        return temp;
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
    const char *Fmt(const char *text, ...)
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
    void ComputeParentWidthPercent(BoxInternal& box, int parent_width)
    {
        parent_width -= box.padding.left + box.padding.right + box.margin.left + box.margin.right;
        parent_width = Max(0, parent_width);
        box.width =                     (uint16_t)Max(0, ParentPercentToPx(box.width,            box.width_unit,             parent_width)); 
        box.min_width =                 (uint16_t)Max(0, ParentPercentToPx(box.min_width,        box.min_width_unit,         parent_width)); 
        box.max_width =                 (uint16_t)Max(0, ParentPercentToPx(box.max_width,        box.max_width_unit,         parent_width)); 
        box.grid_cell_width =           (uint16_t)Max(0, ParentPercentToPx(box.grid_cell_width,  box.grid_cell_width_unit,   parent_width)); 
    }

    //Height
    void ComputeParentHeightPercent(BoxInternal& box, int parent_height)
    {
        parent_height -= box.padding.top + box.padding.bottom + box.margin.top + box.margin.bottom;
        parent_height = Max(0, parent_height);
        box.height =                    (uint16_t)Max(0, ParentPercentToPx(box.height,           box.height_unit,            parent_height)); 
        box.gap_row =                   (uint16_t)Max(0, ParentPercentToPx(box.gap_row,          box.gap_row_unit,           parent_height)); 
        box.min_height =                (uint16_t)Max(0, ParentPercentToPx(box.min_height,       box.min_height_unit,        parent_height)); 
        box.max_height =                (uint16_t)Max(0, ParentPercentToPx(box.max_height,       box.max_height_unit,        parent_height)); 
        box.grid_cell_height =          (uint16_t)Max(0, ParentPercentToPx(box.grid_cell_height, box.grid_cell_height_unit,  parent_height)); 
    }
    void ComputeWidthPercentForHeight(BoxInternal& box)
    {
        if(box.height_unit == Unit::Type::WIDTH_PERCENT)
            box.height = box.width * box.height / 100;
        if(box.min_height_unit == Unit::Type::WIDTH_PERCENT)
            box.min_height = box.width * box.min_height / 100;
        if(box.max_height_unit == Unit::Type::WIDTH_PERCENT)
            box.max_height = box.width * box.max_height / 100;
    }
    void ComputeDetachPositions(BoxInternal& box, const BoxInternal& parent)
    {
        switch(box.detach)
        {
            case Detach::RELATIVE:
                box.x = parent.x + box.x;
                box.y = parent.y + box.y;
                break;
           case Detach::LEFT:
                box.x = parent.x - box.GetRenderingWidth() + box.x;
                box.y = parent.y;
                break;
            case Detach::RIGHT:
                box.x = parent.x + parent.GetRenderingWidth() + box.x;
                box.y = parent.y;
                break;
            case Detach::TOP:
                box.x = parent.x;
                box.y = parent.y - box.GetRenderingHeight();
                break;
            case Detach::BOTTOM:
                box.x = parent.x;
                box.y = parent.y + parent.GetRenderingHeight();
                break;
           case Detach::LEFT_CENTER:
                box.x = parent.x - box.GetRenderingWidth() + box.x;
                box.y = parent.y + (parent.GetRenderingHeight() - box.GetRenderingHeight())/2;
                break;
            case Detach::RIGHT_CENTER:
                box.x = parent.x + parent.GetRenderingWidth() + box.x;
                box.y = parent.y + (parent.GetRenderingHeight() - box.GetRenderingHeight())/2;
                break;
            case Detach::TOP_CENTER:
                box.x = parent.x + (parent.GetRenderingWidth() - box.GetRenderingWidth())/2;
                box.y = parent.y - box.GetRenderingHeight();
                break;
            case Detach::BOTTOM_CENTER:
                box.x = parent.x + (parent.GetRenderingWidth() - box.GetRenderingWidth())/2;
                box.y = parent.y + parent.GetRenderingHeight();
                break;
           case Detach::LEFT_END:
                box.x = parent.x - box.GetRenderingWidth() + box.x;
                box.y = parent.y + parent.GetRenderingHeight() - box.GetRenderingHeight();
                break;
            case Detach::RIGHT_END:
                box.x = parent.x + parent.GetRenderingWidth() + box.x;
                box.y = parent.y + parent.GetRenderingHeight() - box.GetRenderingHeight();
                break;
            case Detach::TOP_END:
                box.x = parent.x + parent.GetRenderingWidth() - box.GetRenderingWidth();
                box.y = parent.y - box.GetRenderingHeight();
                break;
            case Detach::BOTTOM_END:
                box.x = parent.x + parent.GetRenderingWidth() - box.GetRenderingWidth();
                box.y = parent.y + parent.GetRenderingHeight();
                break;
            default:
                break;
        }
    }


    DebugInspector::DebugInspector(uint64_t memory): ui_context(memory / 2), arena(memory / 2)
    {

    }
    Context* DebugInspector::GetContext()
    {
        return &ui_context;
    }
    const char* DebugInspector::CopyStringToArena(const char* str)
    {
        return ArenaCopyString(str, &arena);
    }
    void DebugInspector::Reset()
    {
        root_node = nullptr;
        selected_node = nullptr;
        arena.Reset();
        ui_context.ResetAllStates();
    }
    void DebugInspector::PushNode(const DebugBox& box)
    {
        if(root_node == nullptr)
        {
            assert(stack.IsEmpty() && "DebugInspector: unbalanced Begin/End");
            root_node = arena.New<TreeNodeDebug>();
            assert(root_node && "DebugVew arena out of memory");
            stack.Push(root_node);
            root_node->box = box;
            return;
        }
        TreeNodeDebug* parent = stack.Peek();
        assert(parent);
        TreeNodeDebug child;
        child.box = box;
        TreeNodeDebug* child_ptr = parent->children.Add(child, &arena);
        assert(child_ptr && "DebugInspector arena out of memory");
        stack.Push(child_ptr);
    }
    void DebugInspector::PopNode()
    {
        assert(!stack.IsEmpty() && "Uneven amount of begin/end");
        stack.Pop();
    }
    bool DebugInspector::IsTreeEmpty()
    {
        return root_node == nullptr;
    }
    void DebugInspector::RunDebugInspector(int x, int y, int screen_width, int screen_height, int mouse_x, int mouse_y)
    {
        assert(root_node && "root_node should have been initialized"); //this is for more own sanity
        //root_node->box.style.width = {(float)screen_width};
        //root_node->box.style.height = {(float)screen_height};
        BoxStyle root = 
        {
            .width = GetScreenWidth(),
            .height = GetScreenHeight()
        };
        ui_context.BeginRoot(root);

        ConstructMockUI(root_node);

        // ===== Highlight hovered/selected Node =====
        if(hovered_element.width > 0)
        {
            BoxStyle hovered_element_outline = 
            { 
                .x = {hovered_element.x}, 
                .y = {hovered_element.y}, 
                .width = {hovered_element.width}, 
                .height = {hovered_element.height}, 
                .background_color = {100, 100, 100, 100},
                .border_color = {255, 255, 255, 255},  
                .border_width = 1, 
                .detach = Detach::ABSOLUTE
            };
            ui_context.BeginBox(hovered_element_outline, nullptr,UI_DEBUG("HoverOutline")); ui_context.EndBox();
        }
        hovered_element = Rect();

        if(selected_node)
        {
            const DebugBox& box = selected_node->box;
            BoxStyle selected_node_outline = 
            {
                .x = box.dim.x,
                .y = box.dim.y,
                .width = box.dim.width,
                .height = box.dim.height,
                .background_color = {255, 0, 0, 10},
                .border_color = {255, 0, 0, 255},  
                .border_width = 1, 
                .detach = Detach::ABSOLUTE,
            };
            ui_context.BeginBox(selected_node_outline, nullptr, UI_DEBUG("SelectOutline")); ui_context.EndBox();
        }
        // ===================================

        ConstructInspector(mouse_x, mouse_y);

        ui_context.EndRoot();

        ui_context.Draw();
    }
    void DebugInspector::ConstructMockUI(TreeNodeDebug* node)
    {
        if(node == nullptr)
            return;

        const char* element_id = Fmt("mock_element%ld",(uintptr_t)node);
        BoxInfo info = ui_context.Info(element_id);
        if(info.valid)
        {
            DebugBox& box = node->box;
            box.is_rendered = info.is_rendered;
            box.dim = {info.DrawX(), info.DrawY(), info.DrawWidth(), info.DrawHeight()};
            if(info.is_direct_hover)
            {
                hovered_element.width = box.dim.width;
                hovered_element.height = box.dim.height;
                hovered_element.x = box.dim.x;
                hovered_element.y = box.dim.y;
                //Selecting node from mock ui
                if(IsMousePressed(MOUSE_LEFT))
                {
                    selected_node = node;
                    SearchNodeAndOpenTree(root_node);
                }
            }
        }
        if(node->box.text)
        {
            ui_context.InsertText(node->box.text, element_id);
            return;
        }

        ui_context.BeginBox(node->box.style, element_id, UI_DEBUG("CopiedElement"));
        for(auto temp = node->children.GetHead(); temp != nullptr; temp = temp->next)
            ConstructMockUI(&temp->value);
        ui_context.EndBox();
    }
    void DebugInspector::ConstructInspector(int mouse_x, int mouse_y)
    {
        // === Draggin Logic ===
        if(IsMousePressed(MOUSE_LEFT))
        {
            mouse_drag_x = mouse_x;
            mouse_drag_y = mouse_y;
        }
        int mouse_delta_x = mouse_x - mouse_drag_x;
        int mouse_delta_y = mouse_y - mouse_drag_y;

        Rect base_dim = window_dim;
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
        int new_panel_width = panel_width;
        if(panel_drag)
        {
            new_panel_width += mouse_delta_x;
        }
        base_dim.width = Max(250, base_dim.width);
        base_dim.height = Max(125, base_dim.height);
        new_panel_width = Clamp(new_panel_width, 20, base_dim.width - 50);
        // ====================

        Builder ui;
        ui.SetContext(&ui_context);

        BoxStyle base = 
        {
            .flow = { .axis = Flow::VERTICAL},
            .x = {base_dim.x},
            .y = {base_dim.y},
            .width = {base_dim.width},
            .height = {base_dim.height},
            .background_color = theme.base_color,
            .corner_radius = theme.base_corner_radius,
            .detach = Detach::ABSOLUTE,
        };

        BoxStyle title_bar = 
        {
            .width = {100, Unit::PARENT_PERCENT},
            .height = {100, Unit::CONTENT_PERCENT},
            .padding = {10, 10, 5, 5},
            .background_color = theme.title_bar_color,
            .corner_radius = (unsigned char)(theme.base_corner_radius >> 1)

        };
        BoxStyle h_container = 
        {
            .flow = { .vertical_alignment = Flow::CENTERED},
            .width = {100, Unit::AVAILABLE_PERCENT},
            .height = {100, Unit::AVAILABLE_PERCENT},
            .padding = theme.base_padding
        };

        BoxStyle left_panel =
        {
            .flow = {.axis = Flow::VERTICAL},
            .width = {new_panel_width},
            .height = {100, Unit::AVAILABLE_PERCENT},
            .background_color = theme.left_panel_color,
            .corner_radius = theme.button_corner_radius,
            .scissor = true
        };

        BoxStyle left_panel_title =
        {
            .width = {100, Unit::PARENT_PERCENT},
            .height = {100, Unit::CONTENT_PERCENT},
            .padding = {10, 10, 10, 10}
        };

        BoxStyle left_panel_scroll_box =
        {
            .flow = {.axis = Flow::VERTICAL},
            .width = {100, Unit::PARENT_PERCENT},
            .height = {100, Unit::AVAILABLE_PERCENT},
            .padding = {0, 0, 10, 10},
            .scroll_y = left_panel_scroll,
            .scissor = true
        };

        BoxStyle panel_resize_button;
        panel_resize_button.flow.vertical_alignment = Flow::CENTERED;
        panel_resize_button.flow.horizontal_alignment = Flow::CENTERED;
        panel_resize_button.margin = {2,2};
        panel_resize_button.width = {8};
        panel_resize_button.height = {60, Unit::AVAILABLE_PERCENT};
        panel_resize_button.background_color = theme.title_bar_color;
        panel_resize_button.corner_radius = theme.icon_corner_radius;
        BoxStyle right_panel
        {
            .flow = {.axis = Flow::VERTICAL},
            .width = {100, Unit::AVAILABLE_PERCENT},
            .height = {100, Unit::AVAILABLE_PERCENT},
            .background_color = theme.right_panel_color,
            .corner_radius = theme.button_corner_radius,
            .scissor = true
        };
        BoxStyle right_panel_title =
        {
            .width = {100, Unit::PARENT_PERCENT},
            .height = {100, Unit::CONTENT_PERCENT},
            .padding = {10, 10, 10, 10}
        };
        BoxStyle right_panel_scroll_box =
        {
            .flow = {.axis = Flow::VERTICAL},
            .width = {100, Unit::PARENT_PERCENT},
            .height = {100, Unit::AVAILABLE_PERCENT},
            .padding = {0, 0, 10, 10},
            .scroll_y = right_panel_scroll,
            .scissor = true
        };
        BoxStyle base_resize_button
        {
            .x = {base_dim.x + base_dim.width - 18},
            .y = {base_dim.y + base_dim.height - 18},
            .width{18},
            .height = {18},
            .background_color = theme.title_bar_color,
            .corner_radius = theme.icon_corner_radius,
            .detach = Detach::ABSOLUTE
        };

        // ================ Inspector UI TREE ======================
        ui.Box("base", UI_DEBUG("Inspector")).Style(base).Run([&]
        {
            ui.Box("base-title-bar")
            .Style(title_bar)
            .OnDirectHover([&] { if(IsMousePressed(MOUSE_LEFT)) window_pos_drag = true;})
            .Run([&]{ ui.Text(Fmt("[S:20][C:%s]Inspector", theme.text_color)).Run();});

            ui.Box()
            .Style(h_container)
            .Run([&]
            {
                ui.Box()
                .Style(left_panel)
                .Run([&]
                {
                    //Left panel content
                    ui.Box()
                    .Style(left_panel_title)
                    .Run([&]{ ui.Text(Fmt("[S:20][C:%s]Navigate", theme.text_color)).Run(); });

                    ui.Box("left-panel-scroll-box")
                    .Style(left_panel_scroll_box)
                    .OnHover([&]
                    {
                        left_panel_scroll -= GetMouseScroll() * 20;
                    })
                    .Run([&] 
                    {
                        left_panel_scroll = Clamp(left_panel_scroll, 0, ui.Info().MaxScrollY());
                        ConstructTree(root_node, 0); 
                    });
                });

                ui.Box("left-panel-resize-button")
                .Style(panel_resize_button)
                .OnDirectHover([&]
                { 
                    if(IsMousePressed(MOUSE_LEFT)) panel_drag = true;
                    ui.Style().background_color = theme.button_color_hover;
                })
                .Run([&]
                { 
                    HexColor col = ui.Info().IsDirectHover()? theme.text_color_hover: theme.text_color;
                    ui.Text(Fmt("[S:20][C:%s]||", col)).Run();
                });

                ui.Box()
                .Style(right_panel)
                .Run([&]
                {
                    ui.Box()
                    .Style(right_panel_title)
                    .Run([&]{ui.Text(Fmt("[S:20][C:%s]Details", theme.text_color_hover)).Run();});
                    ui.Box("right-panel-scroll-box")
                    .Style(right_panel_scroll_box)
                    .Run([&]
                    {
                        ConstructEditor();
                    });
                    //Right panel content
                });
            });
        });
        ui.Box("base-resize-button")
        .Style(base_resize_button)
        .OnDirectHover([&]
        {   
            ui.Style().background_color = theme.button_color_hover;
            if(IsMousePressed(MOUSE_LEFT)) window_size_drag = true;
        }).Run();


        // ======================== END Inspector UI TREE ==================================

        if(IsMouseReleased(MOUSE_LEFT))
        {
            window_dim = base_dim;
            window_pos_drag = false;
            panel_width = new_panel_width;
            window_size_drag = false;
            panel_drag = false;
        }
    }

    void DebugInspector::ConstructEditor()
    {
        if(!selected_node)
            return;
        DebugBox& box = selected_node->box;
        BoxStyle& style = box.style;
        Builder ui; 
        ui.SetContext(&ui_context);

        auto GetUnitType = [](Unit::Type type) -> const char*
        {
            switch(type)
            {
                case Unit::Type::PIXEL: return "PIXEL";
                case Unit::Type::PARENT_PERCENT: return "PARENT_PERCENT";
                case Unit::Type::ROOT_PERCENT: return "ROOT_PERCENT";
                case Unit::Type::CONTENT_PERCENT: return "CONTENT_PERCENT" ;
                case Unit::Type::AVAILABLE_PERCENT: return "AVAILABLE_PERCENT";
                case Unit::Type::WIDTH_PERCENT: return "WIDTH_PERCENT"; 
                default: return "NONE";
            }
        };

        BoxStyle editor_base = 
        {
            .flow = { .axis = Flow::VERTICAL },
            .width = {100, Unit::PARENT_PERCENT},
            .height = {100, Unit::CONTENT_PERCENT},
            .padding = {10, 10, 10, 10}
        };
        BoxStyle v_container = 
        {
            .flow = {.axis = Flow::VERTICAL},
            .width = {100, Unit::AVAILABLE_PERCENT},
            .height = {100, Unit::CONTENT_PERCENT}
        };
        BoxStyle info_box = 
        {
            .flow = {.axis = Flow::VERTICAL},
            .width = {100, Unit::AVAILABLE_PERCENT},
            .height = {100, Unit::CONTENT_PERCENT},
            .min_width = {100, Unit::CONTENT_PERCENT},
            .min_height = {theme.button_corner_radius * 2},
            .padding = {5,5,5,5},
            .background_color = theme.info_box_color,
            .corner_radius = theme.button_corner_radius
        };
        BoxStyle info_box_title_bar = 
        {
            .width = {100, Unit::CONTENT_PERCENT},
            .height = {100, Unit::CONTENT_PERCENT},
            .padding = {5,5,5,5}
        };



        ui.Box()
        .Style(editor_base)
        .Run([&]
        {
            ui.Box()
            .Style(info_box_title_bar)
            .Run([&] { ui.Text(Fmt("[S:20][C:%s]Debug", theme.text_color_hover)).Run(); });
            ui.Box()
            .Style(info_box)
            .Run([&]
            {
                //ui.Text(Fmt("[S:18][C:%s]Name: [C:%s][OFF]none", theme.text_color_hover, theme.info_text_color)).Run();
                HexColor unknown_value = {"EE0000FF"};
                if(selected_node->box.debug_info.name)
                    ui.Text(Fmt("[S:18][C:%s]Name: [C:%s][OFF]%s", theme.text_color_hover, theme.info_text_color, selected_node->box.debug_info.name)).Run();
                else
                    ui.Text(Fmt("[S:18][C:%s]Name: [C:%s][OFF]none", theme.text_color_hover, unknown_value)).Run();
                if(selected_node->box.debug_info.file)
                    ui.Text(Fmt("[S:18][C:%s]File: [C:%s][OFF]\"%s\"", theme.text_color_hover, theme.info_text_color, selected_node->box.debug_info.file)).Run();
                else
                    ui.Text(Fmt("[S:18][C:%s]File: [C:%s][OFF]none", theme.text_color_hover, unknown_value)).Run();
                ui.Text(Fmt("[S:18][C:%s]Line: [C:%s]%d", theme.text_color_hover, theme.info_text_color, selected_node->box.debug_info.line)).Run();
                if(selected_node->box.label)
                    ui.Text(Fmt("[S:18][C:%s]Id: [C:%s][OFF]\"%s\"", theme.text_color_hover, theme.id_text_color, selected_node->box.label)).Run();
                else
                    ui.Text(Fmt("[S:18][C:%s]Id: [C:%s][OFF]none", theme.text_color_hover, unknown_value)).Run();
                if(selected_node->box.text)
                    ui.Text(Fmt("[S:18][C:%s]Text: [C:%s][OFF]\"%s\"", theme.text_color_hover, theme.string_color, selected_node->box.text)).Run();
            });

            // ==== Layout Properties ====
            ui.Box().Style(info_box_title_bar).Run([&] { ui.Text(Fmt("[S:20][C:%s]Layout Properties", theme.text_color_hover)).Run(); });
            ui.Box()
            .Style(info_box)
            .Run([&]
            {
            });
        });



    }

    void DebugInspector::ConstructTree(TreeNodeDebug* node, int depth)
    {
        if(node == nullptr)
            return;

        BoxStyle button =
        {
            .flow = {.vertical_alignment = Flow::CENTERED}, 
            .width = {9999},
            .height = {100, Unit::CONTENT_PERCENT},
            .background_color = theme.button_color
        };
        BoxStyle filler = {.width = {9}, .height = {100, Unit::PARENT_PERCENT}}; 

        BoxStyle line = 
        { 
            .width = {1}, 
            .height = {100, Unit::PARENT_PERCENT}, 
            .background_color = theme.button_color_hover 
        };

        BoxStyle content_box = 
        {
            .width = {100, Unit::CONTENT_PERCENT},
            .height = {100, Unit::CONTENT_PERCENT},
            .gap_column = {4},
        };

        BoxStyle open_button =
        {
            .flow = {.vertical_alignment = Flow::CENTERED, .horizontal_alignment = Flow::CENTERED},
            .width = {18},
            .height = {18},
            .border_color = theme.button_color_hover,
            .corner_radius = 2,
            .border_width = 1,
        };
        BoxStyle color_icon = open_button;

        if(!node->box.is_rendered)
            button.background_color = {255, 211, 211, 255};
        
        HexColor text_color = theme.text_color;
        
        auto highlight_button = [&]
        {
            text_color = theme.text_color_hover;
            open_button.background_color = theme.button_color_hover;
            open_button.border_color = theme.button_color;
            line.background_color = theme.button_color;
            button.background_color = theme.button_color_hover;
        };
        if(node == selected_node)
           highlight_button();


        Builder ui;
        ui.SetContext(&ui_context);


        ui.Box(Fmt("tree-element-button%ld", (uintptr_t)node), UI_DEBUG("TreeButton"))
        .OnDirectHover([&]
        {
            highlight_button();
            if(IsMousePressed(MOUSE_LEFT)) selected_node = node;
            hovered_element = node->box.dim;
        })
        .Style(button)
        .Run([&]
        {
            // filler + line + filler
            for(int i = 0; i<depth; i++) { ui.Box().Style(filler).Run(); ui.Box().Style(line).Run(); ui.Box().Style(filler).Run(); }

            if(!node->children.IsEmpty())
            {
                //===== Open Button logic =====
                HexColor icon_color = text_color;
                char icon = '+';
                if(node->box.is_open)
                {
                    icon = '-';
                    open_button.background_color = {100, 255, 100, 255};
                }
                // ============================
                ui.Box(Fmt("tree-element-open-button%ld", (uintptr_t)node))
                .OnDirectHover([&]
                {
                    if(!node->box.is_open)
                        open_button.background_color = theme.button_color_hover;
                    open_button.border_color = theme.button_color_hover;
                    icon_color = theme.text_color_hover;
                    if(IsMousePressed(MOUSE_LEFT))
                        node->box.is_open = !node->box.is_open;
                })
                .Style(open_button)
                .Run([&]
                {
                    ui.Text(Fmt("[S:20][C:%s]%c", icon_color, icon)).Run();
                });
            }
            ui.Box().Style(content_box)
            .Run([&]
            {
                const DebugBox& box = node->box;
                if(box.debug_info.name)
                    ui.Text(Fmt("[S:20][C:%s]%s ", text_color, box.debug_info.name)).Run();
                else
                    ui.Text(Fmt("[S:20][C:%s]Unnamed ", text_color)).Run();
                if(box.text)
                {
                    ui.Text(Fmt("[S:20][C:%s][OFF]\"%s\"", theme.string_color, box.text)).Run();
                }
                else if(box.style.background_color.a > 0)
                {
                    color_icon.background_color = box.style.background_color;
                    ui.Box().Style(color_icon).Run();
                }
                if(box.label)
                {
                    ui.Text(Fmt("[S:20][C:%s]id: [C:%s][OFF]\"%s\"", text_color, theme.id_text_color, box.label)).Run();
                }
            });
        });

        if(node->box.is_open)
        {
            for(auto temp = node->children.GetHead(); temp != nullptr; temp = temp->next)
            {
                DebugBox& box = temp->value.box;
                ConstructTree(&temp->value, depth + 1);
            }
        }
    }


    bool DebugInspector::SearchNodeAndOpenTree(TreeNodeDebug* node)
    {
        if(node == nullptr)
            return false;
        bool found = false; 
        if(node == selected_node) 
            found =  true;
        for(auto temp = node->children.GetHead(); temp != nullptr; temp = temp->next)
        {
            bool result = SearchNodeAndOpenTree(&temp->value);
            if(result)
                found = true;
        }

        if(found)
        {
            node->box.is_open = true;
            return true;
        }
        else
        {
            node->box.is_open = false;
            return false;
        }
    }
    void DebugInspector::CustomComboList(const char * id, int& selected, const char** options, uint64_t valid)
    {
        uint64_t key = Hash(id);

        Builder ui;
        ui.SetContext(&ui_context);
        HexColor text_color = theme.text_color_hover;
        BoxStyle button = 
        {
            .flow = {.horizontal_alignment = Flow::END},
            .width = {100, Unit::AVAILABLE_PERCENT},
            .height = {100, Unit::CONTENT_PERCENT},
            .min_width = {100, Unit::CONTENT_PERCENT},
            .padding = {4,4,1,1},
            .background_color = theme.button_color_hover,
            .corner_radius = theme.icon_corner_radius
        };
        BoxStyle button2 = button; 
        button2.width = {80}; 
        BoxStyle pop_up = 
        {
            .flow = {.axis = Flow::VERTICAL},
            .width = {100, Unit::CONTENT_PERCENT},
            .height = {100, Unit::CONTENT_PERCENT},
            .padding = {2,2,2,2},
            .background_color = theme.base_color,
            .gap_row = 2,
            .corner_radius = theme.icon_corner_radius,
            .detach = Detach::RIGHT
        };
        bool hover = ui_context.Info(Fmt("ComnboList-popup-id-%s", id)).IsHover();
        ui.Box(Fmt("ComboList-button-id-%s", id), UI_DEBUG("[C:6666FF]ComboListBox"))
        .Style(button2)
        .OnDirectHover([&]
        {
            hover = true;
            text_color = theme.text_color;
            ui.Style().background_color = theme.button_color;
        })
        .Run([&]
        {
            ui.Text(Fmt("[S:20][C:%s]%s", text_color, options[selected])).Run();
            if(hover)
            {
                ui.Box(Fmt("ComnboList-popup-id-%s", id))
                .Style(pop_up)
                .Run([&]
                {
                    button.background_color = theme.button_color_hover;
                    for(int i = 0; options[i] != nullptr; i++)
                    {
                        text_color = theme.text_color_hover;
                        ui.Box(Fmt("ComboList-button-%s-id-%s", options[i], id))
                        .Style(button)
                        .OnDirectHover([&] 
                        {
                            text_color = theme.text_color;
                            if(IsMousePressed(MOUSE_LEFT))
                            {
                                selected = i;
                            }
                            ui.Style().background_color = theme.button_color;
                        })
                        .Run([&] { ui.InsertText(Fmt("[S:18][C:%s]%s", text_color, options[i])); });
                    }
                });
            }
        });
    };
    void DebugInspector::CustomDigitInput(const char* id, int& value)
    {
        Builder ui;
        ui.SetContext(&ui_context);
        BoxStyle button = 
        {
            .flow = {.horizontal_alignment = Flow::END},
            .width = {60},
            .height = {100, Unit::CONTENT_PERCENT},
            .min_width = {100, Unit::CONTENT_PERCENT},
            .padding = {1,1,1,1},
            .background_color = theme.button_color_hover,
            .corner_radius = theme.icon_corner_radius
        };
        HexColor text_color = theme.text_color_hover;
        ui.Box(Fmt("CustomDigitInput %s", id))
        .Style(button)
        .OnDirectHover([&]
        {
            text_color = theme.text_color;
            value += GetMouseScroll() * 2;
            ui.Style().background_color = theme.button_color;
        })
        .Run([&]
        {
            ui.InsertText(Fmt("[S:20][C:%s]%d", text_color, value));
        });
    }
    void DebugInspector::UnitEditBox(const char* name, Unit& unit, uint64_t valid)
    {
        Builder ui;
        ui.SetContext(GetContext());
        BoxStyle h_container =
        {
            .width = {100, Unit::AVAILABLE_PERCENT},
            .height = {100, Unit::CONTENT_PERCENT},
            .min_width = {100, Unit::CONTENT_PERCENT}
        };
        BoxStyle input_container = 
        {
            .width = {100, Unit::CONTENT_PERCENT},
            .height = {100, Unit::CONTENT_PERCENT},
            .min_width = {100, Unit::CONTENT_PERCENT}
        };
        const char* unit_options[]
        {
            "PIXEL",
            "PARENT_PERCENT",
            "ROOT_PERCENT",
            "CONTENT_PERCENT",   
            "AVAILABLE_PERCENT",
            "WIDTH_PERCENT",
        };
        ui.Box()
        .Style(h_container)
        .Run([&]
        {
            ui.InsertText(Fmt("[S:20][C:%s]%s", theme.text_color_hover, name));
            ui.Box()
            .Style(input_container)
            .Run([&]
            {
                int digit = unit.value;
                int select = unit.unit;
                CustomDigitInput(name, digit);
                CustomComboList(name, select, unit_options, valid);
                unit.value = digit;
                unit.unit = (Unit::Type)select;
            });
        });
    }


}










namespace UI
{
    Context::Context(uint64_t arena_bytes) :
        arena(arena_bytes)
    {
        uint32_t cap = arena_bytes / 2 / (sizeof(Internal::ArenaMap<BoxInfo>::Item) * 2);
        bool err = double_buffer_map.AllocateBufferCapacity(cap, &arena);
        assert(err && "Arena out of memory");
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
    BoxInfo Context::Info(uint64_t key)
    {
        #if UI_ENABLE_DEBUG
        if(debug_inspector && is_inspecting)
            return BoxInfo();
        #endif

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
    BoxInfo Context::Info(const char* label)
    {
        return Info(Hash(label));
    }
    void Context::ResetAllStates()
    {
        arena.Rewind(root_node);
        double_buffer_map.Reset();

        stack.Clear();
        deferred_elements.Clear();
        root_node = nullptr;
        element_count = 0;
        directly_hovered_element_key = 0;
    }

    void Context::ClearPreviousFrame()
    {
        double_buffer_map.SwapBuffer();
        arena.Rewind(root_node);
        stack.Clear();
        root_node = nullptr;
        element_count = 0;
    }
    void Context::SetInspector(bool activate_pressed, DebugInspector* inspector)
    {
        debug_inspector = inspector; 
        if(debug_inspector)
        {
            if(activate_pressed)
            {
                debug_inspector->Reset();
                if(is_inspecting)
                    is_inspecting = false;
                else
                    copy_tree = true;
            }
        }

    }
    void Context::BeginRoot(BoxStyle style, DebugInfo debug_info)
    {
        style.width = {style.width.value - style.padding.right - style.padding.left - style.margin.right - style.margin.left, Unit::PIXEL};
        style.height = {style.height.value - style.padding.top - style.padding.bottom - style.margin.top - style.margin.bottom, Unit::PIXEL};
        style.min_height.unit = Unit::PIXEL;
        style.min_width.unit = Unit::PIXEL;
        style.max_height.unit = Unit::PIXEL;
        style.max_width.unit = Unit::PIXEL;
        #if UI_ENABLE_DEBUG
        if(debug_inspector)
        {
            if(copy_tree)
            {
                DebugBox box;
                box.debug_info = debug_info;
                box.style = style;
                debug_inspector->PushNode(box);
                return;
            }
            if(is_inspecting)
                return;
        }
        #endif


        if(HasInternalError())
            return;
        ClearPreviousFrame();

        assert(!double_buffer_map.ShouldResize() && "Double Buffer Map out of memory");

        assert(stack.IsEmpty());
        Box root_box = ComputeStyleSheet(style, Box());
        // ========== Debug Mode Only ==========
        #if UI_ENABLE_DEBUG
            root_box.debug_info = debug_info;
        #endif

        if(stack.IsEmpty())//Root Node
        {
            //Checking errors unique to root node
            root_node = arena.New<TreeNode>();
            assert(root_node && "Arena out of space");
            root_node->box = root_box;
            stack.Push(root_node);
        }
        else
        {
            //assert(0 && "ERROR: More than one Root Node");
            HandleInternalError(Error{Error::Type::ROOT_NODE_CONTRADICTION, "More than one BeginRoot()"});
            return;
        }
    }
    void Context::EndRoot()
    {
        #if UI_ENABLE_DEBUG
        if(debug_inspector)
        {
            if(copy_tree)
            {
                copy_tree = false;
                is_inspecting = true;
                debug_inspector->PopNode();
                return;
            }
            if(is_inspecting)
                return;
        }
        #endif

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
        #if UI_ENABLE_DEBUG
        if(debug_inspector)
        {
            if(copy_tree) 
            {
                DebugBox box;
                box.debug_info = debug_info;
                box.style = style;
                if(label)
                {
                    box.label = debug_inspector->CopyStringToArena(label);
                    assert(box.label && "Inspector out of memory");
                }
                debug_inspector->PushNode(box);
                return;
            }
            if(is_inspecting)
                return;
        }
        #endif


        if(HasInternalError())
            return;
        element_count++;

        //Input Handling
        uint64_t label_hash = Hash(label);
        if(label)
        {
            //BoxInfo will be filled in next frame 
            BoxInfo box_info;
            box_info.key = label_hash;
            bool err = double_buffer_map.Insert(label_hash, box_info);
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
        #if UI_ENABLE_DEBUG
        if(debug_inspector)
        {
            if(copy_tree)
            {
                debug_inspector->PopNode();
                return;
            }
            if(is_inspecting)
                return;
        }
        #endif


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
        stack.Pop();
        if(!stack.IsEmpty())
        {
            TreeNode* grand_parent = stack.Peek();
            assert(grand_parent);
            if(HandleInternalError(CheckNodeContradictions(parent_box, grand_parent->box)))
                return;
        }
    }



    void Context::InsertText(const char* text, const char* label, bool should_copy, DebugInfo debug_info)
    {
        #if UI_ENABLE_DEBUG
        if(debug_inspector)
        {
            if(copy_tree)
            {
                DebugBox box;
                box.debug_info = debug_info;
                if(text)
                {
                    box.text = debug_inspector->CopyStringToArena(text);
                    assert(box.text && "Inspector out of memory");
                }
                if(label)
                {
                    box.label = debug_inspector->CopyStringToArena(label);
                    assert(box.label && "Inspector out of memory");
                }
                debug_inspector->PushNode(box);
                debug_inspector->PopNode();
                return;
            }
            if(is_inspecting)
                return;
        }
        #endif


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
        TreeNode node;
        Box box;

        if(label)
        {
            //BoxInfo will be filled in next frame 
            box.label_hash = Hash(label);
            bool no_err = double_buffer_map.Insert(box.label_hash, BoxInfo());
            assert(no_err && "HashMap out of memory, go to BeginRoot and change it");
        }

        if(should_copy && text)
        {
            text = ArenaCopyString(text, &arena);
            assert(text && "Arena out of memory");
        }
        box.text = text;

        Markdown md;
        md.SetInput(text, 9999, 9999);
        while(md.ComputeNextTextRun()){}

        if(parent_box.width_unit == Unit::Type::CONTENT_PERCENT)
        {
            box.width = md.GetMeasuredWidth() + 1;
        }
        else if (parent_box.min_width_unit == Unit::Type::CONTENT_PERCENT)
        {
            box.width = md.GetMeasuredWidth() + 1;
        }
        else
        {
            box.width_unit = Unit::Type::AVAILABLE_PERCENT;
            box.width = 100;
            box.max_width = md.GetMeasuredWidth() + 1;
        }

        node.box = box;
        TreeNode* addr = parent_node->children.Add(node, &arena);
        assert(addr && "Arena out of memory");
    }



    void Context::Draw()
    {
        #if UI_ENABLE_DEBUG
        if(is_inspecting && debug_inspector)
        {
            debug_inspector->RunDebugInspector(0, 0, GetScreenWidth(), GetScreenHeight(), GetMouseX(), GetMouseY()); 
            return;
        }
        #endif



        if(HasInternalError())
            return;
        assert(root_node && "No RootNode Provided");
        
        if(!stack.IsEmpty())
        {
            HandleInternalError(Error{Error::Type::ROOT_NODE_CONTRADICTION, "Missing EndRoot()"});
            return;
        }
        const Box& root_box = root_node->box;


        //Layout pipeline
        WidthContentPercentPass(root_node);
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
    void Context::WidthContentPercentPass_Flow(TreeNode* node)
    {
        assert(node);
        Box& parent_box = node->box;
        int content_width = 0;
        if(parent_box.GetFlowAxis() == Flow::Axis::HORIZONTAL)
        {
            for(auto temp = node->children.GetHead(); temp != nullptr; temp = temp->next)
            {
                WidthContentPercentPass(&temp->value);
                Box& box = temp->value.box;
                if(box.IsDetached())
                    continue;
                if(box.width_unit != Unit::Type::AVAILABLE_PERCENT &&
                box.width_unit != Unit::Type::PARENT_PERCENT &&
                box.max_width_unit != Unit::Type::PARENT_PERCENT &&
                box.min_width_unit != Unit::Type::PARENT_PERCENT)
                {
                    box.width = Clamp(box.width, box.min_width, box.max_width);
                    content_width += box.GetBoxModelWidth();
                }
                else
                {
                    content_width += box.GetBoxExpansionWidth() + box.min_width;
                }
                content_width += parent_box.gap_column;
            }
            content_width -= parent_box.gap_column;
        }
        else
        {
            int largest_width = 0;
            for(auto temp = node->children.GetHead(); temp != nullptr; temp = temp->next)
            {
                WidthContentPercentPass(&temp->value);
                Box& box = temp->value.box;
                if(box.IsDetached())
                    continue;
                if(box.width_unit != Unit::Type::AVAILABLE_PERCENT &&
                box.width_unit != Unit::Type::PARENT_PERCENT &&
                box.max_width_unit != Unit::Type::PARENT_PERCENT &&
                box.min_width_unit != Unit::Type::PARENT_PERCENT)
                {
                    box.width = Clamp(box.width, box.min_width, box.max_width);
                    int width = box.GetBoxModelWidth();
                    if(largest_width < width)
                        largest_width = width;
                }
                else
                {
                    int width = box.GetBoxExpansionWidth() + box.min_width;
                    if(largest_width < width)
                        largest_width = width;
                }
            }
            content_width = largest_width;
        }
        content_width = Max(0, content_width);
        if(parent_box.width_unit == Unit::Type::CONTENT_PERCENT)
            parent_box.width = content_width * parent_box.width / 100;
        if(parent_box.min_width_unit == Unit::Type::CONTENT_PERCENT)
            parent_box.min_width = content_width * parent_box.min_width / 100;
        if(parent_box.max_width_unit == Unit::Type::CONTENT_PERCENT)
            parent_box.max_width = content_width * parent_box.max_width / 100;
    }
    void Context::WidthContentPercentPass(TreeNode* node)
    {
        if(!node)
            return;
        const Box& box = node->box;
        if(box.GetLayout() == Layout::FLOW)
        {
            WidthContentPercentPass_Flow(node);
        }
        else
        {
            assert(0 && "Have not added grid yet");
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

                //Ignore these values
                if(box.height_unit != Unit::Type::AVAILABLE_PERCENT && 
                    box.height_unit != Unit::Type::PARENT_PERCENT && 
                    box.min_height_unit != Unit::Type::PARENT_PERCENT && 
                    box.max_height_unit != Unit::Type::PARENT_PERCENT)
                {
                    box.height = Clamp(box.height, box.min_height, box.max_height);
                    int height = box.GetBoxModelHeight();
                    if(largest_height < height)
                        largest_height = height;
                }
                else
                {
                    int height = box.GetBoxExpansionHeight() + box.min_height;
                    if(largest_height < height)
                        largest_height = height;
                }
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
                //Ignore these values
                if(box.height_unit != Unit::Type::AVAILABLE_PERCENT && 
                    box.height_unit != Unit::Type::PARENT_PERCENT && 
                    box.min_height_unit != Unit::Type::PARENT_PERCENT && 
                    box.max_height_unit != Unit::Type::PARENT_PERCENT)
                {
                    box.height = Clamp(box.height, box.min_height, box.max_height);
                    content_height += box.GetBoxModelHeight();
                }
                else
                {
                    content_height += box.GetBoxExpansionHeight() + box.min_height;
                }
                content_height += parent_box.gap_row;
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
        int render_x = box.x + box.margin.left + x;
        int render_y = box.y + box.margin.top + y;
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
                info->width = box.width;
                info->height = box.height;
                info->x = box.x - box.margin.left;
                info->y = box.y - box.margin.top;
                info->padding = box.padding;
                info->margin = box.margin;
                info->is_rendered = true;
                //Handling mouse hover next frame
                if(Rect::Contains(Rect::Intersection(parent_aabb, Rect{render_x, render_y, render_width, render_height}), GetMouseX(), GetMouseY()))
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
                    ComputeDetachPositions(temp->value.box, parent_box);
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
                int layout_x =        x + cursor_x - parent_box.scroll_x + parent_box.padding.left;
                int layout_y =        y + cursor_y - parent_box.scroll_y + parent_box.padding.top;
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
                    ComputeDetachPositions(temp->value.box, parent_box);
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
                int layout_x =        x + cursor_x - parent_box.scroll_x + parent_box.padding.left;
                int layout_y =        y + cursor_y - parent_box.scroll_y + parent_box.padding.top;
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




    //Builder Implementation
    Builder& Builder::Text(const char* text, const char* id, bool should_copy, DebugInfo debug_info)
    {
        ClearStates();
        if(HasContext())
        {
            this->debug_info = debug_info;
            this->text = text;
            this->id = id;
            this->should_copy = should_copy;
            info = context->Info(id);
        }
        return *this;
    }
    Builder& Builder::Box(const char* id, DebugInfo debug_info)
    {
        ClearStates();
        if(HasContext())
        {
            this->debug_info = debug_info;
            this->id = id; 
            info = context->Info(id);
        }
        return *this;
    }

}