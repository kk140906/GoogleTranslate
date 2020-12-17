#include "../include/circle_progress.h"

#include <QPainter>
#include <QDebug>

CircleProgress::CircleProgress(QWidget* parent): QWidget(parent) {
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFocusPolicy(Qt::NoFocus);
}

bool CircleProgress::IsAnimating () const {
    return (timer_id_ != default_timer_id_);
}

void CircleProgress::Start() {
    cur_angle_ = 0;
    if (!IsAnimating()) timer_id_ = startTimer(speed_);
}

void CircleProgress::Stop() {
    if (IsAnimating()) killTimer(timer_id_);
    timer_id_ = default_timer_id_;
    update();
}

template<typename T>
void CircleProgress::set_value_(T &lhs,T rhs) {
    bool state = IsAnimating();
    if (state) killTimer(timer_id_);
    lhs = rhs;
    update();
    if (state) timer_id_ = startTimer(speed_);
}
void CircleProgress::SetAnimateSpeed(int speed) { set_value_<int>(speed_,speed);}

void CircleProgress::SetColor(const QColor & color) { set_value_<QColor>(color_,color);}

void CircleProgress::SetRotateAngle(const int angle) { set_value_<int>(rotate_angle_,angle);}

void CircleProgress::timerEvent(QTimerEvent *) {
    cur_angle_ = (cur_angle_ + rotate_angle_) % 360;
    update();
}

void CircleProgress::paintEvent(QPaintEvent *) {
    if (!IsAnimating())
        return;

    int width = qMin(this->width(), this->height());
    int outer_radius = static_cast<int>(width * 0.5);
    int inner_radius = static_cast<int>(width * 0.5 * 0.35);

    // draw animate bar
    int bar_length = outer_radius - inner_radius;
    int bar_width  = static_cast<int>(bar_length * 0.35);
    int bar_radius = bar_width / 2;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    for (int i = 0; i< 360 / rotate_angle_; ++i)
    {
        QColor color = color_;
        color.setAlphaF(1 - rotate_angle_ * i / 360.0);
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        painter.save();
        painter.translate(rect().center());
        painter.rotate(cur_angle_ - i * rotate_angle_);
        // start painting from inner circle
        painter.drawRoundedRect(0, inner_radius, bar_width, bar_length, bar_radius, bar_radius);
        painter.restore();
    }
}
