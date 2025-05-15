#include "MUI.hpp"
#include "Memory.hpp"




/* How UI is calculated
Steps
    1. Compute fixed-sizes during the BeginDiv phase (PARENT_PERCENT, MM, CM, etc)
    2. Compute the CONTENT_PERCENT during the EndDiv phase
    3. Finish creating general tree after all BeginDiv/EndDiv's
    4. Tree-Traverse 1 Compute AVAILABLE_PERCENT using tree
    5. Tree-Traverse 2 Compute Positions using tree
    6. Tree-Traverse 3 Draw?
*/


namespace UI
{
    enum class Error: unsigned char 
    {
        NO_ERROR,
        MISSING_END, //More begins than ends
        MISSING_BEGIN, //More begins than ends
        NODE_CONFLICT_0, // c%->p%->null
        NODE_CONFLICT_1, // c%->a%->null
        NODE_CONFLICT_2, // c%->null

        //Limited to cm, mm, inch, px, p%, r%
        X_INCORRECT_UNIT_TYPE,
        Y_INCORRECT_UNIT_TYPE,

        MIN_MAX_INCORRECT_UNIT_TYPE,

        //Limited to cm, mm, inch, px
        GAP_INCORRECT_UNIT_TYPE,
        GRID_INCORRECT_UNIT_TYPE,

        WRAP_CONFLICT_0, //wrapping with a%
    };
    struct Div
    {
        const StyleSheet* style_sheet = nullptr;

        //self
        float width = 0, height = 0;
        float min_width = 0, max_width = 0;
        float min_height = 0, max_height = 0;
        float x = 0, y = 0;

        //layout
        float cursor_x = 0, cursor_y = 0;
        float gap_row = 0, gap_column = 0;
    };
    template<typename T>
    struct TreeNode
    {
        ArenaLL<TreeNode> children;
        T val;
    };

    //Globals
    Error ui_error = Error::NO_ERROR;
    float dpi = 96.0f;
    MemoryArena arena(8192);
    StyleSheet default_style_sheet;
    TreeNode<Div>* root_node = nullptr;
    FixedStack<TreeNode<Div>*, 100> stack;
}





//Helper Functions
namespace UI
{
    inline void DisplayError(Error error);
    void SetError(Error error);
    Error CheckValidUnits(const StyleSheet& child, const StyleSheet& parent);

    //Checks if unit type is cm, mm, inch, px, r%, p%
    bool IsPosCompatible(Unit unit);
    //Checks if unit type is cm, mm, inch, px
    bool IsGapCompatible(Unit unit);

    float MillimeterToPixels(float mm);
    float CentimeterToPixels(float cm);
    float InchToPixels(float inches);
    float min(float a, float b);
    float max(float a, float b);
    float clamp(float minimum, float maximum, float value);

    //Used when descending down the tree
    float DescendUnitToPx(Unit unit, float parent_pixels, float root_pixels);
    float AscendUnitToPx(Unit unit, float child_pixels);



}




//All Passes
namespace UI
{
    void DrawPass(TreeNode<Div>* node);
}







namespace UI
{

