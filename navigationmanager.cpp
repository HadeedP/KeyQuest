/**
 * @file navigationmanager.cpp
 * @brief Implementation of the NavigationManager class for KeyQuest
 * @author Alan Cruz
 * @details This file implements the navigation system for the KeyQuest application,
 *          managing transitions between different pages and screens of the application.
 */

#include "navigationmanager.h"
#include <QDebug>

/**
 * @brief Constructor for NavigationManager
 * @param stackedWidget Pointer to the QStackedWidget that manages page transitions
 * @param parent Pointer to the parent QObject
 * @details Initializes the navigation manager with a reference to the stacked widget
 *          that will handle the actual page transitions.
 */
NavigationManager::NavigationManager(QStackedWidget* stackedWidget, QObject* parent)
    : QObject(parent)
    , m_stackedWidget(stackedWidget)
{
}

/**
 * @brief Navigates to the main page of the application
 * @details Sets the stacked widget's current index to 0 (main page) and emits
 *          a signal indicating the page change.
 */
void NavigationManager::navigateToMainPage()
{
    if (m_stackedWidget) {
        m_stackedWidget->setCurrentIndex(0);  // mainPage
        emit pageChanged(m_stackedWidget->currentWidget());
        SoundManager::instance()->startBackgroundMusic();
    }
}

/**
 * @brief Navigates to the quizzes page
 * @details Sets the stacked widget's current index to 4 (quizzes page) and emits
 *          a signal indicating the page change.
 */
void NavigationManager::navigateToQuizzes()
{
    if (m_stackedWidget) {
        m_stackedWidget->setCurrentIndex(4);  // quizzesPage
        emit pageChanged(m_stackedWidget->currentWidget());
        SoundManager::instance()->startBackgroundMusic();
    }
}

/**
 * @brief Navigates to the settings page
 * @details Sets the stacked widget's current index to 5 (settings page) and emits
 *          a signal indicating the page change.
 */
void NavigationManager::navigateToSettings()
{
    if (m_stackedWidget) {
        m_stackedWidget->setCurrentIndex(5);  // settingsPage
        emit pageChanged(m_stackedWidget->currentWidget());
        SoundManager::instance()->startBackgroundMusic();
    }
}

/**
 * @brief Navigates to the lessons page
 * @details Sets the stacked widget's current index to 6 (lessons page) and emits
 *          a signal indicating the page change.
 */
void NavigationManager::navigateToLessons()
{
    if (m_stackedWidget) {
        m_stackedWidget->setCurrentIndex(6);  // lessonsPage
        emit pageChanged(m_stackedWidget->currentWidget());
        SoundManager::instance()->startBackgroundMusic();
    }
}

/**
 * @brief Navigates to the multiplayer page
 * @details Sets the stacked widget's current index to 7 (multiplayer page) and emits
 *          a signal indicating the page change.
 */
void NavigationManager::navigateToMultiplayer()
{
    if (m_stackedWidget) {
        m_stackedWidget->setCurrentIndex(7);  // multiplayerPage
        emit pageChanged(m_stackedWidget->currentWidget());
        SoundManager::instance()->startBackgroundMusic();
    }
}

/**
 * @brief Navigates to the statistics page
 * @details Sets the stacked widget's current index to 8 (statistics page) and emits
 *          a signal indicating the page change.
 */
void NavigationManager::navigateToStatistics()
{
    if (m_stackedWidget) {
        m_stackedWidget->setCurrentIndex(8);  // statisticsPage
        emit pageChanged(m_stackedWidget->currentWidget());
        SoundManager::instance()->startBackgroundMusic();
    }
}

/**
 * @brief Navigates to the free style page
 * @details Sets the stacked widget's current index to 9 (free style page) and emits
 *          a signal indicating the page change.
 */
void NavigationManager::navigateToFreeStyle()
{
    if (m_stackedWidget) {
        m_stackedWidget->setCurrentIndex(9);  // freeStylePage
        emit pageChanged(m_stackedWidget->currentWidget());
        SoundManager::instance()->stopBackgroundMusic();
    }
}

/**
 * @brief Navigates to the local multiplayer page
 * @details Sets the stacked widget's current index to 2 (local multiplayer page) and emits
 *          a signal indicating the page change.
 */
void NavigationManager::navigateToLocalMultiplayer()
{
    if (m_stackedWidget) {
        m_stackedWidget->setCurrentIndex(2);  // LocalMultiplayerPage
        emit pageChanged(m_stackedWidget->currentWidget());
        SoundManager::instance()->startBackgroundMusic();
    }
}

/**
 * @brief Navigates to the lessons page screen
 * @details Sets the stacked widget's current index to 1 (lessons page screen) and emits
 *          a signal indicating the page change.
 */
void NavigationManager::navigateToLessonsPageScreen()
{
    if(m_stackedWidget){
        m_stackedWidget->setCurrentIndex(1);  // lessonsPageScreen
        emit pageChanged(m_stackedWidget->currentWidget());
        SoundManager::instance()->stopBackgroundMusic();
    }
}

/**
 * @brief Navigates to the game play screen
 * @details Sets the stacked widget's current index to 3 (local game play screen) and emits
 *          a signal indicating the page change.
 */
void NavigationManager::navigateToGamePlay()
{
    if (m_stackedWidget) {
        m_stackedWidget->setCurrentIndex(3);  // localGamePlayScreen
        emit pageChanged(m_stackedWidget->currentWidget());
        SoundManager::instance()->stopBackgroundMusic();
    }
}
