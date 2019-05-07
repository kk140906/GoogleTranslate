#ifndef NEWQAPPLICATION_H
#define NEWQAPPLICATION_H

#include <QApplication>
#include <QLocalSocket>
#include <QLocalServer>
#include <QMessageBox>
#include <QDesktopWidget>

class NewQApplication : public QApplication
{
    Q_OBJECT
public:
    explicit NewQApplication(int argc,char **argv);
    ~NewQApplication();
    bool isRunning();
private slots:
    void newLocalConnect();
private:
    QLocalServer *server;
    bool _isRunning;
};
#endif // NEWQAPPLICATION_H
