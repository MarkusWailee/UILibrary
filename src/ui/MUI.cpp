#include "MUI.hpp"
#include "Memory.hpp"


namespace UI
{
    struct Div
    {
        const StyleSheet* style_sheet = nullptr;

        //self
        float width = 0, height = 0;
        float min_width = 0, max_width = 0;
        float min_height = 0, max_height = 0;
        float x = 0, y = 0;


        //Flow layout
        float cursor_x = 0, cursor_y = 0;
        float gap_row = 0, gap_column = 0;

        //Grid layout
        float grid_column_height = 0;
        float grid_row_width = 0;

        //The positioned used for rendering
        float final_x = 0, final_y = 0;
    };

    template<typename T>
    struct TreeNode
    {
        T val;
        ArenaLL<TreeNode> children;
    };
}


//Helpers
namespace UI
{
    struct Error
    {
        enum class Type : unsigned char
        {
            NO_ERROR,
            INCORRENT_UNIT_TYPE,
            NODE_CONFLICT_0,
            NODE_CONFLICT_1,
            LEAF_NODE_CONFLICT,
            ROOT_NODE_CONFLICT,
            MISSING_END,
            MISSING_BEGIN
        };
        Type type = Type::NO_ERROR;
        char msg[96] = "\0";
        int div_number = 0;
    };
    void DisplayError(const Error& error);
    Error CheckUnitErrors(const StyleSheet& style);
    Error CheckLeafNodeConflicts(const StyleSheet& child, bool is_child_empty);
    Error CheckRootNodeConflicts(const StyleSheet& root);
    Error CheckNodeConflicts(const StyleSheet& child, const StyleSheet& parent);

    Error& GetGlobalError();
    bool HasGlobalError();

    //Returns false and does nothing if no error
    //Returns true, sets internal error, and displays error if true
    bool HandleGlobalError(const Error& error);

    //Used during tree descending
    float DescendFixedUnitToPx(Unit unit, float parent_pixels, float root_pixels);
    Div ComputeFixedValues(const StyleSheet& child_style, const Div& parent_div, const Div& root_div);

    //Used when ascending from the tree, as c% is based on the sum of the childrens sizes
    //Each function call adds the childs units to the total content size that should be calculated
    void ComputeAllParentContentPercentUnits(Div& parent, const StyleSheet& parent_style, const Div& child_to_add);

    float MillimeterToPixels(float mm);
    float CentimeterToPixels(float cm);
    float InchToPixels(float inches);
    float min(float a, float b);
    float max(float a, float b);
    float clamp(float minimum, float maximum, float value);
    
    void DrawPass(TreeNode<Div>* node);
    void AvailablePass(TreeNode<Div>* node);
    void PositionPass(TreeNode<Div>* node);
}


//GLOBALS
namespace UI
{
    Error internal_error;
    float dpi = 96.0f;
    MemoryArena arena(8192);
    StyleSheet default_style_sheet;
    TreeNode<Div>* root_node = nullptr;
    FixedStack<TreeNode<Div>*, 100> stack; //elements should never nest over 100 layers deep
}

namespace UI
{
    void BeginDiv(const UI::StyleSheet* div_style_sheet, const char* label, UI::DivMouseInfo* get_info)
    {
        if(HasGlobalError())
            return;
        const StyleSheet* style_sheet_ptr = div_style_sheet? div_style_sheet: &default_style_sheet;
        StyleSheet div_style = *style_sheet_ptr;

        //Check for unit type errors
        //Could be moved into creating style sheets for more performance, but this is good enough for now
        if(HandleGlobalError(CheckUnitErrors(div_style)))
            return;

        if(stack.IsEmpty())//Root Node
        {
            if(HandleGlobalError(CheckRootNodeConflicts(div_style)))
                return;

            //Creating the new div based on style sheet
            Div new_div = ComputeFixedValues(div_style, Div(), Div());

            //Make sure the style sheet is attached to new div
            new_div.style_sheet = style_sheet_ptr;

            //Initialize root_node
            root_node = arena.New<TreeNode<Div>>();
            assert(root_node && "Arena out of memory");

            //Set new_div inside root_node
            root_node->val = new_div;
            stack.Push(root_node);
        }
        else  // should add to parent
        {
            TreeNode<Div>* parent = stack.Peek();
            assert(parent);
            assert(root_node);
            Div parent_div = parent->val;    
            Div root_div = root_node->val;

            //Creating the new div based on style sheet and parent divs
            Div new_div = ComputeFixedValues(div_style, parent_div, root_div);
            new_div.style_sheet = style_sheet_ptr;

            //inserting new node
            TreeNode<Div> new_node;
            new_node.val = new_div;
            TreeNode<Div>* node_ptr = parent->children.Add(new_node, &arena);
            assert(node_ptr && "Arena out of memory");
            stack.Push(node_ptr);
        }
    }

