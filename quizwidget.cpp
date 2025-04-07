/**
 * @file quizwidget.cpp
 * @brief Implementation of the QuizWidget class for KeyQuest
 * @author Alan Cruz
 * @details This file implements the user interface for the adaptive quiz mode of KeyQuest,
 *          handling quiz display, user input, question progression, and interaction
 *          with the AdaptiveQuiz logic. It manages user skill levels, question presentation,
 *          and score tracking in an intuitive and adaptive learning environment.
 */

#include "quizwidget.h"
#include "pianowidget.h"
#include "datamanager.h"
#include "questionloader.h"
#include "loaddatamanager.h"
#include <QFont>
#include <QFontDatabase>
#include <QMessageBox>
#include <QtCore/QTimer>
#include <QSizePolicy>
#include <QScreen>
#include <QApplication>
#include <QDebug>

// Initialize static members
bool QuizWidget::quizOverHandled = false;

/**
 * @brief Constructor for NewUserDialog
 * @param parent Pointer to the parent widget
 * @details Creates a dialog box for collecting initial skill levels from new users
 *          in three categories: notes, chords, and scales. Each category uses a spin box
 *          with a range of 0-2, representing beginner to advanced skill levels.
 */
NewUserDialog::NewUserDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Your Skill Level");
    
    // Create form layout
    QFormLayout *formLayout = new QFormLayout(this);
    
    // Create spinboxes with range 0-2
    notesSpinBox = new QSpinBox(this);
    notesSpinBox->setRange(0, 2);
    notesSpinBox->setValue(0);
    
    chordsSpinBox = new QSpinBox(this);
    chordsSpinBox->setRange(0, 2);
    chordsSpinBox->setValue(0);
    
    scalesSpinBox = new QSpinBox(this);
    scalesSpinBox->setRange(0, 2);
    scalesSpinBox->setValue(0);
    
    // Add fields to the form
    formLayout->addRow("Notes Skill Level (0-2):", notesSpinBox);
    formLayout->addRow("Chords Skill Level (0-2):", chordsSpinBox);
    formLayout->addRow("Scales Skill Level (0-2):", scalesSpinBox);
    
    // Add buttons
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    
    formLayout->addRow(buttonBox);
    setLayout(formLayout);
}

int NewUserDialog::getNotesLevel() const { return notesSpinBox->value(); }
int NewUserDialog::getChordsLevel() const { return chordsSpinBox->value(); }
int NewUserDialog::getScalesLevel() const { return scalesSpinBox->value(); }

/**
 * @brief Constructor for QuizWidget
 * @param parent Pointer to the parent widget
 * @details Initializes the adaptive quiz widget with UI elements, communication with the
 *          AdaptiveQuiz engine, and timer for handling chord input. Sets up connections
 *          for user interaction and quiz progression. Finds and configures all necessary
 *          UI elements from the parent widget.
 */
QuizWidget::QuizWidget(QWidget *parent)
    : QWidget(parent)
    , quiz(nullptr)
    , currentQuestionId(0)
    , chordTimer(new QTimer(this))
    , isProcessingSubmission(false)
    , questionsAnswered(0)
{
    // Reset the static flag
    resetQuizOverHandled();
    
    // Find the labels from the UI
    titleLabel = parent->findChild<QLabel*>("titleLabel");
    descriptionLabel = parent->findChild<QLabel*>("descriptionLabel");
    scoreLabel = parent->findChild<QLabel*>("playerScoreLabel");
    accuracyLabel = parent->findChild<QLabel*>("accuracyLabel");

    setupUI();
    
    // Set up chord timer
    chordTimer->setSingleShot(true);
    connect(chordTimer, &QTimer::timeout, this, &QuizWidget::submitChord);
}

/**
 * @brief Destructor for QuizWidget
 * @details Cleans up resources by deleting the quiz object if it exists.
 */
QuizWidget::~QuizWidget()
{
    if (quiz) {
        delete quiz;
        quiz = nullptr;
    }
}

/**
 * @brief Sets up the user interface elements
 * @details Configures all UI elements including labels, fonts, and styles.
 *          Connects the piano widget to the quiz for handling user input and
 *          sets up the chord timer for collecting multi-note inputs.
 */
