#include "hudwidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QFont>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QtMath>

HudWidget::HudWidget(QWidget* parent)
    : QWidget(parent),
      m_altitude(0.0f),
      m_heading(0.0f),
      m_groundSpeed(0.0f),
      m_pitch(0.0f),
      m_roll(0.0f) {

    setMinimumSize(400, 350);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Set black background
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(10, 10, 15));
    setPalette(pal);
}

void HudWidget::setAltitude(float altitude) {
    m_altitude = altitude;
    update();
}

void HudWidget::setHeading(float heading) {
    m_heading = heading;
    update();
}

void HudWidget::setGroundSpeed(float speed) {
    m_groundSpeed = speed;
    update();
}

void HudWidget::setPitch(float pitch) {
    // Limit pitch to realistic range (-60 to +60 degrees) to prevent crazy flipping
    // Most drones shouldn't exceed ±30 degrees in normal flight
    m_pitch = qBound(-60.0f, pitch, 60.0f);
    update();
}

void HudWidget::setRoll(float roll) {
    // Limit roll to realistic range (-60 to +60 degrees) to prevent crazy flipping
    m_roll = qBound(-60.0f, roll, 60.0f);
    update();
}

void HudWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QRect rect = this->rect();

    // Draw full-screen artificial horizon with sky/ground gradients
    drawArtificialHorizon(painter, rect);

    // Draw speed tape (left side)
    drawSpeedTape(painter, rect);

    // Draw altitude tape (right side)
    drawAltitudeTape(painter, rect);

    // Draw heading tape with roll indicator (top)
    drawHeadingTape(painter, rect);

    // Draw aircraft symbol (center chevron)
    drawAircraftSymbol(painter, rect);

    // Draw telemetry data corners
    drawTelemetryData(painter, rect);
}

void HudWidget::drawArtificialHorizon(QPainter& painter, const QRect& rect) {
    painter.save();

    int centerX = rect.width() / 2;
    int centerY = rect.height() / 2;

    // Move to center and apply roll rotation
    painter.translate(centerX, centerY);
    painter.rotate(-m_roll);

    // Calculate pitch offset (pixels per degree) - calibrated for realistic movement
    // 3 pixels per degree provides good visibility without excessive movement
    float pitchOffset = m_pitch * 3.0f;

    int maxDim = qMax(rect.width(), rect.height()) * 2;

    // Sky gradient (vibrant blue)
    QLinearGradient skyGradient(0, -maxDim, 0, -pitchOffset);
    skyGradient.setColorAt(0.0, QColor(10, 50, 120));      // Deep sky blue
    skyGradient.setColorAt(0.5, QColor(50, 120, 200));     // Medium blue
    skyGradient.setColorAt(1.0, QColor(100, 180, 255));    // Light blue at horizon
    painter.fillRect(QRect(-maxDim, -maxDim, maxDim * 2, maxDim - pitchOffset), skyGradient);

    // Ground gradient (green to dark green)
    QLinearGradient groundGradient(0, -pitchOffset, 0, maxDim);
    groundGradient.setColorAt(0.0, QColor(80, 140, 60));   // Light green at horizon
    groundGradient.setColorAt(0.3, QColor(50, 100, 40));   // Medium green
    groundGradient.setColorAt(1.0, QColor(30, 60, 20));    // Dark green at bottom
    painter.fillRect(QRect(-maxDim, -pitchOffset, maxDim * 2, maxDim + pitchOffset), groundGradient);

    // Horizon line (crisp white with subtle glow)
    painter.setPen(QPen(QColor(255, 255, 255, 200), 3));
    painter.drawLine(-maxDim, -pitchOffset, maxDim, -pitchOffset);

    // Add subtle glow to horizon line
    painter.setPen(QPen(QColor(0, 255, 255, 100), 6));
    painter.drawLine(-maxDim, -pitchOffset, maxDim, -pitchOffset);

    // Draw pitch ladder - ALL WHITE for maximum contrast
    painter.setPen(QPen(Qt::white, 2.5));
    QFont font("Arial", 10, QFont::Bold);
    painter.setFont(font);

    // Draw pitch lines every 5 degrees
    for (int pitch = -90; pitch <= 90; pitch += 5) {
        if (pitch == 0) continue;  // Skip horizon line

        float y = -pitchOffset - (pitch * 3.0f);  // Match the 3.0 scaling factor

        // Only draw if visible
        if (qAbs(y) < rect.height() * 1.5) {
            int lineWidth = (pitch % 10 == 0) ? 60 : 30;

            // All solid white lines for both sky and ground - maximum readability
            painter.setPen(QPen(Qt::white, 2.5));
            painter.drawLine(-lineWidth, y, lineWidth, y);

            // Draw pitch degree text (every 10 degrees) - BRIGHT WHITE, NO GLOW
            if (pitch % 10 == 0) {
                QString pitchText = QString::number(qAbs(pitch));

                // Simple white text - high contrast
                painter.setPen(Qt::white);
                painter.drawText(-lineWidth - 35, y + 5, pitchText);
                painter.drawText(lineWidth + 15, y + 5, pitchText);
            }
        }
    }

    painter.restore();
}

