/**
 * @file initialize.h
 * @brief Initializes the application's initial data.
 */

#ifndef INITIALIZE_H
#define INITIALIZE_H

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

#endif // INITIALIZE_H
