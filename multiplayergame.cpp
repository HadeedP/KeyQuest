/**
 * @file multiplayergame.cpp
 * @brief Implementation of the MultiplayerGame class for KeyQuest
 * @author Alan Cruz, Hadeed Pall
 * @details This file implements the multiplayer game functionality for KeyQuest,
 *          handling game logic, scoring, and turn-based gameplay between two players.
 */

#include "multiplayergame.h"
#include <QRandomGenerator>
#include <QtCore/QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QDebug>

/**
 * @brief Constructor for MultiplayerGame
 * @param parent Pointer to the parent QObject
 * @param topicID The ID of the topic to load questions for
 * @details Initializes the multiplayer game with the specified topic, loads questions,
 *          and prepares for the first round. Initializes game state variables including
 *          player scores and turn tracking.
 */
MultiplayerGame::MultiplayerGame(QObject *parent, int topicID)
    : QObject(parent)
    , currentPlayer(1)
    , player1Score(0)
    , player2Score(0)
    , currentQuestionIndex(0)
    , gameEnded(false)
{
    loadQuestions(topicID);
    
    if (questions.isEmpty()) {
        gameEnded = true;
        return;
    }
    
    shuffleQuestions();
    startNewRound();
}

/**
 * @brief Destructor for MultiplayerGame
 */
MultiplayerGame::~MultiplayerGame()
{
}

/**
 * @brief Loads questions from the question bank for a specific topic
 * @param topicID The ID of the topic to load questions for
 * @details Reads questions from a JSON file, processes them to add octave information
 *          to notes, and stores them in the questions list.
 */
void MultiplayerGame::loadQuestions(int topicID)
{
    QFile file(":/resources/questionBank.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "MultiplayerGame: Failed to open questionBank.json";
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qDebug() << "MultiplayerGame: JSON document is not an object";
        return;
    }

    QJsonObject root = doc.object();
    if (!root.contains("topics")) {
        qDebug() << "MultiplayerGame: JSON does not contain 'topics' array";
        return;
    }

    QJsonArray topics = root["topics"].toArray();

    // Find questions for the specified topic ID
    bool topicFound = false;
    for (const QJsonValue &topicVal : topics) {
        QJsonObject topic = topicVal.toObject();
        int currentTopicId = topic["topicID"].toInt();
        
        if (currentTopicId == topicID) {
            topicFound = true;
            QJsonArray questionArray = topic["questions"].toArray();
            qDebug() << "MultiplayerGame: Found topic ID" << topicID << "with" << questionArray.size() << "questions";
            
            for (const QJsonValue &questionVal : questionArray) {
                QJsonObject q = questionVal.toObject();
                MultiplayerQuestion question;
                question.questionID = q["questionID"].toInt();
                question.title = q["Title"].toString();
                question.description = q["Description"].toString();
                
                // Get the original expected input
                QString input = q["ExpectedInput"].toString();
                qDebug() << "MultiplayerGame: Original expected input for question" << question.questionID << ":" << input;
                
                // Handle the expected input based on whether it's a single note or multiple notes
                if (input.contains("-")) {
                    // For chords or sequences, add octave to each note
                    QStringList notes = input.split("-");
                    QStringList notesWithOctave;
                    for (const QString &note : notes) {
                        // Check if note already has an octave number
                        if (note.length() > 1 && note.at(note.length()-1).isDigit()) {
                            notesWithOctave.append(note); // Keep as is
                        } else {
                            notesWithOctave.append(note + "4"); // Add octave 4
                        }
                    }
                    question.expectedInput = notesWithOctave.join("-");
                } else {
                    // For single notes, check if it already has an octave
                    if (input.length() > 1 && input.at(input.length()-1).isDigit()) {
                        question.expectedInput = input; // Keep as is
                    } else {
                        question.expectedInput = input + "4"; // Add octave 4
                    }
                }
                
                qDebug() << "MultiplayerGame: Processed expected input for question" << question.questionID << ":" << question.expectedInput;
                questions.append(question);
            }
            break;
        }
    }

    if (!topicFound) {
        qDebug() << "MultiplayerGame: Topic ID" << topicID << "not found";
        return;
    }
}

