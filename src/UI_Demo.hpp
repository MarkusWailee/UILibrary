#pragma once
#include <raylib.h>
#include "ui/ui.hpp"

void LayoutTest(UI::Context* context)
{
    UI::BoxStyle root;
    root.width = {GetScreenWidth()};
    root.height = {GetScreenHeight()};
    root.background_color = {50, 50, 60, 255};

    UI::BoxStyle v_container;
    v_container.width = v_container.height = {100, UI::Unit::PARENT_PERCENT};
    v_container.flow.axis = UI::Flow::VERTICAL;

    UI::BoxStyle top_bar;
    top_bar.flow.horizontal_alignment = UI::Flow::SPACE_AROUND;
    top_bar.flow.vertical_alignment = UI::Flow::CENTERED;
    top_bar.width = {100, UI::Unit::PARENT_PERCENT};
    top_bar.height = {50};
    top_bar.background_color = {30, 30, 30, 255};

    UI::BoxStyle h_container;
    h_container.width = {100, UI::Unit::PARENT_PERCENT};
    h_container.height = {100, UI::Unit::AVAILABLE_PERCENT};
    h_container.gap_column = 4;

    UI::BoxStyle bottom_bar = top_bar;
    bottom_bar.flow.horizontal_alignment = UI::Flow::SPACE_BETWEEN;

    UI::BoxStyle left_panel;
    left_panel.flow.axis = UI::Flow::VERTICAL;
    left_panel.width = {100, UI::Unit::AVAILABLE_PERCENT};
    left_panel.height = {100, UI::Unit::PARENT_PERCENT};
    left_panel.max_width = {225};
    left_panel.min_width = {125};
    left_panel.background_color = {40, 40, 40, 255};

    UI::BoxStyle mid_panel;
    mid_panel.layout = UI::Layout::GRID;
    mid_panel.grid.column_count = 4;
    mid_panel.grid.row_count = 6;
    mid_panel.width = {100, UI::Unit::AVAILABLE_PERCENT};
    mid_panel.height = {100, UI::Unit::PARENT_PERCENT};
    mid_panel.min_width = {50};
    mid_panel.background_color = {40, 40, 40, 255};

    UI::BoxStyle right_panel = left_panel;
    right_panel.layout = UI::Layout::GRID;
    right_panel.grid.column_count = 2;
    right_panel.grid.row_count = 2;
    right_panel.min_width = {100};

    UI::BoxStyle button;
    button.width = {100, UI::Unit::AVAILABLE_PERCENT};
    button.height = {100, UI::Unit::AVAILABLE_PERCENT};
    button.margin = {5,5,5,5};
    button.corner_radius = 10;
    button.background_color = {100, 200, 150, 255};

    UI::BoxStyle button2;
    button2.width = {50};
    button2.height = {30};
    button2.background_color = {150, 100, 150, 255};
    button2.corner_radius = 10;

    UI::BoxStyle pop_up;
    pop_up.width = {100};
    pop_up.height = {100};
    pop_up.background_color = {255,255,255,255};
    pop_up.corner_radius = 5;
    pop_up.detach = UI::Detach::TOP_CENTER;

    UI::Root(context, root, [&]
    {
        UI::Box().Style(v_container).Run([&]
        {
            UI::Box().Style(top_bar).Run([&]
            {
                UI::Box().Style(button2).Run([&]{});
                UI::Box().Style(button2)
                .PreRun([&]
                {
                    UI::Style().width = {50, UI::Unit::AVAILABLE_PERCENT};
                }).Run();
                UI::Box().Style(button2).Run([&]{});
            });
            UI::Box().Style(h_container).Run([&]
            {
                UI::Box().Style(left_panel).Run([&]
                {
                    for(int i = 0; i<10; i ++)
                        UI::Box().Style(button).Run([&]
                        {
                            //UI::Text(font_style, u"Hello World");
                            //UI::Text(font_style, u"Hello World");
                        });
                });
                UI::Box().Style(mid_panel).Run([&]
                {
                    for(int i = 0; i<10; i ++)
                        UI::Box(button)
                        .PreRun([&]
                        {
                            UI::Style().grid.x = i % 4;
                            UI::Style().grid.y = i / 4;
                        })
                        .OnDirectHover([&]
                        {
                            UI::Style().background_color = {200, 200, 200, 255};
                        })
                        .Run([&]{});

                    UI::Box().Style(button)
                    .PreRun([&]
                    {
                        UI::Style().grid.x = 0;
                        UI::Style().grid.y = 5;
                        UI::Style().grid.span_x = 4;
                        UI::Style().grid.span_y = 1;
                        UI::Style().background_color = {200, 100, 100, 255};
                    }).Run([&]
                    {
                        UI::Box(pop_up).Run([&]
                        {
                            UI::Box(pop_up)
                            .PreRun([&]
                            {
                                UI::Style().background_color = {200, 125, 100, 255};
                                UI::Style().height = UI::Style().width = {50};
                                UI::Style().detach = UI::Detach::RIGHT_CENTER;
                                UI::Style().x = -25;
                            })
                            .Run([&]
                            {

                            });
                        });
                    });

                    UI::Box().Style(button)
                    .PreRun([&]
                    {
                        UI::Style().grid.x = 0;
                        UI::Style().grid.y = 3;
                        UI::Style().grid.span_x = 2;
                        UI::Style().grid.span_y = 2;
                        UI::Style().background_color = {100, 100, 255, 255};
                    }).Run([&]{});

                    UI::Box().Style(button)
                    .PreRun([&]
                    {
                        UI::Style().grid.x = 2;
                        UI::Style().grid.y = 2;
                        UI::Style().grid.span_x = 2;
                        UI::Style().grid.span_y = 3;
                        UI::Style().background_color = {100, 100, 255, 255};
                    }).Run([&]{});
                });
                UI::Box().Style(right_panel).Run([&]
                {
                    for(int i = 0; i<4; i ++)
                        UI::Box().Style(button)
                        .PreRun([&]
                        {
                            UI::Style().grid.x = i % 2;
                            UI::Style().grid.y = i / 2;
                            UI::Style().background_color = {100, 150, 150, 255};
                        }).Run([&]{});
                });
            });
            UI::Box().Style(bottom_bar).Run([&]
            {
                button2.background_color = {100, 90, 210, 255};
                button2.width = {100};
                UI::Box().Style(button2).Run([&]{});

                UI::Box().Style(button2).Run([&]{});
                UI::Box().Style(button2).Run([&]{});
            });
        });
    });
}
