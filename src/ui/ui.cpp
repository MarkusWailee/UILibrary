#include "ui.hpp"
#include "Memory.hpp"

namespace UI
{
    using namespace Internal;
    void DisplayError(const Error& error);
    Error CheckUnitErrors(const BoxCore& style);
    Error CheckLeafNodeContradictions(const BoxCore& leaf);
    Error CheckNodeContradictions(const BoxCore& child, const BoxCore& parent);

    //Used during tree descending
    int FixedUnitToPx(Unit unit, int root_size);
    BoxCore ComputeStyleSheet(const BoxStyle& style, const BoxCore& root);

    //Size should include '\0'
    void StringCopy(char* dst, const char* src, uint32_t size);
    //returns pointer pointing to arena

    //size should includes '\0' if null terminated string are used

    bool StringCompare(const char* s1, const char* s2);
    char ToLower(char c);

    //Does not count '\0'
    uint32_t StrToU32(const char* text, bool* error = nullptr);
    uint32_t HexToU32(const char* text, bool* error = nullptr);
    Color HexToRGBA(const char* text, bool* error = nullptr);


    //Computing PARENT_PERCENT
    int ParentPercentToPx(int value , Unit::Type unit_type, int parent_width);
    void ComputeParentWidthPercent(BoxCore& box, int parent_width);
    void ComputeParentHeightPercent(BoxCore& box, int parent_width);