void QuizWidget::setupUI()
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

    QFont scoreFont = systemFont;
    scoreFont.setPointSize(30);  // Direct size for score
    scoreFont.setBold(true);

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

    if (scoreLabel) {
        scoreLabel->setFont(scoreFont);
        scoreLabel->setStyleSheet("QLabel { color: rgb(103, 49, 0); }");
    }

    if (accuracyLabel) {
        accuracyLabel->setFont(accuracyFont);
        accuracyLabel->setStyleSheet("QLabel { color: rgb(103, 49, 0); }");
    }

    // Ensure piano is properly attached
    auto piano = PianoWidget::instance();
    if (piano) {
        // Find the piano placeholder in the parent widget
        QFrame* pianoPlaceholder = parentWidget()->findChild<QFrame*>("lessonsPagePianoHolder");
        if (pianoPlaceholder) {
            piano->attachToPlaceholder(pianoPlaceholder);
            connect(piano, &PianoWidget::keyPressed, this, &QuizWidget::handleKeyPressed);
            connect(piano, &PianoWidget::keyReleased, this, &QuizWidget::handleKeyReleased);
        }
    }

    // Setup chord timer
    chordTimer = new QTimer(this);
    chordTimer->setSingleShot(true);
    connect(chordTimer, &QTimer::timeout, this, &QuizWidget::handleChordTimeout);
}

/**
 * @brief Starts the quiz with appropriate checks and initialization
 * @details Initializes the adaptive quiz by loading questions, checking if the
 *          user is new, and initializing either with user-provided skill levels
 *          or previously saved state. Sets up the quiz engine with appropriate
 *          parameters and displays the first question to the user.
 */
void QuizWidget::startQuiz()
{
    // Reset static flag for quiz over handling
    resetQuizOverHandled();
    
    // 1. Load questions
    std::map<int, Question> questionBank = loadQuestions();
    if (questionBank.empty()) {
        QMessageBox::warning(this, "Error", "Failed to load questions for the quiz.");
        return;
    }

    // 2. Check if this is a new user
    bool isNewUser = LoadDataManager::instance()->isNewUser();
    State userState;
    std::map<State, std::map<int, float>> qTable;

    // 3. If new user, ask for skill levels
    if (isNewUser) {
        userState = showNewUserDialog();
        
        // Save initial state
        LoadDataManager::instance()->saveUserState(userState);
        LoadDataManager::instance()->setNewUser(false);
    } else {
        // Load last known state
        userState = LoadDataManager::instance()->getUserState();
    }

    // 4. Load or create Q-table
    qTable = LoadDataManager::instance()->getQTable();

    // 5. Initialize quiz with questions, Q-table, and user state
    if (quiz) {
        delete quiz;
    }
    quiz = new AdaptiveQuiz(questionBank, qTable, userState, this);
    connect(quiz, &AdaptiveQuiz::highlightKeys, PianoWidget::instance(), &PianoWidget::highlightAttempt);
    questionsAnswered = 0;
    
    // 6. Start the first question
    if (!quiz) {
        QMessageBox::warning(this, "Error", "Failed to initialize quiz.");
        return;
    }
    
    // Get the first question
    currentQuestionId = quiz->getNextAction();
    Question question = quiz->getQuestion(currentQuestionId);
    
    // Update the UI
    updateQuizUI(
        quiz->getScore(),
        question.getTitle(),
        question.getDescription(),
        quiz->getAccuracy()
    );
}

/**
 * @brief Shows a dialog for new users to input their skill levels
 * @return A State object with the user's skill levels
 * @details Creates and displays a dialog allowing new users to input their
 *          initial skill levels in notes, chords, and scales. The values
 *          are used to initialize the adaptive quiz's learning algorithm.
 */
State QuizWidget::showNewUserDialog()
{
    NewUserDialog dialog(this);
    dialog.exec();
    
    int notesLevel = dialog.getNotesLevel();
    int chordsLevel = dialog.getChordsLevel();
    int scalesLevel = dialog.getScalesLevel();
    
    return State{
        notesLevel,
        chordsLevel,
        scalesLevel
    };
}

