#include <QDebug>
#include "mainwindow.h"
#include <newqapplication.h>

int main(int argc, char *argv[])
{
    //Set F6 as a hotkey
    RegisterHotKey(nullptr,1,0,VK_F6);
    NewQApplication a(argc,argv);
    QMessageBox msgBox;
    if(a.isRunning())
    {
        msgBox.warning(nullptr,"GoogleTranslate","Program is Running!");
        exit(0);
    }
    MainWindow win;
    NewQApplication::processEvents();
    MSG msg;
    while(GetMessage(&msg,nullptr,0,0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_HOTKEY){
            if (msg.wParam == 1)
            {
                win.ShowWindow();
            }
        }
        else if(msg.message == WM_LBUTTONUP||WM_LBUTTONDOWN||WM_RBUTTONUP||WM_RBUTTONDOWN)
        {
            if (!win.isActiveWindow())
                win.hide();
        }
    }
    return 0;
}
