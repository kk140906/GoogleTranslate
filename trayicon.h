#ifndef TRAYICON_H
#define TRAYICON_H
#include <QWidget>
#include <QSystemTrayIcon>
#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <mainwindow.h>
#include <googletranslateweb.h>
class TrayIcon : public MainWindow
{
    Q_OBJECT
public:
    explicit TrayIcon(QWidget *parent = 0);
    ~TrayIcon();
    void CreateMenu();
public slots:
    void TrayIconAction(QSystemTrayIcon::ActivationReason);
    void ExitProgram();
private:

    QSystemTrayIcon *systrayicon;
//    If initialized here, the system tray icon will be deleted very slowly
//    QSystemTrayIcon *systrayicon= new QSystemTrayIcon(this);
    QMenu *trayiconmenu = new QMenu(this);
    QAction *action_exit = new QAction(trayiconmenu);
    QAction *action_showmainwindow = new QAction(trayiconmenu);
};

#endif // TRAYICON_H
