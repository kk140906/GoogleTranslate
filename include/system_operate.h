//
// Created by KK on 2020/12/15.
//

#ifndef GOOGLETRANSLATE_SYSTEM_OPERATE_H
#define GOOGLETRANSLATE_SYSTEM_OPERATE_H

#include "windows.h"
#include <QMessageBox>
#include <QRegularExpression>

namespace system_operate {

    /**
     * @brief simulate windows copy operate : ctrl + c
     */
    void simulate_copy_();

    /**
     * @brief from clipboard get data
     */
    QString get_copied_text_();

    /**
     * @brief clear clipboard and close it
     */
    void empty_clipboard_();

    /**
     * @brief set hotkey in windows operator system
     */
    void set_hotkey_();


}


#endif //GOOGLETRANSLATE_SYSTEM_OPERATE_H
