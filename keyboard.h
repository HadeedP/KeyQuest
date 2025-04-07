/**
 * @file keyboard.h
 * @author Alan Cruz
 * @brief Header file for the keyboard input handling
 * @version 1.0
 * @date 2024-03-21
 * 
 * @copyright Copyright (c) 2024
 */

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QtCore/QObject>
#include <QMap>
#include <fluidsynth.h>
#include <mutex>

/**
 * @brief Class managing keyboard input and MIDI note mapping
 * 
 * Handles keyboard input events and maps them to MIDI note numbers
 * for the piano interface
 */
class Keyboard {
public:
    /**
     * @brief Constructs a new Keyboard object
     */
    Keyboard();

    /**
     * @brief Handles cleanup of resources
     */
    ~Keyboard();

    /**
     * @brief Gets the MIDI note number for a given key
     * @param key The key code
     * @return int The MIDI note number, or -1 if invalid
     */
    int getNoteForKey(int key);

    /**
     * @brief Plays a note using FluidSynth
     * @param note The MIDI note number to play
     */
    void playNote(int note);

    /**
     * @brief Stops playing a note
     * @param note The MIDI note number to stop
     */
    void stopNote(int note);

private:
    fluid_settings_t* settings;
    fluid_synth_t* synth;
    fluid_audio_driver_t* adriver;
    std::mutex mtx;
};

#endif // KEYBOARD_H
