#include "missionmodel.h"

MissionModel::MissionModel(QObject* parent)
    : QObject(parent),
      m_modified(false) {
}

const Waypoint* MissionModel::waypointAt(int index) const {
    if (index >= 0 && index < m_waypoints.count()) {
        return &m_waypoints.at(index);
    }
    return nullptr;
}

void MissionModel::addWaypoint(const Waypoint& waypoint) {
    m_waypoints.append(waypoint);
    int index = m_waypoints.count() - 1;

    // Renumber all waypoints
    renumberWaypoints();

    emit waypointAdded(index, waypoint);
    emit countChanged(m_waypoints.count());
    emit missionChanged();
    setModified(true);
}

void MissionModel::insertWaypoint(int index, const Waypoint& waypoint) {
    if (index < 0 || index > m_waypoints.count()) {
        return;
    }

    m_waypoints.insert(index, waypoint);

    // Renumber all waypoints
    renumberWaypoints();

    emit waypointAdded(index, waypoint);
    emit countChanged(m_waypoints.count());
    emit missionChanged();
    setModified(true);
}

void MissionModel::removeWaypoint(int index) {
    if (index < 0 || index >= m_waypoints.count()) {
        return;
    }

    m_waypoints.removeAt(index);

    // Renumber all waypoints
    renumberWaypoints();

    emit waypointRemoved(index);
    emit countChanged(m_waypoints.count());
    emit missionChanged();
    setModified(true);
}

void MissionModel::updateWaypoint(int index, const Waypoint& waypoint) {
    if (index < 0 || index >= m_waypoints.count()) {
        return;
    }

    m_waypoints[index] = waypoint;

    // Ensure sequence number is correct
    m_waypoints[index].setSequence(index);

    emit waypointUpdated(index, waypoint);
    emit missionChanged();
    setModified(true);
}

void MissionModel::moveWaypoint(int fromIndex, int toIndex) {
    if (fromIndex < 0 || fromIndex >= m_waypoints.count() ||
        toIndex < 0 || toIndex >= m_waypoints.count() ||
        fromIndex == toIndex) {
        return;
    }

    m_waypoints.move(fromIndex, toIndex);

    // Renumber all waypoints
    renumberWaypoints();

    emit waypointMoved(fromIndex, toIndex);
    emit missionChanged();
    setModified(true);
}

void MissionModel::clearMission() {
    if (m_waypoints.isEmpty()) {
        return;
    }

    m_waypoints.clear();

    emit missionCleared();
    emit countChanged(0);
    emit missionChanged();
    setModified(true);
}

void MissionModel::loadMission(const QList<Waypoint>& waypoints) {
    m_waypoints = waypoints;

    // Renumber all waypoints to ensure consistency
    renumberWaypoints();

    emit missionLoaded(m_waypoints.count());
    emit countChanged(m_waypoints.count());
    emit missionChanged();
    setModified(false);  // Loading from file/vehicle = not modified
}

void MissionModel::markSaved() {
    setModified(false);
}

void MissionModel::renumberWaypoints() {
    for (int i = 0; i < m_waypoints.count(); ++i) {
        m_waypoints[i].setSequence(i);
    }
}

void MissionModel::setModified(bool modified) {
    if (m_modified != modified) {
        m_modified = modified;
        emit modifiedChanged(m_modified);
    }
}
