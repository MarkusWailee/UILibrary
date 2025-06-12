#include "MUI.hpp"
#include "Memory.hpp"


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
    bool HasInternalError();
    //Returns false and does nothing if no error
    //Returns true, sets internal error, and displays error if true
    bool HandleInternalError(const Error& error);

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
    Box ComputeStyleSheet(const BoxStyle& style, const Box& root);


    //UserInput
    //Returns 0 if str is nullptr. Otherwise it will never return 0
    uint64_t Hash(const char* str);



    //Text Handling

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

    /* ###### Layout Algorithm Pipeline ######
    At a high level the algorithm can be broken down into seperate passes.
    I will talk about some rules that might tell us how this pipeline should be structured.


    Focusing on computing sizing first
    - Fixed values like CM, MM, INCH, PIXEL, ROOT_PERCENT(root is always a fixed unit) can all be computed right away. 
    
    The tricky part will be special units like 
        PARENT_PERCENT      parent%
        CONTENT_PERCENT     content%
        AVAILABLE_PERCENT   available%
        WIDTH_PERCENT       width%
    
    Here are some rules before designing the algorithm
    - parent% relies on the parents size to be known
    - available% relies on the neighboring sizes to be known
    - content% relies on the children sizes to be known
    - width% relies on its own width size to be known

    Here are some potential contradictions to think about.
        While it may be possible to implement, I decided to not allow these combinations 
        to avoid confusion, and for implementation limitations
    - content% cannot have a parent% child
    - content% cannot have a available% child
    - content% cannot have zero children
    */

    //Width
    void WidthPass(TreeNode<Box>* node);
    void WidthPass_Flow(ArenaLL<TreeNode<Box>>::Node* child, const Box& parent_box); //Recurse Helper
    
    //Height
    void HeightContentPercentPass(TreeNode<Box>* node);

    void HeightPass(TreeNode<Box>* node);
    void HeightPass_Flow(ArenaLL<TreeNode<Box>>::Node* child, const Box& parent_box); //Recurse Helper

    //Computes and draw where elements should be. Also computes UserInput
    void DrawPass(TreeNode<Box>* node, int x, int y, const Box& parent_box, Rect parent_aabb);
}


//GLOBALS
namespace UI
{
    uint64_t internal_key = 0;
    Error internal_error;
    BoxStyle default_style_sheet;
    int mouse_x = 0, mouse_y = 0;
    float dpi = 96.0f;
    MemoryArena arena(32768);
    DoubleBufferMap<BoxInfo> double_buffer_map;
    ArenaLL<TreeNode<Box>*> deferred_element;
    TreeNode<Box>* root_node = nullptr;
    FixedStack<TreeNode<Box>*, 100> stack; //elements should never nest over 100 layers deep
}



