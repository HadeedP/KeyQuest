/**
 * @file questionloader.h
 * @brief Header file for question loading functionality
 * @author Alan Cruz, Sarah Lahourpour
 * @details This file declares the function for loading questions from a JSON file
 *          into a map of Question objects, allowing the application to access
 *          the question bank from external storage.
 */

#pragma once
#include <fstream>
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
 *          converting each question entry into a Question object. The returned map
 *          uses question IDs as keys for efficient access to specific questions.
 *          
 *          The expected JSON format is:
 *          {
 *              "topics": [
 *                  {
 *                      "topicID": 101,
 *                      "topicName": "Notes",
 *                      "questions": [
 *                          {
 *                              "questionID": 1001,
 *                              "Title": "Question Title",
 *                              "Description": "Question text",
 *                              "ExpectedInput": "expected-answer",
 *                              "difficulty": 0
 *                          },
 *                          ...
 *                      ]
 *                  },
 *                  ...
 *              ]
 *          }
 *          
 *          If the file cannot be opened or parsed, an empty map is returned.
 */
std::map<int, Question> loadQuestionsFromFile(const QString& filename);
