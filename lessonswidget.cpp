/**
 * @file lessonswidget.cpp
 * @brief Implementation of the LessonsWidget class for KeyQuest
 * @author Alan Cruz
 * @details This file implements the user interface for the lessons section of KeyQuest,
 *          handling game display, user input, and interaction with the lessons game logic.
 */

#include "lessonswidget.h"
#include "pianowidget.h"
#include <QFont>
#include <QFontDatabase>
#include <QMessageBox>
#include <QtCore/QTimer>
#include <QSizePolicy>
#include <QScreen>
#include <QApplication>
#include "loaddatamanager.h"
#include <QDebug>

/**
 * @brief Constructor for LessonsWidget
 * @param parent Pointer to the parent widget
 * @param topicId The ID of the topic to load
 * @details Initializes the lessons widget with UI elements, game logic, and timer
 *          for handling chord input. Sets up connections between game signals and UI updates.
 */
LessonsWidget::LessonsWidget(QWidget *parent, int topicId)
    : QWidget(parent)
    , game(nullptr)
    , currentTopicId(topicId)
    , chordTimer(new QTimer(this))
    , isProcessingSubmission(false)
{
    // Find the labels from the UI
    titleLabel = parent->findChild<QLabel*>("titleLabel");
    descriptionLabel = parent->findChild<QLabel*>("descriptionLabel");
    playerScoreLabel = parent->findChild<QLabel*>("playerScoreLabel");
    accuracyLabel = parent->findChild<QLabel*>("accuracyLabel");

    setupUI();
    
    // Set up chord timer
    chordTimer->setSingleShot(true);
    connect(chordTimer, &QTimer::timeout, this, &LessonsWidget::submitChord);

    game = new Lessonsgame(this, topicId);
    connect(game, &Lessonsgame::updateUI, this, &LessonsWidget::updateGameUI);
    connect(game, &Lessonsgame::gameOver, this, &LessonsWidget::handleGameOver);
    
     // Connect visual feedback signal
    auto piano = PianoWidget::instance();
    if (piano) {
        connect(game, &Lessonsgame::highlightKeys, piano, &PianoWidget::highlightAttempt);
    }
    
    game->startNewRound();
}

/**
 * @brief Destructor for LessonsWidget
 */
LessonsWidget::~LessonsWidget()
{
}

/**
 * @brief Sets up the user interface elements
 * @details Configures all UI elements including labels, fonts, layouts, and styling.
 *          Implements responsive design with scaling based on screen resolution.
 */
void LessonsWidget::setupUI()
{

    // Use system font instead of hardcoded Arial
    QFont systemFont = QFontDatabase::systemFont(QFontDatabase::GeneralFont);

    // Set up fonts with absolute sizes
    QFont titleFont = systemFont;
    titleFont.setPointSize(40);  // Direct large size for title
    titleFont.setBold(true);

    QFont descriptionFont = systemFont;
    descriptionFont.setPointSize(35);  // Direct size for description
    descriptionFont.setBold(true);

    QFont playerFont = systemFont;
    playerFont.setPointSize(30);  // Direct size for score
    playerFont.setBold(true);

    QFont accuracyFont = systemFont;
    accuracyFont.setPointSize(30);  // Direct size for accuracy
    accuracyFont.setBold(true);

    // Apply fonts to labels if they exist
    if (titleLabel) {
        titleLabel->setFont(titleFont);
        titleLabel->setStyleSheet("QLabel { color: rgb(103, 49, 0); }");
        titleLabel->setWordWrap(true);
    }

    if (descriptionLabel) {
        descriptionLabel->setFont(descriptionFont);
        descriptionLabel->setStyleSheet("QLabel { color: rgb(103, 49, 0); }");
        descriptionLabel->setWordWrap(true);
    }

    if (playerScoreLabel) {
        playerScoreLabel->setFont(playerFont);
        playerScoreLabel->setStyleSheet("QLabel { color: rgb(103, 49, 0); }");
    }

    if (accuracyLabel) {
        accuracyLabel->setFont(accuracyFont);
        accuracyLabel->setStyleSheet("QLabel { color: rgb(103, 49, 0); }");
    }

    // Ensure piano is properly attached
    auto piano = PianoWidget::instance();
    if (piano) {
        // Find the piano placeholder in the parent widget
        QFrame* pianoPlaceholder = parentWidget()->findChild<QFrame*>("pianoLocalPlaceholder");
        if (pianoPlaceholder) {
            piano->attachToPlaceholder(pianoPlaceholder);
            connect(piano, &PianoWidget::keyPressed, this, &LessonsWidget::handleKeyPressed);
            connect(piano, &PianoWidget::keyReleased, this, &LessonsWidget::handleKeyReleased);
        }
    }

    // Setup chord timer
    chordTimer = new QTimer(this);
    chordTimer->setSingleShot(true);
    connect(chordTimer, &QTimer::timeout, this, &LessonsWidget::handleChordTimeout);
}