    void BeginDiv(const UI::StyleSheet* div_style_sheet, UI::DivMouseInfo* get_info)
    {

        //Checking Error
        if(ui_error != Error::NO_ERROR)
            return;

        //Creating a Div
        Div div;
        div.style_sheet = div_style_sheet? div_style_sheet: &default_style_sheet;
        StyleSheet style = *div.style_sheet;

        if(stack.IsEmpty()) //Inserting First Root Div
        {
            root_node = arena.New<TreeNode<Div>>();
            assert(root_node && "Arena out of memory"); 

            //All values are expected to be fixed for root
            div.width = DescendUnitToPx(style.width, 0, 0);
            div.height = DescendUnitToPx(style.height, 0, 0);
            div.max_width = div.width;
            div.max_height = div.height;

            div.x = DescendUnitToPx(style.x, 0, 0);
            div.y = DescendUnitToPx(style.y, 0, 0);

            div.gap_column = DescendUnitToPx(style.gap_column, 0, 0);
            div.gap_row = DescendUnitToPx(style.gap_row, 0, 0);

            //Error testing
            if(!IsPosCompatible(style.x))
            {
                SetError(Error::X_INCORRECT_UNIT_TYPE);
                return;
            }
            if(!IsPosCompatible(style.y))
            {
                SetError(Error::Y_INCORRECT_UNIT_TYPE);
                return;
            }
            if( !IsPosCompatible(style.min_height) ||
                !IsPosCompatible(style.min_width) ||  
                !IsPosCompatible(style.max_width) ||  
                !IsPosCompatible(style.max_height)  )
            {
                SetError(Error::MIN_MAX_INCORRECT_UNIT_TYPE);
                return;
            }
            if( !IsGapCompatible(style.gap_column) || 
                !IsGapCompatible(style.gap_row))
            {
                SetError(Error::GAP_INCORRECT_UNIT_TYPE);
                return;
            }

            //Inserting the first Root Node Div
            root_node->val = div;
            stack.Push(root_node);
        }
        else
        {
            //Inserting child into parent
            TreeNode<Div>* parent = stack.Peek();
            assert(parent);
            assert(root_node); 

            //Compute all fixed sizes first
            Div parent_div = parent->val;
            Div root_div = root_node->val;

            div.min_width = DescendUnitToPx(style.min_width, parent_div.width, root_div.width);
            div.max_width = DescendUnitToPx(style.max_width, parent_div.width, root_div.width);

            div.min_height = DescendUnitToPx(style.min_height, parent_div.height, root_div.height);
            div.max_height = DescendUnitToPx(style.max_height, parent_div.height, root_div.height);

            div.width = clamp(div.min_width, div.max_width, DescendUnitToPx(style.width, parent_div.width, root_div.width));
            div.height = clamp(div.min_height, div.max_height, DescendUnitToPx(style.height, parent_div.height, root_div.height));

            div.x = DescendUnitToPx(style.x, parent_div.width, root_div.width);
            div.y = DescendUnitToPx(style.y, parent_div.height, parent_div.height);

            div.gap_column = DescendUnitToPx(style.gap_column, -1, -1);
            div.gap_row = DescendUnitToPx(style.gap_row, -1, -1);

            //Error testing
            if(!IsPosCompatible(style.x))
            {
                SetError(Error::X_INCORRECT_UNIT_TYPE);
                return;
            }
            if(!IsPosCompatible(style.y))
            {
                SetError(Error::Y_INCORRECT_UNIT_TYPE);
                return;
            }
            if( !IsPosCompatible(style.min_height) ||
                !IsPosCompatible(style.min_width) ||  
                !IsPosCompatible(style.max_width) ||  
                !IsPosCompatible(style.max_height)  )
            {
                SetError(Error::MIN_MAX_INCORRECT_UNIT_TYPE);
                return;
            }
            if( !IsGapCompatible(style.gap_column) || 
                !IsGapCompatible(style.gap_row))
            {
                SetError(Error::GAP_INCORRECT_UNIT_TYPE);
                return;
            }

            //Adding child to parent node
            TreeNode<Div> child_node;
            child_node.val = div;
            TreeNode<Div>* child_ptr = parent->children.Add(child_node, &arena);
            stack.Push(child_ptr);
        }
    }













    void EndDiv()
    {
        //Checking error
        if(ui_error != Error::NO_ERROR)
            return;

        //Checking if there is an extra EndDiv() funciton        
        if(stack.IsEmpty())
        {
            SetError(Error::MISSING_BEGIN);
            return;
        }

        TreeNode<Div>* child = stack.Peek();
        assert(child); //Sanity check
        stack.Pop();

        Div child_div = child->val;
        StyleSheet child_style = *child_div.style_sheet;

        //Check for Error::NODE_CONFLICT_2
        if(child->children.IsEmpty()) //checking is_child_leaf
        {
            if(child_style.width.unit == Unit::Type::CONTENT_PERCENT ||
            child_style.height.unit == Unit::Type::CONTENT_PERCENT ||
            child_style.min_width.unit == Unit::Type::CONTENT_PERCENT ||
            child_style.min_height.unit == Unit::Type::CONTENT_PERCENT ||
            child_style.max_width.unit == Unit::Type::CONTENT_PERCENT ||
            child_style.max_height.unit == Unit::Type::CONTENT_PERCENT)
            {
                SetError(Error::NODE_CONFLICT_2);
                return;
            }
        }

        //Checking if child has a parent. Only true if child == root_node
        if(!stack.IsEmpty()) 
        {
            //Compute Content % for parent width/height
            TreeNode<Div>* parent = stack.Peek();
            assert(parent); //Sanity check
            Div parent_div = parent->val;
            StyleSheet parent_style = *parent_div.style_sheet;

            //Checking Illegal operations
            Error error_check = CheckValidUnits(child_style, parent_style);
            if(error_check != Error::NO_ERROR)
            {
                SetError(error_check);
                return;
            }
            if(parent_style.layout == Layout::FLOW)
            {
                if(parent_style.flow.axis == Flow::Axis::HORIZONTAL)
                {
                }
                else
                {

                }
            }
            else if(parent_style.layout == Layout::GRID)
            {
            }
        }
    }