//IMPLEMENTATION
namespace UI
{
    void BeginRoot(unsigned int screen_width, unsigned int screen_height, int mouse_x, int mouse_y)
    {
        UI::mouse_x = mouse_x;
        UI::mouse_y = mouse_y;
        if(HasInternalError())
            return;

        double_buffer_map.SwapBuffer();
        arena.Rewind(root_node);
        stack.Clear();
        root_node = nullptr;
        Error::node_number = 1;
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
            root_node = arena.New<TreeNode<Box>>();
            assert(root_node && "Arena out of space");

            //compute 
            root_node->val = root_box;
            stack.Push(root_node);
        }
        else
        {
            assert(0);
            HandleInternalError(Error{Error::Type::ROOT_NODE_CONTRADICTION, "There can only be 1 Root node"});
            return;
        }
    }
    void EndRoot()
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
    void BeginBox(const UI::BoxStyle& style, const char* label)
    {
        if(HasInternalError())
            return;
        Error::node_number++;

        //Check for unit type errors
        //Could be moved into creating style sheets for more performance, but this is good enough for now
        if(HandleInternalError(CheckUnitErrors(style)))
            return;

        //Input Handling
        uint64_t label_hash = Hash(label);
        if(label)
        {
            assert(double_buffer_map.Insert(label_hash, BoxInfo()));
        }

        if(!stack.IsEmpty())  // should add to parent
        {
            TreeNode<Box>* parent_node = stack.Peek();
            assert(parent_node);
            assert(root_node);

            TreeNode<Box> child_node;
            child_node.val = ComputeStyleSheet(style, root_node->val);
            child_node.val.label_hash = label_hash;
            TreeNode<Box>* child_ptr = parent_node->children.Add(child_node, &arena); 
            assert(child_ptr && "Arena out of memory");
            stack.Push(child_ptr);
        }
        else
        {
            if(HandleInternalError(Error{Error::Type::ROOT_NODE_CONTRADICTION, "Missing BeginRoot()"}))
                return;
        }
    }

    void EndBox()
    {
        if(HasInternalError())
            return;
        if(stack.Size() <= 1)
        {
            HandleInternalError(Error{Error::Type::MISSING_BEGIN, "Missing BeginBox()"});
            return;
        }
        TreeNode<Box>* node = stack.Peek();
        assert(node);
        Box& parent_box = node->val;
        if(node->children.IsEmpty())
        {
            if(HandleInternalError(CheckLeafNodeContradictions(parent_box)))
                return;
        }
        else //Calculate all CONTENT_PERCENT
        {
            ArenaLL<TreeNode<Box>>::Node* child = node->children.GetHead();  
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
                            Box& box = child->value.val;

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
                            Box& box = child->value.val;
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
            TreeNode<Box>* grand_parent = stack.Peek();
            assert(grand_parent);
            if(HandleInternalError(CheckNodeContradictions(parent_box, grand_parent->val)))
                return;
        }

    }



    void InsertText(const char* text, bool should_copy)
    {
        if(HasInternalError())
            return;
        Error::node_number++;
        if(stack.IsEmpty())
        {
            HandleInternalError(Error{Error::Type::TEXT_NODE_CONTRADICTION, "Text node needs a container"});
            return;
        }
        TreeNode<Box>* parent_node = stack.Peek();
        assert(parent_node);
        const Box& parent_box = parent_node->val;
        TreeNode<Box> text_node;
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



        text_node.val = box;
        TreeNode<Box>* addr = parent_node->children.Add(text_node, &arena);
        assert(addr);
    }



    void Draw()
    {
        if(HasInternalError())
            return;
        
        if(!stack.IsEmpty())
        {
            HandleInternalError(Error{Error::Type::ROOT_NODE_CONTRADICTION, "Missing EndRoot()"});
            return;
        }
        const Box& root_box = root_node->val;


        //Layout pipeline
        //WidthContentPercentPass happens during EndBox()

        WidthPass(root_node);
        HeightContentPercentPass(root_node);
        HeightPass(root_node);
        DrawPass(root_node, 0, 0, Box(), Rect{0, 0, root_box.width, root_box.height});

        while(!deferred_element.IsEmpty())
        {
            TreeNode<Box>* node = deferred_element.GetHead()->value;
            DrawPass(node, 0, 0, Box(), UI::Rect{0, 0, INT_MAX, INT_MAX});
            deferred_element.PopHead();
        }
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

}

