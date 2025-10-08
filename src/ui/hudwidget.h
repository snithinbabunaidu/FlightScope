#ifndef HUDWIDGET_H
#define HUDWIDGET_H

#include <QWidget>

class HudWidget : public QWidget {
    Q_OBJECT

public:
    explicit HudWidget(QWidget* parent = nullptr);

    void setAltitude(float altitude);
    void setHeading(float heading);
    void setGroundSpeed(float speed);
    void setPitch(float pitch);
    void setRoll(float roll);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void drawArtificialHorizon(QPainter& painter, const QRect& rect);
    void drawSpeedTape(QPainter& painter, const QRect& rect);
    void drawAltitudeTape(QPainter& painter, const QRect& rect);
    void drawHeadingTape(QPainter& painter, const QRect& rect);
    void drawAircraftSymbol(QPainter& painter, const QRect& rect);
    void drawTelemetryData(QPainter& painter, const QRect& rect);

    float m_altitude;
    float m_heading;
    float m_groundSpeed;
    float m_pitch;
    float m_roll;
};

#endif  // HUDWIDGET_H
