#pragma once
#include "ui.hpp"


namespace UX
{
    using UI::Fmt;
    static inline uint64_t key = 0;
    void ComboList(const char* id, int& select, const char** options, int count, UI::DebugInfo debug_info = UI_DEBUG("ComboList"))
    {
        UI::BoxStyle button = 
        {
            .width = {100, UI::Unit::AVAILABLE_PERCENT},
            .height = {100, UI::Unit::CONTENT_PERCENT},
            .min_width = {100, UI::Unit::CONTENT_PERCENT},
            .padding = {2,2,2,2},
            .background_color = {30, 30, 30, 255},
            .border_color = {200, 200, 200, 255},
            .corner_radius = 3,
            .border_width = 1,
        };
        UI::BoxStyle pop_up = 
        {
            .flow = 
            {
                .axis = UI::Flow::VERTICAL,
            },
            .width = {100, UI::Unit::CONTENT_PERCENT},
            .height = {100, UI::Unit::CONTENT_PERCENT},
            .background_color = {30, 30, 30, 255},
            .detach = UI::Detach::BOTTOM_END
        };

        UI::Box(id)
        .Style(button)
        .OnDirectHover([&]
        {
            if(UI::IsMousePressed(UI::MOUSE_LEFT))
                UX::key = UI::Info().GetKey();
            UI::Style().background_color = {60, 60, 60, 255};
        })
        .Run([&]
        {
            UI::InsertText(Fmt("[S:22]%s", options[select]));
            if(UX::key == UI::Info().GetKey())
            {
                UI::Box()
                .Style(pop_up)
                .Run([&]
                {
                    for(int i = 0; i < count; i++)
                    {
                        UI::Box(Fmt("ComboList-button %s %s", id, options[i]))
                        .Style(button)
                        .OnDirectHover([&]
                        {
                            if(UI::IsMousePressed(UI::MOUSE_LEFT))
                            {
                                select = i;
                                key = 0;
                            }
                            UI::Style().background_color = {60, 60, 60, 255};
                        })
                        .Run([&]
                        {
                            UI::InsertText(Fmt("[S:22]%s", options[i]));
                        });
                    }
                });
            }
        });
    }
}
