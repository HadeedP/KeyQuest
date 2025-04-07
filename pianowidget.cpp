/**
 * @file pianowidget.cpp
 * @brief Implementation of the PianoWidget class for KeyQuest
 * @author Alan Cruz
 * @details This file implements a virtual piano widget that provides an interactive
 *          piano interface with both visual and keyboard input support. It includes
 *          features for key visualization, note labeling, and MIDI note emission.
 */

#include "pianowidget.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QKeyEvent>
#include <QScreen>
#include <QGuiApplication>
#include <QtCore/QTimer>
#include <QDebug>

// Initialize static instance pointer
PianoWidget* PianoWidget::s_instance = nullptr;

/**
 * @brief Gets the singleton instance of the PianoWidget
 * @return Pointer to the PianoWidget instance
 * @details Implements the singleton pattern to ensure only one piano widget exists
 *          throughout the application.
 */
PianoWidget* PianoWidget::instance() {
    if (!s_instance) {
        s_instance = new PianoWidget();
    }
    return s_instance;
}

/**
 * @brief Constructor for PianoWidget
 * @param parent Pointer to the parent widget
 * @details Initializes the piano widget with keyboard support, label toggle button,
 *          and sets up the initial piano layout.
 */
PianoWidget::PianoWidget(QWidget* parent)
    : QWidget(parent)
    , m_currentPlaceholder(nullptr)
    , m_keyboard(new Keyboard())
    , m_labelToggleButton(new QPushButton(this))
    , m_showLabels(false)
    , m_isKeyboardInput(false)
{
    // Set focus policy to receive keyboard events
    setFocusPolicy(Qt::StrongFocus);
    
    // Setup toggle button
    m_labelToggleButton->setText("Keys");
    m_labelToggleButton->setCheckable(true);
    m_labelToggleButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #333;"
        "    color: white;"
        "    border: 1px solid #666;"
        "    border-radius: 4px;"
        "    padding: 4px 8px;"
        "}"
        "QPushButton:checked {"
        "    background-color: #666;"
        "    border: 1px solid #999;"
        "}"
    );
    connect(m_labelToggleButton, &QPushButton::toggled, this, &PianoWidget::onToggleLabels);
    
    initializePiano();
    setupKeyboardMapping();
}

/**
 * @brief Initializes the piano interface
 * @details Creates and positions the white and black keys, sets up their labels,
 *          applies styling, and connects signal handlers.
 */
void PianoWidget::initializePiano() {
    // Set focus policy to receive keyboard events
    setFocusPolicy(Qt::StrongFocus);
    
    // Create white keys first (they go in the back)
    const int numWhiteKeys = 8;
    for (int i = 0; i < numWhiteKeys; i++) {
        QPushButton* key = new QPushButton(this);
        m_whiteKeys.append(key);
        
        // Create label for this key
        QLabel* label = new QLabel(this);
        label->setAlignment(Qt::AlignCenter);
        label->hide();
        m_whiteKeyLabels.append(label);
    }

    // Create black keys (they go on top)
    const int numBlackKeys = 5;
    for (int i = 0; i < numBlackKeys; i++) {
        QPushButton* key = new QPushButton(this);
        m_blackKeys.append(key);
        
        // Create label for this key
        QLabel* label = new QLabel(this);
        label->setAlignment(Qt::AlignCenter);
        label->hide();
        m_blackKeyLabels.append(label);
    }

    applyKeyStyles();
    setupKeyLabels();
    connectKeySignals();
}

/**
 * @brief Sets up keyboard mapping for piano keys
 * @details Maps computer keyboard keys to piano keys:
 *          - White keys: A, S, D, F, G, H, J, K
 *          - Black keys: W, E, T, Y, U
 */
void PianoWidget::setupKeyboardMapping() {
    // Map computer keyboard keys to piano keys
    const Qt::Key whiteKeyMapping[] = {
        Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_F,
        Qt::Key_G, Qt::Key_H, Qt::Key_J, Qt::Key_K
    };
    
    const Qt::Key blackKeyMapping[] = {
        Qt::Key_W, Qt::Key_E, Qt::Key_T, Qt::Key_Y, Qt::Key_U
    };
    
    // Map white keys
    for (int i = 0; i < m_whiteKeys.size(); i++) {
        m_keyToButton[whiteKeyMapping[i]] = m_whiteKeys[i];
    }
    
    // Map black keys
    for (int i = 0; i < m_blackKeys.size(); i++) {
        m_keyToButton[blackKeyMapping[i]] = m_blackKeys[i];
    }
}

