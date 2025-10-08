#ifndef COMPASSWIDGET_H
#define COMPASSWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QFont>

class CompassWidget : public QWidget {
    Q_OBJECT

public:
    explicit CompassWidget(QWidget* parent = nullptr);

    void setHeading(double heading);
    double heading() const { return m_heading; }

    QSize sizeHint() const override { return QSize(140, 140); }
    QSize minimumSizeHint() const override { return QSize(100, 100); }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    double m_heading;  // in degrees (0-360)

    void drawCompassRose(QPainter& painter, int centerX, int centerY, int radius);
    void drawHeadingIndicator(QPainter& painter, int centerX, int centerY, int radius);
    void drawCardinalMarks(QPainter& painter, int centerX, int centerY, int radius);
};

#endif // COMPASSWIDGET_H
