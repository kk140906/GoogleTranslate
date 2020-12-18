#include <tuple>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "system_operate.h"
#include "logger.h"

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow),
        edit_timer_(std::make_shared<QTimer>(new QTimer(this))),
        query_timer_(std::make_shared<QTimer>(new QTimer(this))),
        translator_(std::make_shared<Translate>()) {
    ui->setupUi(this);
    initial_();
}

MainWindow::~MainWindow() noexcept {}


void MainWindow::initial_() {
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);
    setWindowIcon(QIcon(":/Qresource/Google_Translate_Icon.png"));
    // listen the drag operate
    ui->TitleFrame->installEventFilter(this);

    ui->Synonym_enScrollArea->hide();
    ui->Synonym_zh_CNScrollArea->hide();

    tray_icon_ = std::make_shared<TrayIcon>(this);

    tray_icon_->AddAction("设置",
                          QIcon(":/Qresource/Settings_Icon.png"),
                          TrayIcon::Settings);

    tray_icon_->AddAction("退出",
                          QIcon(":/Qresource/Close1_Icon.png"),
                          TrayIcon::Exit_Application);

    // MainWidget constructed by ui->setupUi()
    // so circle_progress_ need by create in here
    circle_progress_ = std::make_shared<CircleProgress>(ui->MainWidget);
    circle_progress_->setGeometry(QRect(180, 200, 25, 25));
    circle_progress_->SetColor(QColor(220, 165, 25));
    circle_progress_->show();


    // bind clicked label clicked event,minimization the window
    QObject::connect(ui->TitleMinimizeLabel, SIGNAL(clicked()), this, SLOT(close()));
    // bind
    QObject::connect(edit_timer_.get(), SIGNAL(timeout()), this, SLOT(edit_timeout_()));
    QObject::connect(query_timer_.get(), SIGNAL(timeout()), this, SLOT(query_timeout_()));
    QObject::connect(ui->InputContent, SIGNAL(textChanged()), this, SLOT(prepare_()));
    QObject::connect(translator_.get(),
                     SIGNAL(ReplyFinished(std::shared_ptr<QJsonArray>)),
                     this,
                     SLOT(parse_result_(std::shared_ptr<QJsonArray>)));

    QObject::connect(this, SIGNAL(synonym_zh_CN_found()), this, SLOT(show_synonym_zh_CN_()));
    QObject::connect(this, SIGNAL(synonym_en_found()), this, SLOT(show_synonym_en_()));
}

bool MainWindow::eventFilter(QObject *object, QEvent *event) {
    //Title Frame DragMove
    if (event->type() == QEvent::MouseButtonPress) {
        auto *mouseEvent = dynamic_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            dragging_window_ = true;
#ifdef QT6
            mw_relative_pos_ = mouseEvent->globalPosition() - pos();
#else
            mw_relative_pos_ = mouseEvent->globalPos() - pos();
#endif
        }
    } else if (dragging_window_ && (event->type() == QEvent::MouseMove)) {
        auto *mouseEvent = dynamic_cast<QMouseEvent *>(event);
#ifdef QT6
        move((mouseEvent->globalPosition() - mw_relative_pos_).toPoint());
#else
        move(mouseEvent->globalPos() - mw_relative_pos_.toPoint());
#endif
    } else if (event->type() == QEvent::MouseButtonRelease)
        dragging_window_ = false;
    return QObject::eventFilter(object, event);
}

void MainWindow::UpdateText() {
    ui->InputContent->setPlainText(system_operate::get_copied_text_());
    ui->InputContent->moveCursor(QTextCursor::End,QTextCursor::MoveAnchor);
    show_window_();
}

void MainWindow::show_window_() {
    show();
    activateWindow();
}

void MainWindow::prepare_() {
    ui->ReturnResult->clear();
    ui->Synonym_enScrollArea->hide();
    ui->Synonym_zh_CNScrollArea->hide();
    text_ = ui->InputContent->toPlainText();
    if (text_.isEmpty()) {
        circle_progress_->Stop();
//        ui->ReturnResult->clear();
        return;
    }
    circle_progress_->Start();
    // start timer
    edit_timer_->start(300);
}

void MainWindow::edit_timeout_() {
    translator_->SetText(text_);
    Log::logger->info(("translate text: " + text_).toStdString());
    translator_->Run();
    edit_timer_->stop();
    circle_progress_->Stop();
}

void MainWindow::query_timeout_() {

}

void MainWindow::parse_result_(const std::shared_ptr<QJsonArray> &result_array) {
    QString result_string;
    for (auto result : result_array->at(0).toArray()) {
        QJsonValue arr = result.toArray().at(0);
        if (arr.isString()) {
            result_string.append(arr.toString());
        }
    }
    ui->ReturnResult->setPlainText(result_string);
    Log::logger->info(("translate result: " + result_string).toStdString());
    if (!result_array->at(1).isNull()) {
        synonym_zh_CN_all_ = result_array->at(1).toArray();
        emit synonym_zh_CN_found();
    }
    if (result_array->size() >= 12 && !result_array->at(11).isNull()) {
        synonym_en_all = result_array->at(11).toArray();
        emit synonym_en_found();
    }
}