/**
 * @brief Applies visual styles to piano keys
 * @details Sets up the appearance of white and black keys with hover and pressed states.
 */
void PianoWidget::applyKeyStyles() {
    // White key style
    QString whiteKeyStyle = 
        "QPushButton {"
        "    background-color: white;"
        "    border: 1px solid #999;"
        "    border-bottom-left-radius: 4px;"
        "    border-bottom-right-radius: 4px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #f0f0f0;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #e0e0e0;"
        "    border: 1px solid #666;"
        "}";

    // Black key style
    QString blackKeyStyle = 
        "QPushButton {"
        "    background-color: #111;"
        "    border: 1px solid #000;"
        "    border-bottom-left-radius: 4px;"
        "    border-bottom-right-radius: 4px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #222;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #333;"
        "    border: 1px solid #000;"
        "}";

    // Apply styles
    for (auto key : m_whiteKeys) {
        key->setStyleSheet(whiteKeyStyle);
    }
    for (auto key : m_blackKeys) {
        key->setStyleSheet(blackKeyStyle);
    }
}

/**
 * @brief Sets up labels for piano keys
 * @details Configures note labels for both white and black keys with appropriate styling.
 */
void PianoWidget::setupKeyLabels() {
    // White key labels with multiple notes
    const QString whiteKeyNotes[][3] = {
        {"B#", "C", "Dbb"},  // First C
        {"C##", "D", "Ebb"}, // D
        {"D##", "E", "Fb"},  // E
        {"E#", "F", "Gbb"},  // F
        {"F##", "G", "Abb"}, // G
        {"G##", "A", "Bbb"}, // A
        {"A##", "B", "Cb"},  // B
        {"B#", "C", "Dbb"}   // Second C
    };
    
    for (int i = 0; i < m_whiteKeyLabels.size(); i++) {
        QString labelText = whiteKeyNotes[i][0] + "\n" + whiteKeyNotes[i][1] + "\n" + whiteKeyNotes[i][2];
        m_whiteKeyLabels[i]->setText(labelText);
        m_whiteKeyLabels[i]->setStyleSheet(
            "QLabel {"
            "    color: black;"
            "    background: transparent;"
            "    font-size: 14px;"  // Smaller font to fit multiple lines
            "    font-weight: bold;"
            "    padding: 2px;"
            "}"
        );
    }
    
    // Black key labels with multiple notes
    const QString blackKeyNotes[][2] = {
        {"C#", "Db"},  // C#/Db
        {"D#", "Eb"},  // D#/Eb
        {"F#", "Gb"},  // F#/Gb
        {"G#", "Ab"},  // G#/Ab
        {"A#", "Bb"}   // A#/Bb
    };
    
    for (int i = 0; i < m_blackKeyLabels.size(); i++) {
        QString labelText = blackKeyNotes[i][0] + "\n" + blackKeyNotes[i][1];
        m_blackKeyLabels[i]->setText(labelText);
        m_blackKeyLabels[i]->setStyleSheet(
            "QLabel {"
            "    color: white;"
            "    background: transparent;"
            "    font-size: 14px;"  // Even smaller font for black keys
            "    font-weight: bold;"
            "    padding: 1px;"
            "}"
        );
    }
}

/**
 * @brief Updates the visibility of key labels
 * @details Shows or hides all key labels based on the current visibility setting.
 */
void PianoWidget::updateKeyLabelsVisibility() {
    for (auto label : m_whiteKeyLabels) {
        label->setVisible(m_showLabels);
    }
    for (auto label : m_blackKeyLabels) {
        label->setVisible(m_showLabels);
    }
}

/**
 * @brief Handles the label toggle button state change
 * @details Updates the visibility of key labels when the toggle button is clicked.
 */
void PianoWidget::onToggleLabels() {
    m_showLabels = m_labelToggleButton->isChecked();
    updateKeyLabelsVisibility();
}

/**
 * @brief Connects signal handlers for piano keys
 * @details Sets up click and release event handlers for both white and black keys.
 */
