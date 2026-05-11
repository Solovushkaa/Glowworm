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
#ifdef QT_DEBUG
    QFile logFile(logs::kLogFileName);
#else
    QFile logFile(logs::kLogPath);
#endif
    if (!logFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
        fprintf(stderr,
                "Unable to open file %s for writing\n",
                logs::kLogPath.toLocal8Bit().constData());
        return;
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    // Log level
    const char *level;
    const char *fileLevel;
    switch (type) {
    case QtDebugMsg:
        level = "\033[00mDEBUG\033[0m";
        fileLevel = "DEBUG";
        break;
    case QtInfoMsg:
        level = "\033[94mINFO\033[0m";
        fileLevel = "INFO";
        break;
    case QtWarningMsg:
        level = "\033[33mWARN\033[0m";
        fileLevel = "WARN";
        break;
    case QtCriticalMsg:
        level = "\033[35mCRIT\033[0m";
        fileLevel = "CRIT";
        break;
    case QtFatalMsg:
        level = "\033[31mFATAL\033[0m";
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
    conStream << QString("[%1] [%2] [%3:%4][%5]\e[107m\033[30m:\033[0m\e[0m %6")
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
