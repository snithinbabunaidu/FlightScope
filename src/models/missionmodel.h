#ifndef MISSIONMODEL_H
#define MISSIONMODEL_H

#include <QObject>
#include <QList>
#include "waypoint.h"

/**
 * @brief Model representing a complete mission (list of waypoints)
 *
 * This class manages the mission state and provides signals for UI updates.
 * It handles mission operations like add, remove, clear, and reordering.
 */
class MissionModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool modified READ modified NOTIFY modifiedChanged)

public:
    explicit MissionModel(QObject* parent = nullptr);
    ~MissionModel() override = default;

    /**
     * @brief Get number of waypoints in mission
     */
    int count() const { return m_waypoints.count(); }

    /**
     * @brief Check if mission has been modified since last save/load
     */
    bool modified() const { return m_modified; }

    /**
     * @brief Get all waypoints
     */
    const QList<Waypoint>& waypoints() const { return m_waypoints; }

    /**
     * @brief Get waypoint at index
     */
    const Waypoint* waypointAt(int index) const;

    /**
     * @brief Check if mission is empty
     */
    bool isEmpty() const { return m_waypoints.isEmpty(); }

public slots:
    /**
     * @brief Add a waypoint to the end of the mission
     */
    void addWaypoint(const Waypoint& waypoint);

    /**
     * @brief Insert a waypoint at a specific index
     */
    void insertWaypoint(int index, const Waypoint& waypoint);

    /**
     * @brief Remove waypoint at index
     */
    void removeWaypoint(int index);

    /**
     * @brief Update waypoint at index
     */
    void updateWaypoint(int index, const Waypoint& waypoint);

    /**
     * @brief Move waypoint from one index to another
     */
    void moveWaypoint(int fromIndex, int toIndex);

    /**
     * @brief Clear all waypoints
     */
    void clearMission();

    /**
     * @brief Load mission from list of waypoints
     */
    void loadMission(const QList<Waypoint>& waypoints);

    /**
     * @brief Mark mission as saved (clears modified flag)
     */
    void markSaved();

signals:
    /**
     * @brief Emitted when waypoint count changes
     */
    void countChanged(int count);

    /**
     * @brief Emitted when a waypoint is added
     */
    void waypointAdded(int index, const Waypoint& waypoint);

    /**
     * @brief Emitted when a waypoint is removed
     */
    void waypointRemoved(int index);

    /**
     * @brief Emitted when a waypoint is updated
     */
    void waypointUpdated(int index, const Waypoint& waypoint);

    /**
     * @brief Emitted when a waypoint is moved
     */
    void waypointMoved(int fromIndex, int toIndex);

    /**
     * @brief Emitted when mission is cleared
     */
    void missionCleared();

    /**
     * @brief Emitted when entire mission is loaded
     */
    void missionLoaded(int count);

    /**
     * @brief Emitted when modified state changes
     */
    void modifiedChanged(bool modified);

    /**
     * @brief Emitted when mission changes in any way
     */
    void missionChanged();

private:
    void renumberWaypoints();
    void setModified(bool modified);

    QList<Waypoint> m_waypoints;
    bool m_modified;
};

#endif  // MISSIONMODEL_H