/**
 * @brief Starts a new game round
 * @details Triggers the start of a new round in the game logic.
 */
void LessonsWidget::startGame()
{
    game->startNewRound();
}

/**
 * @brief Converts a MIDI note index to its corresponding note name
 * @param noteIndex The MIDI note number (60-72)
 * @return The note name as a string (e.g., "C4", "D#4")
 * @details Maps MIDI note numbers to their corresponding note names in the fourth octave.
 */
QString LessonsWidget::noteIndexToName(int noteIndex)
{
    // MIDI note numbers to note names mapping
    // White keys: 60, 62, 64, 65, 67, 69, 71, 72 (C4 to C5)
    // Black keys: 61, 63, 66, 68, 70 (C#4 to A#4)
    switch (noteIndex) {
    case 60: return "C4";
    case 61: return "C#4";
    case 62: return "D4";
    case 63: return "D#4";
    case 64: return "E4";
    case 65: return "F4";
    case 66: return "F#4";
    case 67: return "G4";
    case 68: return "G#4";
    case 69: return "A4";
    case 70: return "A#4";
    case 71: return "B4";
    case 72: return "C5";
    default: return QString();
    }
}

/**
 * @brief Handles keyboard input for note playing
 * @param noteIndex The MIDI note number pressed
 * @details Processes keyboard input and converts it to note names for game logic.
 */
void LessonsWidget::handleKeyPressed(int noteIndex)
{
    // If we're currently processing a submission, ignore new key presses
    if (isProcessingSubmission) {
        return;
    }

    // Add to currently pressed keys set
    currentlyPressedKeys.insert(noteIndex);
    
    // Collect the new note
    QString noteName = noteIndexToName(noteIndex);
    qDebug() << "LessonsWidget: Key pressed - MIDI index:" << noteIndex << "Note name:" << noteName;
    
    if (!noteName.isEmpty()) {
        // Keep the original formatting of the note from noteIndexToName
        if (!currentChordNotes.contains(noteName)) {
            currentChordNotes.append(noteName);
            qDebug() << "LessonsWidget: Added note to chord:" << noteName << "- Current chord:" << currentChordNotes.join("-");
        }
    }
    
    // Restart the timer with a longer window (1 second)
    chordTimer->start(1000);
}

void LessonsWidget::handleKeyReleased(int noteIndex)
{
    // Get the note name for debugging
    QString noteName = noteIndexToName(noteIndex);
    qDebug() << "LessonsWidget: Key released - MIDI index:" << noteIndex << "Note name:" << noteName;
    
    // Remove from currently pressed keys set
    currentlyPressedKeys.remove(noteIndex);
    
    // If all keys are released, submit the collected notes
    if (currentlyPressedKeys.isEmpty()) {
        qDebug() << "LessonsWidget: All keys released, submitting chord:" << currentChordNotes.join("-");
        isProcessingSubmission = false;
        submitChord();
    }
}