    void EndDiv()
    {
        if(HasGlobalError())
            return;

        //Remember to catch all the node conflicts here
        stack.Pop();
    }
    void Draw()
    {
        if(HasGlobalError())
            return;

        PositionPass(root_node);
        DrawPass(root_node);

        arena.Reset(); 
        stack.Clear();
        root_node = nullptr;
    }

}


namespace UI
{
    void DrawPass(TreeNode<Div>* node)
    {
        if(!node)
            return;
        Div div = node->val;
        StyleSheet sheet = *div.style_sheet;
        RectanglePrimitive rect;
        rect.x = div.final_x;
        rect.y = div.final_y;
        rect.width = div.width;
        rect.height = div.height;
        rect.border_width = 0;
        rect.corner_radius = 0;
        rect.background_color = sheet.background_color;
        rect.border_color = sheet.border_color;
        DrawRectangle_impl(rect);
        ArenaLL<TreeNode<Div>>::Node* temp = node->children.GetHead();
        while(temp!=nullptr)
        {
            DrawPass(&temp->value);
            temp = temp->next;
        }
    }

    void AvailablePass(TreeNode<Div>* node)
    {
        if(!node)
            return;
        float available_x = 0; 
        float available_y = 0; 
        ArenaLL<TreeNode<Div>>::Node* first_child = node->children.GetHead();
    }
    //Sets the layout_x, and layout_y for all nodes
    void PositionPass(TreeNode<Div>* node)
    {
        if(!node)
            return;

        Div& parent_div = node->val;
        const StyleSheet& style = *parent_div.style_sheet; 
        ArenaLL<TreeNode<Div>>::Node* first_child = node->children.GetHead();

        if(style.layout == Layout::FLOW)
        {
            if(style.flow.axis == Flow::Axis::HORIZONTAL)
            {
                
            }
            else //Vertical
            {

            }
        }
        else //Grid
        {

        }
        
    }
}

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
            case Error::Type::NODE_CONFLICT_0:
                LogError_impl("ERROR: node conflict 0\n");
                break;
            case Error::Type::NODE_CONFLICT_1:
                LogError_impl("ERROR: node conflict 1\n");
                break;
            case Error::Type::LEAF_NODE_CONFLICT:
                LogError_impl("ERROR: leaf node confict\n");
                break;
            case Error::Type::ROOT_NODE_CONFLICT:
                LogError_impl("ERROR: root node conflict\n");
                break;
            case Error::Type::MISSING_END:
                LogError_impl("ERROR: missing EndDiv()\n");
                break;
            case Error::Type::MISSING_BEGIN:
                LogError_impl("ERROR: missing BeginDiv()\n");
                break;
            default:
                return;
        }
        LogError_impl("Div #");
        LogError_impl(error.div_number);
        LogError_impl("\n");

        LogError_impl("error.msg = '");
        LogError_impl(error.msg);
        LogError_impl("'");
    }


    #define UNIT_CONFLICT(value, unit_type, error_type)\
        if(value == unit_type) return Error{error_type, #value "=" #unit_type}

    Error CheckUnitErrors(const StyleSheet& style)
    {
        UNIT_CONFLICT(style.x.unit,                     Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.y.unit,                     Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.gap_row.unit,               Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.gap_column.unit,            Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.grid.row_width.unit,        Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.grid.column_height.unit,    Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);

        UNIT_CONFLICT(style.min_width.unit,             Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.min_height.unit,            Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.max_width.unit,             Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.max_height.unit,            Unit::Type::CONTENT_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);

        UNIT_CONFLICT(style.x.unit,                     Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.y.unit,                     Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.gap_row.unit,               Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.gap_column.unit,            Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.grid.row_width.unit,        Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.grid.column_height.unit,    Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.min_width.unit,             Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.min_height.unit,            Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.max_width.unit,             Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        UNIT_CONFLICT(style.max_height.unit,            Unit::Type::AVAILABLE_PERCENT, Error::Type::INCORRENT_UNIT_TYPE);
        return Error();
    }



    Error CheckLeafNodeConflicts(const StyleSheet& child, bool is_child_empty)
    {
        if(is_child_empty && child.width.unit == Unit::Type::CONTENT_PERCENT)
            return Error{Error::Type::LEAF_NODE_CONFLICT, "width.unit = Unit::Type::CONTENT_PERCENT with 0 children"};
        if(is_child_empty && child.height.unit == Unit::Type::CONTENT_PERCENT)
            return Error{Error::Type::LEAF_NODE_CONFLICT, "height.unit = Unit::Type::CONTENT_PERCENT with 0 children"};
        return Error();
    }

    Error CheckRootNodeConflicts(const StyleSheet& root)
    {
        UNIT_CONFLICT(root.width.unit,      Unit::Type::PARENT_PERCENT, Error::Type::ROOT_NODE_CONFLICT);
        UNIT_CONFLICT(root.height.unit,     Unit::Type::PARENT_PERCENT, Error::Type::ROOT_NODE_CONFLICT);
        UNIT_CONFLICT(root.min_width.unit,  Unit::Type::PARENT_PERCENT, Error::Type::ROOT_NODE_CONFLICT);
        UNIT_CONFLICT(root.min_height.unit, Unit::Type::PARENT_PERCENT, Error::Type::ROOT_NODE_CONFLICT);
        UNIT_CONFLICT(root.max_width.unit,  Unit::Type::PARENT_PERCENT, Error::Type::ROOT_NODE_CONFLICT);
        UNIT_CONFLICT(root.max_height.unit, Unit::Type::PARENT_PERCENT, Error::Type::ROOT_NODE_CONFLICT);
        UNIT_CONFLICT(root.x.unit,          Unit::Type::PARENT_PERCENT, Error::Type::ROOT_NODE_CONFLICT);
        UNIT_CONFLICT(root.y.unit,          Unit::Type::PARENT_PERCENT, Error::Type::ROOT_NODE_CONFLICT);

        UNIT_CONFLICT(root.width.unit,      Unit::Type::AVAILABLE_PERCENT, Error::Type::ROOT_NODE_CONFLICT);
        UNIT_CONFLICT(root.height.unit,     Unit::Type::AVAILABLE_PERCENT, Error::Type::ROOT_NODE_CONFLICT);
        UNIT_CONFLICT(root.min_width.unit,  Unit::Type::AVAILABLE_PERCENT, Error::Type::ROOT_NODE_CONFLICT);
        UNIT_CONFLICT(root.min_height.unit, Unit::Type::AVAILABLE_PERCENT, Error::Type::ROOT_NODE_CONFLICT);
        UNIT_CONFLICT(root.max_width.unit,  Unit::Type::AVAILABLE_PERCENT, Error::Type::ROOT_NODE_CONFLICT);
        UNIT_CONFLICT(root.max_height.unit, Unit::Type::AVAILABLE_PERCENT, Error::Type::ROOT_NODE_CONFLICT);
        UNIT_CONFLICT(root.x.unit,          Unit::Type::AVAILABLE_PERCENT, Error::Type::ROOT_NODE_CONFLICT);
        UNIT_CONFLICT(root.y.unit,          Unit::Type::AVAILABLE_PERCENT, Error::Type::ROOT_NODE_CONFLICT);

        UNIT_CONFLICT(root.width.unit,      Unit::Type::ROOT_PERCENT, Error::Type::ROOT_NODE_CONFLICT);
        UNIT_CONFLICT(root.height.unit,     Unit::Type::ROOT_PERCENT, Error::Type::ROOT_NODE_CONFLICT);
        UNIT_CONFLICT(root.min_width.unit,  Unit::Type::ROOT_PERCENT, Error::Type::ROOT_NODE_CONFLICT);
        UNIT_CONFLICT(root.min_height.unit, Unit::Type::ROOT_PERCENT, Error::Type::ROOT_NODE_CONFLICT);
        UNIT_CONFLICT(root.max_width.unit,  Unit::Type::ROOT_PERCENT, Error::Type::ROOT_NODE_CONFLICT);
        UNIT_CONFLICT(root.max_height.unit, Unit::Type::ROOT_PERCENT, Error::Type::ROOT_NODE_CONFLICT);
        UNIT_CONFLICT(root.x.unit,          Unit::Type::ROOT_PERCENT, Error::Type::ROOT_NODE_CONFLICT);
        UNIT_CONFLICT(root.y.unit,          Unit::Type::ROOT_PERCENT, Error::Type::ROOT_NODE_CONFLICT);
        return Error();
    }
    Error CheckNodeConflicts(const StyleSheet& child, const StyleSheet& parent)
    {
        //just checking with and height
        bool p_width = parent.width.unit == Unit::Type::CONTENT_PERCENT;
        bool p_height = parent.height.unit == Unit::Type::CONTENT_PERCENT;

        //width
        if(child.width.unit == Unit::Type::PARENT_PERCENT && p_width)
            return Error{Error::Type::NODE_CONFLICT_0, "width.unit = Unit::Type::PARENT_PERCENT && parent.width.unit = Unit::Type::CONTENT_PERCENT"};
        if(child.width.unit == Unit::Type::AVAILABLE_PERCENT && p_width) 
            return Error{Error::Type::NODE_CONFLICT_1, "width.unit = Unit::Type::AVAILABLE_PERCENT && parent.width.unit = Unit::Type::CONTENT_PERCENT"};

        //height
        if(child.height.unit == Unit::Type::PARENT_PERCENT && p_height)
            return Error{Error::Type::NODE_CONFLICT_0, "height.unit = Unit::Type::PARENT_PERCENT && parent.height.unit = Unit::Type::CONTENT_PERCENT"};
        if(child.height.unit == Unit::Type::AVAILABLE_PERCENT && p_height) 
            return Error{Error::Type::NODE_CONFLICT_1, "height.unit = Unit::Type::AVAILABLE_PERCENT && parent.height.unit = Unit::Type::CONTENT_PERCENT"};

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
    }
    inline float CentimeterToPixels(float cm)
    {
        return cm * dpi / 2.54f;
    }
    inline float InchToPixels(float inches)
    {
        return inches * dpi;
    }
    inline float min(float a, float b) {return a < b? a: b;}
    inline float max(float a, float b) {return a >= b? a: b;}
    inline float clamp(float minimum, float maximum, float value)
    {
        return max(min(value, maximum), minimum);
    }

    float DescendFixedUnitToPx(Unit unit, float parent_pixels, float root_pixels)
    {
        switch(unit.unit)
        {
            case Unit::Type::PIXEL:
                return (float)unit.value;
            case Unit::Type::MM:
                return MillimeterToPixels((float)unit.value);
            case Unit::Type::CM:
                return CentimeterToPixels((float)unit.value);
            case Unit::Type::PARENT_PERCENT:
                return (float)unit.value / 100.0f * parent_pixels;
            case Unit::Type::ROOT_PERCENT: 
                return (float)unit.value / 100.0f * root_pixels;
            default:
                return 0; //Wont happen
        }
    }
    Div ComputeFixedValues(const StyleSheet& child_style, const Div& parent_div, const Div& root_div)
    {
        Div child_div;
        //unit type 2
        child_div.width = DescendFixedUnitToPx(child_style.width, parent_div.width, root_div.width);
        child_div.height =DescendFixedUnitToPx(child_style.height, parent_div.height, root_div.height);
        child_div.x = DescendFixedUnitToPx(child_style.x, parent_div.width, root_div.width);
        child_div.y = DescendFixedUnitToPx(child_style.y, parent_div.height, root_div.height);
        child_div.max_width = DescendFixedUnitToPx(child_style.max_width, parent_div.width, root_div.width);
        child_div.max_height = DescendFixedUnitToPx(child_style.max_height, parent_div.height, root_div.height);
        child_div.min_width = DescendFixedUnitToPx(child_style.min_width, parent_div.width, root_div.width);
        child_div.min_height = DescendFixedUnitToPx(child_style.min_height, parent_div.height, root_div.height);

        //unit type 1
        child_div.gap_column = DescendFixedUnitToPx(child_style.gap_column, 0, 0);
        child_div.gap_row = DescendFixedUnitToPx(child_style.gap_row, 0, 0);
        child_div.grid_column_height = DescendFixedUnitToPx(child_style.grid.column_height, 0, 0);
        child_div.grid_row_width = DescendFixedUnitToPx(child_style.grid.row_width, 0, 0);

        //child_div.width = clamp(child_div.min_width, child_div.max_width , DescendFixedUnitToPx(child_style.width, parent_div.width, root_div.width));
        //child_div.height = clamp(child_div.min_height, child_div.max_height, DescendFixedUnitToPx(child_style.height, parent_div.height, root_div.height));
        child_div.width = clamp(child_div.min_width, child_div.max_width , child_div.width);
        child_div.height = clamp(child_div.min_height, child_div.max_height, child_div.height);
        return child_div;
    }

    void ComputeAllParentContentPercentUnits(Div& parent, const StyleSheet& parent_style, const Div& child_to_add)
    {

    }
}