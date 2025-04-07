/**
 * @file soundmanager.h
 * @brief Header file for the SoundManager class
 * @author Alan Cruz, Liam Elliott
 * @details This file defines the SoundManager class which handles all sound effects
 *          and background music in the KeyQuest application. It implements a singleton
 *          pattern to ensure only one instance manages all audio.
 */

#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QSoundEffect>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QObject>
#include <QUrl>
#include <QStringList>

/**
 * @brief Manages sound effects and background music throughout the application
 * 
 * The SoundManager class is responsible for playing sound effects like button clicks
 * and managing background music. It implements a singleton pattern to ensure only
 * one instance exists and manages all audio in the application.
 */
class SoundManager : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Gets the singleton instance of the SoundManager
     * @return Pointer to the SoundManager instance
     */
    static SoundManager* instance();

    /**
     * @brief Plays the button click sound effect
     * @details Plays a short click sound when a button is pressed.
     *          The sound will only play if sound is not muted.
     */
    void playButtonClick();

    /**
     * @brief setSFXVolume
     * @param volume
     */
    void setSFXVolume(float volume);

    /**
     * @brief setBGMusicVolume
     * @param volume
     */
    void setBGMusicVolume(float volume);

    /**
     * @brief Sets whether sound effects are muted
     * @param muted True to mute sound effects, false to unmute
     */
    void setSFXMuted(bool muted);

    /**
     * @brief Sets whether background music is muted.
     * @param muted True to mute background music, false to unmute.
     */
    void setBGMusicMuted(bool muted);

    /**
     * @brief Gets the current mute state for the sound effects.
     * @return True if sound effects are muted, false otherwise.
     */
    bool isSFXMuted() const;

    /**
     * @brief Gets the current mute state for the background music.
     * @return True if the background music is muted, false otherwise.
     */
    bool isBGMusicMuted() const;

    /**
     * @brief Starts playing background music
     * @details Plays the background music tracks in sequence.
     *          The music will only play if not muted.
     */
    void startBackgroundMusic();

    /**
     * @brief Stops playing background music
     */
    void stopBackgroundMusic();

    /**
     * @brief Pauses background music
     */
    void pauseBackgroundMusic();

    /**
     * @brief Resumes background music
     */
    void resumeBackgroundMusic();

private slots:
    /**
     * @brief Handles the end of a music track
     * @details Switches to the next track in the playlist
     */
    void handleMusicEnd(QMediaPlayer::MediaStatus status);

private:
    /**
     * @brief Private constructor to enforce singleton pattern
     * @param parent The parent QObject
     */
    explicit SoundManager(QObject *parent = nullptr);

    /**
     * @brief Switches to the next background music track
     */
    void switchToNextTrack();

    static SoundManager* m_instance;  ///< The singleton instance
    QSoundEffect m_buttonClick;      ///< Sound effect for button clicks
    QMediaPlayer m_backgroundMusic;  ///< Media player for background music
    QAudioOutput* m_audioOutput;      ///< Audio output for background music
    bool m_sfx_muted;                    ///< Whether sound effects are muted
    bool m_bgmusic_muted;               ///< Whether the background music is muted
    int m_currentTrack;              ///< Index of the current track
    QStringList m_backgroundTracks;  ///< List of background music tracks
};

#endif // SOUNDMANAGER_H 