void PianoWidget::connectKeySignals() {
    // Connect white keys
    for (auto key : m_whiteKeys) {
        connect(key, &QPushButton::pressed, this, &PianoWidget::onWhiteKeyPressed);
        connect(key, &QPushButton::released, this, &PianoWidget::onKeyReleased);
    }
    
    // Connect black keys
    for (auto key : m_blackKeys) {
        connect(key, &QPushButton::pressed, this, &PianoWidget::onBlackKeyPressed);
        connect(key, &QPushButton::released, this, &PianoWidget::onKeyReleased);
    }
}

/**
 * @brief Attaches the piano widget to a placeholder frame
 * @param placeholder Pointer to the QFrame where the piano should be displayed
 * @details Positions and sizes the piano widget and its components within the
 *          specified placeholder frame.
 */
void PianoWidget::attachToPlaceholder(QFrame* placeholder) {
    if (!placeholder) return;
    
    m_currentPlaceholder = placeholder;
    
    // Set this widget's size to match placeholder
    this->setParent(placeholder);
    this->setGeometry(0, 0, placeholder->width(), placeholder->height());
    
    // Position toggle button in top-right corner
    const int buttonWidth = 60;
    const int buttonHeight = 25;
    const int buttonMargin = 10;
    m_labelToggleButton->setFixedSize(buttonWidth, buttonHeight);
    m_labelToggleButton->move(placeholder->width() - buttonWidth - buttonMargin, buttonMargin);
    m_labelToggleButton->raise();
    m_labelToggleButton->show();
    
    // Calculate key dimensions based on placeholder size
    const int containerWidth = placeholder->width();
    const int containerHeight = placeholder->height();
    const int numWhiteKeys = 8;
    
    // Calculate white key dimensions
    const int whiteKeyWidth = containerWidth / numWhiteKeys;
    const int whiteKeyHeight = containerHeight * 0.9; // 90% of container height
    const int whiteKeyY = containerHeight * 0.05; // 5% padding from top
    
    // Calculate black key dimensions
    const int blackKeyWidth = whiteKeyWidth * 0.6; // 60% of white key width
    const int blackKeyHeight = whiteKeyHeight * 0.6; // 60% of white key height
    
    // Position white keys and their labels
    for (int i = 0; i < m_whiteKeys.size(); i++) {
        QPushButton* key = m_whiteKeys[i];
        key->setFixedSize(whiteKeyWidth, whiteKeyHeight);
        key->move(i * whiteKeyWidth, whiteKeyY);
        key->show();
        
        // Position label
        QLabel* label = m_whiteKeyLabels[i];
        label->setFixedSize(whiteKeyWidth, whiteKeyHeight / 3);
        label->move(i * whiteKeyWidth, whiteKeyY + whiteKeyHeight * 0.6);
        label->setVisible(m_showLabels);
    }
    
    // Position black keys and their labels
    const double blackKeyPositions[] = {
        whiteKeyWidth - (blackKeyWidth / 2.0),  // C#
        whiteKeyWidth * 2 - (blackKeyWidth / 2.0),  // D#
        whiteKeyWidth * 4 - (blackKeyWidth / 2.0),  // F#
        whiteKeyWidth * 5 - (blackKeyWidth / 2.0),  // G#
        whiteKeyWidth * 6 - (blackKeyWidth / 2.0)   // A#
    };
    
    for (int i = 0; i < m_blackKeys.size(); i++) {
        QPushButton* key = m_blackKeys[i];
        key->setFixedSize(blackKeyWidth, blackKeyHeight);
        key->move(blackKeyPositions[i], whiteKeyY);
        key->raise();
        key->show();
        
        // Position label
        QLabel* label = m_blackKeyLabels[i];
        label->setFixedSize(blackKeyWidth, blackKeyHeight / 2);
        label->move(blackKeyPositions[i], whiteKeyY + blackKeyHeight * 0.1);
        label->raise();
        label->setVisible(m_showLabels);
    }
    
    this->show();
    this->raise();
    
    // Set focus to receive keyboard events immediately
    this->setFocus();
    
    // Also grab keyboard focus when clicked anywhere on the piano
    this->setFocusPolicy(Qt::StrongFocus);
    for (auto key : m_whiteKeys) {
        key->setFocusProxy(this);
    }
    for (auto key : m_blackKeys) {
        key->setFocusProxy(this);
    }
}

/**
 * @brief Detaches the piano widget from its current placeholder
 */
