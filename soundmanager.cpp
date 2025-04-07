/**
 * @file soundmanager.cpp
 * @brief Implementation of the SoundManager class
 * @author Alan Cruz, Liam Elliott
 * @details This file implements the SoundManager class which handles all sound effects
 *          and background music in the KeyQuest application. It implements a singleton
 *          pattern to ensure only one instance manages all audio.
 */

#include "soundmanager.h"
#include <QDebug>

// Initialize static member
SoundManager* SoundManager::m_instance = nullptr;

/**
 * @brief Gets the singleton instance of the SoundManager
 * @return Pointer to the SoundManager instance
 * @details Creates a new instance if one doesn't exist, otherwise returns
 *          the existing instance.
 */
SoundManager* SoundManager::instance()
{
    if (!m_instance) {
        m_instance = new SoundManager();
    }
    return m_instance;
}

/**
 * @brief Constructs a new SoundManager
 * @param parent The parent QObject
 * @details Initializes the button click sound effect and background music player.
 *          Sets up the background music tracks and connects signals.
 */
SoundManager::SoundManager(QObject *parent)
    : QObject(parent)
    , m_sfx_muted(false)
    , m_bgmusic_muted(false)
    , m_currentTrack(0)
    , m_audioOutput(new QAudioOutput(this))
{
    // Set up button click sound
    m_buttonClick.setSource(QUrl("qrc:/sounds/soundFiles/buttonClick.wav"));
    m_buttonClick.setVolume(0.5f);  // 50% volume

    // Set up background music tracks
    m_backgroundTracks << "qrc:/sounds/soundFiles/anoen.mp3"
                      << "qrc:/sounds/soundFiles/quend.mp3";

    // Set up background music player
    m_audioOutput->setVolume(0.3f);  // 30% volume for background music
    m_backgroundMusic.setAudioOutput(m_audioOutput);
    
    connect(&m_backgroundMusic, &QMediaPlayer::mediaStatusChanged,
            this, &SoundManager::handleMusicEnd);
}

/**
 * @brief Plays the button click sound effect
 * @details Plays the sound effect if sound is not muted.
 *          The sound is played at the volume set in the constructor.
 */
void SoundManager::playButtonClick()
{
    if (!m_sfx_muted) {
        m_buttonClick.play();
    }
}

/**
 * @brief Changes the program-wide volume of sound effects, such as the button click.
 * @param volume The new volume of the sound effects, chosen by the slider on the 'Settings' page.
 * @details Sets the volume of the sound effects between 0% and 100&.
 *          If the volume is zero, then the sound effects are considered muted.
 */
void SoundManager::setSFXVolume(float volume)
{
    (volume == 0) ? setSFXMuted(true) : setSFXMuted(false);
    m_buttonClick.setVolume(volume / 100);
}

/**
 * @brief Changes the program-wide volume of the background music.
 * @param volume The new volume of the background music, chosen by the slider on the 'Settings' page.
 * @details Sets the volume of the background music between 0% and 100&.
 *          If the volume is zero, then the background music is considered muted.
 */
void SoundManager::setBGMusicVolume(float volume)
{
    (volume == 0) ? setBGMusicMuted(true) : setBGMusicMuted(false);
    m_audioOutput->setVolume(volume / 100);
}

/**
 * @brief Sets whether sound effects are muted
 * @param muted True to mute sound effects, false to unmute
 */
void SoundManager::setSFXMuted(bool muted)
{
    m_sfx_muted = muted;
}

/**
 * @brief Sets whether the background music is muted.
 * @param muted True to mute the background music, false to unmute.
 * @details Pauses the background music. When unmuted, the music will start right where it left off.
 */
void SoundManager::setBGMusicMuted(bool muted)
{
    m_bgmusic_muted = muted;
    if (m_bgmusic_muted) {
        m_backgroundMusic.pause();
    } else {
        m_backgroundMusic.play();
    }
}

/**
 * @brief Gets the current mute state for the sound effects.
 * @return True if sound effects are muted, false otherwise.
 */
bool SoundManager::isSFXMuted() const
{
    return m_sfx_muted;
}

/**
 * @brief Gets the current mute state for the background music.
 * @return True if the background music is muted, false otherwise.
 */
bool SoundManager::isBGMusicMuted() const
{
    return m_bgmusic_muted;
}

/**
 * @brief Starts playing background music
 * @details Plays the background music tracks in sequence.
 *          The music will only play if not muted.
 */
void SoundManager::startBackgroundMusic()
{
    if (!m_bgmusic_muted && m_backgroundMusic.playbackState() != QMediaPlayer::PlayingState) {
        switchToNextTrack();
        m_backgroundMusic.play();
    }
}

/**
 * @brief Stops playing background music
 */
void SoundManager::stopBackgroundMusic()
{
    m_backgroundMusic.stop();
    m_currentTrack = 0;
}

/**
 * @brief Pauses background music
 */
void SoundManager::pauseBackgroundMusic()
{
    m_backgroundMusic.pause();
}

/**
 * @brief Resumes background music
 */
void SoundManager::resumeBackgroundMusic()
{
    if (!m_bgmusic_muted) {
        m_backgroundMusic.play();
    }
}

/**
 * @brief Handles the end of a music track
 * @details Switches to the next track in the playlist
 */
void SoundManager::handleMusicEnd(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia) {
        switchToNextTrack();
        if (!m_bgmusic_muted) {
            m_backgroundMusic.play();
        }
    }
}

/**
 * @brief Switches to the next background music track
 */
void SoundManager::switchToNextTrack()
{
    m_currentTrack = (m_currentTrack + 1) % m_backgroundTracks.size();
    m_backgroundMusic.setSource(QUrl(m_backgroundTracks[m_currentTrack]));
} 
