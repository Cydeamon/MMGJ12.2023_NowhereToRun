#include <iostream>
#include "Game/Game.h"

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
        Game* game = new Game();
        game->Run();
        delete game;
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
