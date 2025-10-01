#ifndef VEHICLEMODEL_H
#define VEHICLEMODEL_H

#include <QObject>
#include <QString>

class VehicleModel : public QObject
{
    Q_OBJECT
    // Expose the 'armed' status as a property for UI binding
    Q_PROPERTY(bool armed READ armed WRITE setArmed NOTIFY armedChanged)
    // Expose the 'flightMode' as a property for UI binding
    Q_PROPERTY(QString flightMode READ flightMode WRITE setFlightMode NOTIFY flightModeChanged)

  public:
    explicit VehicleModel(QObject *parent = nullptr);

           // Getter functions for the properties
    bool armed() const;
    QString flightMode() const;

  public slots:
    // Setter functions for the properties
    void setArmed(bool armed);
    void setFlightMode(const QString &flightMode);

  signals:
    // Signals that are emitted whenever a property's value changes
    void armedChanged(bool armed);
    void flightModeChanged(const QString &flightMode);

  private:
    bool m_armed = false;
    QString m_flightMode = "Unknown";
};

#endif // VEHICLEMODEL_H
