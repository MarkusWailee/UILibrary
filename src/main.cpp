#include <raylib/raylib.h>
#include "ui/MUI.hpp"

#include <math/vec.h>


void DrawChatGPTWebsiteMockup()
{
    UI::BeginRoot(GetScreenWidth(), GetScreenHeight(), GetMouseX(), GetMouseY());

    // Root container: vertical flow, full screen, light background
    UI::BoxStyle root;
    root.width = {100, UI::Unit::PARENT_PERCENT};
    root.height = {100, UI::Unit::PARENT_PERCENT};
    root.flow.axis = UI::Flow::Axis::VERTICAL;
    root.background_color = {248, 249, 250, 255}; // #f8f9fa light gray background
    root.padding = {0, 0, 12, 0}; // bottom padding for footer spacing

    // Header bar: fixed height, horizontal flow, centered content
    UI::BoxStyle header;
    header.width = {100, UI::Unit::PARENT_PERCENT};
    header.height = {60, UI::Unit::PIXEL};
    header.flow.axis = UI::Flow::Axis::HORIZONTAL;
    header.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
    header.flow.horizontal_alignment = UI::Flow::Alignment::SPACE_BETWEEN;
    header.padding = {24, 24, 0, 0};
    header.background_color = {255, 255, 255, 255};
    header.border_color = {220, 220, 220, 255};
    header.border_width = 1;

    // Logo style (left side)
    UI::BoxStyle logo;
    logo.width = {120, UI::Unit::PIXEL};
    logo.height = {40, UI::Unit::PIXEL};
    logo.background_color = {255, 255, 255, 255};
    logo.padding = {6, 8, 8, 8};
    logo.corner_radius = 6;

    // Navigation buttons on right
    UI::BoxStyle nav_button;
    nav_button.height = {32, UI::Unit::PIXEL};
    nav_button.width = {100, UI::Unit::CONTENT_PERCENT};
    nav_button.background_color = {255, 255, 255, 255};
    nav_button.border_color = {100, 100, 100, 50};
    nav_button.border_width = 1;
    nav_button.corner_radius = 6;
    nav_button.padding = {6, 12, 6, 6};
    nav_button.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
    nav_button.flow.horizontal_alignment = UI::Flow::Alignment::CENTERED;

    // Main container: horizontal flow, fills remaining vertical space
    UI::BoxStyle main_container;
    main_container.width = {100, UI::Unit::PARENT_PERCENT};
    main_container.height = {100, UI::Unit::AVAILABLE_PERCENT};
    main_container.flow.axis = UI::Flow::Axis::HORIZONTAL;
    main_container.gap_column = {20, UI::Unit::PIXEL};
    main_container.padding = {24, 24, 24, 24};

    // Left sidebar: vertical flow, fixed width, white background
    UI::BoxStyle left_sidebar;
    left_sidebar.width = {280, UI::Unit::PIXEL};
    left_sidebar.height = {100, UI::Unit::PARENT_PERCENT};
    left_sidebar.background_color = {255, 255, 255, 255};
    left_sidebar.corner_radius = 10;
    left_sidebar.padding = {16, 16, 16, 16};
    left_sidebar.flow.axis = UI::Flow::Axis::VERTICAL;
    left_sidebar.gap_row = {16, UI::Unit::PIXEL};

    // Sidebar title
    UI::BoxStyle sidebar_title;
    sidebar_title.height = {28, UI::Unit::PIXEL};
    sidebar_title.width = {100, UI::Unit::CONTENT_PERCENT};
    sidebar_title.padding = {4, 4, 0, 0};
    sidebar_title.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;

    // Sidebar button style
    UI::BoxStyle sidebar_button;
    sidebar_button.width = {100, UI::Unit::PARENT_PERCENT};
    sidebar_button.height = {44, UI::Unit::PIXEL};
    sidebar_button.background_color = {245, 245, 245, 255};
    sidebar_button.border_color = {230, 230, 230, 255};
    sidebar_button.border_width = 1;
    sidebar_button.corner_radius = 8;
    sidebar_button.padding = {12, 16, 12, 12};
    sidebar_button.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;

    // Chat area container: fills remaining width, vertical flow
    UI::BoxStyle chat_area;
    chat_area.width = {100, UI::Unit::AVAILABLE_PERCENT};
    chat_area.height = {100, UI::Unit::PARENT_PERCENT};
    chat_area.background_color = {255, 255, 255, 255};
    chat_area.corner_radius = 10;
    chat_area.flow.axis = UI::Flow::Axis::VERTICAL;
    chat_area.padding = {24, 24, 24, 24};
    chat_area.gap_row = {12, UI::Unit::PIXEL};

    // Chat header: horizontal flow, space between
    UI::BoxStyle chat_header;
    chat_header.width = {100, UI::Unit::PARENT_PERCENT};
    chat_header.height = {40, UI::Unit::PIXEL};
    chat_header.flow.axis = UI::Flow::Axis::HORIZONTAL;
    chat_header.flow.horizontal_alignment = UI::Flow::Alignment::SPACE_BETWEEN;
    chat_header.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;

    // Chat message container: vertical flow, fills remaining space
    UI::BoxStyle chat_messages;
    chat_messages.width = {100, UI::Unit::PARENT_PERCENT};
    chat_messages.height = {100, UI::Unit::AVAILABLE_PERCENT};
    chat_messages.flow.axis = UI::Flow::Axis::VERTICAL;
    chat_messages.gap_row = {16, UI::Unit::PIXEL};
    chat_messages.scroll_y = 0; // scrollable area

    // Message bubble style for user and assistant
    UI::BoxStyle message_bubble_user;
    message_bubble_user.width = {60, UI::Unit::PARENT_PERCENT};
    message_bubble_user.height = {100, UI::Unit::CONTENT_PERCENT};
    message_bubble_user.background_color = {10, 132, 255, 255}; // blue user bubble
    message_bubble_user.corner_radius = 16;
    message_bubble_user.padding = {12, 16, 12, 12};
    message_bubble_user.margin = {0, 0, 0, 0};
    message_bubble_user.flow.axis = UI::Flow::Axis::VERTICAL;

    UI::BoxStyle message_bubble_assistant;
    message_bubble_assistant.width = {60, UI::Unit::PARENT_PERCENT};
    message_bubble_assistant.height = {100, UI::Unit::CONTENT_PERCENT};
    message_bubble_assistant.background_color = {230, 230, 230, 255}; // light grey assistant bubble
    message_bubble_assistant.corner_radius = 16;
    message_bubble_assistant.padding = {12, 16, 12, 12};
    message_bubble_assistant.margin = {0, 0, 0, 0};
    message_bubble_assistant.flow.axis = UI::Flow::Axis::VERTICAL;

    // Input area: fixed height, horizontal flow
    UI::BoxStyle input_area;
    input_area.height = {60, UI::Unit::PIXEL};
    input_area.width = {100, UI::Unit::PARENT_PERCENT};
    input_area.flow.axis = UI::Flow::Axis::HORIZONTAL;
    input_area.gap_column = {12, UI::Unit::PIXEL};
    input_area.padding = {8, 8, 8, 8};
    input_area.background_color = {245, 245, 245, 255};
    input_area.corner_radius = 10;

    // Input text box style
    UI::BoxStyle input_box;
    input_box.width = {100, UI::Unit::AVAILABLE_PERCENT};
    input_box.height = {44, UI::Unit::PIXEL};
    input_box.background_color = {255, 255, 255, 255};
    input_box.corner_radius = 8;
    input_box.padding = {8, 12, 8, 8};
    input_box.border_color = {200, 200, 200, 255};
    input_box.border_width = 1;

    // Send button style
    UI::BoxStyle send_button;
    send_button.width = {80, UI::Unit::PIXEL};
    send_button.height = {44, UI::Unit::PIXEL};
    send_button.background_color = {10, 132, 255, 255};
    send_button.corner_radius = 8;
    send_button.padding = {8, 8, 8, 8};
    send_button.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
    send_button.flow.horizontal_alignment = UI::Flow::Alignment::CENTERED;

    // Footer bar: fixed height, horizontal flow, centered content
    UI::BoxStyle footer;
    footer.height = {40, UI::Unit::PIXEL};
    footer.width = {100, UI::Unit::PARENT_PERCENT};
    footer.background_color = {255, 255, 255, 255};
    footer.flow.axis = UI::Flow::Axis::HORIZONTAL;
    footer.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
    footer.flow.horizontal_alignment = UI::Flow::Alignment::CENTERED;
    footer.border_color = {220, 220, 220, 255};
    footer.border_width = 1;

    UI::BeginBox(root);

    // Header bar
    UI::BeginBox(header);

        // Logo placeholder
        UI::BeginBox(logo);
        UI::InsertText("[C:000000]ChatGPT"); // Pretend logo text
        UI::EndBox();

        // Right nav buttons
        const char* navs[] = { "[C:000000]Examples", "[C:000000]Pricing", "[C:000000]Docs", "[C:000000]Sign In" };
        for (int i = 0; i < 4; i++)
        {
            UI::BeginBox(nav_button);
            UI::InsertText(navs[i]);
            UI::EndBox();
        }

    UI::EndBox();

    // Main horizontal container (sidebar + chat area)
    UI::BeginBox(main_container);

        // Left sidebar
        UI::BeginBox(left_sidebar);

            UI::BeginBox(sidebar_title);
            UI::InsertText("[C:000000]Model Settings");
            UI::EndBox();

            const char* sidebar_options[] = { "[C:000000]GPT-4", "[C:000000]GPT-3.5 Turbo", "[C:000000]Custom" };
            for (int i = 0; i < 3; i++)
            {
                UI::BeginBox(sidebar_button);
                UI::InsertText(sidebar_options[i]);
                UI::EndBox();
            }

            UI::BeginBox(sidebar_title);
            UI::InsertText("[C:000000]Examples");
            UI::EndBox();

            const char* examples[] = {
                "\"[C:000000][S:28]Explain quantum computing\"",
                "\"[C:000000][S:28]Got any creative ideas...\"",
                "\"[C:000000][S:28]How do I make an HTTP request..?\""
            };
            for (int i = 0; i < 3; i++)
            {
                UI::BeginBox(sidebar_button);
                UI::InsertText(examples[i]);
                UI::EndBox();
            }

        UI::EndBox();

        // Chat area container
        UI::BeginBox(chat_area);

            // Chat header
            UI::BeginBox(chat_header);
                UI::InsertText("[C:000000]ChatGPT");
                UI::InsertText("[C:000000]New chat");
            UI::EndBox();

            // Chat messages (mock)
            UI::BeginBox(chat_messages);

                // Assistant message
                UI::BeginBox(message_bubble_assistant);
                UI::InsertText("[C:000000]Hello! How can I assist you today?");
                UI::EndBox();

                // User message
                UI::BeginBox(message_bubble_user);
                UI::InsertText("[C:000000]Can you recreate the ChatGPT website UI?");
                UI::EndBox();

            UI::EndBox();

            // Input area
            UI::BeginBox(input_area);
                UI::BeginBox(input_box);
                UI::InsertText("[C:000000]..."); // input field placeholder
                UI::EndBox();

                UI::BeginBox(send_button);
                UI::InsertText("[C:000000]Send");
                UI::EndBox();
            UI::EndBox();

        UI::EndBox();

    UI::EndBox();

    // Footer
    UI::BeginBox(footer);
    UI::InsertText("[C:000000]© 2025 OpenAI. All rights reserved.");
    UI::EndBox();

    UI::EndBox();

    UI::EndRoot();

    UI::Draw();
}
void DrawVSCodeMockup_V2()
{
    UI::BeginRoot(GetScreenWidth(), GetScreenHeight(), GetMouseX(), GetMouseY());

    // Root: vertical flow, full screen
    UI::BoxStyle root;
    root.width = {100, UI::Unit::PARENT_PERCENT};
    root.height = {100, UI::Unit::PARENT_PERCENT};
    root.flow.axis = UI::Flow::Axis::VERTICAL;
    root.background_color = {30, 30, 30, 255};

    // Top bar: fixed height, horizontal flow, buttons fit content
    UI::BoxStyle top_bar;
    top_bar.height = {40, UI::Unit::PIXEL};
    top_bar.width = {100, UI::Unit::PARENT_PERCENT};
    top_bar.background_color = {40, 40, 40, 255};
    top_bar.flow.axis = UI::Flow::Axis::HORIZONTAL;
    top_bar.gap_column = {12, UI::Unit::PIXEL};
    top_bar.padding = {6, 8, 0, 0};
    top_bar.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;

    UI::BoxStyle top_button;
    top_button.height = {28, UI::Unit::PIXEL};
    top_button.width = {100, UI::Unit::CONTENT_PERCENT}; // fit width to text
    top_button.background_color = {60, 60, 60, 255};
    top_button.corner_radius = 4;
    top_button.padding = {8, 10, 6, 6};
    top_button.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
    top_button.flow.horizontal_alignment = UI::Flow::Alignment::CENTERED;

    // Main area: horizontal flow, fills remaining vertical space
    UI::BoxStyle main_area;
    main_area.width = {100, UI::Unit::PARENT_PERCENT};
    main_area.height = {100, UI::Unit::AVAILABLE_PERCENT};  // fills all space below top and above terminal
    main_area.flow.axis = UI::Flow::Axis::HORIZONTAL;
    main_area.gap_column = {10, UI::Unit::PIXEL};
    main_area.padding = {8, 8, 8, 8};

    // Left sidebar: file explorer, fixed-ish width (content fit min, max percent)
    UI::BoxStyle left_sidebar;
    left_sidebar.width = {250, UI::Unit::PIXEL};
    left_sidebar.min_width = {180, UI::Unit::PIXEL};
    left_sidebar.max_width = {30, UI::Unit::PARENT_PERCENT};
    left_sidebar.height = {100, UI::Unit::PARENT_PERCENT};
    left_sidebar.background_color = {40, 40, 40, 255};
    left_sidebar.flow.axis = UI::Flow::Axis::VERTICAL;
    left_sidebar.gap_row = {6, UI::Unit::PIXEL};
    left_sidebar.padding = {12, 12, 12, 12};

    // Editor area: flexible width to fill leftover horizontal space
    UI::BoxStyle editor_area;
    editor_area.width = {100, UI::Unit::AVAILABLE_PERCENT};
    editor_area.height = {100, UI::Unit::PARENT_PERCENT};
    editor_area.background_color = {25, 25, 25, 255};
    editor_area.flow.axis = UI::Flow::Axis::VERTICAL;
    editor_area.gap_row = {4, UI::Unit::PIXEL};
    editor_area.padding = {16, 16, 16, 16};
    editor_area.corner_radius = 6;

    // Right sidebar: fixed width with tabs
    UI::BoxStyle right_sidebar;
    right_sidebar.width = {220, UI::Unit::PIXEL};
    right_sidebar.height = {100, UI::Unit::PARENT_PERCENT};
    right_sidebar.background_color = {40, 40, 40, 255};
    right_sidebar.flow.axis = UI::Flow::Axis::VERTICAL;
    right_sidebar.gap_row = {8, UI::Unit::PIXEL};
    right_sidebar.padding = {12, 12, 12, 12};

    // Bottom terminal panel: fixed height
    UI::BoxStyle terminal_panel;
    terminal_panel.height = {160, UI::Unit::PIXEL};
    terminal_panel.width = {100, UI::Unit::PARENT_PERCENT};
    terminal_panel.background_color = {20, 20, 20, 255};
    terminal_panel.padding = {10, 10, 10, 10};
    terminal_panel.corner_radius = 6;
    terminal_panel.flow.axis = UI::Flow::Axis::VERTICAL;
    terminal_panel.gap_row = {6, UI::Unit::PIXEL};

    // Button style reused for right sidebar tabs (fit content)
    UI::BoxStyle sidebar_button;
    sidebar_button.width = {100, UI::Unit::CONTENT_PERCENT};
    sidebar_button.height = {30, UI::Unit::PIXEL};
    sidebar_button.background_color = {60, 60, 60, 255};
    sidebar_button.corner_radius = 4;
    sidebar_button.padding = {8, 10, 6, 6};
    sidebar_button.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
    sidebar_button.flow.horizontal_alignment = UI::Flow::Alignment::CENTERED;

    // File entry style (similar to before)
    UI::BoxStyle file_entry;
    file_entry.height = {24, UI::Unit::PIXEL};
    file_entry.width = {100, UI::Unit::PARENT_PERCENT};
    file_entry.background_color = {50, 50, 50, 255};
    file_entry.padding = {4, 4, 0, 0};
    file_entry.corner_radius = 4;
    file_entry.flow.axis = UI::Flow::Axis::HORIZONTAL;
    file_entry.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;

    // Code line style
    UI::BoxStyle code_line;
    code_line.height = {22, UI::Unit::PIXEL};
    code_line.width = {100, UI::Unit::PARENT_PERCENT};
    code_line.flow.axis = UI::Flow::Axis::HORIZONTAL;
    code_line.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;

    // Begin root container
    UI::BeginBox(root);

    // Top bar with buttons
    UI::BeginBox(top_bar);
        const char* top_buttons[] = { "File", "Edit", "Selection", "View", "Go", "Run", "Terminal" };
        for (int i = 0; i < 7; ++i)
        {
            UI::BeginBox(top_button);
            UI::InsertText(top_buttons[i]);
            UI::EndBox();
        }
    UI::EndBox();

    // Main horizontal area
    UI::BeginBox(main_area);

        // Left sidebar: file explorer
        UI::BeginBox(left_sidebar);
            // Folder "src"
            UI::BeginBox(file_entry); UI::InsertText("[C:6699cc]📁 src"); UI::EndBox();
            UI::BeginBox(file_entry); UI::InsertText("  [C:ffffff]main.cpp"); UI::EndBox();
            UI::BeginBox(file_entry); UI::InsertText("  [C:ffffff]app.cpp"); UI::EndBox();
            UI::BeginBox(file_entry); UI::InsertText("  [C:ffffff]ui.cpp"); UI::EndBox();
            // Folder "include"
            UI::BeginBox(file_entry); UI::InsertText("[C:6699cc]📁 include"); UI::EndBox();
            UI::BeginBox(file_entry); UI::InsertText("  [C:ffffff]ui.h"); UI::EndBox();
            UI::BeginBox(file_entry); UI::InsertText("  [C:ffffff]app.h"); UI::EndBox();
            // Root files
            UI::BeginBox(file_entry); UI::InsertText("[C:ffffff]README.md"); UI::EndBox();
        UI::EndBox();

        // Editor area with fake syntax highlighted code
        UI::BeginBox(editor_area);
            const char* code_lines[] = {
                "[C:569CD6]int [C:D4D4D4]main[C:D4D4D4]() {",
                "    [C:569CD6]std[C:D4D4D4]::[C:569CD6]cout [C:D4D4D4]<< [C:CE9178]\"Hello, World!\"[C:D4D4D4] << [C:569CD6]std[C:D4D4D4]::[C:569CD6]endl;",
                "    [C:569CD6]return [C:D4D4D4]0;",
                "}"
            };
            for (int i = 0; i < 4; ++i)
            {
                UI::BeginBox(code_line);
                UI::InsertText(code_lines[i]);
                UI::EndBox();
            }
        UI::EndBox();

        // Right sidebar with tabs
        UI::BeginBox(right_sidebar);
            const char* sidebar_tabs[] = { "Outline", "Search", "Git", "Extensions" };
            for (int i = 0; i < 4; ++i)
            {
                UI::BeginBox(sidebar_button);
                UI::InsertText(sidebar_tabs[i]);
                UI::EndBox();
            }
        UI::EndBox();

    UI::EndBox(); // main area

    // Terminal panel at bottom
    UI::BeginBox(terminal_panel);
        UI::InsertText("[C:6A9955]Terminal [C:D4D4D4]$ ./build.sh");
        UI::InsertText("[C:D4D4D4]Compiling source files...");
        UI::InsertText("[C:D4D4D4]Build succeeded.");
    UI::EndBox();

    UI::EndBox(); // root

    UI::EndRoot();

    UI::Draw();
}
void DrawVSCodeMockup()
{
    UI::BeginRoot(GetScreenWidth(), GetScreenHeight(), GetMouseX(), GetMouseY());

    // Root container: full screen, vertical flow: top bar, main area, bottom terminal
    UI::BoxStyle root;
    root.width = {100, UI::Unit::PARENT_PERCENT};
    root.height = {100, UI::Unit::PARENT_PERCENT};
    root.flow.axis = UI::Flow::Axis::VERTICAL;
    root.flow.vertical_alignment = UI::Flow::Alignment::START;
    root.flow.horizontal_alignment = UI::Flow::Alignment::START;
    root.background_color = {30, 30, 30, 255};

    // Top bar: fixed height, horizontal flow, spaced buttons
    UI::BoxStyle top_bar;
    top_bar.height = {40, UI::Unit::PIXEL};
    top_bar.width = {100, UI::Unit::PARENT_PERCENT};
    top_bar.background_color = {40, 40, 40, 255};
    top_bar.flow.axis = UI::Flow::Axis::HORIZONTAL;
    top_bar.flow.horizontal_alignment = UI::Flow::Alignment::START;
    top_bar.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
    top_bar.padding = {5, 5, 0, 0};
    top_bar.gap_column = {12, UI::Unit::PIXEL};

    // Main area below top bar: horizontal flow - left sidebar, editor, right sidebar
    UI::BoxStyle main_area;
    main_area.width = {100, UI::Unit::PARENT_PERCENT};
    main_area.height = {100, UI::Unit::AVAILABLE_PERCENT};
    main_area.flow.axis = UI::Flow::Axis::HORIZONTAL;
    main_area.flow.horizontal_alignment = UI::Flow::Alignment::START;
    main_area.flow.vertical_alignment = UI::Flow::Alignment::START;
    main_area.gap_column = {10, UI::Unit::PIXEL};

    // Left sidebar: file explorer, vertical flow
    UI::BoxStyle left_sidebar;
    left_sidebar.width = {20, UI::Unit::PARENT_PERCENT};
    left_sidebar.height = {100, UI::Unit::PARENT_PERCENT};
    left_sidebar.background_color = {40, 40, 40, 255};
    left_sidebar.flow.axis = UI::Flow::Axis::VERTICAL;
    left_sidebar.flow.vertical_alignment = UI::Flow::Alignment::START;
    left_sidebar.flow.horizontal_alignment = UI::Flow::Alignment::START;
    left_sidebar.padding = {12, 12, 12, 12};
    left_sidebar.gap_row = {6, UI::Unit::PIXEL};

    // Editor area: vertical flow, large portion of width
    UI::BoxStyle editor_area;
    editor_area.width = {60, UI::Unit::PARENT_PERCENT};
    editor_area.height = {100, UI::Unit::PARENT_PERCENT};
    editor_area.background_color = {25, 25, 25, 255};
    editor_area.flow.axis = UI::Flow::Axis::VERTICAL;
    editor_area.flow.vertical_alignment = UI::Flow::Alignment::START;
    editor_area.flow.horizontal_alignment = UI::Flow::Alignment::START;
    editor_area.padding = {16, 16, 16, 16};
    editor_area.gap_row = {4, UI::Unit::PIXEL};
    editor_area.corner_radius = 6;

    // Right sidebar: tabs like Outline/Search, vertical flow
    UI::BoxStyle right_sidebar;
    right_sidebar.width = {20, UI::Unit::PARENT_PERCENT};
    right_sidebar.height = {100, UI::Unit::PARENT_PERCENT};
    right_sidebar.background_color = {40, 40, 40, 255};
    right_sidebar.flow.axis = UI::Flow::Axis::VERTICAL;
    right_sidebar.flow.vertical_alignment = UI::Flow::Alignment::START;
    right_sidebar.flow.horizontal_alignment = UI::Flow::Alignment::START;
    right_sidebar.padding = {12, 12, 12, 12};
    right_sidebar.gap_row = {6, UI::Unit::PIXEL};

    // Bottom terminal panel: fixed height, horizontal flow
    UI::BoxStyle terminal_panel;
    terminal_panel.height = {150, UI::Unit::PIXEL};
    terminal_panel.width = {100, UI::Unit::PARENT_PERCENT};
    terminal_panel.background_color = {20, 20, 20, 255};
    terminal_panel.flow.axis = UI::Flow::Axis::VERTICAL;
    terminal_panel.flow.vertical_alignment = UI::Flow::Alignment::START;
    terminal_panel.flow.horizontal_alignment = UI::Flow::Alignment::START;
    terminal_panel.padding = {10, 10, 10, 10};
    terminal_panel.corner_radius = 6;

    // Style for buttons in top bar and right sidebar tabs
    UI::BoxStyle button_style;
    button_style.width = {80, UI::Unit::PIXEL};
    button_style.height = {28, UI::Unit::PIXEL};
    button_style.background_color = {60, 60, 60, 255};
    button_style.corner_radius = 4;
    button_style.flow.axis = UI::Flow::Axis::VERTICAL;
    button_style.flow.horizontal_alignment = UI::Flow::Alignment::CENTERED;
    button_style.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
    button_style.padding = {6, 6, 4, 4};

    // Style for file tree entries
    UI::BoxStyle file_entry;
    file_entry.height = {22, UI::Unit::PIXEL};
    file_entry.width = {100, UI::Unit::PARENT_PERCENT};
    file_entry.flow.axis = UI::Flow::Axis::HORIZONTAL;
    file_entry.flow.horizontal_alignment = UI::Flow::Alignment::START;
    file_entry.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
    file_entry.padding = {2, 2, 0, 0};
    file_entry.background_color = {50, 50, 50, 255};
    file_entry.corner_radius = 3;

    // Style for code lines
    UI::BoxStyle code_line;
    code_line.height = {20, UI::Unit::PIXEL};
    code_line.width = {100, UI::Unit::PARENT_PERCENT};
    code_line.flow.axis = UI::Flow::Axis::HORIZONTAL;
    code_line.flow.horizontal_alignment = UI::Flow::Alignment::START;
    code_line.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
    code_line.padding = {0, 0, 0, 0};

    // Styles for syntax highlighting - simulated by InsertText tags with color
    // Use hex color codes in InsertText, like "[C:xxxxxx]"
    // We'll fake colors for keywords, strings, comments, identifiers, etc.

    // Begin Root UI
    UI::BeginBox(root);

    // Top bar with buttons
    UI::BeginBox(top_bar);
        UI::BeginBox(button_style); UI::InsertText("[C:ffffff]File"); UI::EndBox();
        UI::BeginBox(button_style); UI::InsertText("[C:ffffff]Edit"); UI::EndBox();
        UI::BeginBox(button_style); UI::InsertText("[C:ffffff]Selection"); UI::EndBox();
        UI::BeginBox(button_style); UI::InsertText("[C:ffffff]View"); UI::EndBox();
        UI::BeginBox(button_style); UI::InsertText("[C:ffffff]Go"); UI::EndBox();
        UI::BeginBox(button_style); UI::InsertText("[C:ffffff]Run"); UI::EndBox();
        UI::BeginBox(button_style); UI::InsertText("[C:ffffff]Terminal"); UI::EndBox();
    UI::EndBox(); // End top_bar

    // Main horizontal area: left sidebar, editor, right sidebar
    UI::BeginBox(main_area);

        // Left sidebar: file explorer tree
        UI::BeginBox(left_sidebar);

            // Folder "src"
            UI::BeginBox(file_entry); UI::InsertText("[C:6699cc]📁 src"); UI::EndBox();

            // File entries inside src, indented by spaces
            UI::BeginBox(file_entry); UI::InsertText("  [C:ffffff]main.cpp"); UI::EndBox();
            UI::BeginBox(file_entry); UI::InsertText("  [C:ffffff]app.cpp"); UI::EndBox();
            UI::BeginBox(file_entry); UI::InsertText("  [C:ffffff]ui.cpp"); UI::EndBox();

            // Folder "include"
            UI::BeginBox(file_entry); UI::InsertText("[C:6699cc]📁 include"); UI::EndBox();

            // File entries inside include
            UI::BeginBox(file_entry); UI::InsertText("  [C:ffffff]ui.h"); UI::EndBox();
            UI::BeginBox(file_entry); UI::InsertText("  [C:ffffff]app.h"); UI::EndBox();

            // File at root
            UI::BeginBox(file_entry); UI::InsertText("[C:ffffff]README.md"); UI::EndBox();

        UI::EndBox(); // End left_sidebar

        // Editor area with fake code lines and syntax highlighting
        UI::BeginBox(editor_area);

            // Fake code lines with basic coloring
            const char* code_lines[] = {
                "[C:569CD6]int [C:D4D4D4]main[C:D4D4D4]() {",
                "    [C:569CD6]std[C:D4D4D4]::[C:569CD6]cout [C:D4D4D4]<< [C:CE9178]\"Hello, World!\"[C:D4D4D4] << [C:569CD6]std[C:D4D4D4]::[C:569CD6]endl;",
                "    [C:569CD6]return [C:D4D4D4]0;",
                "}"
            };
            for (int i = 0; i < 4; ++i)
            {
                UI::BeginBox(code_line);
                UI::InsertText(code_lines[i]);
                UI::EndBox();
            }

        UI::EndBox(); // End editor_area

        // Right sidebar: tabs (Outline, Search)
        UI::BeginBox(right_sidebar);
            UI::BeginBox(button_style); UI::InsertText("[C:ffffff]Outline"); UI::EndBox();
            UI::BeginBox(button_style); UI::InsertText("[C:ffffff]Search"); UI::EndBox();
            UI::BeginBox(button_style); UI::InsertText("[C:ffffff]Git"); UI::EndBox();
            UI::BeginBox(button_style); UI::InsertText("[C:ffffff]Extensions"); UI::EndBox();
        UI::EndBox(); // End right_sidebar

    UI::EndBox(); // End main_area

    // Terminal panel at bottom
    UI::BeginBox(terminal_panel);
        UI::InsertText("[C:6A9955]Terminal [C:D4D4D4]$ ./build.sh");
        UI::InsertText("[C:D4D4D4]Compiling source files...");
        UI::InsertText("[C:D4D4D4]Build succeeded.");
    UI::EndBox();

    UI::EndBox(); // End root

    UI::EndRoot();

    UI::Draw();
}


