#ifndef CLIENTSAVEDCONNECTIONMANAGER_H
#define CLIENTSAVEDCONNECTIONMANAGER_H

#include <QJsonDocument>
#include <QObject>
#include <QVariantHash>

class ClientSavedConnectionManager : public QObject
{
    Q_OBJECT

public:
    ClientSavedConnectionManager();

    void read();
    QList<QVariantHash> getConnections() { return m_savedConnections; }
    QVariantHash getActive()
    {
        emit connectionsChanged();
        return *m_activeConnection;
    }
    void setActive(int indx);

    void add(const QString &name, const QString &protocol, const QString &ip, const QString &port);
    void remove(const QString &name);

    bool isEmpty() const { return m_isConnectionPresetsEmpty; }

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
