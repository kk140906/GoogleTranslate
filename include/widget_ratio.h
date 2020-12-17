#ifndef PERCENTRATIO_H
#define PERCENTRATIO_H
#include <QApplication>
#include <QtWidgets>
#include <QPixmap>
#include <QPainterPath>
#include <QPainter>

class PercentRatio : public QWidget
{
    Q_OBJECT
public:
    explicit PercentRatio(QWidget *parent=nullptr);
    ~PercentRatio();
    void setRatio(qreal ratio) { ratio_ = ratio > 100 ? 100 : ratio; }
    void setRectHeight(int height) { rect_height_ = height; }
protected:
    void paintEvent(QPaintEvent *);
private:
    qreal ratio_{0};
    int rect_height_{0};
};

#endif // PERCENTRATIO_H
