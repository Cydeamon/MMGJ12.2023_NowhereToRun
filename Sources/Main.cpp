#include <iostream>
#include "CMakeConfig.h"
#include "Engine/Engine.h"

#ifdef _WIN32
    #define IS_WINDOWS 1
    #include <windows.h>
#elif __unix__
    #define IS_LINUX 1
    #include <gtk/gtk.h>
#endif

int main()
{
    try
    {
        Cygine::Engine *engine = new Cygine::Engine();

        while (!engine->ShouldClose())
        {
            engine->Update();

            // Check arrow keys
            if (Cygine::Input::IsJustPressed(Cygine::KeyboardKey::ARROW_LEFT))
                std::cout << "Arrow Left just pressed" << std::endl;
            if (Cygine::Input::IsJustPressed(Cygine::KeyboardKey::ARROW_RIGHT))
                std::cout << "Arrow Right just pressed" << std::endl;
            if (Cygine::Input::IsJustPressed(Cygine::KeyboardKey::ARROW_UP))
                std::cout << "Arrow Up just pressed" << std::endl;
            if (Cygine::Input::IsJustPressed(Cygine::KeyboardKey::ARROW_DOWN))
                std::cout << "Arrow Down just pressed" << std::endl;

            if (Cygine::Input::IsJustReleased(Cygine::KeyboardKey::ARROW_LEFT))
                std::cout << "Arrow Left just released" << std::endl;
            if (Cygine::Input::IsJustReleased(Cygine::KeyboardKey::ARROW_RIGHT))
                std::cout << "Arrow Right just released" << std::endl;
            if (Cygine::Input::IsJustReleased(Cygine::KeyboardKey::ARROW_UP))
                std::cout << "Arrow Up just released" << std::endl;
            if (Cygine::Input::IsJustReleased(Cygine::KeyboardKey::ARROW_DOWN))
                std::cout << "Arrow Down just released" << std::endl;

            if (Cygine::Input::IsJustPressed(Cygine::LEFT))
            {
                Cygine::Vector2 pos = Cygine::Input::GetMousePosition();
                std::cout << "Left mouse click at " << pos.x << ", " << pos.y << std::endl;
            }

            // Draw
            engine->BeginFrameDraw();
            engine->EndFrameDraw();
        }

        delete engine;
    }
    catch (const std::exception &e)
    {
        #if(IS_WINDOWS)
            MessageBoxA(NULL, e.what(), "Application Error", MB_OK | MB_ICONERROR);
        #elif (IS_LINUX)
            GtkWidget *dialog = gtk_message_dialog_new(
                NULL,
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                e.what()
            );

            gtk_window_set_title(GTK_WINDOW(dialog), "Application Error");
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        #endif

        std::cerr << e.what() << '\n';
        return 1;
    }

    return 0;
}
