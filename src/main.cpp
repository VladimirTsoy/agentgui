#include <QApplication>

#include "mainform.h"
#include "frameless.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/file.h>
#include <errno.h>
#endif

int main(int argc, char* argv[])
{
    qDebug() << "***** Start *******";
#ifdef _WIN32
    // ensure only one running instance
    HANDLE hMutexHandle = CreateMutexA(NULL, TRUE, "AgentGUIMutex");
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        printf("Error: Another copy of AthenaAgent is already runnig!\n"); // another instance is running
        return 1;
    }
#else
    int pid_file = open("/tmp/.agentGUI.pid\n", O_CREAT | O_RDWR, 0666);
    int rc = flock(pid_file, LOCK_EX | LOCK_NB);
    if(rc) {
        if(EWOULDBLOCK == errno) {
            printf("Error: Another copy of AgentGUI is already runnig!\n"); // another instance is running
            return -1;
        }
    }
#endif


#ifdef _WIN32
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    HWND h = GetConsoleWindow();
    ShowWindow(h, 0);

    if (argc > 1)
    {
        for(int i = 0; i < argc; i++)
        {
            if (!strcmp(argv[i], "--debug"))
            {
                HWND h = GetConsoleWindow();
                ShowWindow(h, 1);
                std::cout << "** Enable debug console output **" << std::endl;
                continue;
            }
        }
    }
    else
    {
        HWND h = GetConsoleWindow();
        ShowWindow(h, 0);
        FreeConsole();
        std::cout << " hide windows" << std::endl;
    }
    //EnableWindow(h, false);
#endif

    qDebug() << "****** Run application *****";
    QApplication *app;
    app = new QApplication(argc, argv);
    app->setQuitOnLastWindowClosed(false); // чтобы не закрывалось по закрытию aboutWidget
    /// NOTE: вообще, сейчас нет выхода из Агента...

    MainForm* form = new MainForm();
    FrameLess f(form);
    form->show();

#ifdef _WIN32
    ReleaseMutex(hMutexHandle);
    CloseHandle(hMutexHandle);
#endif

    return app->exec();
}