void PianoWidget::detach() {
    if (m_currentPlaceholder) {
        setParent(nullptr);
        m_currentPlaceholder = nullptr;
    }
}

/**
 * @brief Resets the piano state
 * @details Releases all pressed keys and resets the piano state to its initial condition
 */
void PianoWidget::reset() {
    // Reset keyboard input flag
    m_isKeyboardInput = false;

    // Release all pressed white keys
    for (auto key : m_whiteKeys) {
        if (key->isDown()) {
            key->setDown(false);
            key->update();
            int index = m_whiteKeys.indexOf(key);
            if (index >= 0) {
                const int whiteKeyNotes[] = {60, 62, 64, 65, 67, 69, 71, 72};
                int noteNumber = whiteKeyNotes[index];
                emit keyReleased(noteNumber);
            }
        }
    }

    // Release all pressed black keys
    for (auto key : m_blackKeys) {
        if (key->isDown()) {
            key->setDown(false);
            key->update();
            int index = m_blackKeys.indexOf(key);
            if (index >= 0) {
                const int blackKeyNotes[] = {61, 63, 66, 68, 70};
                int noteNumber = blackKeyNotes[index];
                emit keyReleased(noteNumber);
            }
        }
    }

    // Reset label toggle state if needed
    if (m_labelToggleButton && m_labelToggleButton->isChecked()) {
        m_labelToggleButton->setChecked(false);
        m_showLabels = false;
        updateKeyLabelsVisibility();
    }
}

/**
 * @brief Handles white key press events
 * @details Processes white key clicks and emits corresponding MIDI note events.
 */
void PianoWidget::onWhiteKeyPressed() {
    // This is a mouse click event
    m_isKeyboardInput = false;
    QPushButton* key = qobject_cast<QPushButton*>(sender());
    if (key && m_keyboard) {
        int index = m_whiteKeys.indexOf(key);
        if (index >= 0) {
            const int whiteKeyNotes[] = {60, 62, 64, 65, 67, 69, 71, 72};
            int noteNumber = whiteKeyNotes[index];
            if (!currentlyPressedKeys.contains(noteNumber)) {
                m_keyboard->playNote(noteNumber);
                emit keyPressed(noteNumber);
                currentlyPressedKeys.insert(noteNumber);
            }
        }
    }
}

/**
 * @brief Handles black key press events
 * @details Processes black key clicks and emits corresponding MIDI note events.
 */
void PianoWidget::onBlackKeyPressed() {
    // This is a mouse click event
    m_isKeyboardInput = false;
    QPushButton* key = qobject_cast<QPushButton*>(sender());
    if (key && m_keyboard) {
        int index = m_blackKeys.indexOf(key);
        if (index >= 0) {
            const int blackKeyNotes[] = {61, 63, 66, 68, 70};
            int noteNumber = blackKeyNotes[index];
            if (!currentlyPressedKeys.contains(noteNumber)) {
                m_keyboard->playNote(noteNumber);
                emit keyPressed(noteNumber);
                currentlyPressedKeys.insert(noteNumber);
            }
        }
    }
}

/**
 * @brief Handles key release events
 * @details Processes key releases and emits corresponding MIDI note-off events.
 */
void PianoWidget::onKeyReleased() {
    QPushButton* key = qobject_cast<QPushButton*>(sender());
    if (key && m_keyboard) {
        int noteNumber = -1;
        
        if (m_whiteKeys.contains(key)) {
            int index = m_whiteKeys.indexOf(key);
            const int whiteKeyNotes[] = {60, 62, 64, 65, 67, 69, 71, 72};
            noteNumber = whiteKeyNotes[index];
        } else if (m_blackKeys.contains(key)) {
            int index = m_blackKeys.indexOf(key);
            const int blackKeyNotes[] = {61, 63, 66, 68, 70};
            noteNumber = blackKeyNotes[index];
        }
        
        if (noteNumber != -1 && currentlyPressedKeys.contains(noteNumber)) {
            m_keyboard->stopNote(noteNumber);
            emit keyReleased(noteNumber);
            currentlyPressedKeys.remove(noteNumber);
        }
    }
}

/**
 * @brief Handles keyboard key press events
 * @param event The key event to process
 * @details Maps computer keyboard input to piano key presses and emits
 *          corresponding MIDI note events.
 */
