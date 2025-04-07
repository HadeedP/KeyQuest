/**
 * @file datamanager.cpp
 * @brief Implementation of the DataManager class
 * @author Alan Cruz, Sarah Laharpour
 * @details This file implements the data storage and retrieval functionality
 *          for the adaptive quiz system, handling Q-tables, user states,
 *          and quiz reports through integration with LoadDataManager.
 */

#include <fstream>
#include <iostream>  
#include <map>
#include <string>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QFileInfo>
#include "adaptivequiz.h"  
#include "datamanager.h"
#include "quizreport.h"
#include "loaddatamanager.h"

/**
 * @brief Load the user's Q-table for adaptive quiz
 * @param filename Kept for API compatibility, but always uses data.json
 * @return The Q-table as a map of States to maps of action IDs and Q-values
 * @details Leverages LoadDataManager to retrieve the Q-table from the central
 *          data store, ensuring consistent data access across the application.
 */
std::map<State, std::map<int, float>> DataManager::loadQTable(const QString& filename) {
    // We'll use LoadDataManager for consistent access to data.json
    // The filename is kept for API compatibility but we always use data.json
    Q_UNUSED(filename);
    
    return LoadDataManager::instance()->getQTable();
}

/**
 * @brief Save the user's Q-table for adaptive quiz
 * @param filename Kept for API compatibility, but always uses data.json
 * @param qTable The Q-table to save
 * @details Delegates to LoadDataManager to store the Q-table in the central
 *          data store, maintaining consistent data handling throughout the application.
 */
void DataManager::saveQTable(const QString& filename, const std::map<State, std::map<int, float>>& qTable) {
    // We'll use LoadDataManager for consistent access to data.json
    // The filename is kept for API compatibility but we always use data.json
    Q_UNUSED(filename);
    
    LoadDataManager::instance()->saveQTable(qTable);
}   

/**
 * @brief Save user's current state for the adaptive quiz
 * @param filename Kept for API compatibility, but always uses data.json
 * @param state The user's current state
 * @details Stores the user's current skill level state (notes, chords, scales)
 *          using the LoadDataManager for centralized data management.
 */
void DataManager::saveState(const QString& filename, const State& state) {
    // We'll use LoadDataManager for consistent access to data.json
    // The filename is kept for API compatibility but we always use data.json
    Q_UNUSED(filename);
    
    LoadDataManager::instance()->saveUserState(state);
}

/**
 * @brief Get user's current state for the adaptive quiz
 * @param filename Kept for API compatibility, but always uses data.json
 * @return The user's current state
 * @details Retrieves the user's skill level state (notes, chords, scales)
 *          from the central data store using LoadDataManager.
 */
State DataManager::loadState(const QString& filename) {
    // We'll use LoadDataManager for consistent access to data.json
    // The filename is kept for API compatibility but we always use data.json
    Q_UNUSED(filename);
    
    return LoadDataManager::instance()->getUserState();
}

/**
 * @brief Save a quiz report to the specified file
 * @param filename The file to save the report to
 * @param quiz The quiz report to save
 * @details Serializes the quiz report to JSON and writes it to the specified file.
 *          Includes score, accuracy, question count, and history of questions asked.
 *          Silently returns without saving if the file cannot be opened.
 */
void DataManager::saveQuizReport(const QString& filename, const QuizReport& quiz) {
    QJsonObject jsonObj = quiz.toJson();
    QJsonDocument jsonDoc(jsonObj);
    
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        // File could not be opened, but we'll continue without error message
        return;
    }
    
    file.write(jsonDoc.toJson(QJsonDocument::Indented));
    file.close();
}

/**
 * @brief Load a quiz report from the specified file
 * @param filename The file to load the report from
 * @return The loaded quiz report, or an empty report if loading fails
 * @details Reads and parses a quiz report from the specified JSON file.
 *          Returns an empty QuizReport if the file cannot be opened or
 *          if the content cannot be parsed as valid JSON.
 */
QuizReport DataManager::loadQuizReport(const QString& filename) {
    QFile file(filename);
    
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        // File could not be opened, returning empty report without error message
        return QuizReport();
    }
    
    QByteArray fileData = file.readAll();
    file.close();
    
    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData);
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        // Invalid JSON, returning empty report without error message
        return QuizReport();
    }
    
    return QuizReport(jsonDoc.object());
}
    


