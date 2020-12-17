#ifndef APP_H
#define APP_H

#include <QApplication>
#include <QLocalSocket>
#include <QLocalServer>
#include <memory>

class App : public QApplication
{
public:
    explicit App(int argc,char **argv);
    bool IsRunning() const {return running_;}
private slots:
//    void new_connect_();
private:
    void init_();
    std::shared_ptr<QLocalServer> server_{nullptr};
    bool running_{false};
    const qint16 timeout_{500};
    const QString app_name_{"GoogleTranslate"};
};
#endif // APP_H
