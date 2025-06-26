#pragma once
#include <raylib.h>
#include "ui/ui.hpp"
bool PlaylistButtonWidget(const char* playlist_title, const char* author, UI::Color album_cover, const char* button_id)
{
    bool result = false;
    UI::BoxStyle button_style; 
    button_style.width = UI::Unit{100, UI::Unit::AVAILABLE_PERCENT};
    button_style.height = UI::Unit{60, UI::Unit::PIXEL};
    button_style.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
    button_style.corner_radius = 2;
    UI::BoxInfo info = UI::GetBoxInfo(button_id);
    if(info.valid)
    {
        if(info.is_direct_hover)
        {
            button_style.background_color = UI::Color{90, 90, 90, 100};
            if(IsMouseButtonDown(0))
                button_style.background_color = UI::Color{110, 110, 110, 100};
            if(IsMouseButtonReleased(0))
                result = true;
        }

    }
    UI::BoxStyle cover; 
    cover.margin = {5, 10};
    cover.background_color = album_cover;
    cover.width = UI::Unit{45, UI::Unit::PIXEL};
    cover.height = UI::Unit{45, UI::Unit::PIXEL};
    cover.corner_radius = 2;
    UI::BeginBox(button_style, button_id);
        UI::BeginBox(cover);
        UI::EndBox();
        UI::InsertText(TextFormat("[S:20]%s\n[S:18][C:666666]Playlist - %s", playlist_title, author), true);
    UI::EndBox();
    return result;
}

bool RoundedButton(const char* text, UI::Color bg, const char* id, UI::Spacing padding = {9, 9, 3, 5})
{
    UI::BoxStyle button_style; 
    button_style.padding = padding;
    button_style.width = UI::Unit{100, UI::Unit::CONTENT_PERCENT};
    button_style.height = UI::Unit{100, UI::Unit::CONTENT_PERCENT};
    button_style.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
    button_style.corner_radius = 14;
    button_style.background_color = bg;

    UI::BoxInfo info = UI::GetBoxInfo(id);
    bool result = false;
    if(info.valid)
    {
        if(info.is_direct_hover)
        {
            int amount = 10;
            if(IsMouseButtonDown(0))
                amount = 20;

            if(IsMouseButtonReleased(0))
                result = true;
            unsigned char r = bg.r + amount;
            unsigned char g = bg.g + amount;
            unsigned char b = bg.b + amount;
            button_style.background_color= UI::Color{r,g,b,255};
        }
    }
    UI::BeginBox(button_style, id); 
        UI::InsertText(text);
    UI::EndBox();
    return result;
}



