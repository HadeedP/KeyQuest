/**
 * @file loaddatamanager.h
 * @brief Header file for the LoadDataManager class
 * @author Alan Cruz, Bashar Hamo
 * @details This file defines the LoadDataManager class which handles loading and saving
 *          of application data, including settings and lesson statistics. It implements
 *          a singleton pattern to ensure only one instance manages all data.
 */

#ifndef LOADDATAMANAGER_H
#define LOADDATAMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QString>
#include <QDebug>
#include <map>
#include "state.h"

/**
 * @brief Manages loading and saving of application data
 * 
 * The LoadDataManager class is responsible for loading settings and lesson statistics
 * from the data.json file, and saving updates back to the file. It implements a
 * singleton pattern to ensure only one instance exists and manages all data.
 */
class LoadDataManager : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Gets the singleton instance of the LoadDataManager
     * @return Pointer to the LoadDataManager instance
     */
    static LoadDataManager* instance();

    /**
     * @brief Loads all data from the data.json file
     * @return true if loading was successful, false otherwise
     */
    bool loadData();

    /**
     * @brief Saves all data back to the data.json file
     * @return true if saving was successful, false otherwise
     */
    bool saveData();

    /**
     * @brief Updates lesson statistics for a specific topic
     * @param topicId The ID of the topic
     * @param score The score achieved
     * @param accuracy The accuracy percentage
     * @param attempts The number of attempts made
     */
    void updateLessonStats(int topicId, int score, double accuracy, int attempts);

    /**
     * @brief Gets the background music volume level
     * @return int The volume level (0-100)
     */
    int getBackgroundMusicLevel() const;

    /**
     * @brief Gets the sound effects volume level
     * @return int The volume level (0-100)
     */
    int getFXSoundLevel() const;

    /**
     * @brief Updates the background music volume level
     * @param level The new volume level (0-100)
     */
    void setBackgroundMusicLevel(int level);

    /**
     * @brief Updates the sound effects volume level
     * @param level The new volume level (0-100)
     */
    void setFXSoundLevel(int level);

    /**
     * @brief Get the user's Q-table for adaptive quiz
     * @return The Q-table as a map of States to maps of action IDs and Q-values
     */
    std::map<State, std::map<int, float>> getQTable() const;

    /**
     * @brief Save the user's Q-table for adaptive quiz
     * @param qTable The Q-table to save
     */
    void saveQTable(const std::map<State, std::map<int, float>>& qTable);

    /**
     * @brief Get whether this is a new user (no Q-table data yet)
     * @return true if this is a new user, false otherwise
     */
    bool isNewUser() const;

    /**
     * @brief Set whether this is a new user
     * @param isNew true if this is a new user, false otherwise
     */
    void setNewUser(bool isNew);

    /**
     * @brief Save user's current state for the adaptive quiz
     * @param state The user's current state
     */
    void saveUserState(const State& state);

    /**
     * @brief Get user's current state for the adaptive quiz
     * @return The user's current state
     */
    State getUserState() const;

    // Add method to get data
    QJsonObject getData() const { return m_data; }

private:
    /**
     * @brief Private constructor to enforce singleton pattern
     * @param parent The parent QObject
     */
    explicit LoadDataManager(QObject *parent = nullptr);

    static LoadDataManager* m_instance;  ///< The singleton instance
    QJsonObject m_data;                 ///< The loaded data
    QString m_dataFilePath;             ///< Path to the data.json file
};

#endif // LOADDATAMANAGER_H 