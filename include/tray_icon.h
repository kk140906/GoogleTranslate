//
// Created by KK on 2020/12/16.
//

#ifndef GOOGLETRANSLATE_TRAYICON_H
#define GOOGLETRANSLATE_TRAYICON_H

#include <QWidget>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>

#include <memory>
#include <functional>
#include <utility>
#include <vector>

class TrayIcon : QObject {
Q_OBJECT
public:
    explicit TrayIcon(QWidget *parent);

    enum Func {
        Settings, Exit_Application
    };

    void AddAction(const QString &text, const QIcon &icon, Func func, bool visible = true);

private:
    QWidget *parent_;
    std::shared_ptr<QSystemTrayIcon> system_tray_icon_;
    std::shared_ptr<QMenu> tray_icon_menu_;
    std::vector<QAction *> actions_;
    QMap<Func, std::function<void()>> func_factory_{
            {Settings,         [this] { settings(); }},
            {Exit_Application, [this] { exit_program(); }}
    };
private slots:

    void tray_icon_action(QSystemTrayIcon::ActivationReason);

    void show_window_();

    void exit_program();

    void settings();
};


#endif //GOOGLETRANSLATE_TRAYICON_H
