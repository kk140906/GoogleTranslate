#include <waitprogress.h>

#include "WaitProgress.h"

#include <QPainter>
#include <QDebug>

WaitProgress::WaitProgress(QWidget* parent)
    : QWidget(parent),
      m_angle(0),
      m_timerId(-1),
      m_delay(40),
      m_displayedWhenStopped(false),
      m_color(Qt::black)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFocusPolicy(Qt::NoFocus);
}

bool WaitProgress::isAnimated () const
{
    return (m_timerId != -1);
}

void WaitProgress::setDisplayedWhenStopped(bool state)
{
    m_displayedWhenStopped = state;
    update();
}

bool WaitProgress::isDisplayedWhenStopped() const
{
    return m_displayedWhenStopped;
}

void WaitProgress::startAnimation()
{
    m_angle = 0;

    if (m_timerId == -1)
        m_timerId = startTimer(m_delay);
}

void WaitProgress::stopAnimation()
{
    if (m_timerId != -1)
        killTimer(m_timerId);

    m_timerId = -1;

    update();
}

void WaitProgress::setAnimationDelay(int delay)
{
    if (m_timerId != -1)
        killTimer(m_timerId);

    m_delay = delay;

    if (m_timerId != -1)
        m_timerId = startTimer(m_delay);
}

void WaitProgress::setColor(const QColor & color)
{
    m_color = color;

    update();
}

QSize WaitProgress::sizeHint() const
{
    return QSize(20,20);
}

int WaitProgress::heightForWidth(int w) const
{
    return w;
}

void WaitProgress::timerEvent(QTimerEvent * /*event*/)
{
    m_angle = (m_angle+30)%360;

    update();
}

void WaitProgress::paintEvent(QPaintEvent * /*event*/)
{
    if (!m_displayedWhenStopped && !isAnimated())
        return;

    int width = qMin(this->width(), this->height());

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    int outerRadius = (width-1)*0.5;
    int innerRadius = (width-1)*0.5*0.38;

    int capsuleHeight = outerRadius - innerRadius;

    int capsuleWidth  = (width > 32 ) ? capsuleHeight *0.23 : capsuleHeight *0.35;

    int capsuleRadius = capsuleWidth/2;

    for (int i=0; i<12; i++)
    {
        QColor color = m_color;
        color.setAlphaF(1.0 - (i/12.0));
        p.setPen(Qt::NoPen);
        p.setBrush(color);
        p.save();
        p.translate(rect().center());
        p.rotate(m_angle - i*30.0);
        p.drawRoundedRect(-capsuleWidth*0.5, -(innerRadius+capsuleHeight), capsuleWidth, capsuleHeight, capsuleRadius, capsuleRadius);
        p.restore();
    }
}
