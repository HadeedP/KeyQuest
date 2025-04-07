/**
 * @file mainwindow.h
 * @author Alan Cruz
 * @brief Header file for the main application window
 * @version 1.0
 * @date 2024-03-21
 * 
 * @copyright Copyright (c) 2024
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "navigationmanager.h"
#include "multiplayergamewidget.h"
#include "lessonswidget.h"
#include "soundmanager.h"
#include "statisticswidget.h"
#include "quizwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief Main window class for the KeyQuest application
 * 
 * Manages the main application window, including navigation between
 * different pages and handling of game sessions
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new MainWindow
     * @param parent The parent widget
     */
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void showEvent(QShowEvent* event) override;

private Q_SLOTS:
    /**
     * @brief Handles navigation between different pages
     * @param newPage The new page widget to display
     */
    void handlePageChange(QWidget* newPage);
    /**
     * @brief Exits the application
     */
    void exitApplication();
    /**
     * @brief Starts a general gameplay session
     */
    void startGeneralGamePlay();
    /**
     * @brief Starts a major/minor chords gameplay session
     */
    void startMajorMinorGamePlay();
    /**
     * @brief Starts a major third gameplay session
     */
    void startMajorThirdGamePlay();
    /**
     * @brief Starts a scale gameplay session
     */
    void startScale();
    /**
     * @brief Starts a triad gameplay session
     */
    void startTriad();
    /**
     * @brief Starts a rhythm gameplay session
     */
    void startRhythm();
    /**
     * @brief Starts the adaptive quiz
     */
    void startAdaptiveQuiz();
    /**
     * @brief Starts a game with the specified topic ID
     * @param topicId The ID of the game topic to start
     */
    void startGameWithTopicId(int topicId);

private:
    /**
     * @brief Sets up the navigation system
     */
    void setupNavigation();
    /**
     * @brief Sets up signal and slot connections
     */
    void setupConnections();
    /**
     * @brief Scales all buttons to fit the window
     */
    void scaleAllButtons();

    Ui::MainWindow *ui;
    NavigationManager* navigationManager;
    MultiplayerGameWidget* gameWidget;
    LessonsWidget* lessonsWidget;
    StatisticsWidget *statisticsWidget;
    QuizWidget* quizWidget;
};

#endif // MAINWINDOW_H
