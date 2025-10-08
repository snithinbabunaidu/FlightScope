#include "geofencemodel.h"

GeofenceModel::GeofenceModel(QObject* parent)
    : QObject(parent),
      m_modified(false),
      m_active(false) {
}

const QGeoCoordinate* GeofenceModel::vertexAt(int index) const {
    if (index >= 0 && index < m_vertices.count()) {
        return &m_vertices.at(index);
    }
    return nullptr;
}

void GeofenceModel::addVertex(const QGeoCoordinate& coordinate) {
    if (!coordinate.isValid()) {
        return;
    }

    m_vertices.append(coordinate);
    int index = m_vertices.count() - 1;

    emit vertexAdded(index, coordinate);
    emit countChanged(m_vertices.count());
    emit geofenceChanged();
    setModified(true);
}

void GeofenceModel::insertVertex(int index, const QGeoCoordinate& coordinate) {
    if (index < 0 || index > m_vertices.count() || !coordinate.isValid()) {
        return;
    }

    m_vertices.insert(index, coordinate);

    emit vertexAdded(index, coordinate);
    emit countChanged(m_vertices.count());
    emit geofenceChanged();
    setModified(true);
}

void GeofenceModel::removeVertex(int index) {
    if (index < 0 || index >= m_vertices.count()) {
        return;
    }

    m_vertices.removeAt(index);

    emit vertexRemoved(index);
    emit countChanged(m_vertices.count());
    emit geofenceChanged();
    setModified(true);
}

void GeofenceModel::updateVertex(int index, const QGeoCoordinate& coordinate) {
    if (index < 0 || index >= m_vertices.count() || !coordinate.isValid()) {
        return;
    }

    m_vertices[index] = coordinate;

    emit vertexUpdated(index, coordinate);
    emit geofenceChanged();
    setModified(true);
}

void GeofenceModel::clearGeofence() {
    if (m_vertices.isEmpty()) {
        return;
    }

    m_vertices.clear();

    emit geofenceCleared();
    emit countChanged(0);
    emit geofenceChanged();
    setModified(true);
    setActive(false);
}

void GeofenceModel::loadGeofence(const QList<QGeoCoordinate>& vertices) {
    m_vertices = vertices;

    emit geofenceLoaded(m_vertices.count());
    emit countChanged(m_vertices.count());
    emit geofenceChanged();
    setModified(false);  // Loading from file/vehicle = not modified
}

void GeofenceModel::markSaved() {
    setModified(false);
}

void GeofenceModel::setActive(bool active) {
    if (m_active != active) {
        m_active = active;
        emit activeChanged(m_active);
    }
}

void GeofenceModel::setModified(bool modified) {
    if (m_modified != modified) {
        m_modified = modified;
        emit modifiedChanged(m_modified);
    }
}
