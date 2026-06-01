#ifndef DIRECTORYMANAGER_HPP
#define DIRECTORYMANAGER_HPP

#include <QAbstractListModel>
#include <QList>
#include <QLoggingCategory>
#include <QtQmlIntegration/qqmlintegration.h>
#include "file_info.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

Q_DECLARE_LOGGING_CATEGORY(directory_manager)

class DirectoryManager : public QAbstractListModel
{
    Q_OBJECT
    QML_UNCREATABLE("Managed from C++")

    enum DirectoryRoles {
        NameRole = Qt::UserRole + 1,
        PathRole,
        CreatedRole,
        ModifiedRole,
        AccessedRole,
        SizeRole,
        IsDirRole,
        IsReadableRole
    };

    // --- ModelView methods ---
public:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // --- Constructors ---
public:
    explicit DirectoryManager(QObject *parent = nullptr);
    ~DirectoryManager();

    // --- Methods ---
public:
    bool containCachedDirectory(const QString &dirPath);
    void updateDirectory(std::vector<std::unique_ptr<FileInfo>> &&directory, const QString &dirPath);

    QList<FileInfo *> &getActiveDirectory() { return m_activeDirectory; }

    // --- Slots ---
public slots:
    void onHostChanged();

    // --- Members ----
private:
    QList<FileInfo *> m_activeDirectory;
    std::unordered_map<QString, std::vector<std::unique_ptr<FileInfo>>>
        m_cachedDirectories; // Key - Path to directory
};

#endif // DIRECTORYMANAGER_HPP
