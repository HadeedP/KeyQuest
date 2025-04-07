/**
 * @file datamanager.h
 * @brief Header file for the DataManager class
 * @author Alan Cruz, Sarah Laharpour
 * @details This file defines the DataManager class which serves as a centralized
 *          interface for persistent data operations in the KeyQuest application.
 *          It provides functionality for loading and saving Q-tables, user states,
 *          and quiz reports, delegating to the LoadDataManager for consistent
 *          data storage in the application's central data file.
 */

#pragma once
#include <string>
#include <map>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QFile>
#include "question.h"
#include "state.h"
#include "quizreport.h"

/**
 * @brief Manages loading and saving data for the adaptive quiz system
 * @details This static utility class handles the persistence of various data structures
 *          used by the adaptive quiz system. It primarily serves as a wrapper around
 *          LoadDataManager, directing most operations to the application's central
 *          data store for consistent data handling.
 *          
 *          The class provides functionality for:
 *          - Saving and loading Q-tables, which store the reinforcement learning data
 *          - Managing user skill states across the notes, chords, and scales domains
 *          - Serializing and deserializing quiz reports for record keeping
 *          
 *          While most methods accept a filename parameter for API compatibility,
 *          actual Q-table and state operations are always directed to the central
 *          data.json file managed by LoadDataManager, maintaining data consistency.
 */
class DataManager {
public:
    /**
     * @brief Load the user's Q-table for adaptive quiz
     * @param filename Kept for API compatibility, but always uses data.json
     * @return The Q-table as a map of States to maps of action IDs and Q-values
     * @details Leverages LoadDataManager to retrieve the Q-table from the central
     *          data store, ensuring consistent data access across the application.
     *          The returned Q-table maps user skill states to question IDs and
     *          their corresponding Q-values, which guide question selection in the
     *          adaptive quiz engine.
     */
    static std::map<State, std::map<int, float>> loadQTable(const QString& filename);
    
    /**
     * @brief Save the user's Q-table for adaptive quiz
     * @param filename Kept for API compatibility, but always uses data.json
     * @param qTable The Q-table to save
     * @details Delegates to LoadDataManager to store the Q-table in the central
     *          data store, maintaining consistent data handling throughout the application.
     *          This preserves the learning progress of the adaptive quiz engine between
     *          sessions, allowing for continuous improvement of question selection.
     */
    static void saveQTable(const QString& filename, const std::map<State, std::map<int, float>>& qTable);

    /**
     * @brief Save user's current state for the adaptive quiz
     * @param filename Kept for API compatibility, but always uses data.json
     * @param state The user's current state
     * @details Stores the user's current skill level state (notes, chords, scales)
     *          using the LoadDataManager for centralized data management. This enables
     *          the application to remember the user's skill levels across different
     *          domains, providing a personalized learning experience in future sessions.
     */
    static void saveState(const QString& filename, const State& state);
    
    /**
     * @brief Get user's current state for the adaptive quiz
     * @param filename Kept for API compatibility, but always uses data.json
     * @return The user's current state
     * @details Retrieves the user's skill level state (notes, chords, scales)
     *          from the central data store using LoadDataManager. This allows
     *          the adaptive quiz to continue with the appropriate difficulty level
     *          based on the user's previously demonstrated skills.
     */
    static State loadState(const QString& filename);

    /**
     * @brief Save a quiz report to the specified file
     * @param filename The file to save the report to
     * @param quiz The quiz report to save
     * @details Serializes the quiz report to JSON and writes it to the specified file.
     *          The report includes score, accuracy, total questions answered,
     *          correct answers, and a history containing all questions presented
     *          during the quiz session with their responses. Unlike Q-table and state
     *          operations, this method does use the provided filename to store
     *          individual quiz reports separately.
     */
    static void saveQuizReport(const QString& filename, const QuizReport& quiz);
    
    /**
     * @brief Load a quiz report from the specified file
     * @param filename The file to load the report from
     * @return The loaded quiz report, or an empty report if loading fails
     * @details Reads and parses a quiz report from the specified JSON file.
     *          Returns a QuizReport object populated with the historical quiz data,
     *          enabling review of past performance and learning progress.
     *          If the file cannot be opened or doesn't contain valid JSON,
     *          an empty QuizReport is returned with no error message.
     */
    static QuizReport loadQuizReport(const QString& filename);
};
