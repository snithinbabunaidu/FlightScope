#include "vehiclemodel.h"

VehicleModel::VehicleModel(QObject *parent) : QObject(parent)
{
}

bool VehicleModel::armed() const
{
    return m_armed;
}

QString VehicleModel::flightMode() const
{
    return m_flightMode;
}

void VehicleModel::setArmed(bool armed)
{
    // Only update and emit signal if the value has actually changed
    if (m_armed == armed)
        return;

    m_armed = armed;
    emit armedChanged(m_armed);
}

void VehicleModel::setFlightMode(const QString &flightMode)
{
    // Only update and emit signal if the value has actually changed
    if (m_flightMode == flightMode)
        return;

    m_flightMode = flightMode;
    emit flightModeChanged(m_flightMode);
}
