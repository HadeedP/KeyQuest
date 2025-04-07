/**
 * @file quizreport.h
 * @brief Header file for the QuizReport structure
 * @author Alan Cruz, Sarah Lahourpour
 * @details This file defines the QuizReport structure which encapsulates the results
 *          and history of a completed quiz session. It stores score, accuracy, 
 *          question count information, and a detailed history of all quiz interactions,
 *          with serialization to/from JSON for persistence.
 */

#pragma once
#include <vector>     // for std::vector
#include <tuple>      // for std::tuple
#include <string>     // for std::string
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include "state.h"

/**
 * @brief Structure representing a quiz session report
 * @details Stores the results of a completed quiz session, including the final score,
 *          accuracy, question counts, and a detailed history of questions answered.
 *          The history includes the user's state at the time of each question, the
 *          question ID, its description, and whether it was answered correctly.
 *          
 *          This structure supports serialization to and deserialization from JSON,
 *          enabling persistent storage of quiz reports for user analytics and review.
 */
struct QuizReport {
    float score;        ///< Final score achieved in the quiz
    float accuracy;     ///< Percentage of correctly answered questions
    int totalQuestions; ///< Total number of questions answered
    int correctAnswers; ///< Number of correctly answered questions

    /**
     * @brief History of quiz interactions
     * @details Each entry is a tuple containing:
     *          - User's state when the question was asked
     *          - Question ID
     *          - Question description
     *          - Boolean indicating if the answer was correct
     */
    std::vector<std::tuple<State, int, QString, bool>> history;

    /**
     * @brief Default constructor
     * @details Creates an empty quiz report with zero values
     */
    QuizReport() 
        : score(0.0f), accuracy(0.0f), totalQuestions(0), correctAnswers(0) {}

    /**
     * @brief Parameterized constructor
     * @param sc Score achieved in the quiz
     * @param acc Accuracy percentage
     * @param totalQ Total number of questions answered
     * @param correctQ Number of correctly answered questions
     * @param hist History of quiz interactions
     * @details Creates a QuizReport with specified values for all properties
     */
    QuizReport(float sc, float acc, int totalQ, int correctQ,
               const std::vector<std::tuple<State, int, QString, bool>>& hist)
        : score(sc), accuracy(acc), totalQuestions(totalQ),
          correctAnswers(correctQ), history(hist) {}
    
    /**
     * @brief JSON constructor
     * @param json QJsonObject containing quiz report data
     * @details Creates a QuizReport by deserializing data from a JSON object
     */
    QuizReport(const QJsonObject &json) {
        fromJson(json);
    }

    /**
     * @brief Gets the final score
     * @return The score as a float
     */
    float getScore() const { return score; }
    
    /**
     * @brief Gets the accuracy percentage
     * @return The accuracy as a float (0-100)
     */
    float getAccuracy() const { return accuracy; }
    
    /**
     * @brief Gets the total number of questions answered
     * @return The question count as an integer
     */
    int getTotalQuestions() const { return totalQuestions; }
    
    /**
     * @brief Gets the number of correctly answered questions
     * @return The correct answer count as an integer
     */
    int getCorrectAnswers() const { return correctAnswers; }

    /**
     * @brief Gets the history of quiz interactions
     * @return Vector of tuples containing state, question ID, description, and correctness
     */
    const std::vector<std::tuple<State, int, QString, bool>>& getHistory() const {
        return history;
    }
    
    /**
     * @brief Converts the quiz report to a JSON object
     * @return QJsonObject representation of the quiz report
     * @details Serializes all properties of the QuizReport into a JSON object for
     *          storage in data files or transmission. The history is converted to
     *          a JSON array with each entry containing the state, question ID,
     *          description, and correctness.
     */
    QJsonObject toJson() const {
        QJsonObject json;
        json["score"] = score;
        json["accuracy"] = accuracy;
        json["totalQuestions"] = totalQuestions;
        json["correctAnswers"] = correctAnswers;
        
        QJsonArray historyArray;
        for (const auto& [state, questionID, description, correct] : history) {
            QJsonObject entryObj;
            entryObj["state"] = state.toJson();
            entryObj["questionID"] = questionID;
            entryObj["description"] = description;
            entryObj["correct"] = correct;
            historyArray.append(entryObj);
        }
        json["history"] = historyArray;
        
        return json;
    }
    
    /**
     * @brief Populates the quiz report from a JSON object
     * @param json QJsonObject containing quiz report data
     * @details Deserializes data from a JSON object to populate the QuizReport's properties.
     *          Extracts score, accuracy, question counts, and rebuilds the history array
     *          by converting each history entry from its JSON representation.
     */
    void fromJson(const QJsonObject &json) {
        if (json.contains("score")) score = json["score"].toDouble();
        if (json.contains("accuracy")) accuracy = json["accuracy"].toDouble();
        if (json.contains("totalQuestions")) totalQuestions = json["totalQuestions"].toInt();
        if (json.contains("correctAnswers")) correctAnswers = json["correctAnswers"].toInt();
        
        history.clear();
        if (json.contains("history") && json["history"].isArray()) {
            QJsonArray historyArray = json["history"].toArray();
            for (const QJsonValue &value : historyArray) {
                if (value.isObject()) {
                    QJsonObject entry = value.toObject();
                    
                    State state;
                    if (entry.contains("state") && entry["state"].isObject()) {
                        state.fromJson(entry["state"].toObject());
                    }
                    
                    int questionID = entry["questionID"].toInt(-1);
                    QString description = entry["description"].toString();
                    bool correct = entry["correct"].toBool(false);
                    
                    history.emplace_back(state, questionID, description, correct);
                }
            }
        }
    }
};