void HudWidget::drawSpeedTape(QPainter& painter, const QRect& rect) {
    painter.save();

    int tapeX = 20;
    int tapeY = 60;
    int tapeWidth = 80;
    int tapeHeight = rect.height() - 120;
    int centerY = tapeY + tapeHeight / 2;

    // Semi-transparent background with subtle gradient - more integrated look
    QLinearGradient bgGradient(tapeX, tapeY, tapeX + tapeWidth, tapeY);
    bgGradient.setColorAt(0.0, QColor(0, 0, 0, 140));
    bgGradient.setColorAt(1.0, QColor(20, 20, 30, 120));
    painter.fillRect(tapeX, tapeY, tapeWidth, tapeHeight, bgGradient);

    // Subtle border - softer, less "boxed-in" look
    painter.setPen(QPen(QColor(100, 100, 120, 120), 1));
    painter.drawRect(tapeX, tapeY, tapeWidth, tapeHeight);

    // Draw speed markers
    painter.setPen(QColor(200, 200, 200));
    QFont font("Arial", 10, QFont::Bold);
    painter.setFont(font);

    float pixelsPerMps = 3.0f;  // pixels per m/s
    int speedStep = 2;  // Show every 2 m/s

    for (int spd = 0; spd <= 100; spd += speedStep) {
        float offset = (m_groundSpeed - spd) * pixelsPerMps;
        int y = centerY + offset;

        if (y >= tapeY && y <= tapeY + tapeHeight) {
            // Draw tick mark - all white for consistency
            if (spd % 10 == 0) {
                painter.setPen(Qt::white);
                painter.drawLine(tapeX, y, tapeX + 15, y);
                QString spdText = QString::number(spd);
                painter.drawText(tapeX + 18, y + 5, spdText);
            } else if (spd % 5 == 0) {
                painter.setPen(QColor(180, 180, 180));
                painter.drawLine(tapeX, y, tapeX + 10, y);
            }
        }
    }

    // Draw current speed box with subtle accent
    QRect speedBox(tapeX - 2, centerY - 18, tapeWidth + 4, 36);

    // Softer background
    QLinearGradient boxGradient(speedBox.topLeft(), speedBox.bottomRight());
    boxGradient.setColorAt(0.0, QColor(0, 30, 40, 200));
    boxGradient.setColorAt(1.0, QColor(0, 20, 30, 220));
    painter.fillRect(speedBox, boxGradient);

    // Subtle border
    painter.setPen(QPen(QColor(0, 150, 170), 2));
    painter.drawRect(speedBox);

    // Speed value - bright white, no glow
    painter.setPen(Qt::white);
    QFont valueFont("Arial", 13, QFont::Bold);
    painter.setFont(valueFont);
    QString speedText = QString::number(m_groundSpeed, 'f', 1);
    painter.drawText(speedBox, Qt::AlignCenter, speedText);

    // Label
    painter.setPen(Qt::white);
    QFont labelFont("Arial", 8, QFont::Bold);
    painter.setFont(labelFont);
    painter.drawText(tapeX, tapeY - 8, "m/s");

    painter.restore();
}

