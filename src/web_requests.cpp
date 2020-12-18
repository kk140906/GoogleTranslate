#include <QMessageBox>
#include <QJsonDocument>

#include "web_requests.h"
#include "logger.h"

Translate::Translate() :
        QObject(nullptr),
        manager_(new QNetworkAccessManager(this)) {
    QObject::connect(manager_, SIGNAL(onlineStateChanged()), this, SLOT(network_changed_));
    QObject::connect(&request_timer_, SIGNAL(timeout()), this, SLOT(request_timeout_()));
};

QString Translate::gen_extra_url_(LANG sl, LANG tl) {
    return QString(
            "client=gtx&ie=UTF-8&oe=UTF-8&dt=bd&dt=ex&dt=ld&dt=md&dt=rw&"
            "dt=rm&dt=ss&dt=t&dt=at&dt=qc&sl=%1&tl=%2&hl=zh-CN&q=%3")
            .arg(lang_map_.value(sl))
            .arg(lang_map_.value(tl))
            .arg(text_);
}

bool Translate::is_to_zh_CN_() {
    qint16 zh_counts_{0}, en_counts_{0};
    for (auto &ch : text_) {
        static_cast<int>(ch.unicode()) > 128 ? ++zh_counts_ : ++en_counts_;
    }
    return zh_counts_ < en_counts_;
}

void Translate::Run() {
    Log::logger->info("request contents");
    request_timer_.start(5000);
    request_ok_ = false;
    reply_ = requests_();
    QObject::connect(reply_, SIGNAL(finished()), this, SLOT(reply_finished_()));
}

QNetworkReply *Translate::requests_() {
    if (!network_ok_ || request_ok_) return nullptr;
    QString extra_url = is_to_zh_CN_() ? gen_extra_url_(EN, ZH_CN) : gen_extra_url_(ZH_CN, EN);
    base_url_.setQuery(extra_url);
    Log::logger->info(( "request url: "+ base_url_.toString() + extra_url).toStdString());
    return manager_->get(QNetworkRequest(base_url_));
}

void Translate::reply_finished_() {
    request_ok_ = true;
    request_timer_.stop();
    Log::logger->info("reply contents");
    QVariant status_code = reply_->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if (status_code.isValid() && status_code.toInt() != 200) network_ok_ = false;

    QByteArray response = reply_->readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(response);
    QJsonArray json_array = json_doc.array();
    result_ = std::make_shared<QJsonArray>(json_array);
    emit ReplyFinished(result_);
}

void Translate::request_timeout_() {
    const char *msg = "network request timeout";
    Log::logger->error(msg);
    throw std::runtime_error(msg);
}

void Translate::network_changed_() {
    network_ok_ = !network_ok_;
}

void Translate::SetText(QString &text) {
    text_ = text;
}