MainWindow::VecTupleType MainWindow::parse_json_(const QJsonArray &arr) {
    VecTupleType vec_tuple;
    for (auto sorts : arr) {
        QJsonArray words_array = sorts.toArray();
        QString words_type = words_array.at(0).toString() + ":";
        vec_tuple.emplace_back(std::make_tuple(words_type, QStringList(), double(0)));
        bool is_zh_CN = words_array.size() > 3;

        if (is_zh_CN) {
            QJsonArray word_array = words_array.at(2).toArray();
            for (auto &&word : word_array) {
                QJsonArray word_array_ = word.toArray();
                QString first = word_array_.at(0).toString();
                QStringList second = get_json_string_list_(word.toArray(), 1);
                QJsonValue last = word_array_.last();
                double third = last.isDouble() ? last.toDouble() : 0;
                vec_tuple.emplace_back(std::make_tuple(first, second, third));
            }
        } else {
            QJsonArray word_array = words_array.at(1).toArray();
            for (auto &&word : word_array) {
                QStringList second = get_json_string_list_(word.toArray(), 0);
                vec_tuple.emplace_back(std::make_tuple(QString(), second, 0));
            }
        }
    }
    return vec_tuple;
}

QStringList MainWindow::get_json_string_list_(const QJsonArray &word_array, int pos) {
    QJsonArray word_array_ = word_array.at(pos).toArray();
    QStringList string_list;
    std::for_each(word_array_.begin(), word_array_.end(), [&string_list](const QJsonValue &w) {
        string_list.append(w.toString());
    });
    return string_list;
}


void MainWindow::show_synonym_zh_CN_() {
    delete_synonym_area_(ui->Synonym_zh_CNScrollArea, ui->Synonym_zh_CNContents);
    ui->Synonym_zh_CNScrollArea->show();

    QFont font("Consolas", 10);
    QFontMetrics font_metrics(font);

    auto *grid_layout = new QGridLayout(ui->Synonym_zh_CNContents);
    grid_layout->setObjectName("Synonym_zh_CNLayout");
    grid_layout->setColumnMinimumWidth(0, 40);
    grid_layout->setColumnMinimumWidth(1, 250);
    grid_layout->setColumnMinimumWidth(2, 50);
    grid_layout->setAlignment(Qt::AlignCenter);

    int row{0}, col{0};
    for (auto sorts: parse_json_(synonym_zh_CN_all_)) {
        QLabel *label;

        // first column
        if (std::get<1>(sorts).isEmpty()) {
            label = ClickedLabel::CreateLabel(ui->Synonym_zh_CNContents, std::get<0>(sorts), font);
            grid_layout->addWidget(label, row, col);
            row += 1;
            continue;
        }
        QString col1_word = std::get<0>(sorts);
        int word_width = font_metrics.horizontalAdvance(col1_word);
        const int max_word_width = 35;
        if (word_width > max_word_width)
            split_word_(col1_word, word_width, max_word_width);
        label = ClickedLabel::CreateLabel(ui->Synonym_zh_CNContents, col1_word, font);
        label->setStyleSheet(QStringLiteral("color:#e55334"));
        label->setTextInteractionFlags(Qt::TextSelectableByMouse);
        grid_layout->addWidget(label, row, col);
        col += 1;

        // second column
        QStringList string_list_ = std::get<1>(sorts);
        auto *v_layout = new QVBoxLayout();
        QString v_layout_obj_name = QString("VLayout").append(col1_word);
        v_layout->setObjectName(v_layout_obj_name);
        auto *h_layout = new QHBoxLayout();
        QString h_layout_obj_name = QString("HLayout").append(col1_word);
        h_layout->setObjectName(h_layout_obj_name);
        int font_width;
        int font_height = font_metrics.height() + 10;
        int width_pos_X = 0;
        for (int k = 0; k < string_list_.size(); k++) {
            const QString &col2_word = string_list_.at(k);
            label = ClickedLabel::CreateLabel(ui->Synonym_zh_CNContents, col2_word, font);
            label->setStyleSheet(QStringLiteral("border-radius: 5px;\n"
                                                "border:1.5px solid;\n"
                                                "padding : 3px;\n"
                                                "color:#b0ee1b"));
            label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            label->setMinimumHeight(font_height);
            label->setTextInteractionFlags(Qt::TextSelectableByMouse);
//              font_width = font_metrics.width(col2_word)+15; //Word Width;
            font_width = font_metrics.horizontalAdvance(col2_word) + 15; //Word Width;
            if (width_pos_X + font_width > 200) {
                auto *h_layout_space1 = new QSpacerItem(240 - width_pos_X,
                                                        font_height,
                                                        QSizePolicy::Expanding,
                                                        QSizePolicy::Minimum);
                h_layout->addItem(h_layout_space1);
                v_layout->addLayout(h_layout);
                h_layout = new QHBoxLayout();
                QString h_layout_next_obj_name = h_layout_obj_name.append(QString::number(k));
                h_layout->setObjectName(h_layout_next_obj_name);
                width_pos_X = 0;
            }
            h_layout->addWidget(label);
            width_pos_X += font_width + 10; //Word Interval;
        }
        auto *h_layout_space2 = new QSpacerItem(240 - width_pos_X,
                                                font_height,
                                                QSizePolicy::Expanding,
                                                QSizePolicy::Minimum);
        h_layout->addItem(h_layout_space2);
        v_layout->addLayout(h_layout);
        grid_layout->addLayout(v_layout, row, col);
        col += 1;

        // third column
        qreal col3_ratio = static_cast<int>(std::get<2>(sorts) * 10000) / 100.0;
        auto *percent_ratio_widget = new PercentRatio(ui->Synonym_zh_CNContents);
        percent_ratio_widget->setObjectName(QString::number(col3_ratio));
        if (col3_ratio < 5)
            col3_ratio = 5;
        percent_ratio_widget->setRatio(col3_ratio);
        grid_layout->addWidget(percent_ratio_widget, row, col);
        percent_ratio_widget->setRectHeight(font_height - 18);
        percent_ratio_widget->show();
        row += 1;
        col = 0;
    }
    ui->Synonym_zh_CNContents->adjustSize();
}

