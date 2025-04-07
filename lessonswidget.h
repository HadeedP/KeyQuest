/**
 * @file lessonswidget.h
 * @author Alan Cruz
 * @brief Header file for the lessons game interface widget
 * @version 1.0
 * @date 2024-03-21
 * 
 * @copyright Copyright (c) 2024
 */

#ifndef LESSONSWIDGET_H
#define LESSONSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QMessageBox>
#include "lessonsgame.h"
#include <QtCore/QTimer>
#include <QSet>

/**
 * @brief Widget class for the lessons game interface
 * 
 * Provides the user interface for the lessons game mode, including
 * display of game information, handling of user input, and chord processing
 */
class LessonsWidget : public QWidget
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
     * @brief Constructs a new LessonsWidget
     * @param parent The parent widget
     * @param topicId The ID of the lesson topic to display
     */
    explicit LessonsWidget(QWidget *parent = nullptr, int topicId = GENERAL_TOPIC_ID);
    ~LessonsWidget();

public slots:
    /**
     * @brief Handles key press events from the piano
     * @param noteIndex The index of the pressed note
     */
    void handleKeyPressed(int noteIndex);

    /**
     * @brief Handles key release events from the piano
     * @param noteIndex The index of the released note
     */
    void handleKeyReleased(int noteIndex);

    /**
     * @brief Updates the game interface with current game state
     * @param playerScore The current player score
     * @param title The current question title
     * @param description The current question description
     * @param accuracy The current accuracy percentage
     */
    void updateGameUI(int playerScore, QString title, QString description, double accuracy);

    /**
     * @brief Handles the game over state
     * @param playerScore The final player score
     * @param accuracy The final accuracy percentage
     */
    void handleGameOver(int playerScore, double accuracy);

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
     */
    void submitChord();

    Lessonsgame *game;
    QLabel *titleLabel;
    QLabel *descriptionLabel;
    QLabel *playerScoreLabel;
    QLabel *accuracyLabel;
    int currentTopicId;

    // For handling chords
    QStringList currentChordNotes;
    static const int CHORD_TIMEOUT_MS = 1000;
    QTimer* chordTimer;
    QSet<int> currentlyPressedKeys;  // Track which keys are currently pressed
    bool isProcessingSubmission;     // Flag to prevent multiple rapid submissions
};
#endif // LESSONSWIDGET_H
