/**
 * @file multiplayergamewidget.h
 * @author Alan Cruz
 * @brief Header file for the multiplayer game interface widget
 * @version 1.0
 * @date 2024-03-21
 * 
 * @copyright Copyright (c) 2024
 */

#ifndef MULTIPLAYERGAMEWIDGET_H
#define MULTIPLAYERGAMEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QMessageBox>
#include "multiplayergame.h"
#include <QtCore/QTimer>
#include <QSet>

/**
 * @brief Widget class for the multiplayer game interface
 * 
 * Provides the user interface for the multiplayer game mode, including
 * display of game information and handling of user input for two players
 * competing in the game. Collects notes played simultaneously and submits
 * them as a chord when all keys are released or after a timeout period.
 */
class MultiplayerGameWidget : public QWidget
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
     * @brief Constructs a new MultiplayerGameWidget
     * @param parent The parent widget
     * @param topicId The ID of the game topic to display
     */
    explicit MultiplayerGameWidget(QWidget *parent = nullptr, int topicId = GENERAL_TOPIC_ID);
    ~MultiplayerGameWidget();

public slots:
    /**
     * @brief Handles key press events from the piano
     * @param noteIndex The index of the pressed note
     * @details Collects the pressed note and starts a timer for chord submission
     */
    void handleKeyPressed(int noteIndex);

    /**
     * @brief Handles key release events from the piano
     * @param noteIndex The index of the released note
     * @details Submits the collected chord when all keys are released
     */
    void handleKeyReleased(int noteIndex);

    /**
     * @brief Updates the game interface with current game state
     * @param currentPlayer The current player number
     * @param player1Score Player 1's score
     * @param player2Score Player 2's score
     * @param title The current question title
     * @param description The current question description
     */
    void updateGameUI(int currentPlayer, int player1Score, int player2Score, QString title, QString description);

    /**
     * @brief Handles the game over state
     * @param player1Score Player 1's final score
     * @param player2Score Player 2's final score
     */
    void handleGameOver(int player1Score, int player2Score);

    /**
     * @brief Handles the chord input timeout
     * @details Submits the collected chord when no new keys are pressed for the timeout period
     */
    void handleChordTimeout();

signals:
    /**
     * @brief Signal emitted when the game is finished
     * @param topicId The ID of the completed topic
     */
    void gameFinished(int topicId);

private:
    /**
     * @brief Sets up the user interface elements
     */
    void setupUI();

    /**
     * @brief Initializes and starts the game
     */
    void startGame();

    /**
     * @brief Converts a note index to its name
     * @param noteIndex The index of the note
     * @return QString The name of the note
     */
    QString noteIndexToName(int noteIndex);

    /**
     * @brief Submits the current chord for evaluation
     * @details Formats the collected notes into a chord and submits it to the game logic
     */
    void submitChord();

    MultiplayerGame *game;
    QLabel *titleLabelLocal;
    QLabel *descriptionLabelLocal;
    QLabel *currentPlayerLabelLocal;
    QLabel *player1ScoreLabelLocal;
    QLabel *player2ScoreLabelLocal;
    QLabel *winnerLabel;
    QVBoxLayout *mainLayout;
    int currentTopicId;
    
    // For handling chords
    QStringList currentChordNotes;
    static const int CHORD_TIMEOUT_MS = 1000;  // Time window for chord input (1 second)
    QTimer* chordTimer;
    QSet<int> currentlyPressedKeys;  // Track which keys are currently pressed
    bool isProcessingSubmission;     // Flag to prevent multiple rapid submissions
};

#endif // MULTIPLAYERGAMEWIDGET_H 
