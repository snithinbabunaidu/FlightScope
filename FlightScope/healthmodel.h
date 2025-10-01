#ifndef HEALTHMODEL_H
#define HEALTHMODEL_H

#include <QObject>

class HealthModel : public QObject
{
    Q_OBJECT
    // Expose GPS fix type as a property
    Q_PROPERTY(int gpsFixType READ gpsFixType WRITE setGpsFixType NOTIFY gpsFixTypeChanged)
    // Expose satellite count as a property
    Q_PROPERTY(int satelliteCount READ satelliteCount WRITE setSatelliteCount NOTIFY satelliteCountChanged)

  public:
    explicit HealthModel(QObject *parent = nullptr);

           // Getters for the properties
    int gpsFixType() const;
    int satelliteCount() const;

  public slots:
    // Setters for the properties
    void setGpsFixType(int gpsFixType);
    void setSatelliteCount(int satelliteCount);

  signals:
    // Change notification signals
    void gpsFixTypeChanged(int gpsFixType);
    void satelliteCountChanged(int satelliteCount);

  private:
    int m_gpsFixType = 0;
    int m_satelliteCount = 0;
};

#endif // HEALTHMODEL_H
