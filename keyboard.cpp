/**
 * @file keyboard.cpp
 * @brief Implementation of the Keyboard class for KeyQuest
 * @author Alan Cruz
 * @details This file implements the keyboard functionality for the KeyQuest game,
 *          handling MIDI sound synthesis using FluidSynth and managing audio playback.
 */

#include "keyboard.h"
#include <unistd.h>
#include <QCoreApplication>
#include <QtCore/QFile>
#include <QStandardPaths>
#include <QtCore/QTimer>

/**
 * @brief Constructor for Keyboard
 * @details Initializes the FluidSynth synthesizer and audio driver based on the operating system.
 *          Sets up the appropriate audio driver (CoreAudio for macOS, DirectSound for Windows,
 *          PulseAudio for Linux) and loads the piano SoundFont from resources.
 *          The SoundFont is extracted to a temporary location for FluidSynth to use.
 */
Keyboard::Keyboard() {
    settings = new_fluid_settings();
    if (!settings) {
        qDebug() << "ERROR: Failed to create FluidSynth settings!";
        return;
    }

    // Configure FluidSynth settings for better polyphony
    fluid_settings_setint(settings, "synth.polyphony", 128);  // Allow up to 128 simultaneous notes
    fluid_settings_setnum(settings, "synth.sample-rate", 44100.0);
    fluid_settings_setnum(settings, "synth.gain", 0.5);
    fluid_settings_setint(settings, "synth.audio-channels", 2);
    fluid_settings_setint(settings, "synth.audio-groups", 1);
    fluid_settings_setint(settings, "synth.effects-channels", 2);
    fluid_settings_setint(settings, "synth.reverb.active", 0);
    fluid_settings_setint(settings, "synth.chorus.active", 0);
    fluid_settings_setint(settings, "audio.periods", 8);
    fluid_settings_setint(settings, "audio.period-size", 256);
    fluid_settings_setstr(settings, "audio.sample-format", "float");

// **Set Correct Audio Driver Based on OS**
#ifdef Q_OS_MAC
    fluid_settings_setstr(settings, "audio.driver", "coreaudio");
#elif defined(Q_OS_WIN)
    fluid_settings_setstr(settings, "audio.driver", "dsound");
#elif defined(Q_OS_LINUX)
    fluid_settings_setstr(settings, "audio.driver", "pulseaudio");
#endif

    synth = new_fluid_synth(settings);
    if (!synth) {
        qDebug() << "ERROR: Failed to create FluidSynth synthesizer!";
        return;
    }

    adriver = new_fluid_audio_driver(settings, synth);
    if (!adriver) {
        qDebug() << "ERROR: Failed to create FluidSynth audio driver!";
        return;
    }

    // Extract the SoundFont from Qt Resources
    QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/piano.sf2";
    QFile soundFontFile(":/sounds/piano.sf2"); // Make sure this matches your .qrc prefix!

    if (!QFile::exists(tempPath)) { // Check if file is already extracted
        if (soundFontFile.exists()) { // Check if the file exists in resources
            if (soundFontFile.open(QIODevice::ReadOnly)) {
                QFile tempFile(tempPath);
                if (tempFile.open(QIODevice::WriteOnly)) {
                    if (tempFile.write(soundFontFile.readAll()) == -1) {
                        qDebug() << "ERROR: Failed to write SoundFont to disk!";
                        return;
                    }
                    tempFile.close();

#ifdef Q_OS_WIN
                    QFile::setPermissions(tempPath, QFile::ReadUser | QFile::WriteUser);
#else
                    QFile::setPermissions(tempPath, QFile::ReadOwner | QFile::WriteOwner);
#endif
                    qDebug() << "Extracted SoundFont to:" << tempPath;
                } else {
                    qDebug() << "ERROR: Failed to create temp file!";
                    return;
                }
                soundFontFile.close();
            } else {
                qDebug() << "ERROR: Failed to open resource SoundFont file!";
                return;
            }
        } else {
            qDebug() << "ERROR: SoundFont resource not found!";
            return;
        }
    } else {
        qDebug() << "SoundFont already exists at:" << tempPath;
    }

    // **Load SoundFont in FluidSynth**
    int sf_id = fluid_synth_sfload(synth, tempPath.toUtf8().constData(), 1);
    if (sf_id == -1) {
        qDebug() << "ERROR: Failed to load SoundFont!";
    } else {
        qDebug() << "SoundFont Loaded Successfully!";
        // Set the instrument to piano (program 0) after loading the soundfont
        fluid_synth_program_select(synth, 0, sf_id, 0, 0);
    }
}

/**
 * @brief Destructor for Keyboard
 * @details Cleans up FluidSynth resources by deleting the audio driver,
 *          synthesizer, and settings in the correct order.
 */
Keyboard::~Keyboard() {
    delete_fluid_audio_driver(adriver);
    delete_fluid_synth(synth);
    delete_fluid_settings(settings);
}

/**
 * @brief Plays a musical note using FluidSynth
 * @param note The MIDI note number to play (0-127)
 * @details Starts playing the specified note at maximum velocity.
 *          The note will continue playing until stopNote is called.
 */
void Keyboard::playNote(int note) {
    if (!synth) return;

    // Use a single lock for the entire operation
    std::lock_guard<std::mutex> lock(mtx);
    
    // Play the note with maximum velocity (127)
    fluid_synth_noteon(synth, 0, note, 127);
}

/**
 * @brief Stops playing a musical note
 * @param note The MIDI note number to stop (0-127)
 * @details Stops the specified note that was previously started with playNote.
 */
void Keyboard::stopNote(int note) {
    if (!synth) return;

    // Use a single lock for the entire operation
    std::lock_guard<std::mutex> lock(mtx);
    
    // Stop the note
    fluid_synth_noteoff(synth, 0, note);
}
