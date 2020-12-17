#include <QDebug>
#include <QMessageBox>

#include <mainwindow.h>
#include <application.h>
#include <system_operate.h>

int main(int argc, char *argv[]) {
    //Set F6 as a hotkey
    RegisterHotKey(nullptr, 1, 0, VK_F6);
    App a(argc, argv);
    if (a.IsRunning()) {
        QMessageBox::warning(nullptr, "GoogleTranslate", "Program is running!");
        exit(0);
    }
    MainWindow win;
    App::processEvents();
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_HOTKEY) {
            if (msg.wParam == 1) {
                win.UpdateText();
            }
        } else if (msg.message == WM_LBUTTONUP || WM_LBUTTONDOWN || WM_RBUTTONUP || WM_RBUTTONDOWN) {
            if (!win.isActiveWindow())
                win.hide();
        }
    }
    return 0;
}
