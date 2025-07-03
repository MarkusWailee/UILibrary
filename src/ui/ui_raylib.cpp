#include <raylib/raylib.h>
#include <raylib/rlgl.h>
#include "ui.hpp"
//Raylib backend

namespace UI
{
    //std::unordered_map<std::string, Font> fonts;
    Font font;
    GlyphInfo font_info[128]{};
    void Init_impl(const char* font_path)
    {
        font = LoadFontEx(font_path, 48, 0, 0);
        if(IsFontValid(font))
        {
            for(int i = 32; i<=126; i++) //Printable asci characters
            font_info[i] = GetGlyphInfo(font, i);
            SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
        }
    }
    ::MouseButton TraslateMouseButtonToRaylib_impl(MouseButton button)
    {
        switch (button)
        {
            case::UI::MouseButton::MOUSE_LEFT    : return ::MouseButton::MOUSE_BUTTON_LEFT    ;
            case::UI::MouseButton::MOUSE_RIGHT   : return ::MouseButton::MOUSE_BUTTON_RIGHT   ;
            case::UI::MouseButton::MOUSE_MIDDLE  : return ::MouseButton::MOUSE_BUTTON_MIDDLE  ;
            case::UI::MouseButton::MOUSE_SIDE    : return ::MouseButton::MOUSE_BUTTON_SIDE    ;
            case::UI::MouseButton::MOUSE_EXTRA   : return ::MouseButton::MOUSE_BUTTON_EXTRA   ;
            case::UI::MouseButton::MOUSE_FORWARD : return ::MouseButton::MOUSE_BUTTON_FORWARD ;
            case::UI::MouseButton::MOUSE_BACK    : return ::MouseButton::MOUSE_BUTTON_BACK    ;
        }
        return ::MouseButton::MOUSE_BUTTON_LEFT;
    }
    KeyboardKey TranslateKeyToRaylib_impl(Key key)
    {
        switch(key)
        {
            case::UI::Key::KEY_NULL          : return KeyboardKey::KEY_NULL            ;
            case::UI::Key::KEY_SPACE         : return KeyboardKey::KEY_SPACE           ;
            case::UI::Key::KEY_APOSTROPHE    : return KeyboardKey::KEY_APOSTROPHE      ;
            case::UI::Key::KEY_COMMA         : return KeyboardKey::KEY_COMMA           ;
            case::UI::Key::KEY_MINUS         : return KeyboardKey::KEY_MINUS           ;
            case::UI::Key::KEY_PERIOD        : return KeyboardKey::KEY_PERIOD          ;
            case::UI::Key::KEY_SLASH         : return KeyboardKey::KEY_SLASH           ;
            case::UI::Key::KEY_ZERO          : return KeyboardKey::KEY_ZERO            ;
            case::UI::Key::KEY_ONE           : return KeyboardKey::KEY_ONE             ;
            case::UI::Key::KEY_TWO           : return KeyboardKey::KEY_TWO             ;
            case::UI::Key::KEY_THREE         : return KeyboardKey::KEY_THREE           ;
            case::UI::Key::KEY_FOUR          : return KeyboardKey::KEY_FOUR            ;
            case::UI::Key::KEY_FIVE          : return KeyboardKey::KEY_FIVE            ;
            case::UI::Key::KEY_SIX           : return KeyboardKey::KEY_SIX             ;
            case::UI::Key::KEY_SEVEN         : return KeyboardKey::KEY_SEVEN           ;
            case::UI::Key::KEY_EIGHT         : return KeyboardKey::KEY_EIGHT           ;
            case::UI::Key::KEY_NINE          : return KeyboardKey::KEY_NINE            ;
            case::UI::Key::KEY_SEMICOLON     : return KeyboardKey::KEY_SEMICOLON       ;
            case::UI::Key::KEY_EQUAL         : return KeyboardKey::KEY_EQUAL           ;
            case::UI::Key::KEY_A             : return KeyboardKey::KEY_A               ;
            case::UI::Key::KEY_B             : return KeyboardKey::KEY_B               ;
            case::UI::Key::KEY_C             : return KeyboardKey::KEY_C               ;
            case::UI::Key::KEY_D             : return KeyboardKey::KEY_D               ;
            case::UI::Key::KEY_E             : return KeyboardKey::KEY_E               ;
            case::UI::Key::KEY_F             : return KeyboardKey::KEY_F               ;
            case::UI::Key::KEY_G             : return KeyboardKey::KEY_G               ;
            case::UI::Key::KEY_H             : return KeyboardKey::KEY_H               ;
            case::UI::Key::KEY_I             : return KeyboardKey::KEY_I               ;
            case::UI::Key::KEY_J             : return KeyboardKey::KEY_J               ;
            case::UI::Key::KEY_K             : return KeyboardKey::KEY_K               ;
            case::UI::Key::KEY_L             : return KeyboardKey::KEY_L               ;
            case::UI::Key::KEY_M             : return KeyboardKey::KEY_M               ;
            case::UI::Key::KEY_N             : return KeyboardKey::KEY_N               ;
            case::UI::Key::KEY_O             : return KeyboardKey::KEY_O               ;
            case::UI::Key::KEY_P             : return KeyboardKey::KEY_P               ;
            case::UI::Key::KEY_Q             : return KeyboardKey::KEY_Q               ;
            case::UI::Key::KEY_R             : return KeyboardKey::KEY_R               ;
            case::UI::Key::KEY_S             : return KeyboardKey::KEY_S               ;
            case::UI::Key::KEY_T             : return KeyboardKey::KEY_T               ;
            case::UI::Key::KEY_U             : return KeyboardKey::KEY_U               ;
            case::UI::Key::KEY_V             : return KeyboardKey::KEY_V               ;
            case::UI::Key::KEY_W             : return KeyboardKey::KEY_W               ;
            case::UI::Key::KEY_X             : return KeyboardKey::KEY_X               ;
            case::UI::Key::KEY_Y             : return KeyboardKey::KEY_Y               ;
            case::UI::Key::KEY_Z             : return KeyboardKey::KEY_Z               ;
            case::UI::Key::KEY_LEFT_BRACKET  : return KeyboardKey::KEY_LEFT_BRACKET    ;
            case::UI::Key::KEY_BACKSLASH     : return KeyboardKey::KEY_BACKSLASH       ;
            case::UI::Key::KEY_RIGHT_BRACKET : return KeyboardKey::KEY_RIGHT_BRACKET   ;
            case::UI::Key::KEY_GRAVE         : return KeyboardKey::KEY_GRAVE           ;
            case::UI::Key::KEY_ESCAPE        : return KeyboardKey::KEY_ESCAPE          ; 
            case::UI::Key::KEY_ENTER         : return KeyboardKey::KEY_ENTER           ;
            case::UI::Key::KEY_TAB           : return KeyboardKey::KEY_TAB             ;
            case::UI::Key::KEY_BACKSPACE     : return KeyboardKey::KEY_BACKSPACE       ;
            case::UI::Key::KEY_INSERT        : return KeyboardKey::KEY_INSERT          ;
            case::UI::Key::KEY_DELETE        : return KeyboardKey::KEY_DELETE          ;
            case::UI::Key::KEY_RIGHT         : return KeyboardKey::KEY_RIGHT           ;
            case::UI::Key::KEY_LEFT          : return KeyboardKey::KEY_LEFT            ;
            case::UI::Key::KEY_DOWN          : return KeyboardKey::KEY_DOWN            ;
            case::UI::Key::KEY_UP            : return KeyboardKey::KEY_UP              ;
            case::UI::Key::KEY_PAGE_UP       : return KeyboardKey::KEY_PAGE_UP         ;
            case::UI::Key::KEY_PAGE_DOWN     : return KeyboardKey::KEY_PAGE_DOWN       ;
            case::UI::Key::KEY_HOME          : return KeyboardKey::KEY_HOME            ;
            case::UI::Key::KEY_END           : return KeyboardKey::KEY_END             ;
            case::UI::Key::KEY_CAPS_LOCK     : return KeyboardKey::KEY_CAPS_LOCK       ;
            case::UI::Key::KEY_SCROLL_LOCK   : return KeyboardKey::KEY_SCROLL_LOCK     ;
            case::UI::Key::KEY_NUM_LOCK      : return KeyboardKey::KEY_NUM_LOCK        ;
            case::UI::Key::KEY_PRINT_SCREEN  : return KeyboardKey::KEY_PRINT_SCREEN    ;
            case::UI::Key::KEY_PAUSE         : return KeyboardKey::KEY_PAUSE           ;
            case::UI::Key::KEY_F1            : return KeyboardKey::KEY_F1              ;
            case::UI::Key::KEY_F2            : return KeyboardKey::KEY_F2              ;
            case::UI::Key::KEY_F3            : return KeyboardKey::KEY_F3              ;
            case::UI::Key::KEY_F4            : return KeyboardKey::KEY_F4              ;
            case::UI::Key::KEY_F5            : return KeyboardKey::KEY_F5              ;
            case::UI::Key::KEY_F6            : return KeyboardKey::KEY_F6              ;
            case::UI::Key::KEY_F7            : return KeyboardKey::KEY_F7              ;
            case::UI::Key::KEY_F8            : return KeyboardKey::KEY_F8              ;
            case::UI::Key::KEY_F9            : return KeyboardKey::KEY_F9              ;
            case::UI::Key::KEY_F10           : return KeyboardKey::KEY_F10             ;
            case::UI::Key::KEY_F11           : return KeyboardKey::KEY_F11             ;
            case::UI::Key::KEY_F12           : return KeyboardKey::KEY_F12             ;
            case::UI::Key::KEY_LEFT_SHIFT    : return KeyboardKey::KEY_LEFT_SHIFT      ;
            case::UI::Key::KEY_LEFT_CONTROL  : return KeyboardKey::KEY_LEFT_CONTROL    ;
            case::UI::Key::KEY_LEFT_ALT      : return KeyboardKey::KEY_LEFT_ALT        ;
            case::UI::Key::KEY_LEFT_SUPER    : return KeyboardKey::KEY_LEFT_SUPER      ;
            case::UI::Key::KEY_RIGHT_SHIFT   : return KeyboardKey::KEY_RIGHT_SHIFT     ;
            case::UI::Key::KEY_RIGHT_CONTROL : return KeyboardKey::KEY_RIGHT_CONTROL   ;
            case::UI::Key::KEY_RIGHT_ALT     : return KeyboardKey::KEY_RIGHT_ALT       ;
            case::UI::Key::KEY_RIGHT_SUPER   : return KeyboardKey::KEY_RIGHT_SUPER     ;
            case::UI::Key::KEY_KB_MENU       : return KeyboardKey::KEY_KB_MENU         ;
            case::UI::Key::KEY_KP_0          : return KeyboardKey::KEY_KP_0            ;
            case::UI::Key::KEY_KP_1          : return KeyboardKey::KEY_KP_1            ;
            case::UI::Key::KEY_KP_2          : return KeyboardKey::KEY_KP_2            ;
            case::UI::Key::KEY_KP_3          : return KeyboardKey::KEY_KP_3            ;
            case::UI::Key::KEY_KP_4          : return KeyboardKey::KEY_KP_4            ;
            case::UI::Key::KEY_KP_5          : return KeyboardKey::KEY_KP_5            ;
            case::UI::Key::KEY_KP_6          : return KeyboardKey::KEY_KP_6            ;
            case::UI::Key::KEY_KP_7          : return KeyboardKey::KEY_KP_7            ;
            case::UI::Key::KEY_KP_8          : return KeyboardKey::KEY_KP_8            ;
            case::UI::Key::KEY_KP_9          : return KeyboardKey::KEY_KP_9            ;
            case::UI::Key::KEY_KP_DECIMAL    : return KeyboardKey::KEY_KP_DECIMAL      ;
            case::UI::Key::KEY_KP_DIVIDE     : return KeyboardKey::KEY_KP_DIVIDE       ;
            case::UI::Key::KEY_KP_MULTIPLY   : return KeyboardKey::KEY_KP_MULTIPLY     ;
            case::UI::Key::KEY_KP_SUBTRACT   : return KeyboardKey::KEY_KP_SUBTRACT     ;
            case::UI::Key::KEY_KP_ADD        : return KeyboardKey::KEY_KP_ADD          ;
            case::UI::Key::KEY_KP_ENTER      : return KeyboardKey::KEY_KP_ENTER        ;
            case::UI::Key::KEY_KP_EQUAL      : return KeyboardKey::KEY_KP_EQUAL        ;
            case::UI::Key::KEY_BACK          : return KeyboardKey::KEY_BACK            ;
            case::UI::Key::KEY_MENU          : return KeyboardKey::KEY_MENU            ;
            default                          : return KeyboardKey::KEY_NULL            ;
        }   
    }                                           
}                                               
                                                
