#include <raylib/raylib.h>
#include <raylib/rlgl.h>
#include "MUI.hpp"
#include <unordered_map>
//Raylib backend

namespace UI
{
    //std::unordered_map<std::string, Font> fonts;
    Font font;
    GlyphInfo font_info[128]{};
    void Init_impl()
    {
        font = LoadFontEx("assets/fonts/SpaceMono-Regular.ttf", 32, 0, 0);
        if(IsFontValid(font))
        {
            for(int i = 32; i<=126; i++) //Printable asci characters
            font_info[i] = GetGlyphInfo(font, i);
        }
    }
}

#include <iostream>
//Drawing functions
namespace UI
{
    void DrawRectangle_impl(float x, float y, float width, float height, float corner_radius, float border_size, Color brdr, Color bg)
    {
        #if 0 //Testing performance
        {
            DrawRectangle(x, y, width, height, {bg.r, bg.g, bg.b, bg.a});
            return;
        }
        #endif

        int far_offset = corner_radius * 2 + border_size * 2;
        int near_offset = corner_radius + border_size;
        int outer_radius = corner_radius + border_size;
        int segments = 4;
        if(corner_radius == 255)
        {
            DrawCircle(x, y, corner_radius, {bg.r, bg.g, bg.b, bg.a});
            DrawRing({x, y}, corner_radius, outer_radius, 0, 360, 32, {brdr.r, brdr.g, brdr.b, brdr.a});
        }
        else
        {
            //inner 
            DrawRing({x + near_offset, y + near_offset}, 0, corner_radius, 180, 270,                segments, {bg.r, bg.g, bg.b, bg.a}); //top left
            DrawRing({x + width - near_offset, y + near_offset}, 0, corner_radius, 270, 360,        segments, {bg.r, bg.g, bg.b, bg.a}); //top right
            DrawRing({x + width - near_offset, y + height - near_offset}, 0, corner_radius, 0, 90,  segments, {bg.r, bg.g, bg.b, bg.a}); //bottom right
            DrawRing({x + near_offset, y + height - near_offset}, 0, corner_radius, 90, 180,        segments, {bg.r, bg.g, bg.b, bg.a}); //bottom left
            DrawRectangle(x + border_size, y + near_offset, corner_radius, height - far_offset, {bg.r, bg.g, bg.b, bg.a}); //left
            DrawRectangle(x + near_offset, y + border_size, width - far_offset, corner_radius, {bg.r, bg.g, bg.b, bg.a}); //top
            DrawRectangle(x + width - near_offset, y + corner_radius + border_size, corner_radius, height - far_offset, {bg.r, bg.g, bg.b, bg.a}); //right
            DrawRectangle(x + near_offset, y + height - corner_radius - border_size, width - far_offset, corner_radius, {bg.r, bg.g, bg.b, bg.a}); //bottom
            DrawRectangle(x + near_offset, y + near_offset, width-far_offset, height-far_offset, {bg.r, bg.g, bg.b, bg.a});

            //Outer
            DrawRing({x + near_offset, y + near_offset}, corner_radius, outer_radius, 180, 270,                 segments, {brdr.r, brdr.g, brdr.b, brdr.a}); //top left
            DrawRing({x + width - near_offset, y + near_offset}, corner_radius, outer_radius, 270, 360,         segments, {brdr.r, brdr.g, brdr.b, brdr.a}); //top right
            DrawRing({x + width - near_offset, y + height - near_offset}, corner_radius, outer_radius, 0, 90,   segments, {brdr.r, brdr.g, brdr.b, brdr.a}); //bottom right
            DrawRing({x + near_offset, y + height - near_offset}, corner_radius, outer_radius, 90, 180,         segments, {brdr.r, brdr.g, brdr.b, brdr.a}); //bottom left
            DrawRectangle(x , y + near_offset, border_size, height - far_offset, {brdr.r, brdr.g, brdr.b, brdr.a}); //left
            DrawRectangle(x + near_offset, y, width - far_offset, border_size, {brdr.r, brdr.g, brdr.b, brdr.a}); //top
            DrawRectangle(x + width - border_size, y + near_offset, border_size, height - far_offset, {brdr.r, brdr.g, brdr.b, brdr.a}); //right
            DrawRectangle(x + near_offset, y + height - border_size, width - far_offset, border_size, {brdr.r, brdr.g, brdr.b, brdr.a}); //bottom
        }
    }
    void SetFont_impl(const char* file_path)
    {
        
    }

    //This is not an optimal implementation for performance, but this works for testing
    //Use raylibs batching system to drastically speed up text rendering
    void DrawText_impl(TextPrimitive p)
    {
        if(!p.text)
            return;
        Color color = p.font_color;
        for(const char *ch = p.text; *ch; ch++)
        {
            char c = *ch;
            int width = MeasureChar_impl(c, p.font_size, p.font_spacing);
            if(c == '\n')
            {
                p.cursor_x = 0;
                p.cursor_y += p.font_size + p.line_spacing;
                continue;
            }
            else if(c == ' ' || c == '\t')
            {
                p.cursor_x += width + p.font_spacing;
                continue;
            }
            int render_x = p.x + p.cursor_x;
            int render_y = p.y + p.cursor_y;
            DrawTextCodepoint(font, c, {(float)render_x, (float)render_y}, p.font_size, {color.r, color.g, color.b, color.a});
            p.cursor_x += width + p.font_spacing;
        }
        //DrawTextEx(font, p.text, Vector2{(float)p.x, (float)p.y}, p.font_size, p.font_spacing, {color.r, color.g, color.b, color.a});
    }
    int MeasureChar_impl(char c, int font_size, int spacing)
    {
        if(IsFontValid(font) && c >= 32)
        {
            return (int)font_info[(int)c].advanceX * font_size / font.baseSize + spacing;
        }
        return 0;
    }
    int MeasureText_impl(const char* text, int font_size, int spacing)
    {
        int width = 0;
        if(text)
        {
            while(*text != '\0')
            {
                width += MeasureChar_impl(*text, font_size, spacing);
                text++;
            }
        }
        return width;
    }
}
void UI::BeginScissorMode_impl(float x, float y, float width, float height)
{
    BeginScissorMode(x, y, width, height);
}
void UI::EndScissorMode_impl()
{
    EndScissorMode();
}
void UI::LogError_impl(const char* msg)
{
    std::cout<<msg; 
}
void UI::LogError_impl(int n)
{
    std::cout<<n; 
}




