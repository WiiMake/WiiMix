#include <QWidget>
#include <QPainter>
#include <QColor>

#pragma once

class StatusCircle : public QWidget
{
    Q_OBJECT

public:
    explicit StatusCircle(QWidget* parent = nullptr);

    void setColor(const QColor& color);
    
    QColor color() const { return m_color; }

    void mousePressEvent(QMouseEvent* event) override;

signals:
    void StatusCircleClicked();
    
protected:
    void paintEvent(QPaintEvent*) override;

private:
    QColor m_color;
};