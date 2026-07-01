#include "log.hpp"
#include <QStandardPaths>

const QString &LogPath()
{
    static QString logPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                             + "/appdata/logs/" + logs::kLogFileName;

    return logPath;
}

void rotateLogFileIfNeeded()
{
#ifdef QT_DEBUG
    QFileInfo logFile(logs::kLogFileName);
    if (logFile.exists() && logFile.size() > logs::kMaxLogSize) {
        // Remove the full log
        QFile::remove(logs::kLogArchiveName);
        // And rename the log to the archive log
        QFile::rename(logs::kLogFileName, logs::kLogArchiveName);
    }
#else
    QFileInfo logFile(LogPath());
    if (logFile.exists() && logFile.size() > logs::kMaxLogSize) {
        // Remove the full log
        QFile::remove(logs::kLogArchiveName);
        // And rename the log to the archive log
        QFile::rename(LogPath(), logs::kLogArchiveName);
    }
#endif
}
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // Checking if rotation is needed before recording
    rotateLogFileIfNeeded();

    // Open file
    QFile logFile(LogPath());
    if (!logFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
        fprintf(stderr,
                "Unable to open file %s for writing: %s\n",
                LogPath().toLocal8Bit().constData(),
                logFile.errorString().toLocal8Bit().constData());
        return;
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    // Log level
#ifdef QT_DEBUG
    const char *level;
#endif
    const char *fileLevel;
    switch (type) {
    case QtDebugMsg:
#ifdef QT_DEBUG
        level = "\033[00mDEBUG\033[0m";
#endif
        fileLevel = "DEBUG";
        break;
    case QtInfoMsg:
#ifdef QT_DEBUG
        level = "\033[94mINFO\033[0m";
#endif
        fileLevel = "INFO";
        break;
    case QtWarningMsg:
#ifdef QT_DEBUG
        level = "\033[33mWARN\033[0m";
#endif
        fileLevel = "WARN";
        break;
    case QtCriticalMsg:
#ifdef QT_DEBUG
        level = "\033[35mCRIT\033[0m";
#endif
        fileLevel = "CRIT";
        break;
    case QtFatalMsg:
#ifdef QT_DEBUG
        level = "\033[31mFATAL\033[0m";
#endif
        fileLevel = "FATAL";
        break;
    }

    QTextStream fileStream(&logFile);
#ifdef QT_DEBUG
    fileStream << QString("[%1] [%2] [%3] [%4:%5][%6]: %7")
                      .arg(timestamp)
                      .arg(fileLevel)
                      .arg(context.category)
                      .arg(context.function)
                      .arg(context.line)
                      .arg(context.file)
                      .arg(msg)
               << Qt::endl;
#else
    fileStream << QString("[%1] [%2] [%3]: %4")
                      .arg(timestamp)
                      .arg(fileLevel)
                      .arg(context.category)
                      .arg(msg)
               << Qt::endl;
#endif

#ifdef QT_DEBUG
    QTextStream conStream(stdout);
    conStream << QString("[%1] [%2] [%3:%4][%5]\033[107m\033[30m:\033[0m\033[0m %6")
                     .arg(level, -13) // alignment
                     .arg(context.category)
                     .arg(context.function)
                     .arg(context.line)
                     .arg(context.file)
                     .arg(msg)
              << Qt::endl;
#endif
}

void setCustomMessageHandler()
{
    qInstallMessageHandler(messageHandler);
}
