/**
 * @file lessonsgame.h
 * @author Alan Cruz, Liam Elliott
 * @brief Header file for the lessons game logic
 * @version 1.0
 * @date 2024-03-21
 * 
 * @copyright Copyright (c) 2024
 */

#ifndef LESSONSGAME_H
#define LESSONSGAME_H

#include <QtCore/QString>
#include <QtCore/QObject>
#include <QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QVector>

/**
 * @brief Structure representing a game question
 */
struct QuestionLesson {
    int questionID;
    QString title;
    QString description;
    QString expectedInput;
};

/**
 * @brief Class managing the lessons game logic
 * 
 * Handles game state, question management, scoring, and accuracy tracking
 * for the lessons mode of the application
 */
class Lessonsgame: public QObject
{
    Q_OBJECT
public:
    // Topic IDs
    static const int GENERAL_TOPIC_ID = 101;
    static const int MAJOR_MINOR_CHORDS_TOPIC_ID = 102;
    static const int TRIAD_TOPIC_ID = 103;
    static const int MAJOR_SCALE_ID = 104;
    static const int PERFECT_ID = 105;
    static const int MELODY_ID = 106;

    /**
     * @brief Constructs a new Lessonsgame object
     * @param parent The parent QObject
     * @param topicID The ID of the game topic to load
     */
    explicit Lessonsgame(QObject *parent = nullptr, int topicID = GENERAL_TOPIC_ID);
    ~Lessonsgame();

    /**
     * @brief Starts a new round of the game
     */
    void startNewRound();

    /**
     * @brief Processes a player's attempt at answering
     * @param playedPattern The player's input attempt
     */
    void playerAttempt(const QString& playedPattern);

    /**
     * @brief Gets the current player's score
     * @return int The current player score
     */
    int getPlayerScore() const;

    /**
     * @brief Gets the current pattern to be played
     * @return QString The current pattern string
     */
    QString getCurrentPattern() const;

    /**
     * @brief Gets the current question description
     * @return QString The current question description
     */
    QString getCurrentDescription() const;

    /**
     * @brief Checks if the game has ended
     * @return bool true if the game is over, false otherwise
     */
    bool isGameOver() const;

    /**
     * @brief Gets the current accuracy percentage
     * @return double The accuracy as a percentage (0-100)
     */
    double getAccuracy() const;

    /**
     * @brief Gets the total number of attempts made
     * @return int The total number of attempts
     */
    int getTotalAttempts() const;

    int noteNameToValue(const QString& noteName);

signals:
    /**
     * @brief Signal emitted when the UI needs to be updated
     * @param playerScore The current player score
     * @param title The current question title
     * @param description The current question description
     * @param accuracy The current accuracy percentage
     */
    void updateUI(int playerScore, QString title, QString description, double accuracy);

    /**
     * @brief Signal emitted when the game ends
     * @param playerScore The final player score
     * @param accuracy The final accuracy percentage
     */
    void gameOver(int playerScore, double accuracy);

    /**
     * @brief Signal emitted for key highlighting feedback
     * @param isCorrect True if the answer was correct, false otherwise
     */
    void highlightKeys(bool isCorrect);

private:
    /**
     * @brief Loads questions from the question bank
     * @param topicID The ID of the topic to load questions for
     */
    void loadQuestions(int topicID);

    /**
     * @brief Randomly shuffles the order of questions
     */
    void shuffleQuestions();

    /**
     * @brief Compares two chords for equality
     * @param chord1 First chord string
     * @param chord2 Second chord string
     * @return true if chords contain the same notes, false otherwise
     */
    bool areChordsSame(const QString& chord1, const QString& chord2);

    /**
     * @brief Advances to the next question
     * @details Increments the question index and either starts a new round or ends the game.
     */
    void moveToNextQuestion();

    QVector<QuestionLesson> questions;
    int playerScore;
    int currentQuestionIndex;
    bool gameEnded;
    QString currentPattern;
    QString currentDescription;
    int correctAnswers;
    int totalAttempts;
    double accuracy;
};

#endif // LESSONSGAME_H
