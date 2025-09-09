#include "ui.hpp"
#include "Memory.hpp"

namespace UI
{
    using namespace Internal;
    void DisplayError(const Error& error);
    Error CheckUnitErrors(const BoxCore& style);
    Error CheckNodeContradictions(const BoxCore& child, const BoxCore& parent);

    //Used during tree descending
    int FixedUnitToPx(Unit unit, int root_size);
    BoxCore ComputeStyleSheet(const BoxStyle& style, const BoxCore& root);

    //Text related functions
    int MeasureTextSpans(BoxCore& box);

    //size should includes '\0' if null terminated string are used



    //Computing PARENT_PERCENT
    int ParentPercentToPx(int value , Unit::Type unit_type, int parent_width);
    void ComputeParentWidthPercent(BoxCore& box, int parent_width);
    void ComputeParentHeightPercent(BoxCore& box, int parent_width);


    inline void BeginScissorMode_impl(const Rect& rect) { BeginScissorMode_impl((float)rect.x, (float)rect.y, (float)rect.width, (float)rect.height);}
    inline int MeasureChar_impl(char32_t c, const TextStyle& style) { return MeasureChar_impl(c, style.GetFontSize(), style.GetFontSpacing()); }

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
        assert(!context_stack.IsFull() && "Missing Draw function or too many contexts");
        assert(!context_queue.IsFull() && "Missing Draw or too many contexts");
        context_stack.Push(context);
        context_queue.Push(context);
    }

    Context* GetContext()
    {
        assert(!context_stack.IsEmpty() && context_stack.Peek() && "No context has been pushed");
        return context_stack.Peek();
    }

    BoxInfo Info(const StringAsci& id)
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

    void BeginBox(const UI::BoxStyle& box_style, const StringAsci& id, DebugInfo debug_info)
    {
        if(IsContextActive())
            GetContext()->BeginBox(box_style, id, debug_info);
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
    void Text(const TextStyle& style, const StringU32& string, bool copy_text, DebugInfo debug_info)
    {
        builder.Text(style, string, copy_text, debug_info);
    }
    Builder& Box(const BoxStyle& style, const StringAsci& id, DebugInfo debug_info)
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
    BoxState& State()
    {
        return builder.State();
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
    TextSpans::Iterator TextSpans::Iterator::Next() const
    {
        Iterator it = *this;
        if(!it.node)
            return it;
        const TextSpan& span = it.node->value;
        it.string_index++;
        if(it.string_index >= (int)span.Size()) //Iterate next
        {
            it.node = it.node->next;
            it.string_index = 0;
        }
        return it;
    }
    TextSpans::Iterator TextSpans::Iterator::Prev() const
    {
        Iterator it = *this;
        if(!it.node)
            return it;
        const TextSpan& span = it.node->value;
        it.string_index--;
        if(it.string_index < 0)
        {
            if(it.node->prev)
            {
                it.node = it.node->prev;
                it.string_index = (int)it.node->value.Size() - 1;
            }
            else
            {
                it.string_index = 0;
            }
        }
        return it;
    }

    char32_t TextSpans::Iterator::GetChar() const
    {
        assert(node && string_index >= 0 && string_index < node->value.Size()); // my own sanity
        return node->value[string_index];
    }
    TextStyle TextSpans::Iterator::GetStyle() const
    {
        assert(node && string_index >= 0 && string_index < node->value.Size()); // my own sanity
        return node->value.style;
    }
    bool TextSpans::Iterator::IsValid() const
    {
        return node;
    }

    StringU32 TextSpans::GetString(Iterator start, Iterator end)
    {
        assert(start.node);
        StringU32& string = start.node->value;
        if(start.node == end.node) //if they are the same style
        {
            int size = Max(0 ,end.string_index - start.string_index);
            return string.SubStr(start.string_index, size);
        }
        //Just returns the whole string in start
        return string.SubStr(start.string_index, -1);
    }

    TextSpan TextSpans::GetTextSpan(Iterator start, Iterator end)
    {
        assert(start.node);
        return TextSpan{GetString(start, end), start.node->value.style};
    }

    TextSpans::Iterator TextSpans::Begin()
    {
        if(GetHead())
            return Iterator{GetHead(), 0};
        return Iterator{};
    }

    //Index is initialized past the largest index when at end
    TextSpans::Iterator TextSpans::End()
    {
        if(GetTail())
            return Iterator{GetTail(), (int)GetTail()->value.Size()};
        return Iterator{};
    }

    inline BoxCore::Type BoxCore::GetElementType() const
    {
        return type;
    }

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
        return !text_style_spans.IsEmpty();
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
        this->text_lines = node.result_text_lines;
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
    Error CheckUnitErrors(const BoxCore& style)
    {
        return Error();
    }
    Error CheckNodeContradictions(const BoxCore& child, const BoxCore& parent)
    {
        return Error();
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
        box.type = style.texture.HasTexture()? BoxCore::Type::IMAGE: BoxCore::Type::BOX;

        box.background_color =          style.color;
        box.border_color =              style.border_color;
        //type 3

        box.scroll_x =                  style.scroll_x;
        box.scroll_y =                  style.scroll_y;

        box.x =                         (int16_t)style.x;
        box.y =                         (int16_t)style.y;
        box.width =                     (uint16_t)style.width.value;
        box.height =                    (uint16_t)style.height.value;
        box.gap_row =                   (uint16_t)style.gap_row;
        box.gap_column =                (uint16_t)style.gap_column;
        box.min_width =                 (uint16_t)style.min_width.value;
        box.max_width =                 (uint16_t)style.max_width.value;
        box.min_height =                (uint16_t)style.min_height.value;
        box.max_height =                (uint16_t)style.max_height.value;

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


    int MeasureTextSpans(BoxCore& box)
    {
        int result = 0;
        for(auto node = box.text_style_spans.GetHead(); node != nullptr; node = node->next)
        {
            for(int i = 0; i < node->value.Size(); i++)
            {
                result += MeasureChar_impl(node->value[i], node->value.style);
            }
        }
        return result;
    }


    //TEXT RENDERING
    StringAsci Fmt(const char *text, ...)
    {
        static int index = 0;
        constexpr uint32_t MAX_LENGTH = 512;
        constexpr uint32_t MAX_BUFFERS = 6;
        static char buffer[MAX_BUFFERS][MAX_LENGTH];  // Fixed-size static buffer
        index = (index + 1) % MAX_BUFFERS;

        va_list args;
        va_start(args, text);
        int count = vsnprintf(buffer[index], MAX_LENGTH, text, args);
        count = Clamp(count, 0, (int)MAX_LENGTH - 1);
        buffer[index][count] = '\0';
        va_end(args);
        return StringAsci{buffer[index], (uint64_t)count};
    }

    StringU32 AsciToStrU32(const StringAsci& str)
    {
        static int index = 0;
        constexpr uint32_t MAX_LENGTH = 512;
        constexpr uint32_t MAX_BUFFERS = 6;
        static char32_t buffer[MAX_BUFFERS][MAX_LENGTH]{};  // Fixed-size static buffer

        index = (index + 1) % MAX_BUFFERS;

        int size = Min((int)MAX_LENGTH-1, (int)str.Size());
        for(int i = 0; i < size; i++)
            buffer[index][i] = (char32_t)str[i];
        buffer[index][size] = U'\0';
        return StringU32{buffer[index], (uint64_t)size};
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
    Context::Context(uint64_t arena_bytes, uint64_t string_bytes) :
        arena1(arena_bytes), arena2(arena_bytes), arena3(string_bytes)
    {

        //Super rough estimate of how many elements we might be able to hold.
        int element_count = arena_bytes /
            (sizeof(TreeNode<BoxCore>) + sizeof(TreeNode<BoxResult>) + sizeof(Internal::ArenaMap<BoxInfo>::Item) * 2);
        double_buffer_map.AllocateBufferCapacity(element_count, &arena1); //assuming all elements need an id
        std::cout<<element_count<<'\n';
        std::cout<<(float)arena1.GetOffset() / arena1.Capacity()<<'\n';
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
    Internal::BoxCore::Type Context::GetPreviousNodeBoxType() const
    {
        if(prev_inserted_box)
            return prev_inserted_box->type;
        return BoxType::NONE;
    }
    BoxInfo Context::Info(uint64_t key)
    {
        #if UI_ENABLE_DEBUG
        #endif
        const BoxInfo* info = double_buffer_map.FrontValue(key);
        if(info)
            return *info;
        return BoxInfo();
    }
    void Context::SetStates(uint64_t key, const BoxState& state)
    {
        BoxInfo* info = double_buffer_map.FrontValue(key);
        if(info)
            info->state = state;
    }
    BoxInfo Context::Info(const StringAsci& id)
    {
        if(id.IsEmpty())
            return BoxInfo();
        return Info(HashBytes(id.data, id.Size()));
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

    void Context::ResetAtBeginRoot()
    {
        double_buffer_map.SwapBuffer();
        arena1.Rewind(tree_core);
        arena3.Reset();

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
        prev_inserted_box = nullptr;
    }
    void Context::ResetArena2()
    {
        arena2.Reset();
        tree_result = nullptr;
    }
    void Context::BeginRoot(BoxStyle style, DebugInfo debug_info)
    {
        //Adjusting the root style based on margin and padding.
        //This is most likely the desired outcome
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
        //ResetArena1();
        ResetAtBeginRoot();


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

            prev_inserted_box = &tree_core->box;
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

    void Context::BeginBox(const UI::BoxStyle& style, const StringAsci& id, DebugInfo debug_info)
    {

        #if UI_ENABLE_DEBUG
        #endif


        if(HasInternalError())
            return;
        element_count++;

        //============ Persistent states =============
        uint64_t id_key = 0;
        if(!id.IsEmpty())
        {
            id_key = HashBytes(id.data, id.Size());
            BoxInfo* current_info = double_buffer_map.FrontValue(id_key);
            //Handling persistent state animation variables
            if(current_info)
            {
                BoxState& s = current_info->state;
                if(current_info->IsHover())
                {
                    s.hover_anim += GetFrameTime();
                }
                else
                    s.hover_anim -= GetFrameTime();

                if(current_info->IsRendered())
                    s.appear_anim += GetFrameTime();
                else
                    s.appear_anim = 0;
                s.hover_anim = Clamp(s.hover_anim, 0.0f, 1.0f);
                s.appear_anim = Clamp(s.appear_anim, 0.0f, 1.0f);
            }
        }
        // ============================================

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

            prev_inserted_box = &child_ptr->box;
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

    //UTF 8 version
    // void Context::InsertText(const UI::TextStyle& style, const StringU8& string, const char* id, bool copy_text, DebugInfo info)
    //
    // {
    //     //Need some special decoding StringU8 to StringU32
    //     assert(0 && "have not made function yet");
    // }

    //UTF 32 version
    //This is complete for now.
    //Not proud of this, but it works temporarily
    void Context::InsertText(const UI::TextStyle& style, const StringU32& string, const char* id, bool copy_text, DebugInfo info)
    {
        #if UI_ENABLE_DEBUG
        #endif

        if(HasInternalError())
            return;
        assert(!stack.IsEmpty() && "Most likely started without root node");

        TreeNode<BoxCore>* parent_node = stack.Peek();
        assert(parent_node);

        if(GetPreviousNodeBoxType() != BoxType::TEXT) //Initialize new text node
        {
            TreeNode<BoxCore>* node = parent_node->children.Add(TreeNode<BoxCore>(), &arena1);
            assert(node && "Arena1 out of memory");
            BoxCore& box = node->box;
            box.type = BoxType::TEXT;
            box.width = 100;
            box.width_unit = Unit::AVAILABLE_PERCENT;
            prev_inserted_box = &node->box;
        }
        assert(prev_inserted_box && "Should not be null");
        const char32_t* str_data = string.data;
        if(copy_text)
        {
            str_data = arena3.NewArrayCopy(string.data, string.Size());
            assert(str_data && "string arena out of memeory");
        }
        TextSpan* span = prev_inserted_box->text_style_spans.Add(TextSpan{StringU32(str_data, string.Size()), style}, &arena1);
        assert(span && "Arena1 out of memory");
        return;
    }


    // IMPORTANT, This is the heart of computing the text layout
    inline void Context::ComputeTextLinesAndHeight(BoxCore& box)
    {
        using Iterator = TextSpans::Iterator;
        struct Int2 { int x = 0, y = 0; };
        auto AddTextLine = [&](const TextSpan& span, Int2 pos, int width)
        {
            TextLine line = {span, pos.x, pos.y, width};
            TextLine* new_line = box.result_text_lines.Add(line, &arena2);
            assert(new_line && "Arena2 out of memory");
        };
        int max_width = box.width;
        int word_width = 0;
        int span_width = 0;
        Int2 pos;
        Int2 cursor;
        Iterator start = box.text_style_spans.Begin();
        Iterator end = box.text_style_spans.Begin();
        Iterator space{}; //Marks down the last white space hit

        //passing the width of the text line
        auto WrapIfPossible = [&](int cursor_x, int width) ->bool
        {
            if(cursor_x > max_width && space.IsValid())
            {
                AddTextLine(TextSpans::GetTextSpan(start, space), pos, width);
                cursor.x = 0;
                cursor.y += start.GetStyle().GetFontSize() + start.GetStyle().GetLineSpacing();
                pos = cursor;
                end = space; //This gets incremented at the end anyway
                start = space.Next();
                space = Iterator{};
                return true;
            }
            return false;
        };

        while(end.IsValid())
        {

            int char_width = MeasureChar_impl(end.GetChar(), end.GetStyle());
            span_width = cursor.x - pos.x;
            cursor.x += char_width;
            word_width += char_width;

            if(end.GetChar() == U' ')
            {
                space = end;
                word_width = 0;
            }
            if(end.GetChar() == U'\n')
            {
                AddTextLine(TextSpans::GetTextSpan(start, end), pos, span_width);
                cursor.x = 0;
                auto next = end.Next();
                cursor.y += end.GetStyle().GetFontSize() + end.GetStyle().GetLineSpacing();
                pos = cursor;
                start = end.Next();
                space = Iterator{};
            }
            else if(start.node != end.node) //Styles are different
            {
                auto it = end.Next();
                int cursor_x = cursor.x;
                int word = word_width;
                int span = span_width;
                bool did_wrap = false;
                while(it.IsValid()) //Test if it needs to wrap
                {
                    int char_width = MeasureChar_impl(it.GetChar(), it.GetStyle());
                    span = cursor_x - pos.x;
                    cursor_x += char_width;
                    word += char_width;
                    if(WrapIfPossible(cursor_x, span - word))
                    {
                        did_wrap = true;
                        break;
                    }
                    if(it.GetChar() == U' ') //if it doest wrap by now, then we can exit
                        break;
                    it = it.Next();
                }
                if(!did_wrap)
                {
                    AddTextLine(TextSpans::GetTextSpan(start, Iterator{}), pos, span_width);
                    pos.x = cursor.x - char_width;
                    pos.y = cursor.y;
                    start = end;
                    space = Iterator{};
                }
            }
            else
            {
                WrapIfPossible(cursor.x, span_width - word_width);
            }
            end = end.Next();
        }

        if(start.IsValid())
        {
            cursor.y += start.GetStyle().GetFontSize();
            span_width = cursor.x - pos.x;
            AddTextLine(TextSpans::GetTextSpan(start, Iterator{}), pos, span_width);
        }

        box.height += cursor.y;
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
        ResetArena2();
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
                if(box.IsDetached()) //later add check for parent being content_percent
                    continue;

                if(box.IsTextElement())
                {
                    content_width += MeasureTextSpans(box);
                }
                else if(box.width_unit != Unit::Type::AVAILABLE_PERCENT &&
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

                if(box.IsTextElement())
                {
                    int width = MeasureTextSpans(box);
                    if(largest_width < width)
                        largest_width = width;
                }

                else if(box.width_unit != Unit::Type::AVAILABLE_PERCENT &&
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


    /*
        This is an important path for measuring text height and generating
        the line spans that will be saved in BoxResult
    */
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

                /* Measure text height here (old way)
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
                if(box.IsTextElement())
                {
                    this->ComputeTextLinesAndHeight(box);
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

    //scissor_aabb is the parents aabb
    void Context::DrawPass(TreeNode<BoxResult>* node, int parent_x, int parent_y, Rect scissor_aabb)
    {
        if(!node || !node->box.core)
            return;

        BoxResult& box_result = node->box;
        BoxCore& box_core = *node->box.core;

        Rect draw;
        draw.x = box_core.x + box_result.rel_x + parent_x;
        draw.y = box_core.y + box_result.rel_y + parent_y;
        draw.width = box_result.draw_width;
        draw.height = box_result.draw_height;
        bool should_render = false;

        if(Rect::Overlap(scissor_aabb, draw))
        {
            should_render = true;
            //Render current box
            if(box_core.IsTextElement())
            {
                for(auto temp = box_result.text_lines.GetHead(); temp != nullptr; temp = temp->next)
                {
                    const TextLine& line = temp->value;
                    int x = draw.x + line.x;
                    int y = draw.y + line.y;
                    DrawRectangle_impl(x, y, line.width, line.style.GetFontSize(), 0, 0, {}, line.style.GetBgColor());
                    DrawText_impl(line.style, x, y, line.data, line.Size());
                }
            }
            else if(box_core.texture.HasTexture())
            {
                DrawTexturedRectangle_impl(draw.x, draw.y, draw.width, draw.height, box_core.texture);
            }
            else
            {
                DrawRectangle_impl(draw.x, draw.y, draw.width, draw.height, box_core.corner_radius, box_core.border_width, box_core.border_color, box_core.background_color);
            }
        }


        //Input handling
        Rect new_aabb = Rect::Intersection(scissor_aabb, draw);
        if(box_core.id_key)
        {
            BoxInfo info;
            info.is_rendered = should_render;
            info.key = box_core.id_key;
            info.x = draw.x;
            info.y = draw.y;
            info.padding = box_core.padding;
            info.margin = box_core.margin;
            info.width = draw.width;
            info.height = draw.height;
            info.content_width = box_result.content_width;
            info.content_height = box_result.content_height;
            if(Rect::Contains(new_aabb, GetMouseX(), GetMouseY()))
            {
                info.is_hover = true;
                if(directly_hovered_element_key != box_core.id_key)
                    directly_hovered_element_key = box_core.id_key;
            }
            const BoxInfo* front_value = double_buffer_map.FrontValue(info.key);
            if(front_value)
                info.state = front_value->state;
            BoxInfo* box_info = double_buffer_map.Insert(info.key, info);
            assert(box_info && "DoubleBufferMap out of memory");
        }

        if(box_core.IsScissor())
            scissor_aabb = new_aabb;


        //Render children boxes
        int x = draw.x - box_core.scroll_x;
        int y = draw.y - box_core.scroll_y;

        for(auto temp = node->children.GetHead(); temp != nullptr; temp = temp->next)
        {
            assert(temp->value.box.core);
            if(temp->value.box.core->IsDetached())
            {
                AddDetachedBoxToQueue(&temp->value, draw);
                continue;
            }

            if(box_core.IsScissor())
                BeginScissorMode_impl(scissor_aabb);
            DrawPass(&temp->value, x, y, scissor_aabb);
        }
        if(box_core.IsScissor())
            EndScissorMode_impl();
    }


    DebugInspector::DebugInspector(uint64_t bytes) : arena(bytes/3), ui(bytes/3, bytes/3)
    {

    }
    void DebugInspector::Push(const BoxStyle& box)
    {

    }
    void DebugInspector::Pop()
    {

    }



}