int song_panel_scroll = 0;
int playlist_panel_scroll = 0;
void SpotifyExample()
{
    UI::BoxStyle h_fill;
    h_fill.width = UI::Unit{60, UI::Unit::AVAILABLE_PERCENT};

    UI::BeginRoot(0, 0,GetScreenWidth(), GetScreenHeight(), GetMouseX(), GetMouseY());
    UI::BoxStyle base_v_panel;
    base_v_panel.flow.axis = UI::Flow::Axis::VERTICAL;
    base_v_panel.width = UI::Unit{100, UI::Unit::PARENT_PERCENT};
    base_v_panel.height = UI::Unit{100, UI::Unit::PARENT_PERCENT};
    UI::BeginBox(base_v_panel);

        UI::BoxStyle top_bar;
        top_bar.background_color = UI::Color{0, 0, 0, 255};
        top_bar.height = UI::Unit{50, UI::Unit::PIXEL};
        top_bar.width = UI::Unit{100, UI::Unit::PARENT_PERCENT};
        top_bar.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
        top_bar.flow.horizontal_alignment = UI::Flow::Alignment::CENTERED;
        top_bar.gap_column = 5;
        top_bar.padding = {10, 10};
        UI::BeginBox(top_bar);
            UI::InsertText("[C:444444]Not Spotify");
            UI::BeginBox(h_fill);
            UI::EndBox();
            if(RoundedButton("[C:666666][S:18]home", UI::Color{40, 40, 40, 255}, "Home Button", {9,9, 9, 9}))
                std::cout<<"Home button\n";
            if(RoundedButton("[C:666666][S:18]What do you want to play?            ", UI::Color{40, 40, 40, 255}, "Main Seach Bar", {9,9,9,9}))
                std::cout<<"Search button\n";
            UI::BeginBox(h_fill);
            UI::EndBox();
            RoundedButton("[S:18]Options", UI::Color{20, 20, 20, 255}, "Options Button", {9,9,5,5});
            RoundedButton("[S:18]Profile", UI::Color{20, 20, 20, 255}, "Profile Button", {9,9,5,5});
            UI::BoxInfo option_info = UI::GetBoxInfo("Options Button");
            if(option_info.is_hover || UI::GetBoxInfo("Pop up").is_hover)
            {
                UI::BoxStyle pop_up;
                pop_up.flow.axis = UI::Flow::Axis::VERTICAL;
                pop_up.corner_radius = 5;
                pop_up.x = UI::Unit{(float)option_info.DrawX(), UI::Unit::PIXEL};
                pop_up.y = UI::Unit{(float)option_info.DrawY() + option_info.DrawHeight(), UI::Unit::PIXEL};
                pop_up.width = UI::Unit{100, UI::Unit::CONTENT_PERCENT};
                pop_up.height = UI::Unit{100, UI::Unit::CONTENT_PERCENT};
                pop_up.background_color = UI::Color{18, 18, 18, 255};
                pop_up.detach = true;
                UI::BeginBox(pop_up, "Pop up");
                    for(int i = 0; i<10; i++)
                    {
                        if(RoundedButton(TextFormat("[S:16]Option%d", i), UI::Color{18, 18, 18, 255}, TextFormat("Option%d", i)))
                            std::cout<<"Options "<<i + 1<<" Pressed\n";
                    }
                UI::EndBox();
            }

        UI::EndBox();

        UI::BoxStyle horizontal_box; //Holds playlist, Song list, Artist discography
        horizontal_box.width = UI::Unit{100, UI::Unit::PARENT_PERCENT};
        horizontal_box.height = UI::Unit{100, UI::Unit::AVAILABLE_PERCENT};
        horizontal_box.gap_column = 6;
        UI::BeginBox(horizontal_box);

            UI::BoxStyle playlist_panel;
            playlist_panel.flow.axis = UI::Flow::Axis::VERTICAL;
            playlist_panel.background_color = UI::Color{18, 18, 18, 255};
            playlist_panel.corner_radius = 10; //pixels
            playlist_panel.height = UI::Unit{100, UI::Unit::AVAILABLE_PERCENT};
            playlist_panel.max_width = UI::Unit{270, UI::Unit::PIXEL};
            playlist_panel.width = UI::Unit{100, UI::Unit::AVAILABLE_PERCENT};
            playlist_panel.min_width = UI::Unit{175, UI::Unit::PIXEL};

            UI::BeginBox(playlist_panel);

                UI::BoxStyle your_library_panel;
                your_library_panel.flow.vertical_alignment = UI::Flow::Alignment::SPACE_BETWEEN;
                your_library_panel.padding = {10, 10, 10, 10};
                your_library_panel.flow.axis = UI::Flow::Axis::VERTICAL;
                your_library_panel.corner_radius = 10;
                your_library_panel.background_color = UI::Color{16, 16, 16, 255};
                your_library_panel.width = UI::Unit{100, UI::Unit::PARENT_PERCENT};
                your_library_panel.height = UI::Unit{70, UI::Unit::PIXEL};

                UI::BeginBox(your_library_panel); //Your library panel
                    UI::InsertText("[S:26]Your Library");
                    UI::BoxStyle temp;
                    temp.gap_column = 5;
                    temp.width = UI::Unit{100, UI::Unit::PARENT_PERCENT};
                    temp.height = UI::Unit{100, UI::Unit::CONTENT_PERCENT};
                    UI::BeginBox(temp);
                        RoundedButton("[S:18]Playlist", UI::Color{50, 50, 50, 255}, "Your Library Playlist Button");
                        RoundedButton("[S:18]Album", UI::Color{50, 50, 50, 255}, "Your Library Album Button");
                        RoundedButton("[S:18]Artists", UI::Color{50, 50, 50, 255}, "Your Library Artists Button");
                    UI::EndBox();
                UI::EndBox();

                UI::BoxStyle playlist_scroll_box;
                playlist_scroll_box.flow.axis = UI::Flow::Axis::VERTICAL;
                playlist_scroll_box.scissor = true;
                playlist_scroll_box.width = UI::Unit{100, UI::Unit::PARENT_PERCENT};
                playlist_scroll_box.height = UI::Unit{100, UI::Unit::AVAILABLE_PERCENT};



                UI::BoxInfo playlist_panel_info = UI::GetBoxInfo("Playlist Panel");
                if(playlist_panel_info.valid)
                {
                    if(playlist_panel_info.is_hover)
                        playlist_panel_scroll -= GetMouseWheelMove() * 20;
                    playlist_panel_scroll = UI::Clamp(playlist_panel_scroll, 0, playlist_panel_info.MaxScrollY());
                }
                playlist_scroll_box.scroll_y = playlist_panel_scroll;

                UI::BeginBox(playlist_scroll_box, "Playlist Panel");
                    unsigned char red = 4;
                    unsigned char green = 24;
                    unsigned char blue = 3;
                    for(int i = 0; i<16; i++)
                    {
                        red ^= 27;
                        green ^= 7;
                        blue ^= 11;
                        red*=23;
                        green *= 57;
                        blue *= 99;
                        if(PlaylistButtonWidget(TextFormat("Playlist %d", i + 1), "Markus", UI::Color{red, green, blue, 255}, TextFormat("Playlist Button%d", i)))
                            std::cout<<"Playlist "<<i + 1<<" Pressed\n";
                    }
                UI::EndBox();
            UI::EndBox();

            UI::BoxStyle song_panel;
            song_panel.flow.axis = UI::Flow::Axis::VERTICAL;
            song_panel.background_color = UI::Color{18, 18, 18, 255};
            song_panel.corner_radius = 10;
            song_panel.width = UI::Unit{100, UI::Unit::AVAILABLE_PERCENT};
            song_panel.height = UI::Unit{100, UI::Unit::PARENT_PERCENT};
            song_panel.min_width = UI::Unit{200, UI::Unit::PIXEL};
            song_panel.scissor = true;
            UI::BoxInfo liked_song_info = UI::GetBoxInfo("Liked Songs Panel");
            if(liked_song_info.valid)
            {
                if(liked_song_info.is_hover)
                    song_panel_scroll -= GetMouseWheelMove() * 20;
                song_panel_scroll = UI::Clamp(song_panel_scroll, 0, liked_song_info.MaxScrollY());
            }
            song_panel.scroll_y = song_panel_scroll;



            UI::BeginBox(song_panel, "Liked Songs Panel");
                UI::BoxStyle liked_songs_panel; //Liked songs with cover
                liked_songs_panel.padding = {20, 20, 20, 20};
                liked_songs_panel.flow.vertical_alignment = UI::Flow::Alignment::END;
                liked_songs_panel.width = UI::Unit{100, UI::Unit::PARENT_PERCENT};
                liked_songs_panel.height = UI::Unit{100, UI::Unit::CONTENT_PERCENT};
                liked_songs_panel.background_color = UI::Color{29, 30, 50, 255};
                liked_songs_panel.gap_column = 6;
                UI::BeginBox(liked_songs_panel);
                    UI::BoxStyle liked_songs_cover;
                    liked_songs_cover.width = UI::Unit{100, UI::Unit::AVAILABLE_PERCENT};
                    liked_songs_cover.max_width = UI::Unit{230, UI::Unit::PIXEL};
                    liked_songs_cover.height = UI::Unit{100, UI::Unit::WIDTH_PERCENT}; //This should be WIDTH_PERCENT
                    liked_songs_cover.background_color = UI::Color{18, 18, 255, 255};
                    liked_songs_cover.corner_radius = 10;
                    liked_songs_cover.flow.horizontal_alignment = UI::Flow::CENTERED;
                    liked_songs_cover.flow.vertical_alignment = UI::Flow::CENTERED;
                    UI::BeginBox(liked_songs_cover);
                        UI::InsertText("L");
                    UI::EndBox();
                    UI::InsertText("[S:19]Playlist\n[S:48]Liked Songs\n[S:17]- Markus's ui example [C:555555]100 songs");
                UI::EndBox();
                red = 243;
                green = 33;
                blue = 22;
                for(int i = 0; i<16; i++)
                {
                    red ^= 27;
                    green ^= 7;
                    blue ^= 11;
                    red*=23;
                    green *= 57;
                    blue *= 99;
                    if(PlaylistButtonWidget(TextFormat("Song %d", i + 1), "Markus", UI::Color{red, green, blue, 255}, TextFormat("Song Button%d", i)))
                        std::cout<<"Song "<<i + 1<<" Pressed\n";
                }
            UI::EndBox();

            UI::BoxStyle discography_panel;
            discography_panel.padding = {10, 10, 10, 10};
            discography_panel.background_color = UI::Color{18, 18, 18, 255};
            discography_panel.corner_radius = 10;
            discography_panel.flow.axis = UI::Flow::Axis::VERTICAL;
            discography_panel.flow.horizontal_alignment = UI::Flow::Alignment::CENTERED;
            discography_panel.width = UI::Unit{100, UI::Unit::AVAILABLE_PERCENT};
            discography_panel.max_width = UI::Unit{270, UI::Unit::PIXEL};
            discography_panel.min_width = UI::Unit{200, UI::Unit::PIXEL};
            discography_panel.height = UI::Unit{100, UI::Unit::PARENT_PERCENT};
            discography_panel.gap_row = 20;
            UI::BeginBox(discography_panel);
                UI::BoxStyle discography_cover;
                discography_cover.width = UI::Unit{100, UI::Unit::PARENT_PERCENT};
                discography_cover.height = UI::Unit{230, UI::Unit::PIXEL};
                discography_cover.background_color = UI::Color{40, 40, 50, 255};
                discography_cover.corner_radius = 10;
                UI::BeginBox(discography_cover);
                UI::EndBox();
                const char* discography_text = 
                "[S:24][C:555555]Artist information...\n\n"
                "[C:FF0000]W "   // Red  
                "[C:FF7F00]E "   // Orange  
                "[C:FFFF00]L "   // Yellow  
                "[C:00FF00]C "   // Green  
                "[C:0000FF]O "   // Blue  
                "[C:4B0082]M "   // Indigo  
                "[C:8B00FF]E\n"
                "[C:666666]This library uses a simple markup for text customizations\n"
                "[S:32]Font Size 32\n"
                "[S:28]Font Size 28\n"
                "[S:24]Font Size 24\n"
                "[S:20]Font Size 20\n";
                UI::InsertText(discography_text);
            UI::EndBox();

        UI::EndBox();


        UI::BoxStyle bottom_bar;
        bottom_bar.background_color = UI::Color{0, 0, 0, 255};
        bottom_bar.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
        bottom_bar.flow.horizontal_alignment = UI::Flow::Alignment::CENTERED;
        bottom_bar.height = UI::Unit{2.05, UI::Unit::CM};
        bottom_bar.width = UI::Unit{100, UI::Unit::PARENT_PERCENT};
        UI::BeginBox(bottom_bar);
                UI::BoxStyle load_bar;
                load_bar.background_color = UI::Color{50, 50, 50, 255};
                load_bar.width = UI::Unit{100, UI::Unit::AVAILABLE_PERCENT};
                load_bar.max_width = UI::Unit{500, UI::Unit::PIXEL};
                load_bar.height = UI::Unit{8, UI::Unit::PIXEL};
                load_bar.corner_radius = 4;
            UI::BeginBox(load_bar);
                UI::BoxStyle load_progress = load_bar;
                load_progress.width = UI::Unit{30, UI::Unit::AVAILABLE_PERCENT};
                load_progress.background_color = UI::Color{255, 100, 100, 255};
                UI::BeginBox(load_progress);
                UI::EndBox();
            UI::EndBox();
            UI::InsertText("[S:16] 1:30");
        UI::EndBox();

    UI::EndBox();
    

    

    UI::EndRoot();

    UI::Draw();
}