/**
 * @brief Updates the game UI with current game state
 * @param playerScore The current player's score
 * @param title The current question title
 * @param description The current question description
 * @param playerAccuracy The current player's accuracy percentage
 * @details Updates all UI elements to reflect the current game state.
 */
void LessonsWidget::updateGameUI(int playerScore, QString title, QString description, double playerAccuracy)
{
    // Safety check - if game is deleted or over, don't update UI
    if (!game) {
        return;
    }

    // Clear any existing chord notes when updating UI (new pattern)
    currentChordNotes.clear();

    titleLabel->setText(title.toUpper());
    descriptionLabel->setText(description);

    // Update scores with more prominent formatting
    playerScoreLabel->setText(QString("%1").arg(playerScore));
    playerScoreLabel->setStyleSheet("QLabel { color: rgb(103, 49, 0); }");

    // Update accuracy with percentage
    accuracyLabel->setText(QString("%1%").arg(QString::number(playerAccuracy, 'f', 0)));
    accuracyLabel->setStyleSheet("QLabel { color: rgb(103, 49, 0); }");
}

/**
 * @brief Handles the end of the game
 * @param playerScore The final player score
 * @param accuracy The final accuracy percentage
 * @details Cleans up resources and emits the game finished signal.
 */
void LessonsWidget::handleGameOver(int playerScore, double accuracy)
{
    // Stop and disconnect the chord timer first
    if (chordTimer) {
        chordTimer->stop();
        disconnect(chordTimer, nullptr, this, nullptr);
    }

    // Clear any pending chord notes
    currentChordNotes.clear();

    // Disconnect piano signals first
    auto piano = PianoWidget::instance();
    if (piano) {
        disconnect(piano, &PianoWidget::keyPressed, this, &LessonsWidget::handleKeyPressed);
        disconnect(piano, &PianoWidget::keyReleased, this, &LessonsWidget::handleKeyReleased);
    }

    // Update UI elements one last time to show final state
    titleLabel->setText("Lesson Complete!");
    if (accuracy >= 90) {
        descriptionLabel->setText("Congratulations! You've mastered this lesson.");
    } else if (accuracy >= 70) {
        descriptionLabel->setText("Good job! You've learned a lot. Keep practicing!");
    } else {
        descriptionLabel->setText("Practice leads to mastery! Keep going!");
    }

    playerScoreLabel->setText(QString("%1").arg(playerScore));
    accuracyLabel->setText(QString("%1%").arg(QString::number(accuracy, 'f', 1)));

    // Update the JSON file with lesson statistics
    LoadDataManager::instance()->updateLessonStats(
        currentTopicId,
        playerScore,
        accuracy,
        1  // Increment attempts by 1
    );

    // Clean up the game object last, after all UI updates and message box
    if (game) {
        disconnect(game, nullptr, this, nullptr);
        game->deleteLater();
        game = nullptr;
    }

    // Emit signal with the current topic ID
    emit gameFinished(currentTopicId);
}

/**
 * @brief Submits a collected chord to the game logic
 * @details Processes collected notes into a chord and submits it to the game.
 *          Clears the chord buffer after submission.
 */
void LessonsWidget::submitChord()
{
    if (currentChordNotes.isEmpty()) {
        return;
    }

    // Sort the notes
    currentChordNotes.sort();
    QString attempt = currentChordNotes.join("-");
    qDebug() << "LessonsWidget: Submitting chord:" << attempt;
    
    // Submit the attempt and let the game logic handle validation
    game->playerAttempt(attempt);
    
    // Clear the chord buffer
    currentChordNotes.clear();
}

/**
 * @brief Handles the chord input timeout
 * @details Submits the collected chord when no new keys are pressed for the timeout period
 */
void LessonsWidget::handleChordTimeout()
{
    // If we have collected notes and we're not currently processing a submission,
    // submit the chord
    if (!currentChordNotes.isEmpty() && !isProcessingSubmission) {
        qDebug() << "LessonsWidget: Chord timeout - submitting chord:" << currentChordNotes.join("-");
        submitChord();
    }
} 
