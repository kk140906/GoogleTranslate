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
    void setRatio(qreal ratio);
    void setRectHeight(int height);
protected:
    void paintEvent(QPaintEvent *);
private:
    qreal _ratio = 0;
    int _rectHeight = 0;
};

#endif // PERCENTRATIO_H