/**
 * @brief Randomizes the order of questions
 * @details Uses the Fisher-Yates shuffle algorithm to randomize the questions array.
 */
void MultiplayerGame::shuffleQuestions()
{
    auto rng = QRandomGenerator::global();
    for (int i = questions.size() - 1; i > 0; --i) {
        int j = rng->bounded(i + 1);
        if (i != j) {
            questions.swapItemsAt(i, j);
        }
    }
}

/**
 * @brief Starts a new round of the game
 * @details Updates the current question, pattern, and description, and emits
 *          signals to update the UI. Ends the game if no more questions are available.
 */
void MultiplayerGame::startNewRound()
{
    if (questions.isEmpty()) {
        gameEnded = true;
        emit gameOver(player1Score, player2Score);
        return;
    }

    if (currentQuestionIndex >= questions.size()) {
        gameEnded = true;
        emit gameOver(player1Score, player2Score);
        return;
    }

    const MultiplayerQuestion &currentQuestion = questions[currentQuestionIndex];
    currentPattern = currentQuestion.expectedInput;
    currentDescription = currentQuestion.description;

    emit updateUI(currentPlayer, player1Score, player2Score, currentQuestion.title, currentDescription);
}

/**
 * @brief Compares two chords for equality
 * @param chord1 First chord to compare
 * @param chord2 Second chord to compare
 * @return true if the chords contain the same notes (order independent), false otherwise
 */
bool MultiplayerGame::areChordsSame(const QString& chord1, const QString& chord2) {
    QStringList notes1 = chord1.split("-");
    QStringList notes2 = chord2.split("-");
    
    // If lengths are different, chords can't be the same
    if (notes1.size() != notes2.size()) return false;
    
    // Normalize both sets of notes (sort them and remove octave numbers)
    QStringList normalizedNotes1;
    QStringList normalizedNotes2;
    
    for (const QString& note : notes1) {
        QString normalizedNote = note;
        // Remove octave number if present
        if (normalizedNote.length() > 1 && normalizedNote.at(normalizedNote.length()-1).isDigit()) {
            normalizedNote.chop(1);
        }
        normalizedNotes1.append(normalizedNote);
    }
    
    for (const QString& note : notes2) {
        QString normalizedNote = note;
        // Remove octave number if present
        if (normalizedNote.length() > 1 && normalizedNote.at(normalizedNote.length()-1).isDigit()) {
            normalizedNote.chop(1);
        }
        normalizedNotes2.append(normalizedNote);
    }
    
    // Sort the notes to make the comparison order-independent
    normalizedNotes1.sort();
    normalizedNotes2.sort();
    
    // Debug output to help diagnose issues
    qDebug() << "Comparing chords:";
    qDebug() << "Original chord1:" << chord1 << "normalized to:" << normalizedNotes1.join("-");
    qDebug() << "Original chord2:" << chord2 << "normalized to:" << normalizedNotes2.join("-");
    
    // Compare the normalized note lists
    return normalizedNotes1 == normalizedNotes2;
}

/**
 * @brief Converts a note name to its MIDI note value
 * @param noteName The note name (e.g., "C#4", "Bb3")
 * @return The MIDI note value (0-127)
 * @details Converts a note name to its MIDI note value for comparison.
 *          Handles sharps, flats, double sharps, and double flats.
 */
