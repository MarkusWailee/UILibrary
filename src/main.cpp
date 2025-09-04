#include <raylib/raylib.h>
#include "ui/ui.hpp"
#include "UI_Demo.hpp"

void LogStringU32(const UI::StringU32& string)
{
    for(int i = 0; i < string.Size(); i++)
        std::cout<<(char)string[i]<<'\n';
}

int main(void)
{
    // {
    //     using namespace UI::Internal;
    //     MemoryArena arena(1 * UI::KB);
    //     TextSpans spans;
    //     spans.Add(TextSpan{UI::MakeStringU32(U"HEL"), UI::TextStyle()}, &arena);
    //     spans.Add(TextSpan{UI::MakeStringU32(U"WORLD"), UI::TextStyle()}, &arena);
    //     auto start = spans.Begin();
    //     TextSpans::Iterator end;
    //     LogStringU32(TextSpans::Iterator::GetTextSpan(start, end));
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
