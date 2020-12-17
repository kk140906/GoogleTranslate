#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkReply>

#include <circle_progress.h>
#include <clicked_label.h>
#include <widget_ratio.h>
#include <web_requests.h>
#include <tray_icon.h>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    using VecTupleType = std::vector<std::tuple<QString, QStringList, double>>;

    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

    void UpdateText();

signals:

    void synonym_zh_CN_found();

    void synonym_en_found();

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private:
    void initial_();

    static QStringList get_json_string_list_(const QJsonArray &word_array, int pos);

    static void delete_synonym_area_(QScrollArea *area, QWidget *contents);

    static VecTupleType parse_json_(const QJsonArray &arr);

    static void split_word_(QString &word, int word_width, const int max_word_width, const int pos = 0) ;
    Ui::MainWindow *ui;

    QAction *action;

    bool dragging_window_{false};
    QPointF mw_relative_pos_; // mouse and window relative position

    std::shared_ptr<QTimer> edit_timer_, query_timer_;
    std::shared_ptr<CircleProgress> circle_progress_;
    QString text_;

    std::shared_ptr<TrayIcon> tray_icon_;
    std::shared_ptr<Translate> translator_;

    QJsonArray synonym_zh_CN_all_, synonym_en_all;

private slots:

    void edit_timeout_();

    void query_timeout_();

    void prepare_();

    void parse_result_(const std::shared_ptr<QJsonArray> &);

    void show_synonym_zh_CN_();

    void show_synonym_en_();

    void show_window_();
};

#endif // MAINWINDOW_H
