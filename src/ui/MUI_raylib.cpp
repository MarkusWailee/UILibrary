#include <raylib/raylib.h>
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
        font = LoadFontEx("assets/fonts/Quicksand-Regular.ttf", 32, 0, 0);
        if(IsFontValid(font))
        {
            for(int i = 32; i<=126; i++) //Printable asci characters
            font_info[i] = GetGlyphInfo(font, i);
        }
    }
}

//Drawing functions
#include <iostream>
namespace UI
{
    void DrawRectangle_impl(float x, float y, float width, float height, float corner_radius, float border_size, Color brdr, Color bg)
    {
        DrawRectangle(x, y, width, height, {brdr.r, brdr.g, brdr.b, brdr.a});
        DrawRectangle(x + border_size, y + border_size, width-border_size * 2, height-border_size * 2, {bg.r, bg.g, bg.b, bg.a});
    }
    void SetFont_impl(const char* file_path)
    {
        
    }
    void DrawText_impl(const char* text, int x, int y, int font_size, int spacing, UI::Color color)
    {
        DrawTextEx(font, text, Vector2{(float)x, (float)y}, font_size, spacing, {color.r, color.g, color.b, color.a});
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