    void Draw()
    {
        if(ui_error != Error::NO_ERROR)
        {
            DisplayError(ui_error);
            stack.Clear();
            arena.Reset();
            root_node = nullptr;
            return;
        }
        assert(stack.IsEmpty() && "Missing EndDiv");
        DrawPass(root_node);
        //DrawRectangle_impl()
        //Remember to reset everything after drawing
        stack.Clear();
        arena.Reset();
        root_node = nullptr;
    }
}




// Different Passes
namespace UI
{
    void DrawPass(TreeNode<Div>* node)
    {
        if(!node)
            return;
        Div div = node->val;
        StyleSheet sheet = *div.style_sheet;
        RectanglePrimitive rect
        {
            .x = div.x, .y = div.y,
            .width = div.width, .height = div.height,
            .border_width = 0,
            .corner_radius = 0,
            .background_color = sheet.background_color,
            .border_color = sheet.border_color
        };
        DrawRectangle_impl(rect);
        ArenaLL<TreeNode<Div>>::Node* temp = node->children.GetHead();
        while(temp!=nullptr)
        {
            DrawPass(&temp->value);
            temp = temp->next;
        }
    }
}




//Helper Functions
namespace UI
{
    inline void DisplayError(Error error)
    {
        TextPrimitive t;
        t.color = Color{255, 100, 100, 255};
        t.font_size = 20;
        t.x = 5;
        t.y = 5;
        switch(error)
        {
            case Error::NO_ERROR:
                DrawText_impl("NO ERROR", t);
                return;
            case Error::MISSING_END: //More begins than ends
                DrawText_impl("ERROR    Missing EndDiv", t);
                return;
            case Error::MISSING_BEGIN: //More begins than ends
                DrawText_impl("ERROR    Missing BeginDiv", t);
                return;
            case Error::NODE_CONFLICT_0: // c%->p%->null
                DrawText_impl("ERROR    NODE_CONFLICT_0 C% with P%", t);
                return;
            case Error::NODE_CONFLICT_1: // c%->a%->null
                DrawText_impl("ERROR    NODE_CONFLICT_1 C% with A%", t);
                return;
            case Error::NODE_CONFLICT_2: // c%->null
                DrawText_impl("ERROR    NODE_CONFLICT_2 leaf C%", t);
                return;
                 //Limited to cm, mm, inch, px, p%, r%
            case Error::X_INCORRECT_UNIT_TYPE:
                DrawText_impl("ERROR    Incorrect unit x", t);
                return;
            case Error::Y_INCORRECT_UNIT_TYPE:
                DrawText_impl("ERROR    Incorrect unit y", t);
                return;
            case Error::MIN_MAX_INCORRECT_UNIT_TYPE:
                DrawText_impl("ERROR    Incorrect unit min or max", t);
                return;
                 //Limited to cm, mm, inch, px
            case Error::GAP_INCORRECT_UNIT_TYPE:
                DrawText_impl("ERROR    Incorrect unit gap", t);
                return;
            case Error::GRID_INCORRECT_UNIT_TYPE:
                DrawText_impl("ERROR    Incorrect unit grid measurement", t);
                return;
            case Error::WRAP_CONFLICT_0: //wrapping with a%
                DrawText_impl("ERROR    wrap with A% conflict", t);
                return;
            default:
                DrawText_impl("ERROR", t);
                return;
        }
    }
    inline Error CheckValidUnits(const StyleSheet& child, const StyleSheet& parent)
    {
        //NODE_CONFLICT_0, // c%->p%->null
        //NODE_CONFLICT_1, // c%->a%->null
        //NODE_CONFLICT_2, // c%->null
        bool is_width_content = parent.width.unit == Unit::Type::CONTENT_PERCENT;
        bool is_height_content = parent.height.unit == Unit::Type::CONTENT_PERCENT;
        bool is_wrap = parent.flow.wrap;
        bool c1 = false; 
        c1 = c1 || child.width.unit == Unit::Type::PARENT_PERCENT;
        c1 = c1 || child.min_width.unit == Unit::Type::PARENT_PERCENT;
        c1 = c1 || child.max_width.unit == Unit::Type::PARENT_PERCENT;
        bool c2 = false;
        c2 = c2 || child.height.unit == Unit::Type::PARENT_PERCENT;
        c2 = c2 || child.min_height.unit == Unit::Type::PARENT_PERCENT;
        c2 = c2 || child.max_height.unit == Unit::Type::PARENT_PERCENT;
        if((c1 && is_width_content) || (c2 && is_height_content))
        {
            return Error::NODE_CONFLICT_0;
        }

        c1 = false;
        c1 = c1 || child.width.unit == Unit::Type::AVAILABLE_PERCENT;
        c1 = c1 || child.min_width.unit == Unit::Type::AVAILABLE_PERCENT;
        c1 = c1 || child.max_width.unit == Unit::Type::AVAILABLE_PERCENT;
        c2 = false;
        c2 = c2 || child.height.unit == Unit::Type::AVAILABLE_PERCENT;
        c2 = c2 || child.min_height.unit == Unit::Type::AVAILABLE_PERCENT;
        c2 = c2 || child.max_height.unit == Unit::Type::AVAILABLE_PERCENT;
        if((c1 && is_width_content) || (c2 && is_height_content))
        {
            return Error::NODE_CONFLICT_1;
        }
        if(is_wrap && (c1 || c2))
            return Error::WRAP_CONFLICT_0;

        return Error::NO_ERROR;
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

    //Used when descending down the tree
    inline float DescendUnitToPx(Unit unit, float parent_pixels, float root_pixels)
    {
        switch(unit.unit)
        {
            case Unit::Type::PIXEL:
                return (float)unit.value;
            case Unit::Type::MM:
                return MillimeterToPixels((float)unit.value);
            case Unit::Type::CM:
                return CentimeterToPixels((float)unit.value);
            case Unit::Type::CONTENT_PERCENT: //Calculated during Ends
                return 0; //Indicating CONTENT_PERCENT
            case Unit::Type::PARENT_PERCENT:
                return (float)unit.value / 100.0f * parent_pixels;
            case Unit::Type::ROOT_PERCENT: 
                return (float)unit.value / 100.0f * root_pixels;
            case Unit::Type::AVAILABLE_PERCENT: //Calculated after all DivEnds in seperate pass.
                return 0; //Indicating AVAILABLE_PERCENT
            default:
                return 0; //Wont happen
        }
    }
    inline float AscendUnitToPx(Unit unit, float child_pixels)
    {
        switch(unit.unit)
        {
            case Unit::Type::CONTENT_PERCENT:
                return child_pixels;
        }
    }
    inline void SetError(Error error)
    {
        ui_error = error;
    }

    //Checks is the unit is compatible with a spacing type
    inline bool IsGapCompatible(Unit unit)
    {
        switch(unit.unit)
        {
            case Unit::Type::PIXEL:
                return true;
            case Unit::Type::MM:
                return true;
            case Unit::Type::CM:
                return true;
            default:
                return false;

        }
    }

    //Checks is the unit is compatible with a postional type
    inline bool IsPosCompatible(Unit unit)
    {
        switch(unit.unit)
        {
            case Unit::Type::PIXEL:
                return true;
            case Unit::Type::MM:
                return true;
            case Unit::Type::CM:
                return true;
            case Unit::Type::PARENT_PERCENT:
                return true;
            case Unit::Type::ROOT_PERCENT: 
                return true;
            default:
                return false;

        }
    }
}