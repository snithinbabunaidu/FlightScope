#ifndef GEOFENCEMODEL_H
#define GEOFENCEMODEL_H

#include <QObject>
#include <QList>
#include <QGeoCoordinate>

/**
 * @brief Model representing a geofence polygon
 *
 * This class manages the geofence state and provides signals for UI updates.
 * A geofence is defined by a list of vertices forming a polygon boundary.
 */
class GeofenceModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool modified READ modified NOTIFY modifiedChanged)
    Q_PROPERTY(bool active READ active NOTIFY activeChanged)

public:
    explicit GeofenceModel(QObject* parent = nullptr);
    ~GeofenceModel() override = default;

    /**
     * @brief Get number of vertices in geofence
     */
    int count() const { return m_vertices.count(); }

    /**
     * @brief Check if geofence has been modified since last save/upload
     */
    bool modified() const { return m_modified; }

    /**
     * @brief Check if geofence is active (has been uploaded to vehicle)
     */
    bool active() const { return m_active; }

    /**
     * @brief Get all vertices
     */
    const QList<QGeoCoordinate>& vertices() const { return m_vertices; }

    /**
     * @brief Get vertex at index
     */
    const QGeoCoordinate* vertexAt(int index) const;

    /**
     * @brief Check if geofence is empty
     */
    bool isEmpty() const { return m_vertices.isEmpty(); }

    /**
     * @brief Check if geofence is valid (at least 3 vertices)
     */
    bool isValid() const { return m_vertices.count() >= 3; }

public slots:
    /**
     * @brief Add a vertex to the end of the geofence
     */
    void addVertex(const QGeoCoordinate& coordinate);

    /**
     * @brief Insert a vertex at a specific index
     */
    void insertVertex(int index, const QGeoCoordinate& coordinate);

    /**
     * @brief Remove vertex at index
     */
    void removeVertex(int index);

    /**
     * @brief Update vertex at index
     */
    void updateVertex(int index, const QGeoCoordinate& coordinate);

    /**
     * @brief Clear all vertices
     */
    void clearGeofence();

    /**
     * @brief Load geofence from list of coordinates
     */
    void loadGeofence(const QList<QGeoCoordinate>& vertices);

    /**
     * @brief Mark geofence as saved (clears modified flag)
     */
    void markSaved();

    /**
     * @brief Set geofence active state (uploaded to vehicle)
     */
    void setActive(bool active);

signals:
    /**
     * @brief Emitted when vertex count changes
     */
    void countChanged(int count);

    /**
     * @brief Emitted when a vertex is added
     */
    void vertexAdded(int index, const QGeoCoordinate& coordinate);

    /**
     * @brief Emitted when a vertex is removed
     */
    void vertexRemoved(int index);

    /**
     * @brief Emitted when a vertex is updated
     */
    void vertexUpdated(int index, const QGeoCoordinate& coordinate);

    /**
     * @brief Emitted when geofence is cleared
     */
    void geofenceCleared();

    /**
     * @brief Emitted when entire geofence is loaded
     */
    void geofenceLoaded(int count);

    /**
     * @brief Emitted when modified state changes
     */
    void modifiedChanged(bool modified);

    /**
     * @brief Emitted when active state changes
     */
    void activeChanged(bool active);

    /**
     * @brief Emitted when geofence changes in any way
     */
    void geofenceChanged();

private:
    void setModified(bool modified);

    QList<QGeoCoordinate> m_vertices;
    bool m_modified;
    bool m_active;
};

#endif  // GEOFENCEMODEL_H
