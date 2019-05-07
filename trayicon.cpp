#include <trayicon.h>

//    If initialized here, the system tray icon will be deleted very slowly
//    QSystemTrayIcon *systrayicon= new QSystemTrayIcon(this);


TrayIcon::TrayIcon(QWidget *parent) :
    MainWindow(parent)
{
    systrayicon= new


    CreateMenu();

}
TrayIcon::~TrayIcon() {systrayicon->deleteLater();}