void HudWidget::drawAltitudeTape(QPainter& painter, const QRect& rect) {
    painter.save();

    int tapeX = rect.width() - 100;
    int tapeY = 60;
    int tapeWidth = 80;
    int tapeHeight = rect.height() - 120;
    int centerY = tapeY + tapeHeight / 2;

    // Semi-transparent background with subtle gradient
    QLinearGradient bgGradient(tapeX, tapeY, tapeX + tapeWidth, tapeY);
    bgGradient.setColorAt(0.0, QColor(20, 20, 30, 120));
    bgGradient.setColorAt(1.0, QColor(0, 0, 0, 140));
    painter.fillRect(tapeX, tapeY, tapeWidth, tapeHeight, bgGradient);

    // Subtle border
    painter.setPen(QPen(QColor(100, 100, 120, 120), 1));
    painter.drawRect(tapeX, tapeY, tapeWidth, tapeHeight);

    // Draw altitude markers
    painter.setPen(QColor(200, 200, 200));
    QFont font("Arial", 10, QFont::Bold);
    painter.setFont(font);

    float pixelsPerMeter = 2.5f;
    int altStep = 5;  // Show every 5 meters

    // Altitude tape starts from 0 and goes up only
    for (int alt = 0; alt <= 500; alt += altStep) {
        float offset = (m_altitude - alt) * pixelsPerMeter;
        int y = centerY + offset;

        if (y >= tapeY && y <= tapeY + tapeHeight) {
            // Draw tick mark - all white
            if (alt % 10 == 0) {
                painter.setPen(Qt::white);
                painter.drawLine(tapeX + tapeWidth - 15, y, tapeX + tapeWidth, y);
                QString altText = QString::number(alt);
                painter.drawText(tapeX + 8, y + 5, altText);
            } else if (alt % 5 == 0) {
                painter.setPen(QColor(180, 180, 180));
                painter.drawLine(tapeX + tapeWidth - 10, y, tapeX + tapeWidth, y);
            }
        }
    }

    // Draw current altitude box with subtle accent
    QRect altBox(tapeX - 2, centerY - 18, tapeWidth + 4, 36);

    // Softer background
    QLinearGradient boxGradient(altBox.topLeft(), altBox.bottomRight());
    boxGradient.setColorAt(0.0, QColor(0, 30, 40, 200));
    boxGradient.setColorAt(1.0, QColor(0, 20, 30, 220));
    painter.fillRect(altBox, boxGradient);

    // Subtle border
    painter.setPen(QPen(QColor(0, 150, 170), 2));
    painter.drawRect(altBox);

    // Altitude value - bright white, no glow
    painter.setPen(Qt::white);
    QFont valueFont("Arial", 13, QFont::Bold);
    painter.setFont(valueFont);
    QString altText = QString::number(qMax(0.0f, m_altitude), 'f', 1);  // Don't show negative altitude
    painter.drawText(altBox, Qt::AlignCenter, altText);

    // Label
    painter.setPen(Qt::white);
    QFont labelFont("Arial", 8, QFont::Bold);
    painter.setFont(labelFont);
    painter.drawText(tapeX, tapeY - 8, "ALT (m)");

    painter.restore();
}

