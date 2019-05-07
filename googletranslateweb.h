#ifndef GOOGLETRANSLATEWEB_H
#define GOOGLETRANSLATEWEB_H

class GoogleTranslateWeb: public QMainWindow
{
    Q_OBJECT

public:
    explicit GoogleTranslateWeb(QWidget *parent = 0);
    ~GoogleTranslateWeb();


private:
    QNetworkAccessManager *managerTKK;
    QNetworkRequest *requestTKK;
    QNetworkReply *replyTKK;

private slots:
    void ReadTkk();
};

#endif // GOOGLETRANSLATEWEB_H
