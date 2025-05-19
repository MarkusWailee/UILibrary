#include <raylib/raylib.h>
#include "MUI.hpp"



//Drawing functions
namespace UI
{
    void DrawRectangle_impl(float x, float y, float width, float height, float corner_radius, float border_size, Color brdr, Color bg)
    {
        DrawRectangle(x, y, width, height, {brdr.r, brdr.g, brdr.b, brdr.a});
        DrawRectangle(x + border_size, y + border_size, width-border_size * 2, height-border_size * 2, {bg.r, bg.g, bg.b, bg.a});
    }
    void DrawText_impl(const char* text, const TextPrimitive& p)
    {
        DrawText(text, p.x, p.y, p.font_size, {p.color.r, p.color.g, p.color.b, p.color.a});
    }
    float MeasureText_impl(const char* text, const TextPrimitive& p)
    {
        return MeasureText(text, p.font_size);
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
#include <iostream>
void UI::LogError_impl(const char* msg)
{
    std::cout<<msg; 
}
void UI::LogError_impl(int n)
{
    std::cout<<n; 
}




