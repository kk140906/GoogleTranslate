//
// Created by KK on 2020/12/15.
//

#include "system_operate.h"

namespace system_operate {

    void simulate_copy_() {
        keybd_event(VK_CONTROL, 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
        keybd_event('C', 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
        keybd_event('C', 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
        keybd_event(VK_CONTROL, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
    }

    QString get_copied_text_() {
        empty_clipboard_();
        simulate_copy_();
        // waiting for the copy completed
        Sleep(20);
        if (!IsClipboardFormatAvailable(CF_TEXT)) {
            QMessageBox::warning(nullptr,
                                 "GoogleTranslate",
                                 "Can't get text!\n"
                                 "Please don't translate the contents of this Application,\n"
                                 "And try to keep the selected contents on the Top-Window!");
            return QString();
        }

        if (!OpenClipboard(nullptr))
            return QString();

        auto clipboard_handle = GetClipboardData(CF_TEXT);
        QString result;
        if (clipboard_handle) {
            auto text = static_cast<LPSTR>(GlobalLock(clipboard_handle));
            if (text) {
                result = QString::fromLocal8Bit(text)
                        .replace(QRegularExpression("(?<=[\\w\\s\\-,])\r\n"), " ");
                GlobalUnlock(clipboard_handle);
            }
        }
        EmptyClipboard();
        CloseClipboard();
        return result;
    }

    void empty_clipboard_() {
        if (!OpenClipboard(nullptr)) return;
        EmptyClipboard();
        CloseClipboard();
    }
}
