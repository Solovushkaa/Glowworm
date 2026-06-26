#ifndef LOG_HPP
#define LOG_HPP

#include <QFileInfo>

/**
 * @brief Namespace with logging constants.
 */
namespace logs {

inline constexpr qint64 kMaxLogSize = 10 * 1024 * 1024; // 10 MB
inline constexpr const char *kLogFileName = "glowworm.log";
inline constexpr const char *kLogArchiveName = "glowworm.log.1";

} // namespace logs

const QString &LogPath();

/**
 * @brief Log file cleaner.
 */
void rotateLogFileIfNeeded();

/**
 * @brief Logging stream handler.
 * @param type Message type
 * @param context 
 * @param msg Message
 */
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

void setCustomMessageHandler();

#endif // LOG_HPP