void GPT_spotify2()
{
UI::BeginRoot(GetScreenWidth(), GetScreenHeight(), GetMouseX(), GetMouseY());

    UI::BoxStyle root;
    root.width = {100, UI::Unit::PARENT_PERCENT};
    root.height = {100, UI::Unit::PARENT_PERCENT};
    root.flow.axis = UI::Flow::Axis::VERTICAL;
    root.flow.vertical_alignment = UI::Flow::Alignment::START;
    root.flow.horizontal_alignment = UI::Flow::Alignment::START;

    UI::BoxStyle top_bar;
    top_bar.height = {60, UI::Unit::PIXEL};
    top_bar.width = {100, UI::Unit::PARENT_PERCENT};
    top_bar.background_color = {18, 18, 18, 255};
    top_bar.flow.axis = UI::Flow::Axis::HORIZONTAL;
    top_bar.flow.horizontal_alignment = UI::Flow::Alignment::SPACE_BETWEEN;
    top_bar.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
    top_bar.padding = {10, 20, 10, 20};

    UI::BoxStyle main_area;
    main_area.width = {100, UI::Unit::PARENT_PERCENT};
    main_area.height = {100, UI::Unit::AVAILABLE_PERCENT};
    main_area.flow.axis = UI::Flow::Axis::HORIZONTAL;
    main_area.flow.horizontal_alignment = UI::Flow::Alignment::START;
    main_area.flow.vertical_alignment = UI::Flow::Alignment::START;

    UI::BoxStyle sidebar;
    sidebar.width = {18, UI::Unit::PARENT_PERCENT};
    sidebar.height = {100, UI::Unit::PARENT_PERCENT};
    sidebar.background_color = {18, 18, 18, 255};
    sidebar.flow.axis = UI::Flow::Axis::VERTICAL;
    sidebar.flow.vertical_alignment = UI::Flow::Alignment::START;
    sidebar.flow.horizontal_alignment = UI::Flow::Alignment::START;
    sidebar.padding = {20, 16, 20, 16};
    sidebar.gap_row = {10, UI::Unit::PIXEL};

    UI::BoxStyle content;
    content.width = {100, UI::Unit::AVAILABLE_PERCENT};
    content.height = {100, UI::Unit::PARENT_PERCENT};
    content.background_color = {24, 24, 24, 255};
    content.flow.axis = UI::Flow::Axis::VERTICAL;
    content.flow.vertical_alignment = UI::Flow::Alignment::START;
    content.flow.horizontal_alignment = UI::Flow::Alignment::START;
    content.padding = {24, 24, 24, 24};
    content.gap_row = {16, UI::Unit::PIXEL};

    UI::BeginBox(root);
        UI::BeginBox(top_bar);
            UI::InsertText("←   →");
            UI::InsertText("[S:32]User ▾");
        UI::EndBox();

        UI::BeginBox(main_area);
            UI::BeginBox(sidebar);
                UI::InsertText("[S:30]Home");
                UI::InsertText("[S:30]Search");
                UI::InsertText("[S:30]Your Library");
                UI::InsertText(" ");
                UI::InsertText("[S:28]Create Playlist");
                UI::InsertText("[S:28]Liked Songs");
                UI::InsertText("[S:28]Albums");
            UI::EndBox();
            UI::BeginBox(content);
                UI::InsertText("[S:32]Good Morning");

                // Playlist Row
                UI::BoxStyle playlist_row;
                playlist_row.width = {100, UI::Unit::PARENT_PERCENT};
                playlist_row.height = {120, UI::Unit::PIXEL};
                playlist_row.flow.axis = UI::Flow::Axis::HORIZONTAL;
                playlist_row.flow.horizontal_alignment = UI::Flow::Alignment::START;
                playlist_row.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
                playlist_row.gap_column = {16, UI::Unit::PIXEL};

                UI::BoxStyle playlist_card;
                playlist_card.width = {160, UI::Unit::PIXEL};
                playlist_card.height = {100, UI::Unit::PIXEL};
                playlist_card.background_color = {40, 40, 40, 255};
                playlist_card.corner_radius = 6;
                playlist_card.padding = {10, 10, 10, 10};
                playlist_card.flow.axis = UI::Flow::Axis::HORIZONTAL;
                playlist_card.flow.horizontal_alignment = UI::Flow::Alignment::START;
                playlist_card.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
                playlist_card.gap_column = {10, UI::Unit::PIXEL};

                UI::BoxStyle cover_square;
                cover_square.width = {80, UI::Unit::PIXEL};
                cover_square.height = {80, UI::Unit::PIXEL};
                cover_square.background_color = {90, 200, 120, 255};
                cover_square.corner_radius = 4;

                UI::BoxStyle song_column;
                song_column.width = {100, UI::Unit::AVAILABLE_PERCENT};
                song_column.height = {100, UI::Unit::PARENT_PERCENT};
                song_column.flow.axis = UI::Flow::Axis::VERTICAL;
                song_column.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
                song_column.flow.horizontal_alignment = UI::Flow::Alignment::START;

                UI::BeginBox(playlist_row);
                    for (int i = 0; i < 5; ++i) {
                        UI::BeginBox(playlist_card);
                            UI::BeginBox(cover_square); UI::EndBox();
                            UI::BeginBox(song_column);
                                UI::InsertText("Daily Mix");
                                UI::InsertText("[S:22]Various Artists");
                            UI::EndBox();
                        UI::EndBox();
                    }
                UI::EndBox();
                // Tracklist Section
                UI::InsertText("[S:22]Recently Played");

                UI::BoxStyle tracklist;
                tracklist.width = {100, UI::Unit::PARENT_PERCENT};
                tracklist.height = {100, UI::Unit::AVAILABLE_PERCENT};
                tracklist.flow.axis = UI::Flow::Axis::VERTICAL;
                tracklist.flow.vertical_alignment = UI::Flow::Alignment::START;
                tracklist.flow.horizontal_alignment = UI::Flow::Alignment::START;
                tracklist.gap_row = {10, UI::Unit::PIXEL};

                UI::BoxStyle track_entry;
                track_entry.width = {100, UI::Unit::PARENT_PERCENT};
                track_entry.height = {60, UI::Unit::PIXEL};
                track_entry.background_color = {30, 30, 30, 255};
                track_entry.corner_radius = 6;
                track_entry.padding = {8, 12, 8, 12};
                track_entry.flow.axis = UI::Flow::Axis::HORIZONTAL;
                track_entry.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
                track_entry.flow.horizontal_alignment = UI::Flow::Alignment::START;
                track_entry.gap_column = {12, UI::Unit::PIXEL};

                UI::BoxStyle thumbnail;
                thumbnail.width = {44, UI::Unit::PIXEL};
                thumbnail.height = {44, UI::Unit::PIXEL};
                thumbnail.corner_radius = 2;
                thumbnail.background_color = {180, 90, 200, 255}; // Example color

                UI::BoxStyle track_text;
                track_text.width = {100, UI::Unit::AVAILABLE_PERCENT};
                track_text.height = {100, UI::Unit::PARENT_PERCENT};
                track_text.flow.axis = UI::Flow::Axis::VERTICAL;
                track_text.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
                track_text.flow.horizontal_alignment = UI::Flow::Alignment::START;

                UI::BeginBox(tracklist);
                    for (int i = 0; i < 5; ++i) {
                        UI::BeginBox(track_entry);
                            UI::BeginBox(thumbnail); UI::EndBox();
                            UI::BeginBox(track_text);
                                UI::InsertText(TextFormat("Track %d", i + 1));
                                UI::InsertText("[S:24]Artist Name");
                            UI::EndBox();
                        UI::EndBox();
                    }
                UI::EndBox();

            UI::EndBox(); // End content
        UI::EndBox(); // End main_area
        // Playback Bar
        UI::BoxStyle playback;
        playback.width = {100, UI::Unit::PARENT_PERCENT};
        playback.height = {80, UI::Unit::PIXEL};
        playback.background_color = {25, 25, 25, 255};
        playback.flow.axis = UI::Flow::Axis::HORIZONTAL;
        playback.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
        playback.flow.horizontal_alignment = UI::Flow::Alignment::SPACE_BETWEEN;
        playback.padding = {10, 20, 10, 20};

        UI::BeginBox(playback);
            UI::InsertText("[S:24]Now Playing: Fake Track - Fake Artist");
            UI::InsertText(">  ||  >>");
        UI::EndBox();

    UI::EndBox(); // End root

    UI::EndRoot();

    UI::Draw();
}

