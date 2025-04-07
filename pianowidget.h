/**
 * @file pianowidget.h
 * @author Alan Cruz
 * @brief Header file for the piano widget interface
 * @version 1.0
 * @date 2024-03-21
 * 
 * @copyright Copyright (c) 2024
 */

#ifndef PIANOWIDGET_H
#define PIANOWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QVector>
#include <QFrame>
#include <QLabel>
#include <QSet>
#include <QMap>
#include "keyboard.h"

/**
 * @brief Class representing a piano widget with interactive keys
 * 
 * Provides a visual and interactive piano interface with both white and black keys,
 * supporting keyboard input and visual feedback
 */
class PianoWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(int currentNote READ getCurrentNote WRITE setCurrentNote)

public:
    /**
     * @brief Gets the singleton instance of the piano widget
     * @return Pointer to the PianoWidget instance
     */
    static PianoWidget* instance();

    /**
     * @brief Attaches the piano widget to a placeholder frame
     * @param placeholder The frame to attach the piano widget to
     */
    void attachToPlaceholder(QFrame* placeholder);

    /**
     * @brief Detaches the piano widget from its current placeholder
     */
    void detach();

    /**
     * @brief Resets the piano state, releasing all pressed keys
     */
    void reset();

    /**
     * @brief Highlights keys to indicate correct/incorrect attempt
     * @param isCorrect Whether the attempt was correct
     */
    void highlightAttempt(bool isCorrect);

    // Property accessors
    int getCurrentNote() const { return m_currentNote; }
    void setCurrentNote(int note) { m_currentNote = note; }

protected:
    /**
     * @brief Handles key press events
     * @param event The key press event
     */
    void keyPressEvent(QKeyEvent* event) override;

    /**
     * @brief Handles key release events
     * @param event The key release event
     */
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    /**
     * @brief Constructs a new PianoWidget
     * @param parent The parent widget
     */
    explicit PianoWidget(QWidget* parent = nullptr);

    static PianoWidget* s_instance;
    QWidget* m_overlayWidget;  // Temporary overlay for highlighting

    // UI Elements
    QVector<QPushButton*> m_whiteKeys;
    QVector<QPushButton*> m_blackKeys;
    QVector<QLabel*> m_whiteKeyLabels;      // Labels for white keys
    QVector<QLabel*> m_blackKeyLabels;      // Labels for black keys
    QPushButton* m_labelToggleButton;       // Button to toggle labels
    QFrame* m_currentPlaceholder;
    Keyboard* m_keyboard;
    QMap<int, QPushButton*> m_keyToButton;
    QSet<int> currentlyPressedKeys;         // Track currently pressed keys
    bool m_showLabels;                      // Whether labels are currently shown
    bool m_isKeyboardInput;                 // Flag to track if current input is from keyboard
    int m_currentNote;                      // Current note being played

    // Setup methods
    /**
     * @brief Initializes the piano widget
     */
    void initializePiano();

    /**
     * @brief Sets up the piano keys
     * @param container The container widget for the keys
     */
    void setupKeys(QWidget* container);

    /**
     * @brief Applies styles to the piano keys
     */
    void applyKeyStyles();

    /**
     * @brief Connects key signals to their respective slots
     */
    void connectKeySignals();

    /**
     * @brief Sets up keyboard mapping for key events
     */
    void setupKeyboardMapping();

    /**
     * @brief Sets up labels for the piano keys
     */
    void setupKeyLabels();                  // New method to setup key labels

    /**
     * @brief Updates the visibility of key labels
     */
    void updateKeyLabelsVisibility();       // New method to update label visibility

private Q_SLOTS:
    /**
     * @brief Handles white key press events
     */
    void onWhiteKeyPressed();

    /**
     * @brief Handles black key press events
     */
    void onBlackKeyPressed();

    /**
     * @brief Handles key release events
     */
    void onKeyReleased();

    /**
     * @brief Toggles the visibility of key labels
     */
    void onToggleLabels();                 // New slot for toggle button

Q_SIGNALS:
    /**
     * @brief Emitted when a key is pressed
     * @param noteIndex The index of the pressed note
     */
    void keyPressed(int noteIndex);

    /**
     * @brief Emitted when a key is released
     * @param noteIndex The index of the released note
     */
    void keyReleased(int noteIndex);
};

#endif // PIANOWIDGET_H 