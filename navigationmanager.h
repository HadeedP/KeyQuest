/**
 * @file navigationmanager.h
 * @author Alan Cruz
 * @brief Header file for the navigation management system
 * @version 1.0
 * @date 2024-03-21
 * 
 * @copyright Copyright (c) 2024
 */

#ifndef NAVIGATIONMANAGER_H
#define NAVIGATIONMANAGER_H

#include <QtCore/QObject>
#include <QStackedWidget>
#include "soundmanager.h"

/**
 * @brief Class managing navigation between different pages in the application
 * 
 * Handles the navigation logic and page transitions throughout the application,
 * ensuring proper cleanup and state management between page changes
 */
class NavigationManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new NavigationManager
     * @param stackedWidget The QStackedWidget to manage
     * @param parent The parent QObject
     */
    explicit NavigationManager(QStackedWidget* stackedWidget, QObject* parent = nullptr);

public Q_SLOTS:
    /**
     * @brief Navigates to the main menu page
     */
    void navigateToMainPage();

    /**
     * @brief Navigates to the quizzes page
     */
    void navigateToQuizzes();

    /**
     * @brief Navigates to the settings page
     */
    void navigateToSettings();

    /**
     * @brief Navigates to the lessons page
     */
    void navigateToLessons();

    /**
     * @brief Navigates to the multiplayer page
     */
    void navigateToMultiplayer();

    /**
     * @brief Navigates to the statistics page
     */
    void navigateToStatistics();

    /**
     * @brief Navigates to the free style page
     */
    void navigateToFreeStyle();

    /**
     * @brief Navigates to the local multiplayer page
     */
    void navigateToLocalMultiplayer();

    /**
     * @brief Navigates to the game play page
     */
    void navigateToGamePlay();

    /**
     * @brief Navigates to the lessons page screen
     */
    void navigateToLessonsPageScreen();

Q_SIGNALS:
    /**
     * @brief Emitted when the page changes
     * @param newPage The new page widget
     */
    void pageChanged(QWidget* newPage);

private:
    QStackedWidget* m_stackedWidget;
};

#endif // NAVIGATIONMANAGER_H 
