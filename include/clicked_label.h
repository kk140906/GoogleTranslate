#ifndef CLICKED_LABEL_H
#define CLICKED_LABEL_H

#include <QLabel>
#include <QWidget>

class ClickedLabel : public QLabel {
Q_OBJECT
public:
    explicit ClickedLabel(QWidget *parent = nullptr) : QLabel(parent) {}

    static QLabel *CreateLabel(QWidget *parent, const QString &text,const QFont &font);

signals:

    void clicked();

protected:
    void mousePressEvent(QMouseEvent *) override { emit clicked(); }
};

#endif // CLICKED_LABEL_H
