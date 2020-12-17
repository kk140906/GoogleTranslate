#ifndef WEB_REQUESTS_H
#define WEB_REQUESTS_H

#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QHostInfo>
#include <QByteArray>
#include <QMap>
#include <QJsonArray>
#include <QTimer>

#include <memory>
#include <functional>

class Translate : public QObject {
Q_OBJECT
public:
    Translate();
    void SetText(QString &text);
    void Run();
//    ~Translate() override;
signals:
    void ReplyFinished(std::shared_ptr<QJsonArray>);
private:
    bool is_to_zh_CN_();

    enum LANG {
        EN, ZH_CN
    };

    QString gen_extra_url_(LANG sl, LANG tl);


    QNetworkReply *requests_();

    QString text_;
    QUrl base_url_{"https://translate.googleapis.com/translate_a/single"};
    const QMap<LANG, QString> lang_map_{{EN,    "en"},
                                        {ZH_CN, "zh-CN"}};

    QNetworkAccessManager *manager_;
    QNetworkReply *reply_{nullptr};
    std::shared_ptr<QJsonArray> result_{nullptr};
    QTimer request_timer_;
    bool request_ok_{true};
    bool network_ok_{true};

private slots:

    void network_changed_();

    static void request_timeout_();

    void reply_finished_();
};

#endif // WEB_REQUESTS_H
