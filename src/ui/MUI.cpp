#include "MUI.hpp"
#include "Memory.hpp"


namespace UI
{
    struct Box
    {
        const char* label = nullptr;
        const char* text = nullptr;
        Color background_color = UI::Color{0, 0, 0, 0}; //used for debugging
        Color border_color = UI::Color{0, 0, 0, 0};

        //type 3
        uint16_t width =            0; //Used for debugging
        uint16_t height =           0; //Used for debugging
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
        bool flow_wrap = false;
        bool scissor = false;
    public:
        void SetPositioning(Positioning p){positioning = p;}
        void SetFlowAxis(Flow::Axis axis){flow_axis = axis;}
        void SetScissor(bool flag){scissor = flag;}
        void SetFlowWrap(bool flag){flow_wrap = flag;}
        Layout GetLayout() const
        {
            return layout;
        }
        Flow::Axis GetFlowAxis() const
        {
            return flow_axis;
        }
        Positioning GetPositioning() const
        {
            return positioning;
        }
        bool IsScissor() const
        {
            return scissor;
        }
        bool IsFlowWrap() const
        {
            return flow_wrap;
        }
        int GetBoxExpansionWidth() const
        {
            return margin.left + margin.right + padding.left + padding.right;
        }
        int GetBoxExpansionHeight() const
        {
            return margin.top + margin.bottom + padding.top + padding.bottom;
        }
        int GetBoxModelWidth() const
        {
            //internal box model
            return margin.left + padding.left + width + padding.right + margin.right;
        }
        int GetBoxModelHeight() const
        {
            return margin.top + padding.top + height + padding.bottom + margin.bottom;
        }
        int GetRenderingWidth() const
        {
            return padding.left + width + padding.right;
        }
        int GetRenderingHeight() const
        {
            return padding.top + height + padding.bottom;
        }

    };

    template<typename T>
    struct TreeNode
    {
        T val;
        ArenaLL<TreeNode> children;
    };
}


//Common helpers and error handling
namespace UI
{
    struct UserInput
    {
        char label[128] = "";
        MouseInfo mouse_info;
    };
    void HandleUserInput(const char* label, int box_x, int box_y, int box_width, int box_height);
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
            TEXT_NODE_CONTRADICTION
        };
        Type type = Type::NO_ERROR;
        char msg[100] = "\0";
        inline static int node_number = 0;
    };
    void DisplayError(const Error& error);
    Error CheckUnitErrors(const StyleSheet& style);
    Error CheckLeafNodeContradictions(const Box& leaf);
    Error CheckRootNodeConflicts(const StyleSheet& root);
    Error CheckNodeContradictions(const Box& child, const Box& parent);

    Error& GetGlobalError();
    bool HasGlobalError();

    //Returns false and does nothing if no error
    //Returns true, sets internal error, and displays error if true
    bool HandleGlobalError(const Error& error);

    //Math helpers
    float MillimeterToPixels(float mm);
    float CentimeterToPixels(float cm);
    float InchToPixels(float inches);
    template<typename T>
    T min(T a, T b);
    template<typename T>
    T max(T a, T b);
    template<typename T>
    T clamp(T minimum, T maximum, T value);


    //Used during tree descending
    Box ComputeStyleSheet(const StyleSheet& style, const Box& root);
}


//TEXT RENDERING
namespace UI
{
    //Draws text based on custom markup
    void DrawTextNode(const char* text, int parent_width, int parent_height, int x, int y);
}

//UI passes
namespace UI
{
    //computes layout and draws
    void SizePass(TreeNode<Box>* node);
    void DrawPass(TreeNode<Box>* node, int x, int y);
}


//GLOBALS
namespace UI
{
    StyleSheet default_style_sheet;
    UserInput user;
    int mouse_x = 0, mouse_y = 0;
    float dpi = 96.0f;
    MemoryArena arena(32768);
    TreeNode<Box>* root_node = nullptr;
    FixedStack<TreeNode<Box>*, 100> stack; //elements should never nest over 100 layers deep
}