//Common helpers and error checking
namespace UI
{
    void DisplayError(const Error& error)
    {
        /*
        This is the only function that uses LogError_impl.
        I might change how logging works later, but this should suffice for now.
        Ideally I would like to create my own console using the ui and log info there.
        This function will stand as an imaginary implementation of this feature.
        For now it just logs into regular console which is implemented into MUI_raylib.cpp
        */
        switch(error.type)
        {
            case Error::Type::INCORRENT_UNIT_TYPE:
                LogError_impl("ERROR: incorrect unit type\n");
                break;
            case Error::Type::NODE_CONTRADICTION:
                LogError_impl("ERROR: node contradiction\n");
                break;
            case Error::Type::LEAF_NODE_CONTRADICTION:
                LogError_impl("ERROR: leaf node contradiction\n");
                break;
            case Error::Type::ROOT_NODE_CONTRADICTION:
                LogError_impl("ERROR: root node contradiction\n");
                break;
            case Error::Type::MISSING_END:
                LogError_impl("ERROR: missing End()\n");
                break;
            case Error::Type::MISSING_BEGIN:
                LogError_impl("ERROR: missing Begin()\n");
                break;
            case Error::Type::TEXT_NODE_CONTRADICTION:
                LogError_impl("ERROR: Text node contradiction\n");
            case Error::Type::TEXT_UNKOWN_ESCAPE_CODE:
                LogError_impl("ERROR: Text unknown escape code\n");
                break;
            default:
                return;
        }
        LogError_impl("Node #");
        LogError_impl(error.node_number);
        LogError_impl("\n");

        LogError_impl("error.msg = '");
        LogError_impl(error.msg);
        LogError_impl("'");
    }


