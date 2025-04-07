/**
 * @file loaddatamanager.cpp
 * @brief Implementation of the LoadDataManager class
 * @author Alan Cruz, Bashar Hamo
 * @details This file implements the LoadDataManager class which handles loading and saving
 *          of application data, including settings and lesson statistics.
 */

#include "loaddatamanager.h"
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

// Initialize static member
LoadDataManager* LoadDataManager::m_instance = nullptr;

/**
 * @brief Gets the singleton instance of the LoadDataManager
 * @return Pointer to the LoadDataManager instance
 */
LoadDataManager* LoadDataManager::instance()
{
    if (!m_instance) {
        m_instance = new LoadDataManager();
    }
    return m_instance;
}

/**
 * @brief Constructs a new LoadDataManager
 * @param parent The parent QObject
 */
LoadDataManager::LoadDataManager(QObject *parent)
    : QObject(parent)
{
    // Set up the data file path in the user's local app data directory
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(appDataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
        qDebug() << "Created app data directory at:" << appDataPath;
    }
    m_dataFilePath = dir.filePath("data.json");
    qDebug() << "User data file location:" << m_dataFilePath;

    // Try to load existing user data
    QFile userDataFile(m_dataFilePath);
    if (!userDataFile.exists()) {
        // If no user data file exists, copy the initial data from resources
        QFile resourceFile(":/resources/data.json");
        if (resourceFile.open(QIODevice::ReadOnly)) {
            QByteArray initialData = resourceFile.readAll();
            resourceFile.close();

            // Save the initial data to the user's data file
            if (userDataFile.open(QIODevice::WriteOnly)) {
                userDataFile.write(initialData);
                userDataFile.close();
                qDebug() << "Copied initial data from resources to:" << m_dataFilePath;
            }
        }
    }

    // Now load the data (either existing user data or newly copied initial data)
    if (!loadData()) {
        qDebug() << "Failed to load data, creating default structure";
        // Create default data structure as fallback
        m_data = QJsonObject{
            {"lessons", QJsonObject{
                {"topics", QJsonObject{
                    {"101", QJsonObject{{"statistics", QJsonObject{{"scores", QJsonArray()}, {"accuracy", QJsonArray()}, {"attempts", QJsonArray()}}}}},
                    {"102", QJsonObject{{"statistics", QJsonObject{{"scores", QJsonArray()}, {"accuracy", QJsonArray()}, {"attempts", QJsonArray()}}}}},
                    {"103", QJsonObject{{"statistics", QJsonObject{{"scores", QJsonArray()}, {"accuracy", QJsonArray()}, {"attempts", QJsonArray()}}}}},
                    {"104", QJsonObject{{"statistics", QJsonObject{{"scores", QJsonArray()}, {"accuracy", QJsonArray()}, {"attempts", QJsonArray()}}}}},
                    {"105", QJsonObject{{"statistics", QJsonObject{{"scores", QJsonArray()}, {"accuracy", QJsonArray()}, {"attempts", QJsonArray()}}}}},
                    {"106", QJsonObject{{"statistics", QJsonObject{{"scores", QJsonArray()}, {"accuracy", QJsonArray()}, {"attempts", QJsonArray()}}}}}
                }}
            }},
            {"settings", QJsonObject{
                {"backgroundMusicLevel", 100},
                {"fxsoundLevel", 100}
            }},
            {"qtable", QJsonObject{
                {"newUser", true},
                {"table", QJsonObject{}},
                {"userState", QJsonObject{
                    {"notes", 0},
                    {"chords", 0},
                    {"scales", 0}
                }}
            }}
        };
        saveData();
    }
}

/**
 * @brief Loads all data from the data.json file
 * @return true if loading was successful, false otherwise
 */
bool LoadDataManager::loadData()
{
    QFile file(m_dataFilePath);
    if (!file.exists()) {
        qDebug() << "Data file does not exist at:" << m_dataFilePath;
        return false;
    }
    
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open data.json file for reading at:" << m_dataFilePath;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qDebug() << "Invalid JSON format in data.json at:" << m_dataFilePath;
        return false;
    }

    m_data = doc.object();
    
    // Ensure the qtable structure exists
    if (!m_data.contains("qtable")) {
        m_data["qtable"] = QJsonObject{
            {"newUser", true},
            {"table", QJsonObject{}},
            {"userState", QJsonObject{
                {"notes", 0},
                {"chords", 0},
                {"scales", 0}
            }}
        };
        saveData();
    }
    
    qDebug() << "Successfully loaded data from:" << m_dataFilePath;
    return true;
}

/**
 * @brief Saves all data back to the data.json file
 * @return true if saving was successful, false otherwise
 */
bool LoadDataManager::saveData()
{
    QFile file(m_dataFilePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to open data.json file for writing";
        return false;
    }

    QJsonDocument doc(m_data);
    QByteArray data = doc.toJson(QJsonDocument::Indented);
    qint64 bytesWritten = file.write(data);
    file.close();

    return bytesWritten != -1;
}

/**
 * @brief Updates lesson statistics for a specific topic
 * @param topicId The ID of the topic
 * @param score The score achieved
 * @param accuracy The accuracy percentage
 * @param attempts The number of attempts made
 */