    inline void BeginScissorMode_impl(const Rect& rect) { BeginScissorMode_impl((float)rect.x, (float)rect.y, (float)rect.width, (float)rect.height);}

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
        assert(!context_queue.IsFull() && "Missing Draw or too many contexts");
        context_stack.Push(context);
        context_queue.Push(context);
    }
    Context* GetContext()
    {
        assert(!context_stack.IsEmpty() && context_stack.Peek() && "No context has been pushed");
        return context_stack.Peek();
    }
    BoxInfo Info(const char* id)
    {
        if(IsContextActive())
            return GetContext()->Info(id);
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
    void BeginBox(const UI::BoxStyle& box_style, const char* id, DebugInfo debug_info)
    {
        if(IsContextActive())
            GetContext()->BeginBox(box_style, id, debug_info);
    }
    void InsertText(const char16_t* text, const char* id, bool copy_text, DebugInfo debug_info)
    {
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
    Builder& Box(const BoxStyle& style, const char* id, DebugInfo debug_info)
    {
        return builder.Box(style, id, debug_info);
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

    inline void BoxCore::SetFlowAxis(Flow::Axis axis){flow_axis = axis;}
    inline void BoxCore::SetScissor(bool flag){scissor = flag;}
    inline Layout BoxCore::GetLayout() const
    {
        return layout;
    }
    inline Flow::Axis BoxCore::GetFlowAxis() const
    {
        return flow_axis;
    }
    inline bool BoxCore::IsScissor() const
    {
        return scissor;
    }
    inline bool BoxCore::IsDetached() const
    {
        return detach != Detach::NONE;
    }
    inline bool BoxCore::IsTextElement() const
    {
        return !text.IsEmpty();
    }
    inline int BoxCore::GetBoxExpansionWidth() const
    {
        return margin.left + margin.right + padding.left + padding.right;
    }
    inline int BoxCore::GetBoxExpansionHeight() const
    {
        return margin.top + margin.bottom + padding.top + padding.bottom;
    }
    inline int BoxCore::GetBoxModelWidth() const
    {
        //internal box model
        return margin.left + padding.left + width + padding.right + margin.right;
    }
    inline int BoxCore::GetBoxModelHeight() const
    {
        return margin.top + padding.top + height + padding.bottom + margin.bottom;
    }
    inline int BoxCore::GetRenderingWidth() const
    {
        return padding.left + width + padding.right;
    }
    inline int BoxCore::GetRenderingHeight() const
    {
        return padding.top + height + padding.bottom;
    }
    inline int BoxCore::GetGridCellWidth() const
    {
        return (width - gap_column * (grid_column_count - 1)) / Max((uint8_t)1, grid_column_count);
    }
    inline int BoxCore::GetGridCellHeight() const
    {
        return (height - gap_row * (grid_row_count - 1)) / Max((uint8_t)1, grid_row_count);
    }


    inline void BoxResult::SetComputedResults(BoxCore& node)
    {
        UpdatePointer(node);
        this->rel_x = node.result_rel_x;
        this->rel_y = node.result_rel_y;
        this->draw_width = node.GetRenderingWidth();
        this->draw_height = node.GetRenderingHeight();
        this->content_width = node.result_content_width;
        this->content_height = node.result_content_height;
    }
    inline void BoxResult::UpdatePointer(BoxCore& node)
    {
        this->core = &node;
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
    Error CheckUnitErrors(const BoxCore& style)
    {
        //The following units cannot equal the specified Unit Types 

        //Content%
        Error error;
        #if UI_ENABLE_DEBUG
            DebugInfo debug_info = style.debug_info;
            UNIT_CONFLICT(style.min_width_unit,             Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
            UNIT_CONFLICT(style.min_height_unit,            Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
            UNIT_CONFLICT(style.max_width_unit,             Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
            UNIT_CONFLICT(style.max_height_unit,            Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        #endif
        return error;
    }



    Error CheckLeafNodeContradictions(const BoxCore& leaf)
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

    Error CheckNodeContradictions(const BoxCore& child, const BoxCore& parent)
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
    BoxCore ComputeStyleSheet(const BoxStyle& style, const BoxCore& root)
    {
        int root_width = root.width - style.margin.left - style.margin.right - style.padding.left - style.padding.right;
        int root_height = root.height - style.margin.top - style.margin.bottom - style.padding.top - style.padding.bottom;
        root_width = Max(0, root_width);
        root_height = Max(0, root_height);

        BoxCore box;
        box.texture = style.texture;

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

        box.width_unit =                style.width.unit;
        box.height_unit =               style.height.unit;
        box.min_width_unit =            style.min_width.unit;
        box.max_width_unit =            style.max_width.unit;
        box.min_height_unit =           style.min_height.unit;
        box.max_height_unit =           style.max_height.unit;

        box.grid_row_count =            Max((uint8_t)1, style.grid.row_count);
        box.grid_column_count =         Max((uint8_t)1, style.grid.column_count);
        box.grid_x =                    style.grid.x;
        box.grid_y =                    style.grid.y;
        box.grid_span_x =               Max((uint8_t)1, style.grid.span_x);
        box.grid_span_y =               Max((uint8_t)1, style.grid.span_y);

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
    int StrAsciLength(const char* text)
    {
        /*
        if(!text)
            return -1;
        int i = 0;
        for(; text[i]; i++);
        return i;
        */
       if(!text)
            return -1;
        return std::strlen(text);
    }
    int StrU16Length(const char16_t* text)
    {
        if(!text)
            return -1;
        int i = 0;
        for(; text[i]; i++);
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





    //Width
    int ParentPercentToPx(int value , Unit::Type unit_type, int parent_size)
    {
        return unit_type == Unit::Type::PARENT_PERCENT? value * parent_size / 100: value;
    }
    void ComputeParentWidthPercent(BoxCore& box, int parent_width)
    {
        parent_width -= box.padding.left + box.padding.right + box.margin.left + box.margin.right;
        parent_width = Max(0, parent_width);
        box.width =                     (uint16_t)Max(0, ParentPercentToPx(box.width,            box.width_unit,             parent_width)); 
        box.min_width =                 (uint16_t)Max(0, ParentPercentToPx(box.min_width,        box.min_width_unit,         parent_width)); 
        box.max_width =                 (uint16_t)Max(0, ParentPercentToPx(box.max_width,        box.max_width_unit,         parent_width)); 
    }

    //Height
    void ComputeParentHeightPercent(BoxCore& box, int parent_height)
    {
        parent_height -= box.padding.top + box.padding.bottom + box.margin.top + box.margin.bottom;
        parent_height = Max(0, parent_height);
        box.height =                    (uint16_t)Max(0, ParentPercentToPx(box.height,           box.height_unit,            parent_height)); 
        box.gap_row =                   (uint16_t)Max(0, ParentPercentToPx(box.gap_row,          box.gap_row_unit,           parent_height)); 
        box.min_height =                (uint16_t)Max(0, ParentPercentToPx(box.min_height,       box.min_height_unit,        parent_height)); 
        box.max_height =                (uint16_t)Max(0, ParentPercentToPx(box.max_height,       box.max_height_unit,        parent_height)); 
    }
    void ComputeWidthPercentForHeight(BoxCore& box)
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
        arena1(arena_bytes), arena2(arena_bytes), arena3(arena_bytes)
    {
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
        #endif
        return BoxInfo();
    }
    BoxInfo Context::Info(const char* id)
    {
        return Info(StrHash(id));
    }
    void Context::ResetAllStates()
    {
        arena1.Rewind(tree_core);

        stack.Clear();
        deferred_elements.Clear();
        tree_core = nullptr;
        element_count = 0;
        directly_hovered_element_key = 0;
    }

    void Context::ResetArena1()
    {
        arena1.Reset();
        stack.Clear();
        tree_core = nullptr;
    }
    void Context::ResetArena2()
    {
        arena2.Reset();
        tree_result = nullptr;
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
        #endif


        if(HasInternalError())
            return;
        ResetArena1();


        assert(stack.IsEmpty());
        BoxCore root_box = ComputeStyleSheet(style, BoxCore());
        // ========== Debug Mode Only ==========
        #if UI_ENABLE_DEBUG
            root_box.debug_info = debug_info;
        #endif

        if(stack.IsEmpty())//Root Node
        {
            //Checking errors unique to root node
            tree_core = arena1.New<TreeNode<BoxCore>>();
            assert(tree_core && "Arena out of space");
            tree_core->box = root_box;
            stack.Push(tree_core);
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

    void Context::BeginBox(const UI::BoxStyle& style, const char* id, DebugInfo debug_info)
    {
        #if UI_ENABLE_DEBUG
        #endif


        if(HasInternalError())
            return;
        element_count++;

        //Input Handling
        uint64_t id_key = 0;

        if(!stack.IsEmpty())  // should add to parent
        {
            TreeNode<BoxCore>* parent_node = stack.Peek();
            assert(parent_node);
            assert(tree_core);

            TreeNode<BoxCore> child_node;
            child_node.box = ComputeStyleSheet(style, tree_core->box);
            child_node.box.id_key = id_key;

            //Might bundle this with macro
            if(HandleInternalError(CheckUnitErrors(child_node.box)))
                return;

            TreeNode<BoxCore>* child_ptr = parent_node->children.Add(child_node, &arena1); 
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
        #endif


        if(HasInternalError())
            return;
        if(stack.Size() <= 1)
        {
            HandleInternalError(Error{Error::Type::MISSING_BEGIN, "Missing BeginBox()"});
            return;
        }
        TreeNode<BoxCore>* node = stack.Peek();
        assert(node);
        BoxCore& parent_box = node->box;
        stack.Pop();
        if(!stack.IsEmpty())
        {
            TreeNode<BoxCore>* grand_parent = stack.Peek();
            assert(grand_parent);
            if(HandleInternalError(CheckNodeContradictions(parent_box, grand_parent->box)))
                return;
        }
    }

    void Context::InsertText(const UI::TextStyle& style, const StringU8& string, const char* id, bool copy_text, DebugInfo info)
    {
        //Need some special decoding StringU8 to StringU32
        assert(0 && "have not made function yet");
    }
    void Context::InsertText(const UI::TextStyle& style, const StringU32& string, const char* id, bool copy_text, DebugInfo info)
    {
        #if UI_ENABLE_DEBUG
        #endif

        if(HasInternalError())
            return;
        if(stack.IsEmpty())
        {
            assert(0 && "Stack is empty. Most likely inserting text without a root node");
            return;
        }
        TreeNode<BoxCore>* parent_node = stack.Peek();
        assert(parent_node);
    }




    void Context::Draw()
    {
        #if UI_ENABLE_DEBUG
        #endif
        


        if(HasInternalError())
            return;
        assert(tree_core && "No RootNode Provided");
        
        if(!stack.IsEmpty())
        {
            HandleInternalError(Error{Error::Type::ROOT_NODE_CONTRADICTION, "Missing EndRoot()"});
            return;
        }


        //Layout pipeline
        WidthContentPercentPass(tree_core);
        WidthPass(tree_core);
        HeightContentPercentPass(tree_core);
        HeightPass(tree_core);
        PositionPass(tree_core, 0, 0, BoxCore());
        GenerateComputedTree();

        DrawPass(tree_result, 0, 0, {0, 0, GetScreenWidth(), GetScreenHeight()});
        while(!deferred_elements.IsEmpty())
        {
            const DeferredBox& box = deferred_elements.GetHead()->value;
            DrawPass(box.node, box.x, box.y, {0, 0, GetScreenWidth(), GetScreenHeight()});
            deferred_elements.PopHead();
        }
    }
    void Context::WidthContentPercentPass_Flow(TreeNode<BoxCore>* node)
    {
        assert(node);
        BoxCore& parent_box = node->box;
        int content_width = 0;
        if(parent_box.GetFlowAxis() == Flow::Axis::HORIZONTAL)
        {
            for(auto temp = node->children.GetHead(); temp != nullptr; temp = temp->next)
            {
                WidthContentPercentPass(&temp->value);
                BoxCore& box = temp->value.box;
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
                BoxCore& box = temp->value.box;
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
    void Context::WidthContentPercentPass_Grid(TreeNode<BoxCore>* node)
    {
        assert(node);
        BoxCore& parent_box = node->box;
        int cell_width = 0;

        //Finding the largest cell width
        for(auto temp = node->children.GetHead(); temp != nullptr; temp = temp->next)
        {
            WidthContentPercentPass(&temp->value);
            BoxCore& box = temp->value.box;
            if(box.IsDetached())
                return;
            if(box.width_unit != Unit::Type::AVAILABLE_PERCENT &&
            box.width_unit != Unit::Type::PARENT_PERCENT &&
            box.max_width_unit != Unit::Type::PARENT_PERCENT &&
            box.min_width_unit != Unit::Type::PARENT_PERCENT)
            {
                box.width = Clamp(box.width, box.min_width, box.max_width);
                int width = box.GetBoxModelWidth() / box.grid_span_x;
                if(cell_width < width)
                    cell_width = width;
            }
            else
            {
                int width = box.GetBoxExpansionWidth() + box.min_width;
                if(cell_width < width)
                    cell_width = width;
            }
        }
        int total_width = cell_width * parent_box.grid_column_count + parent_box.gap_column * (parent_box.grid_column_count - 1);
        if(parent_box.width_unit == Unit::Type::CONTENT_PERCENT)
            parent_box.width = total_width;
        if(parent_box.min_width_unit == Unit::Type::CONTENT_PERCENT)
            parent_box.min_width = total_width;
        if(parent_box.max_width_unit == Unit::Type::CONTENT_PERCENT)
            parent_box.max_width = total_width;
    }
    void Context::WidthContentPercentPass(TreeNode<BoxCore>* node)
    {
        if(!node)
            return;
        const BoxCore& box = node->box;
        if(box.GetLayout() == Layout::FLOW)
        {
            WidthContentPercentPass_Flow(node);
        }
        else
        {
            WidthContentPercentPass_Grid(node);
        }
    }
    void Context::WidthPass(TreeNode<BoxCore>* node)
    {
        if(node == nullptr || node->children.IsEmpty())
            return;
        BoxCore& box = node->box;

        //Might aswell compute this here since width is all calculated
        ComputeWidthPercentForHeight(box);

        if(box.GetLayout() == Layout::FLOW)
        {
            WidthPass_Flow(node->children.GetHead(), box);
        }
        else
        {
            WidthPass_Grid(node->children.GetHead(), box);
        }
    }

    void Context::WidthPass_Flow(ArenaLL<TreeNode<BoxCore>>::Node* child, const BoxCore& parent_box)
    {
        assert(child);
        ArenaLL<TreeNode<BoxCore>>::Node* temp;

        struct GrowBox {
            BoxCore* box = nullptr;
            float result = 0;
        };
        ArenaLL<GrowBox> growing_elements;

        if(parent_box.GetFlowAxis() == Flow::Axis::HORIZONTAL)
        {
            float available_width = parent_box.width;
            float total_percent = 0;
            for(temp = child; temp != nullptr; temp = temp->next)
            {
                BoxCore& box = temp->value.box;
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
                    bool err = (bool)growing_elements.Add(GrowBox{&box, 0}, &arena1);
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
            arena1.Rewind(growing_elements.GetHead());
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
                BoxCore& box = temp->value.box;
                if(box.width_unit == Unit::Type::AVAILABLE_PERCENT)
                    box.width_unit = Unit::Type::PARENT_PERCENT;
                ComputeParentWidthPercent(box, parent_box.width);
                box.width = Clamp(box.width, box.min_width, box.max_width);
                WidthPass(&temp->value);
            }
        } //End vertical
    }
    void Context::WidthPass_Grid(Internal::ArenaLL<TreeNode<BoxCore>>::Node* child, const BoxCore& parent_box) //Recurse Helpe
    {
        assert(child);

        int cell_width = parent_box.GetGridCellWidth();
        for(auto temp = child; temp!=nullptr; temp = temp->next)
        {
            BoxCore& box = temp->value.box;
            if(box.width_unit == Unit::Type::AVAILABLE_PERCENT)
                box.width_unit = Unit::Type::PARENT_PERCENT;
            
            ComputeParentWidthPercent(box, cell_width * box.grid_span_x + parent_box.gap_column * (box.grid_span_x - 1));
            box.width = Clamp(box.width, box.min_width, box.max_width);
            WidthPass(&temp->value);
        }
    }



    void Context::HeightPass(TreeNode<BoxCore>* node)
    {
        if(node == nullptr || node->children.IsEmpty())
            return;
        const BoxCore& box = node->box;
        if(box.GetLayout() == Layout::FLOW)
        {
            HeightPass_Flow(node->children.GetHead(), box);
        }
        else
        {
            HeightPass_Grid(node->children.GetHead(), box);
        }
    }
    void Context::HeightPass_Flow(ArenaLL<TreeNode<BoxCore>>::Node* child, const BoxCore& parent_box)
    {
        assert(child);
        ArenaLL<TreeNode<BoxCore>>::Node* temp;

        struct GrowBox {
            BoxCore* box = nullptr;
            float result = 0;
        };
        ArenaLL<GrowBox> growing_elements;

        if(parent_box.GetFlowAxis() == Flow::Axis::VERTICAL)
        {
            float available_height = parent_box.height;
            float total_percent = 0;
            for(temp = child; temp != nullptr; temp = temp->next)
            {
                BoxCore& box = temp->value.box;
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
                    bool err = growing_elements.Add(GrowBox{&box, 0}, &arena1);
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
            arena1.Rewind(growing_elements.GetHead());
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
                BoxCore& box = temp->value.box;
                if(box.height_unit == Unit::Type::AVAILABLE_PERCENT)
                    box.height_unit = Unit::Type::PARENT_PERCENT;
                ComputeParentHeightPercent(box, parent_box.height);
                box.height = Clamp(box.height, box.min_height, box.max_height);
                HeightPass(&temp->value);
            }
        }
    }
    void Context::HeightPass_Grid(Internal::ArenaLL<TreeNode<BoxCore>>::Node* child, const BoxCore& parent_box) //Recurse Helpe
    {
        assert(child);
        int cell_height = parent_box.GetGridCellHeight();
        for(auto temp = child; temp!=nullptr; temp = temp->next)
        {
            BoxCore& box = temp->value.box;
            if(box.height_unit == Unit::Type::AVAILABLE_PERCENT)
                box.height_unit = Unit::Type::PARENT_PERCENT;
            ComputeParentHeightPercent(box, cell_height * box.grid_span_y + parent_box.gap_row * (box.grid_span_y - 1));
            box.height = Clamp(box.height, box.min_height, box.max_height);
            HeightPass(&temp->value);
        }
    }


    void Context::HeightContentPercentPass_Flow(TreeNode<BoxCore>* node)
    {
        assert(node);
        BoxCore& parent_box = node->box;
        ArenaLL<TreeNode<BoxCore>>::Node* child = node->children.GetHead();
        int content_height = 0;
        if(parent_box.GetFlowAxis() == Flow::Axis::HORIZONTAL)
        {
            int largest_height = 0;
            for(ArenaLL<TreeNode<BoxCore>>::Node* temp = child; temp != nullptr; temp = temp->next)
            {
                HeightContentPercentPass(&temp->value);
                BoxCore& box = temp->value.box;
                
                if(box.IsDetached()) //Ignore layout for detached boxes
                    continue;

                /* Measure text height here
                if(box.text)
                {
                    //Width should be computed by this point
                    Markdown md;
                    box.width = Min(parent_box.width, box.width);
                    md.SetInput(box.text, box.width, INT_MAX);
                    while(md.ComputeNextTextRun()){}
                    box.height = md.GetMeasuredHeight();
                }
                */

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
            for(ArenaLL<TreeNode<BoxCore>>::Node* temp = child; temp != nullptr; temp = temp->next)
            {
                HeightContentPercentPass(&temp->value);
                BoxCore& box = temp->value.box;

                if(box.IsDetached()) //Ignore layout for detached boxes
                    continue;

                /* Measure text height here
                if(box.text)
                {
                    //Width should be computed by this point
                    Markdown md;
                    box.width = Min(parent_box.width, box.width);
                    md.SetInput(box.text, box.width, INT_MAX);
                    while(md.ComputeNextTextRun()){}
                    box.height = md.GetMeasuredHeight();
                }
                */
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
    void Context::HeightContentPercentPass_Grid(TreeNode<BoxCore>* node)
    {
        assert(node);
        BoxCore& parent_box = node->box;
        int content_width = 0;
        for(auto temp = node->children.GetHead(); temp != nullptr; temp = temp->next)
        {
            WidthContentPercentPass(&temp->value);
            BoxCore& box = temp->value.box;
        }
    }


    void Context::HeightContentPercentPass(TreeNode<BoxCore>* node)
    {
        if(!node)    
            return;
        assert(node);
        const BoxCore& box = node->box;
        if(box.GetLayout() == Layout::FLOW)
        {
            HeightContentPercentPass_Flow(node);
        }
        else
        {
            HeightContentPercentPass_Grid(node);
        }
    }



    void Context::PositionPass(TreeNode<BoxCore>* node, int x, int y, const BoxCore& parent_box)
    {
        if(node == nullptr)
            return;
        BoxCore& box = node->box;
        box.result_rel_x += parent_box.padding.left + box.margin.left;
        box.result_rel_y += parent_box.padding.top + box.margin.right;
        x += box.result_rel_x;
        y += box.result_rel_y;

        if(node->children.IsEmpty())
            return;

        if(box.GetLayout() == Layout::FLOW)
        {
            PositionPass_Flow(node->children.GetHead(), x, y, box);
        }
        else
        {
            PositionPass_Grid(node->children.GetHead(), x, y, box);
        }
    }
    void Context::PositionPass_Flow(Internal::ArenaLL<TreeNode<BoxCore>>::Node* child, int x, int y, const BoxCore& parent)
    {
        assert(child);
        int content_height = 0;
        int content_width = 0;
        if(parent.GetFlowAxis() == Flow::HORIZONTAL)
        {
            int count = 0;
            for(auto temp = child; temp != nullptr; temp = temp->next)
            {
                const BoxCore& box = temp->value.box;
                if(box.IsDetached()) continue;
                count++;
                content_width += box.GetBoxModelWidth();
            }
            content_width += Max(0, count - 1) * parent.gap_column;
            int cursor_x = 0;
            int offset = 0;
            int available_width = parent.width - content_width;
            switch(parent.flow_horizontal_alignment)
            {
                case Flow::END:             cursor_x = available_width; break;
                case Flow::CENTERED:        cursor_x = available_width/2; break;
                case Flow::SPACE_AROUND:    cursor_x = offset = available_width / (count + 1); break;
                case Flow::SPACE_BETWEEN:   if(count > 1) offset = available_width / (count - 1); break;
                default: break;
            }
            for(auto temp = child; temp != nullptr; temp = temp->next)
            {
                BoxCore& box = temp->value.box;
                if(box.IsDetached())
                {
                    PositionPass(&temp->value, x, y, parent);
                    continue;
                }
                int cursor_y = 0;
                int box_height = box.GetBoxModelHeight();
                if(content_height < box_height)
                    content_height = box_height;
                int available_height = parent.height - box_height;
                switch(parent.flow_vertical_alignment)
                {
                    case Flow::START:   break;
                    case Flow::END:     cursor_y = available_height; break;
                    default:            cursor_y = available_height/2; break; 
                }

                box.result_rel_x = cursor_x;
                box.result_rel_y = cursor_y;

                PositionPass(&temp->value, x, y, parent);
                cursor_x += box.GetBoxModelWidth() + parent.gap_column + offset;
            }
        }
        else
        {
            int count = 0;
            for(auto temp = child; temp != nullptr; temp = temp->next)
            {
                const BoxCore& box = temp->value.box;
                if(box.IsDetached()) continue;
                count++;
                content_height += box.GetBoxModelHeight();
            }
            content_height += Max(0, count - 1) * parent.gap_row;
            int cursor_y = 0;
            int offset = 0;
            int available_height = parent.height - content_height;
            switch(parent.flow_vertical_alignment)
            {
                case Flow::END:             cursor_y = available_height; break;
                case Flow::CENTERED:        cursor_y = available_height/2; break;
                case Flow::SPACE_AROUND:    cursor_y = offset = available_height / (count + 1); break;
                case Flow::SPACE_BETWEEN:   if(count > 1) offset = available_height / (count - 1); break;
                default: break;
            }
            for(auto temp = child; temp != nullptr; temp = temp->next)
            {
                BoxCore& box = temp->value.box;
                if(box.IsDetached())
                {
                    PositionPass(&temp->value, x, y, parent);
                    continue;
                }
                int cursor_x = 0;
                int box_width = box.GetBoxModelWidth();
                if(content_width < box_width)
                    content_width = box_width;
                int available_width = parent.width - box_width;
                switch(parent.flow_horizontal_alignment)
                {
                    case Flow::START:   break;
                    case Flow::END:     cursor_x = available_width; break;
                    default:            cursor_x = available_width/2; break; 
                }

                box.result_rel_x = cursor_x;
                box.result_rel_y = cursor_y;

                PositionPass(&temp->value, x, y, parent);
                cursor_y += box.GetBoxModelHeight() + parent.gap_row + offset;
            }
        }
    }
    void Context::PositionPass_Grid(Internal::ArenaLL<TreeNode<BoxCore>>::Node* child, int x, int y, const BoxCore& parent)
    {
        assert(child);
        int cell_width = parent.GetGridCellWidth() + parent.gap_column;
        int cell_height = parent.GetGridCellHeight() + parent.gap_row;
        for(auto temp = child; temp!=nullptr; temp = temp->next)
        {
            BoxCore& box = temp->value.box;
            box.result_rel_x = cell_width * box.grid_x;
            box.result_rel_y = cell_height * box.grid_y;
            PositionPass(&temp->value, x, y, parent);
        }
    }

    void Context::AddDetachedBoxToQueue(TreeNode<BoxResult>* node, const Rect& parent)
    {
        assert(node);
        const BoxResult& box = node->box;
        assert(box.core);

        DeferredBox result;
        result.node = node;

        switch(box.core->detach)
        {
           case Detach::LEFT:
                result.x = box.draw_height;
                break;
            case Detach::RIGHT:
                result.x = parent.width;
                break;
            case Detach::TOP:
                result.y = -box.draw_height;
                break;
            case Detach::BOTTOM:
                result.y = parent.height;
                break;
           case Detach::LEFT_CENTER:
                result.x = box.draw_width;
                result.y = (parent.height - box.draw_height)/2;
                break;
            case Detach::RIGHT_CENTER:
                result.x = parent.width;
                result.y = (parent.height - box.draw_height)/2;
                break;
            case Detach::TOP_CENTER:
                result.x = (parent.width - box.draw_width)/2;
                result.y = -box.draw_height;
                break;
            case Detach::BOTTOM_CENTER:
                result.x = (parent.width - box.draw_width)/2;
                result.y = parent.height;
                break;
           case Detach::LEFT_END:
                result.x = box.draw_width;
                result.y = parent.height - box.draw_height;
                break;
            case Detach::RIGHT_END:
                result.x = parent.width;
                result.y = parent.height - box.draw_height;
                break;
            case Detach::TOP_END:
                result.x = parent.width - box.draw_width;
                result.y = -box.draw_height;
                break;
            case Detach::BOTTOM_END:
                result.x = parent.width - box.draw_width;
                result.y = parent.height;
                break;
            default:
                break;
        }
        result.x += parent.x;
        result.y += parent.y;
        
        auto err = deferred_elements.Add(result, &arena2);
        assert(err && "Arena2 out of memory");
    }

    void Context::GenerateComputedTree()
    {
        if(!tree_core)
            return;

        ResetArena2();
        tree_result = arena2.New<TreeNode<BoxResult>>(); 
        assert(tree_result && "Arena2 out of memory");
        tree_result->box.SetComputedResults(tree_core->box);

        GenerateComputedTree_h(tree_core, tree_result);
    }
    void Context::GenerateComputedTree_h(TreeNode<BoxCore>* tree_core, TreeNode<BoxResult>* tree_result)
    {
        assert(tree_core && tree_result);
        for(auto temp = tree_core->children.GetHead(); temp != nullptr; temp = temp->next)
        {
            TreeNode<BoxResult> node;
            node.box.SetComputedResults(temp->value.box);

            TreeNode<BoxResult>* result_node = tree_result->children.Add(node, &arena2);
            assert(result_node && "Arena2 out of memory");

            GenerateComputedTree_h(&temp->value, result_node);
        }
    }

    void Context::DrawPass(TreeNode<BoxResult>* node, int parent_x, int parent_y, Rect scissor_aabb)
    {
        if(!node || !node->box.core)
            return;

        const BoxResult& box = node->box;
        const BoxCore& core = *node->box.core;

        Rect draw;
        draw.x = core.x + box.rel_x + parent_x;
        draw.y = core.y + box.rel_y + parent_y;
        draw.width = box.draw_width;
        draw.height = box.draw_height;


        //Render current box
        if(core.IsTextElement())
        {

        }
        else if(core.texture.HasTexture())
        {
            DrawTexturedRectangle_impl(draw.x, draw.y, draw.width, draw.height, core.texture);  
        }
        else
        {
            DrawRectangle_impl(draw.x, draw.y, draw.width, draw.height, core.corner_radius, core.border_width, core.border_color, core.background_color);
        }


        //Render children boxes
        int x = draw.x - core.scroll_x;
        int y = draw.y - core.scroll_y;

        if(core.IsScissor())
            scissor_aabb = Rect::Intersection(scissor_aabb, draw);

        for(auto temp = node->children.GetHead(); temp != nullptr; temp = temp->next)
        {
            assert(temp->value.box.core);
            if(temp->value.box.core->IsDetached())
            {
                AddDetachedBoxToQueue(&temp->value, draw);
                continue;
            }

            if(core.IsScissor())
                BeginScissorMode_impl(scissor_aabb);
            DrawPass(&temp->value, x, y, scissor_aabb);
        }
        if(core.IsScissor())
            EndScissorMode_impl();
    }


}