void HudWidget::drawHeadingTape(QPainter& painter, const QRect& rect) {
    painter.save();

    int centerX = rect.width() / 2;
    int arcY = 15;
    int arcRadius = 140;

    // Draw curved heading tape background
    QPainterPath arcPath;
    arcPath.moveTo(centerX - 180, arcY + 40);
    arcPath.arcTo(centerX - arcRadius, arcY - arcRadius + 40, arcRadius * 2, arcRadius * 2, 210, 120);
    arcPath.lineTo(centerX + 180, arcY + 40);
    arcPath.lineTo(centerX + 180, arcY + 55);
    arcPath.lineTo(centerX - 180, arcY + 55);
    arcPath.closeSubpath();

    QLinearGradient arcGradient(centerX, arcY, centerX, arcY + 55);
    arcGradient.setColorAt(0.0, QColor(0, 0, 0, 180));
    arcGradient.setColorAt(1.0, QColor(20, 20, 30, 160));
    painter.fillPath(arcPath, arcGradient);

    painter.setPen(QPen(QColor(0, 180, 200), 1));
    painter.drawPath(arcPath);

    // Draw heading markers - ALL WHITE for maximum contrast
    QFont font("Arial", 9, QFont::Bold);
    painter.setFont(font);

    float degreesPerPixel = 0.6f;

    for (int hdg = 0; hdg <= 360; hdg += 5) {
        // Normalize heading difference
        float diff = hdg - m_heading;
        while (diff > 180) diff -= 360;
        while (diff < -180) diff += 360;

        if (qAbs(diff) <= 60) {  // Only draw visible headings
            float offset = diff / degreesPerPixel;
            int x = centerX + offset;

            if (hdg % 30 == 0) {
                painter.setPen(Qt::white);
                painter.drawLine(x, arcY + 40, x, arcY + 55);

                QString hdgText;
                if (hdg == 0 || hdg == 360) hdgText = "N";
                else if (hdg == 90) hdgText = "E";
                else if (hdg == 180) hdgText = "S";
                else if (hdg == 270) hdgText = "W";
                else hdgText = QString::number(hdg);

                painter.drawText(x - 15, arcY + 35, 30, 15, Qt::AlignCenter, hdgText);
            } else if (hdg % 10 == 0) {
                painter.setPen(QColor(180, 180, 180));
                painter.drawLine(x, arcY + 45, x, arcY + 55);
            }
        }
    }

    // Roll indicator arc (top of heading tape)
    painter.setPen(QPen(QColor(0, 200, 220), 2));
    painter.drawArc(centerX - arcRadius, arcY - arcRadius + 40, arcRadius * 2, arcRadius * 2, 210 * 16, 120 * 16);

    // Roll tick marks
    painter.setPen(QColor(200, 200, 200));
    for (int angle = -45; angle <= 45; angle += 15) {
        float rad = qDegreesToRadians((float)angle);
        int x1 = centerX + arcRadius * qSin(rad);
        int y1 = arcY + 40 - arcRadius * qCos(rad);
        int x2 = centerX + (arcRadius - 10) * qSin(rad);
        int y2 = arcY + 40 - (arcRadius - 10) * qCos(rad);

        if (angle == 0) {
            painter.setPen(QPen(QColor(0, 255, 255), 3));
        } else {
            painter.setPen(QColor(200, 200, 200));
        }
        painter.drawLine(x1, y1, x2, y2);
    }

    // Aircraft roll pointer (triangle at center top)
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 100, 0));  // Orange

    painter.translate(centerX, arcY + 40);
    painter.rotate(-m_roll);

    QPolygon rollPointer;
    rollPointer << QPoint(0, -arcRadius + 5)
                << QPoint(-8, -arcRadius + 18)
                << QPoint(8, -arcRadius + 18);
    painter.drawPolygon(rollPointer);

    // Glow effect
    painter.setBrush(QColor(255, 150, 0, 100));
    QPolygon glowPointer;
    glowPointer << QPoint(0, -arcRadius + 3)
                << QPoint(-10, -arcRadius + 20)
                << QPoint(10, -arcRadius + 20);
    painter.drawPolygon(glowPointer);

    painter.resetTransform();

    // Current heading box - subtle styling
    QRect hdgBox(centerX - 40, arcY + 58, 80, 28);

    QLinearGradient boxGradient(hdgBox.topLeft(), hdgBox.bottomRight());
    boxGradient.setColorAt(0.0, QColor(0, 30, 40, 200));
    boxGradient.setColorAt(1.0, QColor(0, 20, 30, 220));
    painter.fillRect(hdgBox, boxGradient);

    painter.setPen(QPen(QColor(0, 150, 170), 2));
    painter.drawRect(hdgBox);

    // Heading value - bright white
    painter.setPen(Qt::white);
    QFont valueFont("Arial", 12, QFont::Bold);
    painter.setFont(valueFont);
    int hdgInt = ((int)m_heading + 360) % 360;
    QString hdgText = QString::number(hdgInt) + "°";
    painter.drawText(hdgBox, Qt::AlignCenter, hdgText);

    painter.restore();
}

