#include "directory_manager.hpp"
#include "constants.hpp"

Q_LOGGING_CATEGORY(directory_manager, "directory.manager")

DirectoryManager::DirectoryManager(QObject *parent)
    : QAbstractListModel(parent)
{
    qCDebug(directory_manager) << "DirectoryManager - created";
}

DirectoryManager::~DirectoryManager()
{
    qCDebug(directory_manager) << "DirectoryManager - destroyed";
}

int DirectoryManager::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        qCDebug(directory_manager) << "QModelIndex parent is not valid";
        return 0;
    }
    return m_activeDirectory.size();
}

QVariant DirectoryManager::data(const QModelIndex &index, int role) const
{
    qCDebug(directory_manager) << "Getting DirectoryManager data for the QML List Model";

    if (!index.isValid() || index.row() >= m_activeDirectory.size())
        return QVariant();

    FileInfo *info = m_activeDirectory[index.row()];
    switch (role) {
    case NameRole:
        return info->m_name;
    case PathRole:
        return info->m_path;
    case CreatedRole:
        return info->m_created;
    case ModifiedRole:
        return info->m_modified;
    case AccessedRole:
        return info->m_accessed;
    case SizeRole:
        return info->m_size;
    case IsDirRole:
        return info->m_isDir;
    case IsReadableRole:
        return info->m_isReadable;
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> DirectoryManager::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = constants::kName.toUtf8();
    roles[PathRole] = constants::kPath.toUtf8();
    roles[CreatedRole] = constants::kCreated.toUtf8();
    roles[ModifiedRole] = constants::kModified.toUtf8();
    roles[AccessedRole] = constants::kAccessed.toUtf8();
    roles[SizeRole] = constants::kSize.toUtf8();
    roles[IsDirRole] = constants::kIsDir.toUtf8();
    roles[IsReadableRole] = constants::kIsReadable.toUtf8();
    return roles;
}

bool DirectoryManager::containCachedDirectory(const QString &dirPath)
{
    qCDebug(directory_manager) << "Checking the contents of the path:" << dirPath << "in the cache";

    if (m_cachedDirectories.contains(dirPath)) {
        return true;
    }
    return false;
}

void DirectoryManager::updateDirectory(std::vector<std::unique_ptr<FileInfo>> &&directory,
                                       const QString &dirPath)
{
    qCDebug(directory_manager) << "Updating the active directory to:" << dirPath;

    m_cachedDirectories[dirPath] = std::move(directory);

    beginResetModel();
    m_activeDirectory.clear();
    for (auto &uptrFileInfo : m_cachedDirectories[dirPath]) {
        m_activeDirectory.push_back(uptrFileInfo.get());
    }
    endResetModel();
}

void DirectoryManager::clearActiveDirectory()
{
    beginResetModel();
    m_activeDirectory.clear();
    endResetModel();
}

void DirectoryManager::onHostChanged()
{
    qCDebug(directory_manager) << "Clearing the directories of the old host";

    m_cachedDirectories.clear();

    updateDirectory(std::vector<std::unique_ptr<FileInfo>>(), constants::kFakePath.toString());
}
