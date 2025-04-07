/**
 * @file quizwidget.h
 * @brief Header file for the QuizWidget class
 * @author Alan Cruz
 * @details This file defines the user interface classes for the adaptive quiz mode of KeyQuest,
 *          including QuizWidget for the main quiz interface and NewUserDialog for collecting
 *          initial user skill levels. It declares functionality for question display,
 *          user input handling, and adaptive quiz progression.
 */

#ifndef QUIZWIDGET_H
#define QUIZWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDialog>
#include <QSpinBox>
#include <QPushButton>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QtCore/QTimer>
#include <QSet>
#include "adaptivequiz.h"
// "Question.h" is already included by AdaptiveQuiz.h, no need to include it again

/**
 * @brief Widget class for the adaptive quiz interface
 * @details Provides the user interface for the adaptive quiz mode, including
 *          display of quiz information, handling of user input via the piano widget,
 *          chord recognition, question progression, and interaction with the 
 *          AdaptiveQuiz engine. It manages user skill levels, question presentation,
 *          score tracking, and provides an adaptive learning experience.
 */
class QuizWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief Constructor for QuizWidget
     * @param parent Pointer to the parent widget
     * @details Initializes the adaptive quiz widget with UI elements, communication with the
     *          AdaptiveQuiz engine, and timer for handling chord input. Sets up connections
     *          for user interaction and quiz progression. Finds and configures all necessary
     *          UI elements from the parent widget.
     */
    explicit QuizWidget(QWidget *parent = nullptr);
    
    /**
     * @brief Destructor for QuizWidget
     * @details Cleans up resources by deleting the quiz object if it exists.
     */
    ~QuizWidget();

    /**
     * @brief Starts the quiz with appropriate checks and initialization
     * @details Initializes the adaptive quiz by loading questions, checking if the
     *          user is new, and initializing either with user-provided skill levels
     *          or previously saved state. Sets up the quiz engine with appropriate
     *          parameters and displays the first question to the user.
     */
    void startQuiz();

public slots:
    /**
     * @brief Handles keyboard input for note playing
     * @param noteIndex The MIDI note number pressed
     * @details Processes piano key presses, converts them to note names,
     *          and adds them to the current chord collection. Manages
     *          the chord timer to collect multi-note inputs when needed.
     */
    void handleKeyPressed(int noteIndex);

    /**
     * @brief Handles key release events from the piano
     * @param noteIndex The index of the released note
     * @details Processes key release events, updates the currently pressed keys set,
     *          and submits the collected chord when all keys have been released.
     */
    void handleKeyReleased(int noteIndex);

    /**
     * @brief Updates the quiz UI with current game state
     * @param score The current score
     * @param title The current question title
     * @param description The current question description
     * @param accuracy The current accuracy percentage
     * @details Updates all UI elements to reflect the current quiz state,
     *          including the title, description, score, and accuracy.
     *          Clears any existing chord notes when updating UI.
     */
    void updateQuizUI(int score, QString title, QString description, double accuracy);

    /**
     * @brief Handles the end of the quiz
     * @param score The final score
     * @param accuracy The final accuracy percentage
     * @details Saves the final quiz state, including the Q-table and user state,
     *          creates and saves a quiz report, displays completion information,
     *          and emits the quizFinished signal.
     */
    void handleQuizOver(int score, double accuracy);

    /**
     * @brief Handles the chord timeout event
     * @details Submits the collected chord when no new keys are pressed for the timeout period,
     *          even if not all keys have been released. This ensures the user's input
     *          is processed after a reasonable waiting period.
     */
    void handleChordTimeout();

signals:
    /**
     * @brief Signal emitted when the quiz is finished
     * @details This signal is emitted when the quiz is completed, either by
     *          answering all questions or by other termination conditions.
     *          Connected slots can handle cleanup and transition to other views.
     */
    void quizFinished();

