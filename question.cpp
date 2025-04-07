/**
 * @file question.cpp
 * @brief Implementation of the Question class
 * @author Alan Cruz, Sarah Lahourpour
 * @details This file implements the Question class methods, including
 *          constructors, getters, setters, and JSON serialization functions.
 */

#include "question.h"
#include <QJsonObject>

/**
 * @brief Parameterized constructor for Question
 * @param qid Question ID
 * @param tid Topic ID
 * @param qTitle Title of the question
 * @param desc Description of the question
 * @param expecIn Expected input/answer
 * @param diff Difficulty level
 * @param tname Topic name (optional)
 */
Question::Question(int qid, int tid, const QString& qTitle, const QString& desc, const QString expecIn, int diff, const QString& tname)
    : questionID(qid), 
    topicID(tid), 
    title(qTitle),
    description(desc), 
    expectedInput(expecIn),
    difficulty(diff), 
    topicName(tname) {}

/**
 * @brief JSON constructor for Question
 * @param json QJsonObject containing question data
 */
Question::Question(const QJsonObject &json) {
    fromJson(json);
}

/**
 * @brief Sets the question ID
 * @param id The new question ID
 */
void Question::setQuestionID(int id) {
    questionID = id;
}

/**
 * @brief Sets the topic ID
 * @param id The new topic ID
 */
void Question::setTopicID(int id) {
    topicID = id;
}

/**
 * @brief Sets the question description
 * @param desc The new description
 */
void Question::setDescription(const QString& desc) {
    description = desc;
}

/**
 * @brief Sets the question title
 * @param qTitle The new title
 */
void Question::setTitle(const QString& qTitle) {
    title = qTitle;
}

/**
 * @brief Sets the expected input/answer
 * @param expecIn The new expected input
 */
void Question::setExpectedInput(const QString& expecIn){
    expectedInput = expecIn;
}

/**
 * @brief Sets the difficulty level
 * @param diff The new difficulty level
 */
void Question::setDifficulty(int diff) {
    difficulty = diff;
}

/**
 * @brief Sets the topic name
 * @param name The new topic name
 */
void Question::setTopicName(const QString& name) {
    topicName = name;
}

/**
 * @brief Gets the question ID
 * @return The question ID
 */
int Question::getQuestionID() const {
    return questionID;
}

/**
 * @brief Gets the topic ID
 * @return The topic ID
 */
int Question::getTopicID() const {
    return topicID;
}

/**
 * @brief Gets the question description
 * @return The description as a QString
 */
QString Question::getDescription() const {
    return description;
}

/**
 * @brief Gets the question title
 * @return The title as a QString
 */
QString Question::getTitle() const {
    return title;
}

/**
 * @brief Gets the expected input/answer
 * @return The expected input as a QString
 */
QString Question::getExpectedInput() const {
    return expectedInput;
}

/**
 * @brief Gets the difficulty level
 * @return The difficulty level (0-2)
 */
int Question::getDifficulty() const {
    return difficulty;
}

/**
 * @brief Gets the topic name
 * @return The topic name as a QString
 */
QString Question::getTopicName() const {
    return topicName;
}

/**
 * @brief Converts the question to a JSON object
 * @return QJsonObject representation of the question
 */
QJsonObject Question::toJson() const {
    QJsonObject json;
    json["questionID"] = questionID;
    json["topicID"] = topicID;
    json["Description"] = description;
    json["ExpectedInput"] = expectedInput;
    json["Title"] = title;
    json["difficulty"] = difficulty;
    json["topicName"] = topicName;
    return json;
}

/**
 * @brief Populates the question from a JSON object
 * @param json QJsonObject containing question data
 */
void Question::fromJson(const QJsonObject &json) {
    if (json.contains("questionID")) questionID = json["questionID"].toInt();
    if (json.contains("topicID")) topicID = json["topicID"].toInt();
    if (json.contains("Description")) description = json["Description"].toString();
    if (json.contains("ExpectedInput")) expectedInput = json["ExpectedInput"].toString();
    if (json.contains("Title")) title = json["Title"].toString();
    if (json.contains("difficulty")) difficulty = json["difficulty"].toInt();
    if (json.contains("topicName")) topicName = json["topicName"].toString();
}
