/**
 * Created by KK on 2020/12/16.
 */

#include <tray_icon.h>

TrayIcon::TrayIcon(QWidget *parent) :
        parent_(parent),
        system_tray_icon_(new QSystemTrayIcon(parent)),
        tray_icon_menu_(new QMenu(parent)) {
    system_tray_icon_->setIcon(QIcon(":/Qresource/Google_Translate_Icon.png"));
    system_tray_icon_->show();
    system_tray_icon_->setContextMenu(tray_icon_menu_.get());
    tray_icon_menu_->setStyleSheet(QLatin1String("background-color: rgb(255, 255, 255);\n"
                                                 "selection-background-color: rgb(188, 188, 188);"));
//    func_factory_ = {
//            {Settings,         [this] { settings(); }},
//            {Exit_Application, [this] { exit_program(); }}
//    };
    QObject::connect(system_tray_icon_.get(),
                     SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                     this,
                     SLOT(tray_icon_action(QSystemTrayIcon::ActivationReason)));
}

void TrayIcon::tray_icon_action(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            show_window_();
            break;
        case QSystemTrayIcon::Context:
            tray_icon_menu_->show();
            break;
        default:
            break;
    }
}

void TrayIcon::AddAction(const QString &text, const QIcon &icon, Func func, bool visible) {
    auto action = new QAction(tray_icon_menu_.get());
    action->setText(text);
    action->setIcon(icon);
    action->setIconVisibleInMenu(visible);
    tray_icon_menu_->addAction(action);
    actions_.push_back(action);
    QObject::connect(action, &QAction::triggered, this, TrayIcon::func_factory_[func]);
}



void TrayIcon::show_window_() {
    parent_->show();
    parent_->activateWindow();
}

void TrayIcon::exit_program() {
    system_tray_icon_->hide();
    exit(EXIT_SUCCESS);
}

void TrayIcon::settings() {
    show_window_();
}


