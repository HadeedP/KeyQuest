/**
 * @file multiplayergamewidget.cpp
 * @brief Implementation of the MultiplayerGameWidget class for KeyQuest
 * @author Alan Cruz, Liam Elliott
 * @details This file implements the user interface for the multiplayer game mode,
 *          handling game display, player input, and interaction with the multiplayer game logic.
 */

#include "multiplayergamewidget.h"
#include "pianowidget.h"
#include <QFont>
#include <QFontDatabase>
#include <QMessageBox>
#include <QTimer>
#include <QSizePolicy>
#include <QScreen>
#include <QApplication>
#include <QDebug>

/**
 * @brief Constructor for MultiplayerGameWidget
 * @param parent Pointer to the parent widget
 * @param topicId The ID of the topic to load
 * @details Initializes the multiplayer game widget with UI elements, game logic,
 *          and timer for handling chord input. Sets up connections between game
 *          signals and UI updates.
 */
MultiplayerGameWidget::MultiplayerGameWidget(QWidget *parent, int topicId)
    : QWidget(parent)
    , game(nullptr)
    , winnerLabel(new QLabel(this))
    , mainLayout(new QVBoxLayout(this))
    , currentTopicId(topicId)
    , chordTimer(new QTimer(this))
    , isProcessingSubmission(false)
{
    // Find the labels from the UI
    titleLabelLocal = parent->findChild<QLabel*>("titleLabelLocal");
    descriptionLabelLocal = parent->findChild<QLabel*>("descriptionLabelLocal");
    currentPlayerLabelLocal = parent->findChild<QLabel*>("currentPlayerLabelLocal");
    player1ScoreLabelLocal = parent->findChild<QLabel*>("player1ScoreLabelLocal");
    player2ScoreLabelLocal = parent->findChild<QLabel*>("player2ScoreLabelLocal");

    setupUI();
    
    // Set up chord timer
    chordTimer->setSingleShot(true);
    connect(chordTimer, &QTimer::timeout, this, &MultiplayerGameWidget::submitChord);
    
    game = new MultiplayerGame(this, topicId);
    connect(game, &MultiplayerGame::updateUI, this, &MultiplayerGameWidget::updateGameUI);
    connect(game, &MultiplayerGame::gameOver, this, &MultiplayerGameWidget::handleGameOver);
    
    // Connect visual feedback signal
    auto piano = PianoWidget::instance();
    if (piano) {
        connect(game, &MultiplayerGame::highlightKeys, piano, &PianoWidget::highlightAttempt);
    }
    
    game->startNewRound();
}

/**
 * @brief Destructor for MultiplayerGameWidget
 */
MultiplayerGameWidget::~MultiplayerGameWidget()
{
}

/**
 * @brief Sets up the user interface elements
 * @details Configures all UI elements including labels, fonts, layouts, and styling.
 *          Implements responsive design with scaling based on screen resolution.
 */
void MultiplayerGameWidget::setupUI()
{

    // Use system font instead of hardcoded Arial
    QFont systemFont = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
    
    // Set up fonts with relative sizes
    QFont titleFont = systemFont;
    titleFont.setPointSize(40);
    titleFont.setBold(true);
    
    QFont descriptionFont = systemFont;
    descriptionFont.setPointSize(40);
    descriptionFont.setBold(true);
    
    QFont currentPlayerFont = systemFont;
    currentPlayerFont.setPointSize(40);
    currentPlayerFont.setBold(true);

    QFont scoreFont = systemFont;
    scoreFont.setPointSize(30);
    scoreFont.setBold(true);
    
    QFont winnerFont = systemFont;
    winnerFont.setPointSize(12);
    winnerFont.setBold(true);

    if (titleLabelLocal) {
        titleLabelLocal->setFont(titleFont);
        titleLabelLocal->setStyleSheet("QLabel { color: rgb(103, 49, 0); }");
    }

    if (descriptionLabelLocal) {
        descriptionLabelLocal->setFont(descriptionFont);
        descriptionLabelLocal->setStyleSheet("QLabel { color: rgb(103, 49, 0); }");
    }

    if (currentPlayerLabelLocal) {
        currentPlayerLabelLocal->setFont(currentPlayerFont);
        currentPlayerLabelLocal->setAlignment(Qt::AlignCenter);  // Center both horizontally and vertically
        currentPlayerLabelLocal->setStyleSheet("QLabel { color: rgb(103, 49, 0); }");
    }

    if (player1ScoreLabelLocal) {
        player1ScoreLabelLocal->setFont(scoreFont);
        player1ScoreLabelLocal->setStyleSheet("QLabel { color: rgb(103, 49, 0); }");
    }

    if (player2ScoreLabelLocal) {
        player2ScoreLabelLocal->setFont(scoreFont);
        player2ScoreLabelLocal->setStyleSheet("QLabel { color: rgb(103, 49, 0); }");
    }
    
    // Ensure piano is properly attached
    auto piano = PianoWidget::instance();
    if (piano) {
        QFrame* pianoPlaceholder = parentWidget()->findChild<QFrame*>("pianoLocalPlaceholder");
        if (pianoPlaceholder) {
            piano->attachToPlaceholder(pianoPlaceholder);
            connect(piano, &PianoWidget::keyPressed, this, &MultiplayerGameWidget::handleKeyPressed);
            connect(piano, &PianoWidget::keyReleased, this, &MultiplayerGameWidget::handleKeyReleased);
        }
    }

    // Setup chord timer
    chordTimer = new QTimer(this);
    chordTimer->setSingleShot(true);
    connect(chordTimer, &QTimer::timeout, this, &MultiplayerGameWidget::handleChordTimeout);
}

/**
 * @brief Starts a new game round
 * @details Triggers the start of a new round in the game logic.
 */
