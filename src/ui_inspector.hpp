#pragma once
#include <raylib/raylib.h>
#include "ui/ui.hpp"



namespace UI::ToolKit
{
    Context debug_context(64000);
    void Inspector(Context* context)
    {
        if(!context)
            return;
        UI::SetContext(&debug_context);

        BoxStyle root;
        root.flow.vertical_alignment = Flow::CENTERED;
        root.width = {100, Unit::PARENT_PERCENT};
        root.height = {100, Unit::PARENT_PERCENT};
        BoxStyle inspect_panel;
        inspect_panel.width = {300};
        inspect_panel.height = {100, Unit::PARENT_PERCENT};
        inspect_panel.background_color = {60, 50, 50, 140};

        BoxStyle drag_panel;
        drag_panel.width = {10};
        drag_panel.height = {100};
        drag_panel.background_color = {59, 30, 35, 255};
        drag_panel.corner_radius = 4;

        BeginRoot(0, 0, GetScreenWidth(), GetScreenHeight(), GetMouseX(), GetMouseY());
        BeginBox(root);
            BeginBox(inspect_panel);
            EndBox();
            BeginBox(drag_panel);
            EndBox();
        EndBox();
        EndRoot();

        Draw();
        UI::SetContext(context);
    }
}