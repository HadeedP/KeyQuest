/**
 * @file question.h
 * @brief Header file for the Question class
 * @author Alan Cruz, Sarah Lahourpour
 * @details This file defines the Question class which represents individual
 *          questions in the KeyQuest application. Each question contains
 *          metadata such as topic, difficulty, expected input, and descriptive
 *          text, and provides serialization to/from JSON for persistence.
 */

#ifndef QUESTION_H
#define QUESTION_H

#include <string>
#include <QJsonObject>
#include <QString>

/**
 * @brief Class representing a question in the adaptive quiz system
 * @details Encapsulates all data related to a specific question, including its
 *          identifier, topic, difficulty level, expected answer, and descriptive text.
 *          The class provides methods for serialization to and deserialization from
 *          JSON, enabling storage and retrieval in the application's data files.
 *          
 *          Questions are primarily categorized by topic (notes, chords, scales)
 *          and difficulty level, which is used by the adaptive quiz engine to
 *          select appropriate questions based on the user's current skill level.
 */
class Question {
    private:
        int questionID;       ///< Unique identifier for the question
        int topicID;          ///< Topic identifier (101=notes, 102-103=chords, 104+=scales)
        QString description;  ///< Detailed description or instruction for the question
        QString expectedInput; ///< The expected input/answer in standardized format
        QString title;        ///< Short title or summary of the question
        int difficulty;       ///< Difficulty level (0=beginner, 1=intermediate, 2=advanced)
        QString topicName;    ///< Human-readable name of the topic

    public:
        /**
         * @brief Default constructor
         * @details Creates an empty Question object with default values
         */
        Question() = default;
        
        /**
         * @brief Parameterized constructor
         * @param qid Question ID
         * @param tid Topic ID
         * @param qTitle Title of the question
         * @param desc Description of the question
         * @param expecIn Expected input/answer
         * @param diff Difficulty level
         * @param tname Topic name (optional)
         * @details Creates a Question object with specified values for all properties
         */
        Question(int qid, int tid, const QString& qTitle, const QString& desc, const QString expecIn, int diff, const QString& tname = "");
        
        /**
         * @brief JSON constructor
         * @param json QJsonObject containing question data
         * @details Creates a Question object from a JSON object representation
         */
        Question(const QJsonObject &json);

        // Setters
        /**
         * @brief Sets the question ID
         * @param id The new question ID
         */
        void setQuestionID(int id);
        
        /**
         * @brief Sets the topic ID
         * @param id The new topic ID
         */
        void setTopicID(int id);
        
        /**
         * @brief Sets the question description
         * @param desc The new description
         */
        void setDescription(const QString& desc);
        
        /**
         * @brief Sets the question title
         * @param title The new title
         */
        void setTitle(const QString& title);
        
        /**
         * @brief Sets the expected input/answer
         * @param expecIn The new expected input
         */
        void setExpectedInput(const QString& expecIn);
        
        /**
         * @brief Sets the difficulty level
         * @param diff The new difficulty level
         */
        void setDifficulty(int diff);
        
        /**
         * @brief Sets the topic name
         * @param name The new topic name
         */
        void setTopicName(const QString& name);

        // Getters
        /**
         * @brief Gets the topic ID
         * @return The topic ID
         */
        int getTopicID() const;
        
        /**
         * @brief Gets the question ID
         * @return The question ID
         */
        int getQuestionID() const;
        
        /**
         * @brief Gets the question description
         * @return The description as a QString
         */
        QString getDescription() const;
        
        /**
         * @brief Gets the question title
         * @return The title as a QString
         */
        QString getTitle() const;
        
        /**
         * @brief Gets the expected input/answer
         * @return The expected input as a QString
         */
        QString getExpectedInput() const;
        
        /**
         * @brief Gets the difficulty level
         * @return The difficulty level (0-2)
         */
        int getDifficulty() const;
        
        /**
         * @brief Gets the topic name
         * @return The topic name as a QString
         */
        QString getTopicName() const;

        // JSON methods
        /**
         * @brief Converts the question to a JSON object
         * @return QJsonObject representation of the question
         * @details Serializes all properties of the Question object into a JSON object
         *          for storage in data files or transmission
         */
        QJsonObject toJson() const;
        
        /**
         * @brief Populates the question from a JSON object
         * @param json QJsonObject containing question data
         * @details Deserializes data from a JSON object to populate the Question's properties
         */
        void fromJson(const QJsonObject &json);
};

#endif 
