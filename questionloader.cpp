/**
 * @file questionloader.cpp
 * @brief Implementation of question loading functionality
 * @author Alan Cruz, Sarah Lahourpour
 * @details This file implements the function for loading questions from a JSON file,
 *          parsing the structure, and converting JSON elements into Question objects.
 */

// This will prevents the same header file to be compiled more than once
#include <fstream>
#include <iostream>  
#include <map>
#include <vector>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include "question.h"

/**
 * @brief Loads questions from a JSON file
 * @param filename Path to the JSON file containing questions
 * @return Map of question IDs to Question objects
 * @details Reads and parses a JSON file containing questions organized by topics,
 *          converting each question entry into a Question object. The function follows
 *          these steps:
 *          1. Open and read the specified JSON file
 *          2. Parse the JSON document and verify it contains a "topics" array
 *          3. For each topic in the array, extract the topic ID and name
 *          4. For each question in the topic, create a Question object and add it to the map
 *          5. Return the populated map using question IDs as keys
 */
std::map<int, Question> loadQuestionsFromFile(const QString& filename) {
    QFile file(filename);
    std::map<int, Question> questionBank;
    
    // Attempt to open the file
    if (!file.open(QIODevice::ReadOnly)) {
        // If file cannot be opened, return empty question bank
        return questionBank;
    }

    // Read all data from the file
    QByteArray fileData = file.readAll();
    file.close();

    // Parse the JSON document
    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData);
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        // If JSON parsing fails, return empty question bank
        return questionBank;
    }

    // Check for "topics" array in the JSON object
    QJsonObject jsonObj = jsonDoc.object();
    if (!jsonObj.contains("topics") || !jsonObj["topics"].isArray()) {
        // If "topics" array is missing, return empty question bank
        return questionBank;
    }

    // Process each topic in the array
    QJsonArray topicsArray = jsonObj["topics"].toArray();
    for (const QJsonValue &topicValue : topicsArray) {
        if (!topicValue.isObject()) continue;
        
        // Extract topic information
        QJsonObject topic = topicValue.toObject();
        int topicID = topic["topicID"].toInt();
        QString topicName = topic["topicName"].toString();

        // Process questions within the topic
        if (topic.contains("questions") && topic["questions"].isArray()) {
            QJsonArray questions = topic["questions"].toArray();
            
            for (const QJsonValue &qValue : questions) {
                if (!qValue.isObject()) continue;
                
                // Extract question data
                QJsonObject q = qValue.toObject();
                int qid = q["questionID"].toInt();
                QString desc = q["Description"].toString();
                QString expecIn = q["ExpectedInput"].toString();
                QString title = q["Title"].toString();
                int diff = q["difficulty"].toInt();

                // Create Question object and add to map
                Question question(qid, topicID, title, desc, expecIn, diff, topicName);
                questionBank[qid] = question;
            }
        }
    }

    return questionBank;
}
