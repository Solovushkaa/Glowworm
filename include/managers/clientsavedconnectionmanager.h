#ifndef CLIENTSAVEDCONNECTIONMANAGER_H
#define CLIENTSAVEDCONNECTIONMANAGER_H

#include <QJsonDocument>
#include <QQuickItem>

class ClientSavedConnectionManager : public QObject
{
    Q_OBJECT
    QML_SINGLETON

public:
    ClientSavedConnectionManager();

    Q_INVOKABLE void read();
    Q_INVOKABLE QList<QVariantHash> getConnections() { return m_savedConnections; }
    Q_INVOKABLE QVariantHash getActive()
    {
        emit connectionsChanged();
        return *m_activeConnection;
    }
    Q_INVOKABLE void setActive(int indx);

    Q_INVOKABLE void add(const QString &name,
                         const QString &protocol,
                         const QString &ip,
                         const QString &port);
    Q_INVOKABLE void remove(const QString &name);

    Q_INVOKABLE bool isEmpty() const { return m_isConnectionPresetsEmpty; }

private:
    void rewriteSavedConnectionsToFile();
    // void updateHttpClientHostKeys();

signals:

    void connectionsLoaded();
    void connectionsChanged();

private:
    // QString m_presetPath;
    QJsonDocument m_jsonSavedConnection;
    QList<QVariantHash> m_savedConnections;
    QVariantHash *m_activeConnection;
    bool m_isConnectionPresetsEmpty;
};

#endif // CLIENTSAVEDCONNECTIONMANAGER_H
