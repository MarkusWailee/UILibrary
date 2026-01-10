#pragma once
#include <ios>
#include <raylib.h>
#include "ui/ui.hpp"
#include <cmath>

inline void TextLayoutTest(UI::Context* context)
{
    UI::BoxStyle root;
    root.flow = {.vertical_alignment = UI::Flow::CENTERED, .horizontal_alignment = UI::Flow::CENTERED};
    root.width = {GetScreenWidth()};
    root.height = {GetScreenHeight()};
    root.color = {50, 50, 60, 255};

    UI::BoxStyle box1;
    box1.width = {75, UI::Unit::PARENT_PERCENT};
    box1.height = {100, UI::Unit::CONTENT_PERCENT};
    box1.min_height = {5};
    box1.min_width = {5};
    box1.color = {100, 100, 100, 255};
    box1.padding = {4,4,4,4};
    box1.border_width = 4;
    box1.border_color = {0, 0, 0, 255};

    UI::Root(context, root, [&]
    {
        UI::Box(box1)
        .Id("SomeBox")
        .PreRun([&]
        {
            if(UI::IsHover() && UI::IsMousePressed(UI::MOUSE_LEFT))
                UI::State().custom_flags = 1;
            else
                UI::State().custom_flags = 0;
            UI::Style().color = UI::Mix(UI::Color{255, 0, 0 , 255}, UI::Color{255, 0, 0, 0}, UI::State().hover_anim);
        })
        .Run([&]
        {
            UI::TextStyle text_style;
            text_style.FontSize(64).FgColor({255, 255, 255, 255}).BgColor({20, 20, 20, 100}).LineSpacing(2);
            text_style.FontSize(24).FgColor({0, 255, 0, 255});
            UI::Text(text_style,  U"Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchang");
            UI::LineBreak();
            UI::Text(text_style, "Hello");

        });
    });
}

