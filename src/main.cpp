#include <QDebug>
#include <QMessageBox>

#include <mainwindow.h>
#include <application.h>
#include <system_operate.h>
#include <logger.h>

//std::shared_ptr<spdlog::logger> Log::logger = spdlog::rotating_logger_st("rotate_logger", "logs", 1024 * 1024 * 10, 5);

std::shared_ptr<spdlog::logger> Log::logger{nullptr};

int main(int argc, char *argv[]) {
    Log::Init();
    //Set F6 as a hotkey
    RegisterHotKey(nullptr, 1, 0, VK_F6);
    App a(argc, argv);
    if (a.IsRunning()) {
        const char* msg = "application is running!";
        Log::logger->warn(msg);
        QMessageBox::warning(nullptr, "GoogleTranslate", msg);
        exit(0);
    }
    Log::logger->info("application start");
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
