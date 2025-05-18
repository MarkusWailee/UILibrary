#include <raylib/raylib.h>
#include "MUI.hpp"



//Drawing functions
namespace UI
{
    void DrawRectangle_impl(const RectanglePrimitive& p)
    {
        DrawRectangle(p.x, p.y, p.width, p.height, {p.background_color.r, p.background_color.g, p.background_color.b, p.background_color.a});
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