//just to test ui code
inline void LayoutTest(UI::Context* context)
{
    UI::BoxStyle root;
    root.flow.axis = UI::Flow::VERTICAL;
    root.width = {GetScreenWidth()};
    root.height = {GetScreenHeight()};
    root.color = {50, 50, 60, 255};
    root.gap_row = 10;

    UI::BoxStyle top_bar =
    {
        .flow =
        {
            .vertical_alignment = UI::Flow::CENTERED,
            .horizontal_alignment = UI::Flow::CENTERED,
        },
        .width = {100, UI::Unit::PARENT_PERCENT},
        .height = {65},
        .color = {25, 25, 25, 255}
    };
    UI::BoxStyle bottom_bar = top_bar;

    UI::BoxStyle middle_base =
    {
        .flow =
        {
            .axis = UI::Flow::HORIZONTAL,
        },
        .width = {100, UI::Unit::PARENT_PERCENT},
        .height = {100, UI::Unit::AVAILABLE_PERCENT},
        .gap_column = 10
    };

    UI::BoxStyle left_panel =
    {
        .flow =
        {
            .axis = UI::Flow::VERTICAL,
        },
        .width = {100, UI::Unit::AVAILABLE_PERCENT},
        .height = {100, UI::Unit::PARENT_PERCENT},
        .min_width = {100, UI::Unit::CONTENT_PERCENT},
        .max_width = {400},
        .color = {25, 25, 25, 255},
        .corner_radius = 10
    };
    UI::BoxStyle right_panel = left_panel;
    right_panel.padding = {10, 10, 10, 10};
    right_panel.min_width = {0};

    UI::BoxStyle middle_panel =
    {
        .flow =
        {
            .axis = UI::Flow::VERTICAL
        },
        .width = {100, UI::Unit::AVAILABLE_PERCENT},
        .height = {100, UI::Unit::PARENT_PERCENT},
        .min_width = {100, UI::Unit::CONTENT_PERCENT},
        .color = {25, 25, 25, 255},
        .corner_radius = 5,
    };
    UI::BoxStyle timeline =
    {
        .width = {60, UI::Unit::PARENT_PERCENT},
        .height = {12},
        .color = {5, 5, 5, 255},
        .corner_radius = 6
    };

    auto CustomButton = [&](UI::Color color, const UI::StringU32& name, const UI::StringAsci& id)
    {
        UI::BoxStyle h_container
        {
            .flow =
            {
                .vertical_alignment = UI::Flow::CENTERED,
            },
            .width = {100, UI::Unit::PARENT_PERCENT},
            .height = {100, UI::Unit::CONTENT_PERCENT},
            .padding = {5, 5, 5, 5},
            //.color = {60, 60, 60, 255},
            .gap_column = 10,
            .corner_radius = 10,
        };

        UI::BoxStyle image
        {
            .width = {50},
            .height = {50},
            .color = color,
            .corner_radius = 10

        };

        UI::Box(h_container)
        .Id(id)
        .OnDirectHover([&]
        {
            UI::Style().color = {50, 50, 50, 255};
        })
        .Run([&]
        {
            UI::Box(image)
            .Run([&]
            {

            });
            UI::Text({.font_size = 24}, name);
            UI::Text({.font_size = 21}, U"\nplaylists");
        });
    };
    auto BasicButton = [&](const UI::StringU32& str, const UI::StringAsci& id)
    {
        UI::BoxStyle basic_button
        {
            .width = {100, UI::Unit::CONTENT_PERCENT},
            .height = {100, UI::Unit::CONTENT_PERCENT},
            .padding = {10,10,9,9},
            .color = {42, 42, 42, 255},
            .corner_radius = 20
        };
        UI::Box(basic_button)
        .Id(id)
        .OnDirectHover([&]
        {
            UI::Style().color = {60, 60, 60, 255};
        })
        .Run([&]
        {
            UI::Text({.font_size = 22}, str);
        });
    };

    auto LeftPanel = [&]
    {
        UI::BoxStyle v_container =
        {
            .flow =
            {
                .axis = UI::Flow::VERTICAL,
            },
            .width = {100, UI::Unit::PARENT_PERCENT},
            .height = {100, UI::Unit::CONTENT_PERCENT},
            .min_width = {100, UI::Unit::CONTENT_PERCENT},
            .padding = {10, 10, 10, 10},
            .color = {20, 20, 20, 255},
            .gap_row = 10,
            .corner_radius = 10
        };
        UI::BoxStyle h_container =
        {
            .width = {100, UI::Unit::PARENT_PERCENT},
            .height = {100, UI::Unit::CONTENT_PERCENT},
            .min_width = {100, UI::Unit::CONTENT_PERCENT},
            .gap_column = 10
        };

        UI::BoxStyle playlist_container
        {
            .flow =
            {
                .axis = UI::Flow::VERTICAL,
            },
            .width = {100,UI::Unit::PARENT_PERCENT},
            .height = {100, UI::Unit::AVAILABLE_PERCENT},
            .scissor = true
        };

        UI::Box(v_container).Run([&]
        {
            UI::Box(h_container).Run([&]
            {
                UI::Text({.font_size = 28},U"Your Library");
            });
            UI::Box(h_container).Run([&]
            {
                BasicButton(U"Playlists", "Playlists");
                BasicButton(U"Albums", "Albums");
                BasicButton(U"Artists", "Artists");
            });
        });

        UI::Box(playlist_container)
        .Id("playlists-container")
        .OnHover([&]
        {
            float scroll_y = UI::State().custom_anim;
            scroll_y -= UI::GetMouseScroll() * 40;
            UI::State().custom_anim = UI::Clamp(scroll_y, 0.0f, (float)UI::Info().MaxScrollY());
        })
        .PreRun([&]
        {
            UI::Style().scroll_y = UI::State().custom_anim;
        })
        .Run([&]
        {
            //PLAYLIST selection goes here
            for(int i = 0; i < 10; i++)
                CustomButton({230, 200, 200, 255}, U"Name", UI::Fmt("Playlists %d", i + 1));
        });
    };

    auto MiddlePanel = [&]
    {
        UI::BoxStyle h_container
        {
            .flow =
            {
                .vertical_alignment = UI::Flow::CENTERED
            },
            .width = {100, UI::Unit::CONTENT_PERCENT},
            .height = {100, UI::Unit::CONTENT_PERCENT},
            .padding = {16, 16, 16, 16},
            .gap_column = 10,
        };
        UI::BoxStyle image
        {
            .flow =
            {
                .vertical_alignment = UI::Flow::CENTERED,
                .horizontal_alignment = UI::Flow::CENTERED,
            },
            .width = {200},
            .height = {200},
            .color = {255, 210, 210, 255},
            .corner_radius = 10,
        };

        UI::BoxStyle play_button
        {
            .width = {50},
            .height = {50},
            .color = {200, 255, 200, 255},
            .corner_radius = 24
        };
        static float liked_song_scroll = 0;
        UI::BoxStyle v_container =
        {
            .flow =
            {
                .axis = UI::Flow::VERTICAL,
            },
            .width = {100, UI::Unit::PARENT_PERCENT},
            .height = {100, UI::Unit::AVAILABLE_PERCENT},
            .scroll_y = (int)liked_song_scroll,
            .scissor = true
        };

        UI::Box(h_container).Run([&]
        {
            UI::Box(image).Run([&]
            {
                UI::Text({}, U"L");
            });
            UI::Text({}, U"Playlists\n");
            UI::Text({.font_size = 64}, U"Liked Songs\n");
            UI::Text({.font_size = 24}, U"Profile - Markus Angrignon");
        });
        UI::Box(v_container)
        .Id("liked-song-panel")
        .OnHover([&]
        {
            liked_song_scroll -= UI::GetMouseScroll() * 40;
            liked_song_scroll = UI::Clamp(liked_song_scroll, 0.0f, (float)UI::Info().MaxScrollY());
        })
        .Run([&]
        {
            for(int i = 0; i < 16; i ++)
                CustomButton({200, 200, 255, 255}, U"Song Name", UI::Fmt("Play Song id%d", i + 1));
        });

    };


    //The actual ui tree
    UI::Root(context, root, [&]
    {
        UI::Box(top_bar).Run([&]
        {
            UI::BoxStyle search_bar =
            {
                .flow = {.vertical_alignment = UI::Flow::CENTERED, .horizontal_alignment = UI::Flow::CENTERED},
                .width = {40, UI::Unit::PARENT_PERCENT},
                .height = {100, UI::Unit::PARENT_PERCENT},
                .margin = {10, 10, 12, 12},
                .color = {60, 60, 60, 255},
                .corner_radius = 16
            };
            UI::Box(search_bar)
            .Run([&]
            {
                UI::Text({.fg_color={100, 100, 100, 255}, .font_size = 32}, U"Search Bar");
            });
        });
        UI::Box(middle_base).Run([&]
        {
            UI::Box(left_panel).Run([&]
            {
                LeftPanel();
            });
            UI::Box(middle_panel).Run([&]
            {
                MiddlePanel();

            });
            UI::Box(right_panel).Run([&]
            {
                UI::StringU32 text = U"Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.";
                for(int i = 0; i < text.Size(); i++)
                {
                    float amount = (float)i / text.Size();
                    UI::Color c = {255, 255, 255, 255};
                    c.g = amount * 255;
                    c.r = 255 - amount * 255;
                    UI::Text({.fg_color = c, .font_size = 24}, text.SubStr(i, 1));
                }
            });
        });
        UI::Box(bottom_bar).Run([&]
        {
            UI::Box(timeline).Run();
            UI::Text({.font_size = 18}, U"1:30");
        });
    });

}
