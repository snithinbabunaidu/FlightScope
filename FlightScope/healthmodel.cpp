#include "healthmodel.h"

HealthModel::HealthModel(QObject *parent) : QObject(parent)
{
}

int HealthModel::gpsFixType() const
{
    return m_gpsFixType;
}

int HealthModel::satelliteCount() const
{
    return m_satelliteCount;
}

void HealthModel::setGpsFixType(int gpsFixType)
{
    if (m_gpsFixType == gpsFixType)
        return;

    m_gpsFixType = gpsFixType;
    emit gpsFixTypeChanged(m_gpsFixType);
}

void HealthModel::setSatelliteCount(int satelliteCount)
{
    if (m_satelliteCount == satelliteCount)
        return;

    m_satelliteCount = satelliteCount;
    emit satelliteCountChanged(m_satelliteCount);
}
