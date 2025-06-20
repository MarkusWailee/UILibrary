#pragma once
#include <raylib/raylib.h>
#include "ui/ui.hpp"
#include <iostream>



#if UI_ENABLE_DEBUG
namespace UI::ToolKit
{
    UI::Internal::MemoryArena arena(4096 * 2 * 2);
    UI::Internal::Map<bool> map;
    Context debug_context(1048576); //1MB
    void Inspector_recurse(Internal::TreeNode* node, int depth);
    void Inspector(Context* context)
    {
        if(!context)
            return;
        if(!map.Capacity())
            assert(map.AllocateCapacity(512, &arena));
        UI::SetContext(&debug_context);

        BoxStyle root;
        root.flow.vertical_alignment = Flow::CENTERED;
        root.width = {100, Unit::PARENT_PERCENT};
        root.height = {100, Unit::PARENT_PERCENT};
        BoxStyle inspect_panel;
        inspect_panel.flow.axis = Flow::VERTICAL;
        inspect_panel.width = {300};
        inspect_panel.height = {100, Unit::PARENT_PERCENT};
        inspect_panel.background_color = {60, 50, 50, 240};

        BoxStyle drag_button;
        drag_button.width = {10};
        drag_button.height = {100};
        drag_button.background_color = {59, 30, 35, 255};
        drag_button.corner_radius = 4;

        BeginRoot(0, 0, GetScreenWidth(), GetScreenHeight(), GetMouseX(), GetMouseY());
        BeginBox(root);
            BeginBox(inspect_panel);
                Inspector_recurse(context->GetInternalTree(), 0);
            EndBox();
            BeginBox(drag_button);
            EndBox();
        EndBox();
        EndRoot();

        Draw();
        UI::SetContext(context);
    }
    void Inspector_recurse(Internal::TreeNode* node, int depth)
    {
        if(!node)
            return;

        BoxStyle element_drop_down;
        element_drop_down.flow.vertical_alignment = Flow::CENTERED;
        //element_drop_down.border_width = 1;
        //element_drop_down.border_color = {70, 60, 60, 255};
        element_drop_down.flow.axis = Flow::HORIZONTAL;
        element_drop_down.width = {100, Unit::PARENT_PERCENT};
        element_drop_down.min_width = {100, Unit::CONTENT_PERCENT};
        element_drop_down.height = {100, Unit::CONTENT_PERCENT};

        BoxStyle spacer;
        spacer.width = {20};
        spacer.height = {1};

        BoxStyle line;
        line.width = {1};
        line.height = {20, Unit::PIXEL};
        line.background_color = {100, 90, 90, 255};
        const Internal::Box& box = node->box;

        uint64_t key = Hash(StringFormat("element id %d", box.debug_line));
        //std::cout<<StringFormat("element id %d", box.debug_line)<<'\n';

        bool* is_opened = map.GetValue(key);
        if(map.ShouldResize())
            assert(0 && "Map needs more memory");

        if(!is_opened)
        {
            map.Insert(key, false);
            is_opened = map.GetValue(key);
            assert(is_opened);
        }
        char c = *is_opened? '-': '+';

        const char* element_tag = box.text? "[S:20][C:666666]Text": StringFormat("[S:20]%c Box %d", c, box.debug_line);
        BoxInfo button_info = GetBoxInfo(element_tag);
        if(button_info.valid && !box.text)
        {
            if(button_info.is_direct_hover)
            {
                if(IsMouseButtonPressed(0))
                    *is_opened = *is_opened? false: true;
                element_drop_down.background_color = {80, 70, 70, 255};
                DrawRectangleLines(box.x, box.y, box.GetRenderingWidth(), box.GetRenderingHeight(), {255,255,255,255});
            }
            else
                element_drop_down.background_color = {70, 60, 60, 255};
        }

        BeginBox(element_drop_down, element_tag);
        for(int i = 0; i<=depth; i++)
        {
            BeginBox(spacer);
            EndBox();
            BeginBox(line);
            EndBox();
        }
            InsertText(element_tag, true);
        EndBox();

        if(!*is_opened)
            return;
        for(auto temp = node->children.GetHead(); temp != nullptr; temp = temp->next)
        {
            Inspector_recurse(&temp->value, depth + 1);
        }
    }
}
#endif