#include <iostream>                            
//Drawing functions                           
namespace UI
{
    void DrawRectangle_impl(float x, float y, float width, float height, float corner_radius, float border_size, Color brdr, Color bg)
    {
        if(brdr.a  == 0&& bg.a == 0)
            return;
        corner_radius = Min(corner_radius, width / 2);
        #if 0 //Testing performance
        {
            DrawRectangle(x, y, width, height, {bg.r, bg.g, bg.b, bg.a});
            //DrawRectangleRounded({x+border_size, y+border_size, width-border_size, height-border_size}, ((float)corner_radius / Min(width-border_size, height - border_size)), 4, {bg.r, bg.g, bg.b, bg.a});
            //DrawRectangleRoundedLinesEx({x+border_size, y+border_size, width-border_size, height-border_size}, ((float)corner_radius / Min(width-border_size, height - border_size)), 4,border_size, {brdr.r, brdr.g, brdr.b, brdr.a});
            
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
    if(msg != nullptr)
        std::cout<<msg; 
}


// ===== Input =====

bool UI::IsKeyPressed(Key key)
{
    return ::IsKeyPressed(key);
}
bool UI::IsKeyReleased(Key key)
{
    return ::IsKeyReleased(key);
}
bool UI::IsKeyDown(Key key)
{
    return ::IsKeyDown(key);
}
bool UI::IsKeyRepeat(Key key)
{
    return ::IsKeyPressedRepeat(key);
}
char UI::GetPressedChar()
{
    return GetCharPressed();
}
bool UI::IsMousePressed(MouseButton button)
{
    return ::IsMouseButtonDown(button);
}
bool UI::IsMouseReleased(MouseButton button)
{
    return ::IsMouseButtonReleased(button);
}
bool UI::IsMouseDown(MouseButton button)
{
    return ::IsMouseButtonDown(button);
}
int UI::GetMouseScroll()
{
    return (int)::GetMouseWheelMove();
}
int UI::GetMouseX()
{
    return ::GetMouseX();
}
int UI::GetMouseY()
{
    return ::GetMouseY();
}
int UI::GetScreenWidth()
{
    return ::GetScreenWidth();
}
int UI::GetScreenHeight()
{
    return ::GetScreenHeight();
}