private:
    /**
     * @brief Sets up the user interface elements
     * @details Configures all UI elements including labels, fonts, and styles.
     *          Connects the piano widget to the quiz for handling user input and
     *          sets up the chord timer for collecting multi-note inputs.
     */
    void setupUI();

    /**
     * @brief Shows a dialog for new users to input their skill levels
     * @return A State object with the user's skill levels
     * @details Creates and displays a dialog allowing new users to input their
     *          initial skill levels in notes, chords, and scales. The values
     *          are used to initialize the adaptive quiz's learning algorithm.
     */
    State showNewUserDialog();

    /**
     * @brief Converts a MIDI note index to its corresponding note name
     * @param noteIndex The MIDI note number (60-72)
     * @return The note name as a string (e.g., "C4", "D#4")
     * @details Maps MIDI note numbers to their corresponding note names with octave
     *          information. Handles both white and black keys in the octave range
     *          from C4 to C5.
     */
    QString noteIndexToName(int noteIndex);

    /**
     * @brief Submits the current chord for evaluation
     * @details Normalizes the collected notes, compares them with the expected input,
     *          and sends the result to the adaptive quiz engine. Handles progression
     *          to the next question or quiz completion based on the number of questions
     *          answered.
     */
    void submitChord();
    
    /**
     * @brief Loads questions from the question bank
     * @return A map of question IDs to Question objects
     * @details Loads questions from the resource file using the QuestionLoader,
     *          and returns them as a map for use by the adaptive quiz engine.
     */
    std::map<int, Question> loadQuestions();
    
    /**
     * @brief Resets the quiz over handled flag
     * @details Static method to reset the flag that prevents multiple calls to
     *          handleQuizOver for the same quiz session. Called when starting a new quiz.
     */
    static void resetQuizOverHandled();

    AdaptiveQuiz *quiz;                 ///< Pointer to the adaptive quiz engine
    QLabel *titleLabel;                 ///< Label displaying the question title
    QLabel *descriptionLabel;           ///< Label displaying the question description
    QLabel *scoreLabel;                 ///< Label displaying the current score
    QLabel *accuracyLabel;              ///< Label displaying the current accuracy
    
    int currentQuestionId;              ///< ID of the current question being displayed
    
    // For handling chords
    QStringList currentChordNotes;      ///< Collection of notes in the current chord attempt
    static const int CHORD_TIMEOUT_MS = 1000;  ///< Timeout in milliseconds for chord collection
    QTimer* chordTimer;                 ///< Timer for handling chord input timeouts
    QSet<int> currentlyPressedKeys;     ///< Set of currently pressed piano keys
    bool isProcessingSubmission;        ///< Flag to prevent multiple rapid submissions
    
    // Constants
    static const int NUM_QUIZ_QUESTIONS = 10;  ///< Number of questions in a quiz session
    int questionsAnswered;              ///< Count of questions answered in current session
    
    // Static flag to prevent multiple quiz over handling
    static bool quizOverHandled;        ///< Flag to prevent multiple calls to handleQuizOver
};

/**
 * @brief Dialog for collecting initial skill levels from new users
 * @details Creates a dialog box for collecting initial skill levels from new users
 *          in three categories: notes, chords, and scales. Each category uses a spin box
 *          with a range of 0-2, representing beginner to advanced skill levels.
 */
class NewUserDialog : public QDialog
{
    Q_OBJECT
public:
    /**
     * @brief Constructor for NewUserDialog
     * @param parent Pointer to the parent widget
     * @details Creates a dialog box for collecting initial skill levels from new users
     *          in three categories: notes, chords, and scales. Each category uses a spin box
     *          with a range of 0-2, representing beginner to advanced skill levels.
     */
    explicit NewUserDialog(QWidget *parent = nullptr);
    
    /**
     * @brief Gets the notes skill level chosen by the user
     * @return Integer value between 0-2 representing beginner to advanced skill
     */
    int getNotesLevel() const;
    
    /**
     * @brief Gets the chords skill level chosen by the user
     * @return Integer value between 0-2 representing beginner to advanced skill
     */
    int getChordsLevel() const;
    
    /**
     * @brief Gets the scales skill level chosen by the user
     * @return Integer value between 0-2 representing beginner to advanced skill
     */
    int getScalesLevel() const;
    
private:
    QSpinBox *notesSpinBox;    ///< Spin box for setting notes skill level
    QSpinBox *chordsSpinBox;   ///< Spin box for setting chords skill level
    QSpinBox *scalesSpinBox;   ///< Spin box for setting scales skill level
};

#endif // QUIZWIDGET_H 
