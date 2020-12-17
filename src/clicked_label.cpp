/**
 * Created by KK on 2020/12/15.
 */
#include <clicked_label.h>

QLabel *ClickedLabel::CreateLabel(QWidget *parent, const QString &text,const QFont &font) {
    auto *label = new QLabel(parent);
    label->setObjectName(text);
    label->setText(text);
    label->setAlignment(Qt::AlignCenter);
    label->setFont(font);
    label->show();
    return label;
}