namespace UI
{
    void BeginRoot(unsigned int screen_width, unsigned int screen_height, int mouse_x, int mouse_y)
    {
        UI::mouse_x = mouse_x;
        UI::mouse_y = mouse_y;
        if(HasGlobalError())
            return;
        arena.Reset(); 
        stack.Clear();
        root_node = nullptr;
        Error::node_number = 1;

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
            HandleGlobalError(Error{Error::Type::ROOT_NODE_CONTRADICTION, "There can only be 1 Root node"});
            return;
        }
    }
    void EndRoot()
    {
        if(HasGlobalError())
            return;
        if(stack.Size() == 1)
        {
            stack.Pop();
        }
        else if(stack.Size() < 1)
        {
            HandleGlobalError(Error{Error::Type::ROOT_NODE_CONTRADICTION, "More than one RootEnd() function"});
            return;
        }
        else
        {
            HandleGlobalError(Error{Error::Type::MISSING_END, "Missing EndBox()"});
            return;
        }
    }
    void BeginBox(const UI::StyleSheet* style_sheet, const char* label, UI::MouseInfo* get_info)
    {
        if(HasGlobalError())
            return;
        Error::node_number++;
        StyleSheet style = style_sheet? *style_sheet: default_style_sheet;

        //Check for unit type errors
        //Could be moved into creating style sheets for more performance, but this is good enough for now
        if(HandleGlobalError(CheckUnitErrors(style)))
            return;

        //Input Handling
        if(label)
        {
            if(get_info)
            {
                *get_info = GetMouseInfo(label);
            }
        }

        if(!stack.IsEmpty())  // should add to parent
        {
            TreeNode<Box>* parent_node = stack.Peek();
            assert(parent_node);
            assert(root_node);

            TreeNode<Box> child_node;
            child_node.val = ComputeStyleSheet(style, root_node->val);
            child_node.val.label = label;
            TreeNode<Box>* child_ptr = parent_node->children.Add(child_node, &arena); 
            assert(child_ptr && "Arena out of memory");
            stack.Push(child_ptr);
        }
        else
        {
            if(HandleGlobalError(Error{Error::Type::ROOT_NODE_CONTRADICTION, "Missing BeginRoot()"}))
                return;
        }
    }

    void EndBox()
    {
        if(HasGlobalError())
            return;
        if(stack.Size() <= 1)
        {
            HandleGlobalError(Error{Error::Type::MISSING_BEGIN, "Missing BeginBox()"});
            return;
        }
        TreeNode<Box>* node = stack.Peek();
        assert(node);
        Box& node_box = node->val;
        if(node->children.IsEmpty())
        {
            if(HandleGlobalError(CheckLeafNodeContradictions(node_box)))
                return;
        }
        else //Calculate all CONTENT_PERCENT
        {
            ArenaLL<TreeNode<Box>>::Node* temp = node->children.GetHead();  
            int content_width = 0;
            int content_height = 0;
            if(node_box.GetLayout() == Layout::FLOW && (node_box.width_unit == Unit::Type::CONTENT_PERCENT || node_box.height_unit == Unit::Type::CONTENT_PERCENT))
            {
                if(node_box.GetFlowAxis() == Flow::Axis::HORIZONTAL)
                {
                    int largest_height = 0;
                    for(;temp != nullptr; temp = temp->next)
                    {
                        const Box& box = temp->value.val;
                        content_width += box.GetBoxModelWidth() + node_box.gap_column;
                        int height = box.GetBoxModelHeight();
                        if(largest_height < height)
                            largest_height = height;
                    }
                    content_height = largest_height;
                    content_width -= node_box.gap_column;
                }
                else
                {
                    int largest_width = 0;
                    for(;temp != nullptr; temp = temp->next)
                    {
                        const Box& box = temp->value.val;
                        content_height += box.GetBoxModelHeight() + node_box.gap_row;
                        int width = box.GetBoxModelWidth();
                        if(largest_width < width)
                            largest_width = width;
                    }
                    content_width = largest_width;
                    content_height -= node_box.gap_row;
                }
                if(node_box.width_unit == Unit::Type::CONTENT_PERCENT)
                    node_box.width = content_width * node_box.width / 100;
                if(node_box.height_unit == Unit::Type::CONTENT_PERCENT)
                    node_box.height = content_height * node_box.height / 100;

                if(node_box.max_width_unit == Unit::Type::CONTENT_PERCENT)
                    node_box.max_width = content_width * node_box.max_width / 100;
                if(node_box.max_height_unit == Unit::Type::CONTENT_PERCENT)
                    node_box.max_height = content_height * node_box.max_height / 100;

                if(node_box.min_width_unit == Unit::Type::CONTENT_PERCENT)
                    node_box.min_width = content_width * node_box.min_width / 100;
                if(node_box.max_height_unit == Unit::Type::CONTENT_PERCENT)
                    node_box.min_height = content_height * node_box.min_height / 100;
            }
        }
        stack.Pop();
        if(!stack.IsEmpty())
        {
            TreeNode<Box>* parent = stack.Peek();
            assert(parent);
            if(HandleGlobalError(CheckNodeContradictions(node_box, parent->val)))
                return;
        }

    }

    void InsertText(const char* text)
    {
        if(HasGlobalError())
            return;
        Error::node_number++;
        if(stack.IsEmpty())
        {
            HandleGlobalError(Error{Error::Type::TEXT_NODE_CONTRADICTION, "Text node needs a container"});
            return;
        }
        TreeNode<Box>* parent_node = stack.Peek();
        assert(parent_node);
        TreeNode<Box> text_node;
        Box box;

        //DEBUGGING TEXT
        box.width = 10;
        box.height = 10;
        box.background_color = {255, 255, 255, 255};
        box.text = text;

        text_node.val = box;
        TreeNode<Box>* addr = parent_node->children.Add(text_node, &arena);
        assert(addr);
    }



    void Draw()
    {
        if(HasGlobalError())
            return;
        
        if(!stack.IsEmpty())
        {
            HandleGlobalError(Error{Error::Type::ROOT_NODE_CONTRADICTION, "Missing EndRoot()"});
            return;
        }
        SizePass(root_node);
        DrawPass(root_node, 0, 0);

        
        //Resetting everything
    }

}




