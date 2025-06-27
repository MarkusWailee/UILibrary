#include <raylib/raylib.h>
#include "ui/ui.hpp"
#include <math/vec.h>
#include "UI_Demo.hpp"
#include "ui_inspector.hpp"

namespace UI
{
    class Builder
    {
    public:
        void SetContext(Context* context) 
        {
            this->context = context; 
        }
        Builder& Box(const char* id = nullptr)
        {
            if(context == nullptr)
                return *this;
            this->id = id;
            if(id)
            {
                info = context->GetBoxInfo(id);
            }
            return *this;
        }
        Builder& Style(const BoxStyle& style)
        {
            this->style = style;
            return *this;
        }
        template<typename Func>
        Builder& OnHover(Func func)
        {
            if(context == nullptr)
                return *this;
            if(info.valid && info.is_hover)
            {
                func();
            }
            return *this;
        }
        template<typename Func>
        Builder& OnDirectHover(Func func)
        {
            if(context == nullptr)
                return *this;
            if(info.valid && info.is_direct_hover)
            {
                func();
            }
            return *this;
        }
        template<typename Func>
        void Run(Func func)
        {
            if(context == nullptr)
                return;
            context->BeginBox(style, id); 
            func();
            context->EndBox();
        }
    private:
        BoxStyle style;
        BoxInfo info;
        const char* text = nullptr;
        const char* id = nullptr;
        bool should_copy = true;
        Context* context = nullptr;
    };
}
void ExampleDemo()
{
    UI::BeginRoot(0, 0, GetScreenWidth(), GetScreenHeight(), GetMouseX(), GetMouseY());
    UI::Draw();
}
int main(void)
{
    float screenWidth = 960;
    float screenHeight = 600;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetExitKey(0);
    SetTargetFPS(256);

    UI::Init_impl();
    UI::Context ui_context(32768 * 4);
    UI::SetContext(&ui_context);

    UI::Builder ui;
    ui.SetContext(&ui_context);

    float time = 0;
    bool flip = false;
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        BeginDrawing();
        ClearBackground(Color{0, 0, 0, 255});

        UI::SetDebugInput(IsMouseButtonPressed(0), IsMouseButtonReleased(0), GetMouseWheelMove(),IsKeyPressed(KEY_F));
        //SpotifyExample();
        //UI::BeginRoot(0, 0, GetScreenWidth(), GetScreenHeight(), GetMouseX(), GetMouseY());
        ui_context.BeginRoot(0, 0, GetScreenWidth(), GetScreenHeight(), GetMouseX(), GetMouseY());

        UI::BoxStyle style;
        style.width = {200};
        style.height = {200};
        style.background_color = {255, 0, 0, 255};

        ui.Box("test box")
            .OnDirectHover([&]{style.background_color = {255,255,255,255};})
            .Style(style)
            .Run([&]
                {
                    UI::BoxStyle style;
                    style.background_color = {0,0,255,255};
                    ui.Box("box 2")
                    .OnDirectHover([&style] {style.background_color = {0, 255, 0, 255};})
                    .Style(style)
                    .Run([]{});
                });

        ui_context.EndRoot();
        ui_context.Draw(); 

        //UI::ToolKit::Inspector(UI::GetContext());
        //DrawText(TextFormat("fps = %d", GetFPS()), 10, 10, 20, WHITE);
        EndDrawing();
    }
    CloseWindow();        // Close window and OpenGL context

    return 0;
}