/**
 * @brief Converts a MIDI note index to its corresponding note name
 * @param noteIndex The MIDI note number (60-72)
 * @return The note name as a string (e.g., "C4", "D#4")
 * @details Maps MIDI note numbers to their corresponding note names with octave
 *          information. Handles both white and black keys in the octave range
 *          from C4 to C5.
 */
QString QuizWidget::noteIndexToName(int noteIndex)
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
 * @details Processes piano key presses, converts them to note names,
 *          and adds them to the current chord collection. Manages
 *          the chord timer to collect multi-note inputs when needed.
 */
void QuizWidget::handleKeyPressed(int noteIndex)
{
    // If we're currently processing a submission, ignore new key presses
    if (isProcessingSubmission || !quiz) {
        return;
    }

    // Add to currently pressed keys set
    currentlyPressedKeys.insert(noteIndex);
    
    // Collect the new note
    QString noteName = noteIndexToName(noteIndex);
    
    if (!noteName.isEmpty()) {
        QString baseNote = noteName;
        baseNote.chop(1);  // Remove the octave number
        QString noteWithOctave = baseNote + "4";
        
        if (!currentChordNotes.contains(noteWithOctave)) {
            currentChordNotes.append(noteWithOctave);
        }
    }
    
    // Restart the timer with a longer window
    chordTimer->start(CHORD_TIMEOUT_MS);
}

/**
 * @brief Handles key release events from the piano
 * @param noteIndex The index of the released note
 * @details Processes key release events, updates the currently pressed keys set,
 *          and submits the collected chord when all keys have been released.
 */
void QuizWidget::handleKeyReleased(int noteIndex)
{
    // If quiz isn't running, ignore key releases
    if (!quiz) {
        return;
    }
    
    // Remove from currently pressed keys set
    currentlyPressedKeys.remove(noteIndex);
    
    // If all keys are released, submit the collected notes
    if (currentlyPressedKeys.isEmpty()) {
        isProcessingSubmission = false;
        submitChord();
    }
}

/**
 * @brief Handles the chord timeout event
 * @details Submits the collected chord when no new keys are pressed for the timeout period,
 *          even if not all keys have been released. This ensures the user's input
 *          is processed after a reasonable waiting period.
 */
void QuizWidget::handleChordTimeout()
{
    // Only submit if there are notes to submit and we're not already processing
    if (!currentChordNotes.isEmpty() && !isProcessingSubmission && quiz) {
        submitChord();
    }
}

/**
 * @brief Submits the current chord for evaluation
 * @details Normalizes the collected notes, compares them with the expected input,
 *          and sends the result to the adaptive quiz engine. Handles progression
 *          to the next question or quiz completion based on the number of questions
 *          answered.
 */
