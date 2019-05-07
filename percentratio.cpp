#include <percentratio.h>
#include <QDebug>
PercentRatio::PercentRatio(QWidget *parent) : QWidget(parent) {}

PercentRatio::~PercentRatio() {}

void PercentRatio::paintEvent(QPaintEvent *)
{
    QPainter *painter = new QPainter(this);
    painter->setPen(QPen(Qt::black,1.5,Qt::SolidLine,Qt::SquareCap,Qt::RoundJoin));
    painter->setRenderHints(QPainter::Antialiasing);
    QRect bound = rect();
    QPoint boundCenter = bound.center();
    int boundWidth = bound.width();
    int boundHeight = bound.height();
    if(!_rectHeight || _rectHeight > boundHeight-1)
        bound.setHeight(boundWidth/10);
    else
        bound.setHeight(_rectHeight);
    boundHeight = bound.height();
    bound.moveTop(boundCenter.y()-boundHeight/2);
    painter->drawRect(bound);
    painter->fillRect(bound,QBrush(QColor("#b9c4d0"),Qt::SolidPattern));
    QRect boundRatio = bound;
    boundRatio.setWidth(static_cast<int>(_ratio/100.0 *boundWidth));
    painter->fillRect(boundRatio,QBrush(QColor("#26cedf"),Qt::SolidPattern));
//    painter->drawText(bound,Qt::AlignCenter,(QString::number(_ratio)).append("%"));
    painter->end();
}
void PercentRatio::setRatio(qreal ratio)
{
    if(ratio>100)
        ratio = 100;
    _ratio = ratio;
}

void PercentRatio::setRectHeight(int height)
{
    _rectHeight = height;
}
