/**
 * @file multiplayergame.h
 * @author Alan Cruz, Hadeed Pall
 * @brief Header file for the multiplayer game logic
 * @version 1.0
 * @date 2024-03-21
 * 
 * @copyright Copyright (c) 2024
 */

#ifndef MULTIPLAYERGAME_H
#define MULTIPLAYERGAME_H

#include <QtCore/QString>
#include <QtCore/QObject>
#include <QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QVector>

/**
 * @brief Structure representing a game question
 */
struct MultiplayerQuestion {
    int questionID;
    QString title;
    QString description;
    QString expectedInput;
};

/**
 * @brief Class managing the multiplayer game logic
 * 
 * Handles game state, question management, scoring, and turn management
 * for two players competing in the game
 */
class MultiplayerGame: public QObject
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
     * @brief Constructs a new MultiplayerGame object
     * @param parent The parent QObject
     * @param topicID The ID of the game topic to load
     */
    explicit MultiplayerGame(QObject *parent = nullptr, int topicID = GENERAL_TOPIC_ID);
    ~MultiplayerGame();

    /**
     * @brief Starts a new round of the game
     */
    void startNewRound();

    /**
     * @brief Processes a player's attempt at answering
     * @param playedPattern The player's input attempt
     * @return bool True if the attempt was correct, false otherwise
     */
    bool playerAttempt(const QString& playedPattern);

    /**
     * @brief Gets the current active player number
     * @return int The current player (1 or 2)
     */
    int getCurrentPlayer() const;

    /**
     * @brief Gets player 1's current score
     * @return int Player 1's score
     */
    int getPlayer1Score() const;

    /**
     * @brief Gets player 2's current score
     * @return int Player 2's score
     */
    int getPlayer2Score() const;

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

    int noteNameToValue(const QString& noteName);

signals:
    /**
     * @brief Signal emitted when the UI needs to be updated
     * @param currentPlayer The current player number
     * @param player1Score Player 1's score
     * @param player2Score Player 2's score
     * @param currentPattern The current pattern to play
     * @param description The current question description
     */
    void updateUI(int currentPlayer, int player1Score, int player2Score, QString currentPattern, QString description);

    /**
     * @brief Signal emitted when the game ends
     * @param player1Score Player 1's final score
     * @param player2Score Player 2's final score
     */
    void gameOver(int player1Score, int player2Score);

    /**
     * @brief Signal emitted for key highlighting feedback
     * @param isCorrect True if the answer was correct, false otherwise
     */
    void highlightKeys(bool isCorrect);

private:
    /**
     * @brief Switches turn to the next player
     */
    void switchTurn();

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

    int currentPlayer;
    int player1Score;
    int player2Score;
    QString currentPattern;
    QString currentDescription;
    
    QVector<MultiplayerQuestion> questions;
    int currentQuestionIndex;
    bool gameEnded;
};

#endif // MULTIPLAYERGAME_H
