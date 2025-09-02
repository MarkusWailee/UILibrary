#include <raylib/raylib.h>
#include "ui/ui.hpp"
#include "UI_Demo.hpp"

int main(void)
{
    // {
    //
    //     UI::TextStyle style;
    //     style.FgColor({255,0 ,0, 255}).FontSize(12);
    //     UI::Internal::TextSpan spans[] =
    //     {
    //         UI::Internal::TextSpan{UI::MakeStringU32(U"Hello"), style},
    //         UI::Internal::TextSpan{UI::MakeStringU32(U" World"), style}
    //     };
    //     UI::Internal::TextSpans a = {UI::Internal::ArrayView<UI::Internal::TextSpan>{spans, 2}};
    //     UI::Internal::TextSpans::Iterator it = a.Begin();
    //
    //     UI::StringU32 str = a.GetSubString(it, it);
    //     //UI::StringU32 str = a.GetSubString(it, it);
    //     for(int i = 0; i < str.Size(); i++)
    //     {
    //         std::cout << (char)str[i];
    //     }
    //
    //     std::cin.get();
    //     return 0;
    // }

    float screenWidth = 960;
    float screenHeight = 600;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    SetExitKey(0);
    SetTargetFPS(60);

    UI::Init_impl("assets/fonts/RobotoMonoNerdFont-Regular.ttf");
    UI::Context context(128 * UI::KB);
    UI::TextStyle style;
    style.FgColor({255, 255, 255, 255}).FontSize(32);
    int count = 0;
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        count++;
        //LayoutTest(&context);
        TextLayoutTest(&context);

        BeginDrawing();
        ClearBackground(Color{0, 0, 0, 255});
        UI::Draw();
        //DrawText(TextFormat("Fps = %d", GetFPS()), 10, 10, 20, WHITE);
        //UI::StringU32 str = UI::MakeStringU32(U"helo world");
        //UI::DrawText_impl(style, 100, 100, str.data, str.size);
        EndDrawing();
    }

    CloseWindow(); // Close window and OpenGL context

    return 0;
}