int MultiplayerGame::noteNameToValue(const QString& noteName) {
    // Strip the octave number if present
    QString name = noteName;
    int octave = 4; // Default octave
    if (name.length() > 1 && name.at(name.length()-1).isDigit()) {
        octave = name.at(name.length()-1).digitValue();
        name.chop(1);
    }
    
    // Base values for C major scale (C=0, D=2, E=4, F=5, G=7, A=9, B=11)
    static QMap<QChar, int> baseValues = {
        {'C', 0}, {'D', 2}, {'E', 4}, {'F', 5}, 
        {'G', 7}, {'A', 9}, {'B', 11}
    };
    
    if (name.isEmpty()) return -1;
    
    QChar noteLetter = name[0];
    int value = baseValues.value(noteLetter, -1);
    if (value == -1) return -1;
    
    // Process accidentals
    int accidentalOffset = 0;
    
    if (name.contains("##")) {
        // Double sharp increases by 2 semitones
        accidentalOffset += 2;
    } else if (name.contains("bb")) {
        // Double flat decreases by 2 semitones
        accidentalOffset -= 2;
    } else {
        // Process single accidentals
        if (name.length() > 1) {
            for (int i = 1; i < name.length(); i++) {
                if (name[i] == '#') accidentalOffset += 1;
                else if (name[i] == 'b') accidentalOffset -= 1;
            }
        }
    }
    
    value += accidentalOffset;
    
    // Normalize to 0-11 range
    value = (value + 12) % 12;
    
    return value + (octave * 12);
}

/**
 * @brief Processes a player's attempt to answer the current question
 * @param attempt The player's input attempt
 * @details Evaluates the attempt, updates the score for the current player,
 *          switches turns, and emits appropriate signals for UI updates.
 */
bool MultiplayerGame::playerAttempt(const QString& attempt)
{
    if (currentQuestionIndex >= questions.size()) {
        return false;
    }

    MultiplayerQuestion currentQuestion = questions[currentQuestionIndex];
    bool isCorrect;

    if (attempt.contains("-")) {
        // For chords, use order-independent comparison
        isCorrect = areChordsSame(attempt, currentQuestion.expectedInput);
    } else {
        // For single notes, compare the MIDI note values for enharmonic equivalence
        qDebug() << "Comparing single notes:";
        qDebug() << "Original attempt:" << attempt << "normalized to:" << attempt.chopped(1);
        qDebug() << "Original expected:" << currentQuestion.expectedInput << "normalized to:" << currentQuestion.expectedInput.chopped(1);
        
        // Using the MIDI note value comparison for enharmonic equivalence
        int attemptValue = noteNameToValue(attempt);
        int expectedValue = noteNameToValue(currentQuestion.expectedInput);
        
        qDebug() << "MultiplayerGame: MIDI value comparison: attempt =" << attemptValue << "expected =" << expectedValue;
        isCorrect = (attemptValue == expectedValue);
    }

    // Emit visual feedback signal before any game state changes
    emit highlightKeys(isCorrect);

    // Update score for current player
    if (isCorrect) {
        if (currentPlayer == 1) {
            player1Score += 10;
        } else {
            player2Score += 10;
        }
        
        // Move to next question
        currentQuestionIndex++;
        
        if (currentQuestionIndex >= questions.size()) {
            gameEnded = true;
            emit gameOver(player1Score, player2Score);
            return true;
        }
        
        // Start a new round with the same player
        startNewRound();
    } else {
        // Switch player turns
        currentPlayer = (currentPlayer == 1) ? 2 : 1;
        
        // Update UI with player switch
        const MultiplayerQuestion &nextQuestion = questions[currentQuestionIndex];
        emit updateUI(currentPlayer, player1Score, player2Score, nextQuestion.title, nextQuestion.description);
    }
    
    return isCorrect;
}

/**
 * @brief Gets the current player's turn number
 * @return The current player number (1 or 2)
 */
int MultiplayerGame::getCurrentPlayer() const { return currentPlayer; }

/**
 * @brief Gets player 1's current score
 * @return Player 1's score
 */
int MultiplayerGame::getPlayer1Score() const { return player1Score; }

/**
 * @brief Gets player 2's current score
 * @return Player 2's score
 */
int MultiplayerGame::getPlayer2Score() const { return player2Score; }

/**
 * @brief Gets the current pattern to be played
 * @return The current pattern as a string
 */
QString MultiplayerGame::getCurrentPattern() const { return currentPattern; }

/**
 * @brief Gets the current question's description
 * @return The current question's description
 */
QString MultiplayerGame::getCurrentDescription() const { return currentDescription; }

/**
 * @brief Checks if the game has ended
 * @return true if the game has ended, false otherwise
 */
bool MultiplayerGame::isGameOver() const { return gameEnded; }