void HudWidget::drawAircraftSymbol(QPainter& painter, const QRect& rect) {
    painter.save();

    int centerX = rect.width() / 2;
    int centerY = rect.height() / 2;

    // Draw stylized chevron aircraft symbol (orange/red)
    painter.setPen(Qt::NoPen);

    // Outer glow
    painter.setBrush(QColor(255, 100, 0, 80));
    QPolygon glowChevron;
    glowChevron << QPoint(centerX, centerY)
                << QPoint(centerX - 50, centerY + 8)
                << QPoint(centerX - 35, centerY + 8)
                << QPoint(centerX - 35, centerY + 12)
                << QPoint(centerX - 15, centerY + 12)
                << QPoint(centerX - 15, centerY + 8)
                << QPoint(centerX + 15, centerY + 8)
                << QPoint(centerX + 15, centerY + 12)
                << QPoint(centerX + 35, centerY + 12)
                << QPoint(centerX + 35, centerY + 8)
                << QPoint(centerX + 50, centerY + 8);
    painter.drawPolygon(glowChevron);

    // Main symbol
    painter.setBrush(QColor(255, 80, 0));
    QPolygon chevron;
    chevron << QPoint(centerX, centerY)
            << QPoint(centerX - 45, centerY + 6)
            << QPoint(centerX - 32, centerY + 6)
            << QPoint(centerX - 32, centerY + 10)
            << QPoint(centerX - 12, centerY + 10)
            << QPoint(centerX - 12, centerY + 6)
            << QPoint(centerX + 12, centerY + 6)
            << QPoint(centerX + 12, centerY + 10)
            << QPoint(centerX + 32, centerY + 10)
            << QPoint(centerX + 32, centerY + 6)
            << QPoint(centerX + 45, centerY + 6);
    painter.drawPolygon(chevron);

    // Center dot with glow
    painter.setBrush(QColor(0, 255, 255, 150));
    painter.drawEllipse(QPoint(centerX, centerY), 5, 5);
    painter.setBrush(Qt::white);
    painter.drawEllipse(QPoint(centerX, centerY), 3, 3);

    painter.restore();
}

void HudWidget::drawTelemetryData(QPainter& painter, const QRect& rect) {
    painter.save();

    QFont font("Arial", 9, QFont::Bold);
    painter.setFont(font);

    // Bottom left - Additional data
    int bottomY = rect.height() - 15;

    painter.setPen(QColor(0, 200, 220));
    painter.drawText(20, bottomY - 30, "GPS:");
    painter.setPen(Qt::white);
    painter.drawText(65, bottomY - 30, "3D FIX");

    painter.setPen(QColor(0, 200, 220));
    painter.drawText(20, bottomY - 10, "BATT:");
    painter.setPen(Qt::white);
    painter.drawText(65, bottomY - 10, "12.6V");

    // Bottom right - Flight mode
    painter.setPen(QColor(0, 200, 220));
    painter.drawText(rect.width() - 150, bottomY - 30, "MODE:");
    painter.setPen(QColor(100, 255, 100));
    QFont modeFont("Arial", 10, QFont::Bold);
    painter.setFont(modeFont);
    painter.drawText(rect.width() - 100, bottomY - 30, "AUTO");

    painter.restore();
}
