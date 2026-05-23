/**
 * @file initialize.hpp
 * @brief Initializes the application's initial data.
 */

#ifndef INITIALIZE_HPP
#define INITIALIZE_HPP

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(application_initialize)

/**
 * @class Initialize
 * @brief Initializes the application's initial data.
 */
class Initialize
{
public:
    /**
     * @brief Constructor that starts the initialization process
     */
    Initialize();

private:
    /**
     * @brief Creates/Loads application data paths
     * @param path Path to config
     * @param directoryName Directory name for debug logging
     */
    void loadPath(const char *path, const char *directoryName);
};

#endif // INITIALIZE_HPP