void PianoWidget::keyPressEvent(QKeyEvent* event) {
    if (event->isAutoRepeat()) {
        return;
    }

    m_isKeyboardInput = true;  // Mark that we're handling keyboard input
    
    // Check if this key maps to a piano key
    if (m_keyToButton.contains(event->key())) {
        QPushButton* button = m_keyToButton[event->key()];
        if (button) {
            // Set the button's visual state
            button->setDown(true);
            button->update();
            
            // Determine which note to play
            int noteNumber = -1;
            if (m_whiteKeys.contains(button)) {
                int index = m_whiteKeys.indexOf(button);
                if (index >= 0) {
                    const int whiteKeyNotes[] = {60, 62, 64, 65, 67, 69, 71, 72};
                    noteNumber = whiteKeyNotes[index];
                }
            } else {
                int index = m_blackKeys.indexOf(button);
                if (index >= 0) {
                    const int blackKeyNotes[] = {61, 63, 66, 68, 70};
                    noteNumber = blackKeyNotes[index];
                }
            }
            
            // If we found a valid note and it's not already playing, play it
            if (noteNumber != -1 && !currentlyPressedKeys.contains(noteNumber)) {
                currentlyPressedKeys.insert(noteNumber);
                m_keyboard->playNote(noteNumber);
                emit keyPressed(noteNumber);
            }
        }
    }
}

/**
 * @brief Handles keyboard key release events
 * @param event The key event to process
 * @details Maps computer keyboard input to piano key releases and emits
 *          corresponding MIDI note-off events.
 */
void PianoWidget::keyReleaseEvent(QKeyEvent* event) {
    if (event->isAutoRepeat()) {
        return;
    }

    // Check if this key maps to a piano key
    if (m_keyToButton.contains(event->key())) {
        QPushButton* button = m_keyToButton[event->key()];
        if (button) {
            button->setDown(false);
            button->update();
            
            int noteNumber = -1;
            if (m_whiteKeys.contains(button)) {
                int index = m_whiteKeys.indexOf(button);
                const int whiteKeyNotes[] = {60, 62, 64, 65, 67, 69, 71, 72};
                noteNumber = whiteKeyNotes[index];
            } else if (m_blackKeys.contains(button)) {
                int index = m_blackKeys.indexOf(button);
                const int blackKeyNotes[] = {61, 63, 66, 68, 70};
                noteNumber = blackKeyNotes[index];
            }
            
            if (noteNumber != -1 && currentlyPressedKeys.contains(noteNumber)) {
                m_keyboard->stopNote(noteNumber);
                emit keyReleased(noteNumber);
                currentlyPressedKeys.remove(noteNumber);
            }
        }
    }

    // If all keys are released, reset the keyboard input flag
    if (currentlyPressedKeys.isEmpty()) {
        m_isKeyboardInput = false;
    }
}

/**
 * @brief Highlights keys to indicate correct/incorrect attempt
 * @param isCorrect Whether the attempt was correct
 */
void PianoWidget::highlightAttempt(bool isCorrect)
{
    if (!m_currentPlaceholder) {
        return;
    }

    // Create a new label for the highlight as a child of the piano widget
    QLabel* highlightLabel = new QLabel(this);
    highlightLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    highlightLabel->setGeometry(0, 0, width(), height());
    
    // Set the highlight color using a solid background with alpha
    QString highlightColor = isCorrect ? 
        "background-color: rgba(144, 238, 144, 0.5);" :  // Light green overlay (50% opacity)
        "background-color: rgba(255, 182, 193, 0.5);";   // Light red overlay (50% opacity)
    highlightLabel->setStyleSheet(highlightColor);
    
    // Show and raise the highlight label to the very top
    highlightLabel->show();
    highlightLabel->raise();
    
    // Ensure all piano keys are below the highlight
    for (auto key : m_whiteKeys) {
        key->stackUnder(highlightLabel);
    }
    for (auto key : m_blackKeys) {
        key->stackUnder(highlightLabel);
    }
    for (auto label : m_whiteKeyLabels) {
        label->stackUnder(highlightLabel);
    }
    for (auto label : m_blackKeyLabels) {
        label->stackUnder(highlightLabel);
    }
    
    // Make sure the toggle button stays on top
    m_labelToggleButton->raise();

    // Delete the highlight label after 500ms
    QTimer::singleShot(500, highlightLabel, &QLabel::deleteLater);
} 