void MainWindow::split_word_(QString &word,
                             int word_width,
                             const int max_word_width,
                             const int pos) {
    int char_counts = word.size();

    double char_width = static_cast<double>(word_width) / char_counts;
    for (int i = 0; i < char_counts; ++i) {
        if (i * char_width > max_word_width - pos &&
            word.at(i) == ' ' ||
            word.at(i) == ';' ||
            word.at(i) == ',' ||
            word.at(i) == '/') {
            word.insert(i, '\n');
            break;
        }
    }
}

void MainWindow::show_synonym_en_() {
    delete_synonym_area_(ui->Synonym_enScrollArea, ui->Synonym_enContents);
    ui->Synonym_enScrollArea->show();
    QFont font("Consolas", 10);
    QFontMetrics font_metrics(font);
    const int font_height = font_metrics.height() + 15;
    int width_pos_X = 0;
    int height_pos_Y = 0;
    const int height_gap = 10;
    for (auto sorts : parse_json_(synonym_en_all)) {
        QLabel *label;

        // word type
        if (std::get<1>(sorts).isEmpty()) {
            label = ClickedLabel::CreateLabel(ui->Synonym_enContents, std::get<0>(sorts), font);
            label->setGeometry(QRect(width_pos_X,
                                     height_pos_Y,
                                     font_metrics.horizontalAdvance(std::get<0>(sorts)),
                                     font_height));
            width_pos_X = 0;
            height_pos_Y += font_height;
            continue;
        }
        // words
        for (auto word : std::get<1>(sorts)) {
            const int max_scroll_area_width = 210;
            int font_width = font_metrics.horizontalAdvance(word) + 10;
            int font_new_height = font_height;
            int font_new_width = font_width;
            // over range in the direction of X
            if (width_pos_X + font_new_width > max_scroll_area_width) {
                width_pos_X = 0;
                height_pos_Y += font_height + height_gap;
            }

            if (font_width > max_scroll_area_width) {
                split_word_(word, font_width, max_scroll_area_width, max_scroll_area_width / 2);
                font_new_height = font_height * 2 - height_gap;
                font_new_width = max_scroll_area_width;
            }

            label = ClickedLabel::CreateLabel(ui->Synonym_enContents, word, font);
            label->setGeometry(QRect(width_pos_X, height_pos_Y, font_new_width, font_new_height));
            if (font_width > max_scroll_area_width)
                height_pos_Y += font_new_height - font_height;
            label->setStyleSheet(QStringLiteral("border-radius: 5px;\n"
                                                "border:1.5px solid;\n"
                                                "padding : 3px;\n"
                                                "color:#f4ed1f"));
            label->setTextInteractionFlags(Qt::TextSelectableByMouse);
            width_pos_X += (font_new_width + 10);
        }
        width_pos_X = 0;
        height_pos_Y += font_height + height_gap;
    }
    ui->Synonym_enContents->adjustSize();
}

void MainWindow::delete_synonym_area_(QScrollArea *area, QWidget *contents) {
    QScrollBar *scroll_bar = area->verticalScrollBar();
    scroll_bar->setValue(scroll_bar->minimum());
    const QObjectList &contents_obj_counts = contents->children();
    if (!contents_obj_counts.isEmpty()) {
        for (auto content_obj : contents_obj_counts) {
            delete content_obj;
        }
    }
}