void GPT_spotify1()
{
        UI::BoxStyle root;
root.width = {100, UI::Unit::PARENT_PERCENT};
root.height = {100, UI::Unit::PARENT_PERCENT};
root.flow.axis = UI::Flow::Axis::VERTICAL;
root.flow.vertical_alignment = UI::Flow::Alignment::START;
root.flow.horizontal_alignment = UI::Flow::Alignment::START;

UI::BoxStyle top_bar;
top_bar.height = {60, UI::Unit::PIXEL};
top_bar.width = {100, UI::Unit::PARENT_PERCENT};
top_bar.background_color = {18, 18, 18, 255};
top_bar.flow.axis = UI::Flow::Axis::HORIZONTAL;
top_bar.flow.horizontal_alignment = UI::Flow::Alignment::SPACE_BETWEEN;
top_bar.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
top_bar.padding = {10, 20, 10, 20};

UI::BoxStyle main_section;
main_section.width = {100, UI::Unit::PARENT_PERCENT};
main_section.height = {100, UI::Unit::AVAILABLE_PERCENT};
main_section.flow.axis = UI::Flow::Axis::HORIZONTAL;

UI::BoxStyle sidebar;
sidebar.width = {18, UI::Unit::PARENT_PERCENT};
sidebar.height = {100, UI::Unit::PARENT_PERCENT};
sidebar.background_color = {0, 0, 0, 255};
sidebar.flow.axis = UI::Flow::Axis::VERTICAL;
sidebar.padding = {20, 20, 20, 20};
sidebar.gap_row = {16, UI::Unit::PIXEL};

UI::BoxStyle content;
content.width = {100, UI::Unit::AVAILABLE_PERCENT};
content.height = {100, UI::Unit::PARENT_PERCENT};
content.background_color = {18, 18, 18, 255};
content.flow.axis = UI::Flow::Axis::VERTICAL;
content.padding = {20, 20, 20, 20};
content.gap_row = {24, UI::Unit::PIXEL};

UI::BoxStyle playlist_header;
playlist_header.width = {100, UI::Unit::PARENT_PERCENT};
playlist_header.height = {120, UI::Unit::PIXEL};
playlist_header.background_color = {40, 40, 40, 255};
playlist_header.corner_radius = 4;
playlist_header.flow.axis = UI::Flow::Axis::HORIZONTAL;
playlist_header.padding = {20, 20, 20, 20};
playlist_header.gap_column = {20, UI::Unit::PIXEL};

UI::BoxStyle image_box;
image_box.width = {100, UI::Unit::PIXEL};
image_box.height = {100, UI::Unit::PIXEL};
image_box.background_color = {100, 100, 100, 255};
image_box.corner_radius = 4;

UI::BoxStyle text_column;
text_column.width = {100, UI::Unit::AVAILABLE_PERCENT};
text_column.height = {100, UI::Unit::PARENT_PERCENT};
text_column.flow.axis = UI::Flow::Axis::VERTICAL;
text_column.gap_row = {4, UI::Unit::PIXEL};

UI::BoxStyle song_entry;
song_entry.width = {100, UI::Unit::PARENT_PERCENT};
song_entry.height = {50, UI::Unit::PIXEL};
song_entry.background_color = {35, 35, 35, 255};
song_entry.corner_radius = 4;
song_entry.flow.axis = UI::Flow::Axis::HORIZONTAL;
song_entry.padding = {12, 12, 12, 12};
song_entry.gap_column = {10, UI::Unit::PIXEL};

UI::BoxStyle footer;
footer.height = {60, UI::Unit::PIXEL};
footer.width = {100, UI::Unit::PARENT_PERCENT};
footer.background_color = {24, 24, 24, 255};
footer.flow.axis = UI::Flow::Axis::HORIZONTAL;
footer.padding = {10, 20, 10, 20};
footer.flow.horizontal_alignment = UI::Flow::Alignment::SPACE_BETWEEN;
footer.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;

UI::BeginRoot(GetScreenWidth(), GetScreenHeight(), GetMouseX(), GetMouseY());

UI::BeginBox(root);

    UI::BeginBox(top_bar);
        UI::InsertText("←   →");
        UI::InsertText("🔍 Search     👤 User ▾");
    UI::EndBox();

    UI::BeginBox(main_section);

        // Sidebar
        UI::BeginBox(sidebar);
            UI::InsertText("Home");
            UI::InsertText("Search");
            UI::InsertText("Your Library");
            UI::InsertText("Create Playlist");
            UI::InsertText("Liked Songs");
        UI::EndBox();

        // Content
        UI::BeginBox(content);

            // Playlist Header
            UI::BeginBox(playlist_header);
                UI::BeginBox(image_box);
                UI::EndBox();

                UI::BeginBox(text_column);
                    UI::InsertText("[S:20]Your Daily Mix");
                    UI::InsertText("Curated just for you");
                UI::EndBox();
            UI::EndBox();

            // Song Entries
            for (int i = 0; i < 5; i++) {
                UI::BeginBox(song_entry);
                    UI::InsertText(TextFormat("Track %d", i + 1));
                    UI::InsertText("Artist");
                UI::EndBox();
            }

        UI::EndBox();
    UI::EndBox();

    // Footer
    UI::BeginBox(footer);
        UI::InsertText("? Track Name - Artist  ?");
        UI::InsertText("? 01:23 / 03:45 ?");
    UI::EndBox();

UI::EndBox();
UI::EndRoot();
UI::Draw();
}

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetExitKey(0);
    //SetTargetFPS();

    UI::Init_impl();

    UI::BoxStyle base;
    base.padding = {10, 10, 10, 10};
    base.background_color = {40, 40, 50, 255};
    base.flow.horizontal_alignment = UI::Flow::Alignment::SPACE_AROUND;
    base.flow.vertical_alignment = UI::Flow::Alignment::CENTERED;
    base.width = UI::Unit{100, UI::Unit::PARENT_PERCENT};
    base.height = UI::Unit{100, UI::Unit::PARENT_PERCENT};

    UI::BoxStyle h_container;
    h_container.flow.axis = UI::Flow::Axis::VERTICAL;
    //h_container.padding = {10, 10, 10, 10};
    h_container.background_color = {80, 80, 100, 255};
    h_container.flow.horizontal_alignment = UI::Flow::Alignment::END;
    h_container.flow.vertical_alignment = UI::Flow::Alignment::START;
    h_container.width = UI::Unit{100, UI::Unit::PARENT_PERCENT};
    h_container.height = UI::Unit{100, UI::Unit::PARENT_PERCENT};
    h_container.scissor = true;

    UI::BoxStyle button;
    button.padding = {10, 10, 10, 10};
    button.background_color = {90, 90, 90, 255};
    button.width = UI::Unit{100, UI::Unit::PIXEL};
    button.height = UI::Unit{100, UI::Unit::PIXEL};
    button.corner_radius = 10;
    button.border_color = {0, 0, 0, 255};
    button.border_width = 2;

    float time = 0;
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        time += GetFrameTime() * 20;
        BeginDrawing();
        ClearBackground(Color{0, 0, 0, 255});

        h_container.scroll_y -= GetMouseWheelMove() * 20;
        UI::BoxInfo scroll_info = UI::GetBoxInfo("Scroll Container");
        if(scroll_info.valid)
        {
            h_container.scroll_y = UI::Clamp(h_container.scroll_y, 0, scroll_info.MaxScrollY());
        }

        for(int i = 0; i< 4; i++)
        {
            UI::BoxInfo info = UI::GetBoxInfo(TextFormat("Button%d", i + 1));
            if(info.valid)
            {
                if(info.is_direct_hover && IsMouseButtonPressed(0))
                    std::cout<<"Button"<<i+1<<" Pressed\n";
            }
        }

        UI::BeginRoot(GetScreenWidth(), GetScreenHeight(), GetMouseX(), GetMouseY());
            UI::BeginBox(base);
                UI::BeginBox(h_container, "Scroll Container");
                    UI::BeginBox(button, "Button1");
                        UI::InsertText("b1");
                    UI::EndBox(); //end button1
                    UI::BeginBox(button, "Button2");
                        UI::InsertText("b2");
                    UI::EndBox();
                    UI::BeginBox(button, "Button3");
                        UI::InsertText("b3");
                    UI::EndBox(); //end button1
                    UI::BeginBox(button, "Button4");
                        UI::InsertText("b4");
                    UI::EndBox();
                    UI::BeginBox(button, "Button4");
                        UI::InsertText("b5");
                    UI::EndBox();
                    UI::BeginBox(button);
                        UI::InsertText("b6");
                    UI::EndBox();
                UI::EndBox();
            UI::EndBox();
        UI::EndRoot();

        UI::Draw();
        //DrawText(TextFormat("%i", GetFPS()), GetScreenWidth() - 80, 10, 30, GRAY);
        EndDrawing();
    }

    CloseWindow();        // Close window and OpenGL context

    return 0;
}