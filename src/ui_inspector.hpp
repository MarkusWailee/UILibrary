#pragma once
#include <raylib/raylib.h>
#include "ui/ui.hpp"
#include <iostream>



#if UI_ENABLE_DEBUG
namespace UI::ToolKit
{
    UI::Internal::Map<bool> map;
}
#endif