void QuizWidget::submitChord()
{
    if (!quiz || currentChordNotes.isEmpty()) {
        return;
    }
    
    isProcessingSubmission = true;
    
    // Get the current question
    Question question = quiz->getQuestion(currentQuestionId);
    
    // Prepare the user input and expected input for comparison
    QStringList normalizedUserNotes;
    for (const QString& note : currentChordNotes) {
        // Strip octave numbers from user input
        QString baseNote = note;
        baseNote.chop(1);  // Remove the octave number
        normalizedUserNotes.append(baseNote);
    }
    
    // Sort notes alphabetically to make order irrelevant
    normalizedUserNotes.sort();
    QString normalizedUserInput = normalizedUserNotes.join("-");
    
    // Similarly normalize the expected input
    QStringList expectedNotes = question.getExpectedInput().split("-");
    QStringList normalizedExpectedNotes;
    for (const QString& note : expectedNotes) {
        // Remove octave numbers if present
        QString baseNote = note;
        if (baseNote.length() > 1 && baseNote.at(baseNote.length()-1).isDigit()) {
            baseNote.chop(1);
        }
        normalizedExpectedNotes.append(baseNote);
    }
    normalizedExpectedNotes.sort();
    QString normalizedExpectedInput = normalizedExpectedNotes.join("-");
    
    // Format original input for debug display
    QString originalUserInput;
    if (currentChordNotes.size() > 1) {
        originalUserInput = currentChordNotes.join("-");
    } else {
        originalUserInput = currentChordNotes.first();
    }
    
    // Check if the answer is correct using normalized inputs
    bool correct = (normalizedUserInput == normalizedExpectedInput);
    
    // Submit the answer to the quiz
    quiz->evaluateResponse(currentQuestionId, correct);
    
    // No popup messages - rely on the piano's visual feedback through the highlightKeys signal
    
    // Increment the number of questions answered
    questionsAnswered++;
    
    // Check if we've reached the end of the quiz
    if (questionsAnswered >= NUM_QUIZ_QUESTIONS) {
        // Quiz is complete, save data and show final results
        handleQuizOver(quiz->getScore(), quiz->getAccuracy());
    } else {
        // Get the next question
        currentQuestionId = quiz->getNextAction();
        Question nextQuestion = quiz->getQuestion(currentQuestionId);
        
        // Update the UI
        updateQuizUI(
            quiz->getScore(),
            nextQuestion.getTitle(),
            nextQuestion.getDescription(),
            quiz->getAccuracy()
        );
    }
    
    // Clear current chord notes for the next question
    currentChordNotes.clear();
    isProcessingSubmission = false;
}

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
void QuizWidget::updateQuizUI(int score, QString title, QString description, double accuracy)
{
    // Safety check - if quiz is deleted or over, don't update UI
    if (!quiz) {
        return;
    }

    // Clear any existing chord notes when updating UI (new pattern)
    currentChordNotes.clear();
    
    if (titleLabel) {
        titleLabel->setText(title.toUpper());
    }
    
    if (descriptionLabel) {
        descriptionLabel->setText(description);
    }

    // Update scores with more prominent formatting
    if (scoreLabel) {
        scoreLabel->setText(QString("%1").arg(score));
    }

    // Update accuracy with percentage
    if (accuracyLabel) {
        accuracyLabel->setText(QString("%1%").arg(QString::number(accuracy, 'f', 0)));
    }
}

/**
 * @brief Handles the end of the quiz
 * @param score The final score
 * @param accuracy The final accuracy percentage
 * @details Saves the final quiz state, including the Q-table and user state,
 *          creates and saves a quiz report, displays completion information,
 *          and emits the quizFinished signal.
 */
void QuizWidget::handleQuizOver(int score, double accuracy)
{
    // Prevent multiple quiz over events for the same session
    static bool quizOverHandled = false;
    if (quizOverHandled || !quiz) {
        return;
    }
    quizOverHandled = true;
    
    // 1. Save the updated Q-table
    LoadDataManager::instance()->saveQTable(quiz->getQTable());
    State currentState = quiz->getCurrentState();
    LoadDataManager::instance()->saveUserState(currentState);
    
    // 2. Create and save quiz report
    QuizReport report(
        quiz->getScore(),
        quiz->getAccuracy(),
        quiz->getTotalQuestions(),
        quiz->getCorrectAnswers(),
        quiz->getHistory()
    );
    
    DataManager::saveQuizReport("quiz_report.json", report);
    
    // 3. Show completion dialog
    QString message = QString(
        "Quiz complete!\n\n"
        "Score: %1\n"
        "Accuracy: %2%\n"
        "Questions Answered: %3\n"
        "Correct Answers: %4\n\n"
        "Your progress has been saved."
    ).arg(score)
     .arg(QString::number(accuracy, 'f', 0))
     .arg(quiz->getTotalQuestions())
     .arg(quiz->getCorrectAnswers());
    
    QMessageBox::information(this, "Quiz Complete", message);
    
    // 4. Emit signal that quiz is finished
    emit quizFinished();
}

/**
 * @brief Loads questions from the question bank
 * @return A map of question IDs to Question objects
 * @details Loads questions from the resource file using the QuestionLoader,
 *          and returns them as a map for use by the adaptive quiz engine.
 */
std::map<int, Question> QuizWidget::loadQuestions()
{
    auto questions = loadQuestionsFromFile(":/resources/questionBank.json");
    return questions;
}

/**
 * @brief Resets the quiz over handled flag
 * @details Static method to reset the flag that prevents multiple calls to
 *          handleQuizOver for the same quiz session. Called when starting a new quiz.
 */
void QuizWidget::resetQuizOverHandled()
{
    quizOverHandled = false;
} 