void MultiplayerGameWidget::startGame()
{
    game->startNewRound();
}

/**
 * @brief Converts a MIDI note index to its corresponding note name
 * @param noteIndex The MIDI note number (60-72)
 * @return The note name as a string (e.g., "C4", "D#4")
 * @details Maps MIDI note numbers to their corresponding note names in the fourth octave.
 */
QString MultiplayerGameWidget::noteIndexToName(int noteIndex)
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
void MultiplayerGameWidget::handleKeyPressed(int noteIndex)
{
    // If we're currently processing a submission, ignore new key presses
    if (isProcessingSubmission) {
        return;
    }

    // Add to currently pressed keys set
    currentlyPressedKeys.insert(noteIndex);
    
    // Collect the new note
    QString noteName = noteIndexToName(noteIndex);
    qDebug() << "MultiplayerGameWidget: Key pressed - MIDI index:" << noteIndex << "Note name:" << noteName;
    
    if (!noteName.isEmpty()) {
        // Keep the original note with its octave
        if (!currentChordNotes.contains(noteName)) {
            currentChordNotes.append(noteName);
            qDebug() << "MultiplayerGameWidget: Added note to chord:" << noteName << "- Current chord:" << currentChordNotes.join("-");
        }
    }
    
    // Restart the timer with a longer window (1 second)
    chordTimer->start(1000);
}

/**
 * @brief Handles key release events from the piano
 * @param noteIndex The index of the released note
 * @details Submits the collected chord when all keys are released
 */
void MultiplayerGameWidget::handleKeyReleased(int noteIndex)
{
    // Get the note name for debugging
    QString noteName = noteIndexToName(noteIndex);
    qDebug() << "MultiplayerGameWidget: Key released - MIDI index:" << noteIndex << "Note name:" << noteName;
    
    // Remove from currently pressed keys set
    currentlyPressedKeys.remove(noteIndex);
    
    // If all keys are released, submit the collected notes
    if (currentlyPressedKeys.isEmpty()) {
        qDebug() << "MultiplayerGameWidget: All keys released, submitting chord:" << currentChordNotes.join("-");
        isProcessingSubmission = false;
        submitChord();
    }
}

/**
 * @brief Submits the current chord for evaluation
 * @details Formats the collected notes into a chord and submits it to the game logic
 */
void MultiplayerGameWidget::submitChord()
{
    if (currentChordNotes.isEmpty()) {
        return;
    }
    
    // Sort the notes
    currentChordNotes.sort();
    QString attempt = currentChordNotes.join("-");
    qDebug() << "MultiplayerGameWidget: Submitting chord:" << attempt;
    
    // Submit the attempt and let the game logic handle validation
    game->playerAttempt(attempt);
    
    // Clear the chord buffer
    currentChordNotes.clear();
}

/**
 * @brief Updates the game UI with current game state
 * @param currentPlayer The current player's turn number
 * @param player1Score Player 1's current score
 * @param player2Score Player 2's current score
 * @param title The current question title
 * @param description The current question description
 * @details Updates all UI elements to reflect the current game state.
 */
void MultiplayerGameWidget::updateGameUI(int currentPlayer, int player1Score, int player2Score, QString title, QString description)
{
    // Safety check - if game is deleted or over, don't update UI
    if (!game) {
        return;
    }

    // Clear any existing chord notes when updating UI (new pattern)
    currentChordNotes.clear();
    
    titleLabelLocal->setText(title.toUpper());
    descriptionLabelLocal->setText(description);
    currentPlayerLabelLocal->setText(QString("%1").arg(currentPlayer));
    player1ScoreLabelLocal->setText(QString("%1").arg(player1Score));
    player2ScoreLabelLocal->setText(QString("%1").arg(player2Score));
}

/**
 * @brief Handles the end of the game
 * @param player1Score Player 1's final score
 * @param player2Score Player 2's final score
 * @details Determines the winner, displays the result, and emits the game finished signal.
 */
void MultiplayerGameWidget::handleGameOver(int player1Score, int player2Score)
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
        disconnect(piano, &PianoWidget::keyPressed, this, &MultiplayerGameWidget::handleKeyPressed);
        disconnect(piano, &PianoWidget::keyReleased, this, &MultiplayerGameWidget::handleKeyReleased);
    }

    // Prepare the winner text
    QString winnerText;
    if (player1Score > player2Score) {
        winnerText = QString("Player 1 Wins!\nFinal Score: %1 - %2").arg(player1Score).arg(player2Score);
    } else if (player2Score > player1Score) {
        winnerText = QString("Player 2 Wins!\nFinal Score: %1 - %2").arg(player2Score).arg(player1Score);
    } else {
        winnerText = QString("It's a Tie!\nFinal Score: %1 - %1").arg(player1Score);
    }

    // Update UI elements one last time to show final state
    titleLabelLocal->setText("GAME OVER");
    descriptionLabelLocal->setText(winnerText);
    currentPlayerLabelLocal->setText("");
    player1ScoreLabelLocal->setText(QString("%1").arg(player1Score));
    player2ScoreLabelLocal->setText(QString("%1").arg(player2Score));

    // Clean up the game object last, after all UI updates and message box
    if (game) {
        disconnect(game, nullptr, this, nullptr);
        game->deleteLater();
        game = nullptr;
    }

    // Emit signal with the current topic ID
    emit gameFinished(currentTopicId);
}

void MultiplayerGameWidget::handleChordTimeout()
{
    // If we have collected notes and we're not currently processing a submission,
    // submit the chord
    if (!currentChordNotes.isEmpty() && !isProcessingSubmission) {
        qDebug() << "MultiplayerGameWidget: Chord timeout - submitting chord:" << currentChordNotes.join("-");
        submitChord();
    }
} 