//Common helpers and error checking
namespace UI
{
    MouseInfo GetMouseInfo(const char* label)
    {
        if(label && StringCompare(label, user.label))
        {
            return user.mouse_info;
        }
        else
        {
            return MouseInfo();
        }
    }
    void HandleUserInput(const char* label, int box_x, int box_y, int box_width, int box_height)
    {
        if(!label)
            return;

        if(mouse_x >= box_x && mouse_x <= box_x + box_width
            && mouse_y >= box_y && mouse_y <= box_y + box_height)
        {
            StringCopy(user.label, label, 128);
            user.mouse_info = MouseInfo{box_x, box_y, box_width, box_height, true};
        }
        else if(StringCompare(user.label, label))
        {
            user = UserInput();
        }
    }
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

    Error CheckUnitErrors(const StyleSheet& style)
    {
        //The following units cannot equal the specified Unit Types 

        //Content%
        UNIT_CONFLICT(style.x.unit,                     Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.y.unit,                     Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.gap_row.unit,               Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.gap_column.unit,            Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.grid.cell_width.unit,       Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.grid.cell_height.unit,      Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);

        UNIT_CONFLICT(style.min_width.unit,             Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.min_height.unit,            Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.max_width.unit,             Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.max_height.unit,            Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);

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

    Error CheckRootNodeConflicts(const StyleSheet& root)
    {
        //Root node style sheet cannot equal any of these Unit types
        //The following errors are contradictions
        //Parent%
        UNIT_CONFLICT(root.width.unit,      Unit::Type::PARENT_WIDTH_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.height.unit,     Unit::Type::PARENT_WIDTH_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.min_width.unit,  Unit::Type::PARENT_WIDTH_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.min_height.unit, Unit::Type::PARENT_WIDTH_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.max_width.unit,  Unit::Type::PARENT_WIDTH_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.max_height.unit, Unit::Type::PARENT_WIDTH_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.x.unit,          Unit::Type::PARENT_WIDTH_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.y.unit,          Unit::Type::PARENT_WIDTH_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.gap_column.unit, Unit::Type::PARENT_WIDTH_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.gap_row.unit,    Unit::Type::PARENT_WIDTH_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        
        UNIT_CONFLICT(root.width.unit,      Unit::Type::PARENT_HEIGHT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.height.unit,     Unit::Type::PARENT_HEIGHT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.min_width.unit,  Unit::Type::PARENT_HEIGHT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.min_height.unit, Unit::Type::PARENT_HEIGHT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.max_width.unit,  Unit::Type::PARENT_HEIGHT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.max_height.unit, Unit::Type::PARENT_HEIGHT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.x.unit,          Unit::Type::PARENT_HEIGHT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.y.unit,          Unit::Type::PARENT_HEIGHT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.gap_column.unit, Unit::Type::PARENT_HEIGHT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.gap_row.unit,    Unit::Type::PARENT_HEIGHT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);

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
        UNIT_CONFLICT(root.width.unit,      Unit::Type::ROOT_WIDTH_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.height.unit,     Unit::Type::ROOT_WIDTH_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.min_width.unit,  Unit::Type::ROOT_WIDTH_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.min_height.unit, Unit::Type::ROOT_WIDTH_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.max_width.unit,  Unit::Type::ROOT_WIDTH_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.max_height.unit, Unit::Type::ROOT_WIDTH_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.x.unit,          Unit::Type::ROOT_WIDTH_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.y.unit,          Unit::Type::ROOT_WIDTH_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.gap_column.unit, Unit::Type::ROOT_WIDTH_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.gap_row.unit,    Unit::Type::ROOT_WIDTH_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);

        UNIT_CONFLICT(root.width.unit,      Unit::Type::ROOT_HEIGHT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.height.unit,     Unit::Type::ROOT_HEIGHT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.min_width.unit,  Unit::Type::ROOT_HEIGHT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.min_height.unit, Unit::Type::ROOT_HEIGHT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.max_width.unit,  Unit::Type::ROOT_HEIGHT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.max_height.unit, Unit::Type::ROOT_HEIGHT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.x.unit,          Unit::Type::ROOT_HEIGHT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.y.unit,          Unit::Type::ROOT_HEIGHT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.gap_column.unit, Unit::Type::ROOT_HEIGHT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
        UNIT_CONFLICT(root.gap_row.unit,    Unit::Type::ROOT_HEIGHT_PERCENT, Error::Type::ROOT_NODE_CONTRADICTION);
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
        if(child.width_unit == Unit::Type::PARENT_WIDTH_PERCENT && p_width)
            return Error{Error::Type::NODE_CONTRADICTION, "width.unit = Unit::Type::PARENT_WIDTH_PERCENT && parent.width.unit = Unit::Type::CONTENT_PERCENT"};
        if(child.height_unit == Unit::Type::PARENT_WIDTH_PERCENT && p_width)
            return Error{Error::Type::NODE_CONTRADICTION, "height.unit = Unit::Type::PARENT_WIDTH_PERCENT && parent.width.unit = Unit::Type::CONTENT_PERCENT"};
        if(child.width_unit == Unit::Type::AVAILABLE_PERCENT && p_width) 
            return Error{Error::Type::NODE_CONTRADICTION, "width.unit = Unit::Type::AVAILABLE_PERCENT && parent.width.unit = Unit::Type::CONTENT_PERCENT"};

        //VERTICAL
        //height
        if(child.height_unit == Unit::Type::PARENT_HEIGHT_PERCENT && p_height)
            return Error{Error::Type::NODE_CONTRADICTION, "height.unit = Unit::Type::PARENT_HEIGHT_PERCENT && parent.height.unit = Unit::Type::CONTENT_PERCENT"};
        if(child.width_unit == Unit::Type::PARENT_HEIGHT_PERCENT && p_height)
            return Error{Error::Type::NODE_CONTRADICTION, "width.unit = Unit::Type::PARENT_HEIGHT_PERCENT && parent.height.unit = Unit::Type::CONTENT_PERCENT"};
        if(child.height_unit == Unit::Type::AVAILABLE_PERCENT && p_height) 
            return Error{Error::Type::NODE_CONTRADICTION, "height.unit = Unit::Type::AVAILABLE_PERCENT && parent.height.unit = Unit::Type::CONTENT_PERCENT"};

        if(parent.GetLayout() == Layout::FLOW)
        {
            if(parent.IsFlowWrap())
            {
                if(parent.GetFlowAxis() == Flow::Axis::HORIZONTAL && child.width_unit == Unit::Type::AVAILABLE_PERCENT)
                    return Error{Error::Type::NODE_CONTRADICTION, "parent.flow.axis = Flow::Axis::HORIZONTAL && child.width.unit = Unit::Type::AVAILABLE_PERCENT"};
                else if(parent.GetFlowAxis() == Flow::Axis::VERTICAL && child.height_unit == Unit::Type::AVAILABLE_PERCENT)
                    return Error{Error::Type::NODE_CONTRADICTION, "parent.flow.axis = Flow::Axis::VERTICAL && child.height.unit = Unit::Type::AVAILABLE_PERCENT"};
            }
        }
        //no error
        return Error();
    }

    Error& GetGlobalError()
    {
        static Error internal_error;
        return internal_error;
    }
    bool HasGlobalError()
    {
        return GetGlobalError().type != Error::Type::NO_ERROR;
    }
    bool HandleGlobalError(const Error& error)
    {
        if(error.type != Error::Type::NO_ERROR)
        {
            GetGlobalError() = error;
            DisplayError(error);
            return true;
        }
        return false;
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
    template<typename T>
    inline T min(T a, T b) {return a < b? a: b;}
    template<typename T>
    inline T max(T a, T b) {return a >= b? a: b;}
    template<typename T>
    inline T clamp(T minimum, T maximum, T value)
    {
        return max(min(value, maximum), minimum);
    }
}



//Pass 1
//Compute unit CM, MM, INCH, ROOT_PERCENT
namespace UI
{
    int DescendFixedUnitToPx(Unit unit, int root_width, int root_height)
    {
        switch(unit.unit)
        {
            case Unit::Type::PIXEL:
                return unit.value;
            case Unit::Type::MM:
                return MillimeterToPixels((float)unit.value);
            case Unit::Type::CM:
                return CentimeterToPixels((float)unit.value);
            case Unit::Type::ROOT_HEIGHT_PERCENT: 
                return unit.value * root_height / 100;
            case Unit::Type::ROOT_WIDTH_PERCENT: 
                return unit.value * root_width / 100;
            default:
                return unit.value; //Only meant for width/height
        }
    }
    Box ComputeStyleSheet(const StyleSheet& style, const Box& root)
    {
        int root_width = root.width - style.margin.left - style.margin.right - style.padding.left - style.padding.right;
        int root_height = root.height - style.margin.top - style.margin.bottom - style.padding.top - style.padding.bottom;
        root_width = max(0, root_width);
        root_height = max(0, root_height);

        Box box;
        box.background_color =          style.background_color;
        box.border_color =              style.border_color;
        //type 3
        
        box.width =                     (uint16_t)max(0, DescendFixedUnitToPx(style.width, root_width, root_height));
        box.height =                    (uint16_t)max(0, DescendFixedUnitToPx(style.height, root_width, root_height));
        box.gap_row =                   (uint16_t)max(0, DescendFixedUnitToPx(style.gap_row, root_width, root_height));
        box.gap_column =                (uint16_t)max(0, DescendFixedUnitToPx(style.gap_column, root_width, root_height));
        box.min_width =                 (uint16_t)max(0, DescendFixedUnitToPx(style.min_width, root_width, root_height));
        box.max_width =                 (uint16_t)max(0, DescendFixedUnitToPx(style.max_width, root_width, root_height));
        box.min_height =                (uint16_t)max(0, DescendFixedUnitToPx(style.min_height, root_width, root_height));
        box.max_height =                (uint16_t)max(0, DescendFixedUnitToPx(style.max_height, root_width, root_height));
        box.x =                         (int16_t)DescendFixedUnitToPx(style.x, root_width, root_height);
        box.y =                         (int16_t)DescendFixedUnitToPx(style.y, root_width, root_height);
        box.grid_cell_width =           (uint16_t)max(0, DescendFixedUnitToPx(style.grid.cell_width, root_width, root_height));
        box.grid_cell_height =          (uint16_t)max(0, DescendFixedUnitToPx(style.grid.cell_height, root_width, root_height));

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
        box.SetFlowWrap(style.flow.wrap);
        return box;
    }

}


//Unit Helpers
//Computes AVAILABLE_PERCENT and PARENT_PERCENT 
namespace UI
{
    int DescendParentPercent(int value, Unit::Type unit_type, int parent_width, int parent_height);
    void ComputeParentPercentForBox(Box& box, int parent_width, int parent_height);
    //Traverses children, computing possible parent% and available%
    void AvailablePass_FlowHelper(ArenaLL<TreeNode<Box>>::Node* child_node, const Box& parent_box);
    int DescendParentPercent(int value, Unit::Type unit_type, int parent_width, int parent_height)
    {
        value = unit_type == Unit::Type::PARENT_WIDTH_PERCENT? value * parent_width / 100: value;
        value = unit_type == Unit::Type::PARENT_HEIGHT_PERCENT? value * parent_height / 100: value;
        return value;
    }
    void ComputeParentPercentForBox(Box& box, int parent_width, int parent_height)
    {
        parent_width -= box.padding.left + box.padding.right + box.margin.left + box.margin.right;
        parent_height -= box.padding.top + box.padding.bottom + box.margin.top + box.margin.bottom;
        parent_width = max(0, parent_width);
        parent_height = max(0, parent_height);
        box.width =                     (uint16_t)max(0, DescendParentPercent(box.width,            box.width_unit,             parent_width, parent_height)); 
        box.height =                    (uint16_t)max(0, DescendParentPercent(box.height,           box.height_unit,            parent_width, parent_height)); 
        box.gap_row =                   (uint16_t)max(0, DescendParentPercent(box.gap_row,          box.gap_row_unit,           parent_width, parent_height)); 
        box.gap_column =                (uint16_t)max(0, DescendParentPercent(box.gap_column,       box.gap_column_unit,        parent_width, parent_height)); 
        box.min_width =                 (uint16_t)max(0, DescendParentPercent(box.min_width,       box.min_width_unit,         parent_width, parent_height)); 
        box.max_width =                 (uint16_t)max(0, DescendParentPercent(box.max_width,        box.max_width_unit,         parent_width, parent_height)); 
        box.min_height =                (uint16_t)max(0, DescendParentPercent(box.min_height,       box.min_height_unit,        parent_width, parent_height)); 
        box.max_height =                (uint16_t)max(0, DescendParentPercent(box.max_height,       box.max_height_unit,        parent_width, parent_height)); 
        box.x =                         (int16_t)DescendParentPercent(box.x,                box.x_unit,                 parent_width, parent_height); 
        box.y =                         (int16_t)DescendParentPercent(box.y,                box.y_unit,                 parent_width, parent_height); 
        box.grid_cell_width =           (uint16_t)max(0, DescendParentPercent(box.grid_cell_width,  box.grid_cell_width_unit,   parent_width, parent_height)); 
        box.grid_cell_height =          (uint16_t)max(0, DescendParentPercent(box.grid_cell_height, box.grid_cell_height_unit,  parent_width, parent_height)); 
    }

}


//PASS 3
//Size calculations
namespace UI
{
    void SizePass_FlowNoWrap(ArenaLL<TreeNode<Box>>::Node* child, const Box& parent_box);
    void SizePass(TreeNode<Box>* node)
    {
        if(node == nullptr || node->children.IsEmpty())
            return;
        const Box& box = node->val;
        if(box.GetLayout() == Layout::FLOW)
        {
            if(box.IsFlowWrap())
            {
                assert("have not added wrapping");
            }
            else
            {
                SizePass_FlowNoWrap(node->children.GetHead(), box);
            }
        }
        else
        {
            assert("have not added grid");
        }
    }
    void SizePass_FlowNoWrap(ArenaLL<TreeNode<Box>>::Node* child, const Box& parent_box)
    {
        assert(child);
        ArenaLL<TreeNode<Box>>::Node* temp;
        if(parent_box.GetFlowAxis() == Flow::Axis::HORIZONTAL)
        {
            int available_width = parent_box.width;
            int total_percent = 0;
            for(temp = child; temp != nullptr; temp = temp->next)
            {
                Box& box = temp->value.val;
                if(box.height_unit == Unit::Type::AVAILABLE_PERCENT)
                    box.height_unit = Unit::Type::PARENT_HEIGHT_PERCENT;
                ComputeParentPercentForBox(box, parent_box.width, parent_box.height);
                box.height = clamp(box.min_height, box.max_height, box.height);
                if(box.width_unit != Unit::Type::AVAILABLE_PERCENT)
                {
                    box.width = clamp(box.min_width, box.max_width, box.width);
                    available_width -= box.GetBoxModelWidth() + parent_box.gap_column;
                }
                else
                {
                    available_width -= box.GetBoxExpansionWidth() + parent_box.gap_column;
                    total_percent += box.width;
                }
            }
            available_width += parent_box.gap_column;

            //Solves available_percent with min/max contraints
            bool complete = false;
            while(!complete && total_percent)
            {
                complete = true;
                int new_available_width = available_width;
                int new_total_percent = total_percent;
                for(temp = child; temp != nullptr; temp = temp->next)
                {
                    Box& box = temp->value.val;
                    if(box.width_unit != Unit::Type::AVAILABLE_PERCENT)
                        continue;
                    //Calculates what the size would be
                    int new_width = available_width * box.width / total_percent;
                    //Clamps size if its not within bounds and changes unit to PIXEL
                    if(new_width < box.min_width || new_width > box.max_width)
                    {
                        new_total_percent -= box.width;
                        box.width_unit = Unit::Type::PIXEL;
                        box.width = clamp(box.min_width, box.max_width, (uint16_t)max(0 ,new_width));
                        new_available_width -= box.width;
                        complete = false;
                    }
                }
                available_width = new_available_width;
                total_percent = new_total_percent;
            }

            //Sets all final sizes
            for(temp = child; temp != nullptr; temp = temp->next)
            {
                Box& box = temp->value.val;
                if(box.width_unit == Unit::Type::AVAILABLE_PERCENT)
                    box.width = available_width * box.width / total_percent;
                SizePass(&temp->value);
            }

        } //End Horizontal
        else //Vertical
        {
            int available_height = parent_box.height;
            int total_percent = 0;
            for(temp = child; temp != nullptr; temp = temp->next)
            {
                Box& box = temp->value.val;
                if(box.width_unit == Unit::Type::AVAILABLE_PERCENT)
                    box.width_unit = Unit::Type::PARENT_WIDTH_PERCENT;
                ComputeParentPercentForBox(box, parent_box.width, parent_box.height);
                box.width = clamp(box.min_width, box.max_width, box.width);
                if(box.height_unit != Unit::Type::AVAILABLE_PERCENT)
                {
                    box.height = clamp(box.min_height, box.max_height, box.height);
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
                    int new_height = available_height * box.height / total_percent;
                    //Clamps size if its not within bounds and changes unit to PIXEL
                    if(new_height < box.min_height || new_height > box.max_height)
                    {
                        new_total_percent -= box.height;
                        box.height_unit = Unit::Type::PIXEL;
                        box.height = clamp(box.min_height, box.max_height, (uint16_t)max(0 ,new_height));
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
                    box.height = available_height * box.height / total_percent;
                SizePass(&temp->value);
            }
        } //End vertical
    }
}


//TEXT RENDERING
namespace UI
{
}


//Pass 4
//Draw, position, and send user input data to next frame
namespace UI
{

    //This is a temporary function to test things
    void DrawPass_FlowNoWrap(ArenaLL<TreeNode<Box>>::Node* child, const Box& parent_box, int x, int y);
    void DrawPass_FlowNoWrap_TESTING(ArenaLL<TreeNode<Box>>::Node* child, const Box& parent_box, int x, int y);

    void DrawPass(TreeNode<Box>* node, int x, int y)
    {
        if(node == nullptr || node->children.IsEmpty())
            return;
        const Box& box = node->val;
        if(box.GetLayout() == Layout::FLOW)
        {
            if(box.IsFlowWrap())
            {
                assert("Have not added wrapping");
            }
            else
            {
                DrawPass_FlowNoWrap(node->children.GetHead(), box, x, y);
            }
        }
        else
        {
            assert("Grid has not been added yet");
        }
    }

    void DrawPass_FlowNoWrap(ArenaLL<TreeNode<Box>>::Node* child, const Box& parent_box, int x, int y)
    {
        ArenaLL<TreeNode<Box>>::Node* temp = child;
        assert(temp);
        //Horizontal
        if(parent_box.GetFlowAxis() == Flow::Axis::HORIZONTAL)
        {
            int count = 0;
            int content_width = 0;
            for(temp = child; temp!=nullptr; temp = temp->next)
            {
                const Box& box = temp->value.val;
                count++;
                content_width += box.GetBoxModelWidth();
            }
            content_width = count? content_width + (count-1) * parent_box.gap_column: content_width;

            int cursor_x = 0;
            int offset_x = 0;
            int available_width = parent_box.width - content_width;
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
            for(temp = child; temp!=nullptr; temp = temp->next)
            {
                const Box& box = temp->value.val;
                int cursor_y = 0;
                switch(parent_box.flow_vertical_alignment) //START, END, CENTERED, SPACE_AROUND, SPACE_BETWEEN
                {
                    case Flow::Alignment::START:
                        cursor_y = 0;
                        break;
                    case Flow::Alignment::END:
                        cursor_y = parent_box.height - box.GetBoxModelHeight();
                        break;
                    default:
                        cursor_y = parent_box.height/2 - box.GetBoxModelHeight()/2;
                        break;
                }
                int render_width =    box.GetRenderingWidth();
                int render_height =   box.GetRenderingHeight();
                int render_x =        x + cursor_x + box.margin.left + parent_box.padding.left;
                int render_y =        y + cursor_y + box.margin.top + parent_box.padding.top;
                int corner_radius =   box.corner_radius;
                int border_size =     box.border_width;
                Color border_c =        box.border_color;
                Color bg_c =            box.background_color;
                DrawRectangle_impl(render_x, render_y, render_width, render_height, corner_radius, border_size, border_c, bg_c);
                HandleUserInput(box.label, render_x, render_y, render_width, render_height);
                DrawPass(&temp->value, render_x, render_y);
                cursor_x += box.GetBoxModelWidth() + parent_box.gap_column + offset_x;
            }
        }
        else //Vertical
        {
            int count = 0;
            int content_height = 0;
            for(temp = child; temp!=nullptr; temp = temp->next)
            {
                const Box& box = temp->value.val;
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
            for(temp = child; temp!=nullptr; temp = temp->next)
            {
                const Box& box = temp->value.val;
                int cursor_x = 0;
                switch(parent_box.flow_horizontal_alignment) //START, END, CENTERED, SPACE_AROUND, SPACE_BETWEEN
                {
                    case Flow::Alignment::START:
                        cursor_x = 0;
                        break;
                    case Flow::Alignment::END:
                        cursor_x = parent_box.width - box.GetBoxModelWidth();
                        break;
                    default:
                        cursor_x = parent_box.width/2 - box.GetBoxModelWidth()/2;
                        break;
                }
                int render_width =    box.GetRenderingWidth();
                int render_height =   box.GetRenderingHeight();
                int render_x =        x + cursor_x + box.margin.left + parent_box.padding.left;
                int render_y =        y + cursor_y + box.margin.top + parent_box.padding.top;
                int corner_radius =   box.corner_radius;
                int border_size =     box.border_width;
                Color border_c =        box.border_color;
                Color bg_c =            box.background_color;
                DrawRectangle_impl(render_x, render_y, render_width, render_height, corner_radius, border_size, border_c, bg_c);
                HandleUserInput(box.label, render_x, render_y, render_width, render_height);
                DrawPass(&temp->value, render_x, render_y);
                cursor_y += box.GetBoxModelHeight() + parent_box.gap_row + offset_y;
            }

        }
    }


}