void LoadDataManager::updateLessonStats(int topicId, int score, double accuracy, int attempts)
{
    // Convert topicId to string format (e.g., "101")
    QString topicStr = QString::number(topicId);

    // Navigate to the correct topic in the JSON structure
    QJsonObject lessons = m_data["lessons"].toObject();
    QJsonObject topics = lessons["topics"].toObject();
    QJsonObject topicData = topics[topicStr].toObject();
    QJsonObject stats = topicData["statistics"].toObject();

    // Update the statistics arrays
    QJsonArray scores = stats["scores"].toArray();
    QJsonArray accuracies = stats["accuracy"].toArray();
    QJsonArray attemptsArray = stats["attempts"].toArray();

    scores.append(score);
    accuracies.append(accuracy);
    attemptsArray.append(attempts);

    stats["scores"] = scores;
    stats["accuracy"] = accuracies;
    stats["attempts"] = attemptsArray;

    topicData["statistics"] = stats;
    topics[topicStr] = topicData;
    lessons["topics"] = topics;
    m_data["lessons"] = lessons;

    // Save the updated data
    saveData();
}

/**
 * @brief Gets the background music volume level
 * @return int The volume level (0-100)
 */
int LoadDataManager::getBackgroundMusicLevel() const
{
    return m_data["settings"].toObject()["backgroundMusicLevel"].toInt();
}

/**
 * @brief Gets the sound effects volume level
 * @return int The volume level (0-100)
 */
int LoadDataManager::getFXSoundLevel() const
{
    return m_data["settings"].toObject()["fxsoundLevel"].toInt();
}

/**
 * @brief Updates the background music volume level
 * @param level The new volume level (0-100)
 */
void LoadDataManager::setBackgroundMusicLevel(int level)
{
    QJsonObject settings = m_data["settings"].toObject();
    settings["backgroundMusicLevel"] = level;
    m_data["settings"] = settings;
    saveData();
}

/**
 * @brief Updates the sound effects volume level
 * @param level The new volume level (0-100)
 */
void LoadDataManager::setFXSoundLevel(int level)
{
    QJsonObject settings = m_data["settings"].toObject();
    settings["fxsoundLevel"] = level;
    m_data["settings"] = settings;
    saveData();
}

/**
 * @brief Get the user's Q-table for adaptive quiz
 * @return The Q-table as a map of States to maps of action IDs and Q-values
 */
std::map<State, std::map<int, float>> LoadDataManager::getQTable() const
{
    std::map<State, std::map<int, float>> qTable;
    
    // Get the QTable from the JSON data
    QJsonObject qtableObj = m_data["qtable"].toObject();
    QJsonObject tableObj = qtableObj["table"].toObject();
    
    // Iterate through each state in the Q-table
    for (auto stateIt = tableObj.begin(); stateIt != tableObj.end(); ++stateIt) {
        QString stateKey = stateIt.key();
        State state = State::fromKey(stateKey);
        
        // Get the actions for this state
        if (stateIt.value().isObject()) {
            QJsonObject actionMap = stateIt.value().toObject();
            
            // Iterate through each action for this state
            for (auto actionIt = actionMap.begin(); actionIt != actionMap.end(); ++actionIt) {
                QString actionKey = actionIt.key();
                // Parse the question ID from the action key (format: "[id]")
                QString processedKey = actionKey.mid(1, actionKey.length() - 2);
                int questionID = processedKey.toInt();
                float qValue = actionIt.value().toDouble();
                
                // Add to our map
                qTable[state][questionID] = qValue;
            }
        }
    }
    
    return qTable;
}

/**
 * @brief Save the user's Q-table for adaptive quiz
 * @param qTable The Q-table to save
 */
void LoadDataManager::saveQTable(const std::map<State, std::map<int, float>>& qTable)
{
    // Get the qtable object from the data
    QJsonObject qtableObj = m_data["qtable"].toObject();
    QJsonObject tableObj;
    
    // Convert the Q-table map to JSON format
    for (const auto& [state, actionMap] : qTable) {
        QString stateKey = state.toKey();
        QJsonObject actionObj;
        
        for (const auto& [questionID, qValue] : actionMap) {
            QString actionKey = QString("[%1]").arg(questionID);
            actionObj[actionKey] = qValue;
        }
        
        tableObj[stateKey] = actionObj;
    }
    
    // Update the Q-table in our data
    qtableObj["table"] = tableObj;
    
    // If we're saving a Q-table, the user is no longer new
    qtableObj["newUser"] = false;
    
    // Update the main data object
    m_data["qtable"] = qtableObj;
    
    // Save changes to disk
    saveData();
}

/**
 * @brief Get whether this is a new user (no Q-table data yet)
 * @return true if this is a new user, false otherwise
 */
bool LoadDataManager::isNewUser() const
{
    QJsonObject qtableObj = m_data["qtable"].toObject();
    return qtableObj["newUser"].toBool(true);
}

/**
 * @brief Set whether this is a new user
 * @param isNew true if this is a new user, false otherwise
 */
void LoadDataManager::setNewUser(bool isNew)
{
    QJsonObject qtableObj = m_data["qtable"].toObject();
    qtableObj["newUser"] = isNew;
    m_data["qtable"] = qtableObj;
    saveData();
}

/**
 * @brief Save user's current state for the adaptive quiz
 * @param state The user's current state
 */
void LoadDataManager::saveUserState(const State& state)
{
    QJsonObject qtableObj = m_data["qtable"].toObject();
    qtableObj["userState"] = state.toJson();
    m_data["qtable"] = qtableObj;
    saveData();
}

/**
 * @brief Get user's current state for the adaptive quiz
 * @return The user's current state
 */
State LoadDataManager::getUserState() const
{
    QJsonObject qtableObj = m_data["qtable"].toObject();
    QJsonObject stateObj = qtableObj["userState"].toObject();
    
    State state;
    state.fromJson(stateObj);
    return state;
} 