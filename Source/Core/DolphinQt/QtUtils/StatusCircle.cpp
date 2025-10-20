#include "DolphinQt/QtUtils/StatusCircle.h"

StatusCircle::StatusCircle(QWidget* parent)
    : QWidget(parent), m_color(Qt::red)
{
    setMinimumSize(24, 24);
}

void StatusCircle::setColor(const QColor& color)
{
    if (m_color != color)
    {
        m_color = color;
        update();
    }
}

void StatusCircle::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int diameter = qMin(width(), height()) - 4;
    QRect rect((width() - diameter) / 2, (height() - diameter) / 2, diameter, diameter);

    painter.setBrush(m_color);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(rect);
}

void StatusCircle::mousePressEvent(QMouseEvent* event)
{
    emit StatusCircleClicked();
}