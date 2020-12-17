#ifndef CIRCLE_PROGRESS_H
#define CIRCLE_PROGRESS_H

#include <QWidget>
#include <QColor>
#include <QPainter>

class CircleProgress : public QWidget
{
    Q_OBJECT
public:
    CircleProgress(QWidget* parent = nullptr);
    int AnimateSpeed() const { return speed_; }
    const QColor & Color() const { return color_; }
    bool IsAnimating () const;

public slots:
    void Start();
    void Stop();
    void SetAnimateSpeed(const int speed); // set wait time,when the bar was rotated once
    void SetColor(const QColor & color); // set bar color
    void SetRotateAngle(const int angle); // set bar rotate angle
protected:
    void timerEvent(QTimerEvent *) override;
    void paintEvent(QPaintEvent *) override;
private:
    template<typename T>
    void set_value_(T &lhs,T rhs);

    int rotate_angle_{30};
    int cur_angle_{0};
    int timer_id_{-1};
    const int default_timer_id_{-1};
    int speed_{35};
    QColor color_{Qt::black};
};
#endif // CIRCLE_PROGRESS_H
