#include "compasswidget.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QFontMetrics>
#include <QtMath>

CompassWidget::CompassWidget(QWidget* parent)
    : QWidget(parent), m_heading(0.0) {
    setMinimumSize(100, 100);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void CompassWidget::setHeading(double heading) {
    // Normalize heading to 0-360
    while (heading < 0) heading += 360.0;
    while (heading >= 360.0) heading -= 360.0;

    if (qAbs(m_heading - heading) > 0.1) {
        m_heading = heading;
        update();
    }
}

void CompassWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int side = qMin(width(), height());
    int centerX = width() / 2;
    int centerY = height() / 2;
    int radius = (side / 2) - 10;

    // Draw background circle
    painter.setPen(QPen(QColor("#E0E0E0"), 2));
    painter.setBrush(QColor("#FAFAFA"));
    painter.drawEllipse(centerX - radius, centerY - radius, radius * 2, radius * 2);

    // Save painter state
    painter.save();

    // Translate to center and rotate based on heading (negative = compass rose rotates opposite)
    painter.translate(centerX, centerY);
    painter.rotate(-m_heading);

    // Draw compass rose
    drawCompassRose(painter, 0, 0, radius);
    drawCardinalMarks(painter, 0, 0, radius);

    // Restore painter state
    painter.restore();

    // Draw heading indicator (fixed at top)
    drawHeadingIndicator(painter, centerX, centerY, radius);

    // Draw numeric heading below compass
    painter.setPen(QColor("#424242"));
    QFont headingFont = painter.font();
    headingFont.setPixelSize(16);
    headingFont.setBold(true);
    painter.setFont(headingFont);

    QString headingText = QString("%1°").arg(static_cast<int>(qRound(m_heading)));
    QFontMetrics fm(headingFont);
    int textWidth = fm.horizontalAdvance(headingText);
    painter.drawText(centerX - textWidth / 2, centerY + radius + 25, headingText);
}

void CompassWidget::drawCompassRose(QPainter& painter, int centerX, int centerY, int radius) {
    // Draw tick marks
    painter.setPen(QPen(QColor("#BDBDBD"), 1));

    for (int i = 0; i < 360; i += 5) {
        painter.save();
        painter.rotate(i);

        int tickLength;
        int tickWidth;

        if (i % 90 == 0) {
            // Major cardinal marks (N, E, S, W)
            tickLength = 15;
            tickWidth = 3;
            painter.setPen(QPen(QColor("#0277BD"), tickWidth));
        } else if (i % 45 == 0) {
            // Intercardinal marks (NE, SE, SW, NW)
            tickLength = 12;
            tickWidth = 2;
            painter.setPen(QPen(QColor("#757575"), tickWidth));
        } else if (i % 15 == 0) {
            // 15-degree marks
            tickLength = 10;
            tickWidth = 2;
            painter.setPen(QPen(QColor("#BDBDBD"), tickWidth));
        } else {
            // 5-degree marks
            tickLength = 6;
            tickWidth = 1;
            painter.setPen(QPen(QColor("#E0E0E0"), tickWidth));
        }

        painter.drawLine(centerX, centerY - radius + 5, centerX, centerY - radius + 5 + tickLength);
        painter.restore();
    }
}

void CompassWidget::drawCardinalMarks(QPainter& painter, int centerX, int centerY, int radius) {
    QFont cardinalFont = painter.font();
    cardinalFont.setPixelSize(14);
    cardinalFont.setBold(true);
    painter.setFont(cardinalFont);

    QFontMetrics fm(cardinalFont);

    struct Cardinal {
        int angle;
        QString text;
        QColor color;
    };

    Cardinal cardinals[] = {
        {0, "N", QColor("#00C853")},    // Green for North
        {45, "NE", QColor("#424242")},
        {90, "E", QColor("#424242")},
        {135, "SE", QColor("#424242")},
        {180, "S", QColor("#F44336")},  // Red for South
        {225, "SW", QColor("#424242")},
        {270, "W", QColor("#424242")},
        {315, "NW", QColor("#424242")}
    };

    for (const auto& cardinal : cardinals) {
        painter.save();
        painter.rotate(cardinal.angle);

        painter.setPen(cardinal.color);
        int textWidth = fm.horizontalAdvance(cardinal.text);
        int textHeight = fm.height();
        painter.drawText(centerX - textWidth / 2, centerY - radius + 30 + textHeight / 2, cardinal.text);

        painter.restore();
    }
}

void CompassWidget::drawHeadingIndicator(QPainter& painter, int centerX, int centerY, int radius) {
    // Draw fixed heading indicator at top (triangle pointing down)
    painter.setPen(QPen(QColor("#FF6F00"), 2));
    painter.setBrush(QColor("#FF9800"));

    QPolygon triangle;
    triangle << QPoint(centerX, centerY - radius + 2)
             << QPoint(centerX - 8, centerY - radius - 8)
             << QPoint(centerX + 8, centerY - radius - 8);

    painter.drawPolygon(triangle);

    // Draw center dot
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor("#0277BD"));
    painter.drawEllipse(centerX - 4, centerY - 4, 8, 8);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor("#FFFFFF"));
    painter.drawEllipse(centerX - 2, centerY - 2, 4, 4);
}
