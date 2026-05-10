#include "log.hpp"

void rotateLogFileIfNeeded()
{
    QFileInfo logFile(logs::kLogFileName);
    if (logFile.exists() && logFile.size() > logs::kMaxLogSize) {
        // Remove the full log
        QFile::remove(logs::kLogArchiveName);
        // And rename the log to the archive log
        QFile::rename(logs::kLogFileName, logs::kLogArchiveName);
    }
}
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // Checking if rotation is needed before recording
    rotateLogFileIfNeeded();

    // Open file
    QFile logFile(logs::kLogFileName);
    // QFile logFile(logs::kLogPath); // Later
    if (!logFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
        fprintf(stderr,
                "Unable to open file %s for writing\n",
                logs::kLogPath.toLocal8Bit().constData());
        return;
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    // Log level
    const char *level;
    switch (type) {
    case QtDebugMsg:
        level = "\033[00mDEBUG\033[0m";
        break;
    case QtInfoMsg:
        level = "\033[00mINFO\033[0m";
        break;
    case QtWarningMsg:
        level = "\033[33mWARN\033[0m";
        break;
    case QtCriticalMsg:
        level = "\033[31mCRIT\033[0m";
        break;
    case QtFatalMsg:
        level = "\033[31mFATAL\033[0m";
        break;
    }
    QTextStream fileStream(&logFile);
#ifdef QT_DEBUG
    QString outString = QString("[%1] [%2] [%3] [%4:%5]: %6")
                            .arg(timestamp)
                            .arg(level, -14) // alignment
                            .arg(context.category)
                            .arg(context.function)
                            .arg(context.line)
                            .arg(msg);
#else
    QString outString = QString("[%1] [%2] [%3]: %6")
                            .arg(timestamp)
                            .arg(level, -14) // alignment
                            .arg(context.category)
                            .arg(msg);
#endif
    fileStream << outString << Qt::endl;
#ifdef QT_DEBUG
    QTextStream conStream(stdout);
    conStream << outString << Qt::endl;
#endif
}

void setCustomMessageHandler()
{
    qInstallMessageHandler(messageHandler);
}