    #define UNIT_CONFLICT(value, unit_type, error_type)\
        if(value == unit_type) return Error{error_type, #value " = " #unit_type}

    Error CheckUnitErrors(const BoxStyle& style)
    {
        //The following units cannot equal the specified Unit Types 

        //Content%
        UNIT_CONFLICT(style.x.unit,                     Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.y.unit,                     Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.gap_row.unit,               Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.gap_column.unit,            Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.grid.cell_width.unit,       Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.grid.cell_height.unit,      Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);

        //UNIT_CONFLICT(style.min_width.unit,             Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        //UNIT_CONFLICT(style.min_height.unit,            Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        //UNIT_CONFLICT(style.max_width.unit,             Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        //UNIT_CONFLICT(style.max_height.unit,            Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);

        //Available%
        UNIT_CONFLICT(style.x.unit,                     Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.y.unit,                     Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.gap_row.unit,               Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.gap_column.unit,            Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.grid.cell_width.unit,       Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.grid.cell_height.unit,      Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);

        UNIT_CONFLICT(style.min_width.unit,             Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.min_height.unit,            Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.max_width.unit,             Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.max_height.unit,            Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        return Error();
    }



    Error CheckLeafNodeContradictions(const Box& leaf)
    {
        //The Following erros are contradictions
        if(leaf.width_unit == Unit::Type::CONTENT_PERCENT)
            return Error{Error::Type::LEAF_NODE_CONTRADICTION, "width.unit = Unit::Type::CONTENT_PERCENT with 0 children"};
        if(leaf.height_unit == Unit::Type::CONTENT_PERCENT)
            return Error{Error::Type::LEAF_NODE_CONTRADICTION, "height.unit = Unit::Type::CONTENT_PERCENT with 0 children"};
        return Error();
    }

    Error CheckRootNodeConflicts(const BoxStyle& root)
    {
        //Root node style sheet cannot equal any of these Unit types
        //The following errors are contradictions
        //Parent%
        UNIT_CONFLICT(root.width.unit,      Unit::Type::PARENT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.height.unit,     Unit::Type::PARENT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.min_width.unit,  Unit::Type::PARENT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.min_height.unit, Unit::Type::PARENT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.max_width.unit,  Unit::Type::PARENT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.max_height.unit, Unit::Type::PARENT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.x.unit,          Unit::Type::PARENT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.y.unit,          Unit::Type::PARENT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.gap_column.unit, Unit::Type::PARENT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.gap_row.unit,    Unit::Type::PARENT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        
        //Available%
        UNIT_CONFLICT(root.width.unit,      Unit::Type::AVAILABLE_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.height.unit,     Unit::Type::AVAILABLE_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.min_width.unit,  Unit::Type::AVAILABLE_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.min_height.unit, Unit::Type::AVAILABLE_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.max_width.unit,  Unit::Type::AVAILABLE_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.max_height.unit, Unit::Type::AVAILABLE_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.x.unit,          Unit::Type::AVAILABLE_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.y.unit,          Unit::Type::AVAILABLE_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.gap_column.unit, Unit::Type::AVAILABLE_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.gap_row.unit,    Unit::Type::AVAILABLE_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);

        //Root%
        UNIT_CONFLICT(root.width.unit,      Unit::Type::ROOT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.height.unit,     Unit::Type::ROOT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.min_width.unit,  Unit::Type::ROOT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.min_height.unit, Unit::Type::ROOT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.max_width.unit,  Unit::Type::ROOT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.max_height.unit, Unit::Type::ROOT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.x.unit,          Unit::Type::ROOT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.y.unit,          Unit::Type::ROOT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.gap_column.unit, Unit::Type::ROOT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.gap_row.unit,    Unit::Type::ROOT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);

        return Error();
    }
    Error CheckNodeContradictions(const Box& child, const Box& parent)
    {
        //The following errors are contradictions between parent and child

        //just checking with and height
        bool p_width = parent.width_unit == Unit::Type::CONTENT_PERCENT;
        bool p_height = parent.height_unit == Unit::Type::CONTENT_PERCENT;

        //HORIZONTAL
        //width
        if(child.width_unit == Unit::Type::PARENT_PERCENT && p_width)
            return Error{Error::Type::NODE_CONTRADICTION, "width.unit = Unit::Type::PARENT_PERCENT && parent.width.unit = Unit::Type::CONTENT_PERCENT"};
        if(child.width_unit == Unit::Type::AVAILABLE_PERCENT && p_width) 
            return Error{Error::Type::NODE_CONTRADICTION, "width.unit = Unit::Type::AVAILABLE_PERCENT && parent.width.unit = Unit::Type::CONTENT_PERCENT"};

        //VERTICAL
        if(child.height_unit == Unit::Type::PARENT_PERCENT && p_height)
            return Error{Error::Type::NODE_CONTRADICTION, "height.unit = Unit::Type::PARENT_PERCENT && parent.height.unit = Unit::Type::CONTENT_PERCENT"};
        if(child.height_unit == Unit::Type::AVAILABLE_PERCENT && p_height) 
            return Error{Error::Type::NODE_CONTRADICTION, "height.unit = Unit::Type::AVAILABLE_PERCENT && parent.height.unit = Unit::Type::CONTENT_PERCENT"};

        if(child.min_width_unit == Unit::Type::PARENT_PERCENT && p_width)
            return Error{Error::Type::NODE_CONTRADICTION, "min_width.unit = Unit::Type::PARENT_PERCENT && parent.width.unit = Unit::Type::CONTENT_PERCENT"};
        if(child.min_height_unit == Unit::Type::PARENT_PERCENT && p_height)
            return Error{Error::Type::NODE_CONTRADICTION, "min_height.unit = Unit::Type::PARENT_PERCENT && parent.height.unit = Unit::Type::CONTENT_PERCENT"};

        if(child.max_width_unit == Unit::Type::PARENT_PERCENT && p_width)
            return Error{Error::Type::NODE_CONTRADICTION, "max_width.unit = Unit::Type::PARENT_PERCENT && parent.width.unit = Unit::Type::CONTENT_PERCENT"};
        if(child.max_height_unit == Unit::Type::PARENT_PERCENT && p_height)
            return Error{Error::Type::NODE_CONTRADICTION, "max_height.unit = Unit::Type::PARENT_PERCENT && parent.height.unit = Unit::Type::CONTENT_PERCENT"};

        //Might not need this
        if(child.min_width_unit == Unit::Type::AVAILABLE_PERCENT && p_width) 
            return Error{Error::Type::NODE_CONTRADICTION, "min_width.unit = Unit::Type::AVAILABLE_PERCENT && parent.width.unit = Unit::Type::CONTENT_PERCENT"};
        if(child.min_height_unit == Unit::Type::AVAILABLE_PERCENT && p_height) 
            return Error{Error::Type::NODE_CONTRADICTION, "min_height.unit = Unit::Type::AVAILABLE_PERCENT && parent.height.unit = Unit::Type::CONTENT_PERCENT"};
        if(child.max_width_unit == Unit::Type::AVAILABLE_PERCENT && p_width) 
            return Error{Error::Type::NODE_CONTRADICTION, "max_width.unit = Unit::Type::AVAILABLE_PERCENT && parent.width.unit = Unit::Type::CONTENT_PERCENT"};
        if(child.max_height_unit == Unit::Type::AVAILABLE_PERCENT && p_height) 
            return Error{Error::Type::NODE_CONTRADICTION, "max_height.unit = Unit::Type::AVAILABLE_PERCENT && parent.height.unit = Unit::Type::CONTENT_PERCENT"};
        //no error
        return Error();
    }

    bool HasInternalError()
    {
        return internal_error.type != Error::Type::NO_ERROR;
    }
    bool HandleInternalError(const Error& error)
    {
        if(error.type != Error::Type::NO_ERROR)
        {
            internal_error = error;
            DisplayError(error);
            return true;
        }
        return false;
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
}



//Pass 1
//Compute unit CM, MM, INCH, ROOT_PERCENT
namespace UI
{
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
        box.gap_row =                   (uint16_t)Max(0, FixedUnitToPx(style.gap_row, root_height));
        box.gap_column =                (uint16_t)Max(0, FixedUnitToPx(style.gap_column, root_width));
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
        box.gap_row_unit =              style.gap_row.unit; //might not need
        box.gap_column_unit =           style.gap_column.unit; //might not need
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


    BoxInfo GetBoxInfo(const char* label)
    {
        uint64_t key = Hash(label);
        BoxInfo* info = double_buffer_map.FrontValue(key);
        if(info)
        {
            info->valid = true;
            if(internal_key == key)
                info->is_direct_hover = true;
            return *info;
        }
        return BoxInfo();
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

}


//Parent percent helpers
namespace UI
{
    //Width
    int ParentPercentToPx(int value , Unit::Type unit_type, int parent_width);
    void ComputeParentWidthPercent(Box& box, int parent_width);

    //Height
    int ParentHeightPercentToPx(int value , Unit::Type unit_type, int parent_width);
    void ComputeParentHeightPercent(Box& box, int parent_width);


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
        box.gap_column =                (uint16_t)Max(0, ParentPercentToPx(box.gap_column,       box.gap_column_unit,        parent_width)); 
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
    


    //NOT IN USE ANYMORE
}


//PASS 3
//Size calculations
namespace UI
{
    void WidthPass(TreeNode<Box>* node)
    {
        if(node == nullptr || node->children.IsEmpty())
            return;
        Box& box = node->val;

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

    void WidthPass_Flow(ArenaLL<TreeNode<Box>>::Node* child, const Box& parent_box)
    {
        assert(child);
        ArenaLL<TreeNode<Box>>::Node* temp;

        struct GrowBox {
            Box* box = nullptr;
            float result = 0;
        };
        ArenaLL<GrowBox> growing_elements;

        if(parent_box.GetFlowAxis() == Flow::Axis::HORIZONTAL)
        {
            float available_width = parent_box.width;
            int total_percent = 0;
            for(temp = child; temp != nullptr; temp = temp->next)
            {
                Box& box = temp->value.val;
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

            #if 0 //New available_percent solver
            bool complete =  false;
            while(!complete && available_width > 0)
            {
                complete = true;
                int total_p = 0;
                for(auto node = growing_elements.GetHead(); node != nullptr; node = node->next)
                {
                    GrowBox& var = node->value; assert(var.box);
                    Box& box = *var.box;
                    if(var.result < box.max_width)
                    {
                        complete = false;
                        total_p += box.width;
                    }
                }
                float min_p = 1;
                for(auto node = growing_elements.GetHead(); node != nullptr; node = node->next)
                {
                    GrowBox& var = node->value; assert(var.box);
                    Box& box = *var.box;
                    if(var.result < box.max_width)
                    {
                        float p = (float)(box.max_width - var.result) * total_p / (box.width * available_width);
                        min_p = Min(p, min_p);
                    }
                }

                float delta_total = 0;
                for(auto node = growing_elements.GetHead(); node != nullptr; node = node->next)
                {
                    GrowBox& var = node->value; assert(var.box);
                    Box& box = *var.box;
                    if(var.result < box.max_width)
                    {
                        float delta = min_p * available_width * box.width / total_p;
                        var.result += delta;
                        delta_total += delta;
                    }
                }
                available_width -= delta_total;
            }
            for(auto node = growing_elements.GetHead(); node != nullptr; node = node->next)
            {
                GrowBox& var = node->value; assert(var.box);
                Box& box = *var.box;
                box.width = var.result;
            }
            //Clearing arena chunk
            arena.Rewind(growing_elements.GetHead());
            growing_elements.Clear();
            #endif

            #if 1
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
                available_width = total_p < 100? available_width * total_p / 100: available_width;
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
            for(auto node = growing_elements.GetHead(); node != nullptr; node = node->next)
                node->value.box->width = node->value.result;
            arena.Rewind(growing_elements.GetHead());
            growing_elements.Clear();
            #endif

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
                Box& box = temp->value.val;
                if(box.width_unit == Unit::Type::AVAILABLE_PERCENT)
                    box.width_unit = Unit::Type::PARENT_PERCENT;
                ComputeParentWidthPercent(box, parent_box.width);
                box.width = Clamp(box.width, box.min_width, box.max_width);
                WidthPass(&temp->value);
            }
        } //End vertical
    }



    void HeightPass(TreeNode<Box>* node)
    {
        if(node == nullptr || node->children.IsEmpty())
            return;
        const Box& box = node->val;
        if(box.GetLayout() == Layout::FLOW)
        {
            HeightPass_Flow(node->children.GetHead(), box);
        }
        else
        {
            assert("have not added grid");
        }
    }
    void HeightPass_Flow(ArenaLL<TreeNode<Box>>::Node* child, const Box& parent_box)
    {
        assert(child);
        ArenaLL<TreeNode<Box>>::Node* temp;
        if(parent_box.GetFlowAxis() == Flow::Axis::HORIZONTAL) //Computed horizontal in earlier pass
        {
            for(temp = child; temp != nullptr; temp = temp->next)
            {
                Box& box = temp->value.val;
                if(box.height_unit == Unit::Type::AVAILABLE_PERCENT)
                    box.height_unit = Unit::Type::PARENT_PERCENT;
                ComputeParentHeightPercent(box, parent_box.height);
                box.height = Clamp(box.height, box.min_height, box.max_height);
                HeightPass(&temp->value);
            }
        } 
        else //Vertical
        {
            int available_height = parent_box.height;
            int total_percent = 0;
            for(temp = child; temp != nullptr; temp = temp->next)
            {
                Box& box = temp->value.val;
                ComputeParentHeightPercent(box, parent_box.height);
                if(box.height_unit != Unit::Type::AVAILABLE_PERCENT)
                {
                    box.height = Clamp(box.height, box.min_height, box.max_height);

                    //Detached boxes cannot be AVAILABLE_PERCENT
                    if(!box.IsDetached()) //Ignore layout for detached boxes
                        available_height -= box.GetBoxModelHeight() + parent_box.gap_row;
                }
                else
                {
                    available_height -= box.GetBoxExpansionHeight() + parent_box.gap_row;
                    total_percent += box.height;
                }
            }
            available_height += parent_box.gap_row;

            //Solves available_percent with min/max contraints
            bool complete = false;
            while(!complete && total_percent)
            {
                complete = true;
                int new_available_height = available_height;
                int new_total_percent = total_percent;
                for(temp = child; temp != nullptr; temp = temp->next)
                {
                    Box& box = temp->value.val;
                    if(box.height_unit != Unit::Type::AVAILABLE_PERCENT)
                        continue;
                    //Calculates what the size would be
                    int new_height = available_height * box.height / Max(100, total_percent); //Any thing below 100% will not fill in the entire space
                    //Clamps size if its not within bounds and changes unit to PIXEL
                    if(new_height < box.min_height || new_height > box.max_height)
                    {
                        new_total_percent -= box.height;
                        box.height_unit = Unit::Type::PIXEL;
                        box.height = Clamp((uint16_t)Max(0 ,new_height), box.min_height, box.max_height);
                        new_available_height -= box.height;
                        complete = false;
                    }
                }
                available_height = new_available_height;
                total_percent = new_total_percent;
            }

            //Sets all final sizes
            for(temp = child; temp != nullptr; temp = temp->next)
            {
                Box& box = temp->value.val;
                if(box.height_unit == Unit::Type::AVAILABLE_PERCENT)
                    box.height = available_height * box.height / Max(100, total_percent);  //Anything below 100% will not fill in the entire space
                HeightPass(&temp->value);
            }
        } //End vertical
    }


    void HeightContentPercentPass_Flow(TreeNode<Box>* node);
    void HeightContentPercentPass(TreeNode<Box>* node)
    {
        if(!node)    
            return;
        assert(node);
        const Box& box = node->val;
        if(box.GetLayout() == Layout::FLOW)
        {
            HeightContentPercentPass_Flow(node);
        }
        else
        {
            assert(0 && "Have not added grid yet");
        }
    }
    void HeightContentPercentPass_Flow(TreeNode<Box>* node)
    {
        assert(node);
        Box& parent_box = node->val;
        ArenaLL<TreeNode<Box>>::Node* child = node->children.GetHead();
        int content_height = 0;
        if(parent_box.GetFlowAxis() == Flow::Axis::HORIZONTAL)
        {
            int largest_height = 0;
            for(ArenaLL<TreeNode<Box>>::Node* temp = child; temp != nullptr; temp = temp->next)
            {
                HeightContentPercentPass(&temp->value);
                Box& box = temp->value.val;
                
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
            for(ArenaLL<TreeNode<Box>>::Node* temp = child; temp != nullptr; temp = temp->next)
            {
                HeightContentPercentPass(&temp->value);
                Box& box = temp->value.val;

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

}


//TEXT RENDERING
namespace UI
{
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
        #if 1
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
        #endif
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
}


//Pass 4
//Draw, position, and send user input data to next frame
namespace UI
{

    //This is a temporary function to test things
    void DrawPass_FlowNoWrap(ArenaLL<TreeNode<Box>>::Node* child, const Box& parent_box, int x, int y, Rect parent_aabb);

    void DrawPass(TreeNode<Box>* node, int x, int y, const Box& parent_box, Rect parent_aabb)
    {
        if(node == nullptr)
            return;
        const Box& box = node->val;

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

        #if UI_DEBUG
            if(Rect::Contains(Rect::Intersection(parent_aabb, Rect{x, y, render_width, render_height}), mouse_x, mouse_y))
            {
                DrawRectangle_impl(render_x, render_y, render_width, render_height, 0, 2, UI::Color{255, 0, 0, 255}, UI::Color{0, 0, 0, 0});
            }
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
                    internal_key = box.label_hash;
                    info->is_hover = true;
                }
                else if(internal_key == box.label_hash)
                {
                    internal_key = 0;
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

        if(parent_box.IsScissor())
            EndScissorMode_impl();
    }

    void DrawPass_FlowNoWrap(ArenaLL<TreeNode<Box>>::Node* child, const Box& parent_box, int x, int y, Rect parent_aabb)
    {
        ArenaLL<TreeNode<Box>>::Node* temp = child;
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
                const Box& box = temp->value.val;

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
                const Box& box = temp->value.val;

                if(box.IsDetached()) //Ignore layout for detached boxes
                {
                    deferred_element.Add(&temp->value, &arena);
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
                const Box& box = temp->value.val;

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
                const Box& box = temp->value.val;

                if(box.IsDetached()) //Ignore layout for detached boxes
                {
                    deferred_element.Add(&temp->value, &arena);
                    continue;
                }

                int cursor_x = 0;


                //Computing content_height
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
    }


}