#ifndef LOG_H
#define LOG_H

#include <QFileInfo>

/**
 * @brief Namespace with logging constants.
 */
namespace logs {

constexpr qint64 MAX_LOG_SIZE = 10 * 1024 * 1024; // 10 MB
constexpr const char *LOG_FILE_NAME = "dfs.log";
constexpr const char *LOG_ARCHIVE_NAME = "dfs.1";

} // namespace logs

/**
 * @brief Log file cleaner.
 */
void rotateLogFileIfNeeded()
{
    QFileInfo info(logs::LOG_FILE_NAME);
    if (info.exists() && info.size() > logs::MAX_LOG_SIZE) {
        // Remove the full log
        QFile::remove(logs::LOG_ARCHIVE_NAME);
        // And rename the log to the archive log
        QFile::rename(logs::LOG_FILE_NAME, logs::LOG_ARCHIVE_NAME);
    }
}

/**
 * @brief Logging stream handler.
 * @param type Message type
 * @param context 
 * @param msg Message
 */
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // Checking if rotation is needed before recording
    rotateLogFileIfNeeded();

    // Open file
    QFile logFile(logs::LOG_FILE_NAME);
    if (!logFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
        return;
    }

    // Create stream in file and create time
    QTextStream stream(&logFile);
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

    // Log level
    QString level;
    switch (type) {
    case QtDebugMsg:
        level = "\033[00mDEBUG\033[0m";
        break;
    case QtInfoMsg:
        level = "\033[33mINFO\033[0m";
        break;
    case QtWarningMsg:
        level = "\033[33mWARN\033[0m";
        break;
    case QtCriticalMsg:
        level = "\033[31mERROR\033[0m";
        break;
    case QtFatalMsg:
        level = "\033[31mFATAL\033[0m";
        break;
    }

    stream << QString("[%1] [%2] %3")
                  .arg(timestamp)
                  .arg(level, -14) // alignment
                  .arg(msg)
           << Qt::endl;
}

#endif // LOG_H
