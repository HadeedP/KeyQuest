/**
 * @file mainwindow.cpp
 * @brief Implementation of the MainWindow class for KeyQuest
 * @author Alan Cruz, Liam Elliott
 * @details This file implements the main window functionality for the KeyQuest game,
 *          handling window management, navigation, and game mode initialization.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScreen>
#include "mathutils.h"
#include <QKeyEvent>
#include <QtCore/QTimer>
#include "pianowidget.h"
#include <QPainter>
#include "soundmanager.h"
#include "loaddatamanager.h"

/**
 * @brief Constructor for MainWindow
 * @param parent Pointer to the parent widget
 * @details Initializes the main window, sets up the UI, handles screen resolution,
 *          and initializes navigation and connections.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , navigationManager(nullptr)
    , gameWidget(nullptr)
    , lessonsWidget(nullptr)
    , statisticsWidget(nullptr)
    , quizWidget(nullptr)
{
    ui->setupUi(this);

    // Get the primary screen
    QScreen* screen = QGuiApplication::primaryScreen();
    QSize screenSize = screen->size();

    // Ensure we don't exceed 4K resolution (3840x2160)
    if (screenSize.width() > 3840 || screenSize.height() > 2160) {
        double scaleX = 3840.0 / screenSize.width();
        double scaleY = 2160.0 / screenSize.height();
        double scale = qMin(scaleX, scaleY);

        screenSize = QSize(screenSize.width() * scale, screenSize.height() * scale);
    }

    // Set window to full screen
    showFullScreen();

    // Scale widgets based on the actual screen size
    scaleAllButtons();

    setupNavigation();
    setupConnections();

    // Load saved volume levels
    int bgMusicLevel = LoadDataManager::instance()->getBackgroundMusicLevel();
    int fxSoundLevel = LoadDataManager::instance()->getFXSoundLevel();
    
    // Set the slider values
    ui->musicVolumeSlider->setValue(bgMusicLevel);
    ui->sfxVolumeSlider->setValue(fxSoundLevel);
    
    // Apply the volumes
    SoundManager::instance()->setBGMusicVolume(bgMusicLevel);
    SoundManager::instance()->setSFXVolume(fxSoundLevel);
}

/**
 * @brief Destructor for MainWindow
 * @details Cleans up UI and navigation manager resources.
 */
MainWindow::~MainWindow()
{
    delete ui;
    delete navigationManager;
}

/**
 * @brief Sets up the navigation system
 * @details Initializes the navigation manager and connects its signals.
 */
void MainWindow::setupNavigation()
{
    navigationManager = new NavigationManager(ui->stackedWidget, this);
    connect(navigationManager, &NavigationManager::pageChanged, this, &MainWindow::handlePageChange);
}

/**
 * @brief Sets up all signal and slot connections
 * @details Connects all UI buttons to their respective navigation and game mode functions.
 */
void MainWindow::setupConnections()
{
    // Connect navigation buttons with sound effects
    connect(ui->settingsButton, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);
    connect(ui->lessonsButton, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);
    connect(ui->statisticsButton, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);
    connect(ui->quizzesButton, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);
    connect(ui->multiplayerButton, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);
    connect(ui->freeStyleButton, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);
    connect(ui->exitButton, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);
    connect(ui->localMatchButton, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);

    // Connect navigation actions
    connect(ui->settingsButton, &QPushButton::clicked, navigationManager, &NavigationManager::navigateToSettings);
    connect(ui->lessonsButton, &QPushButton::clicked, navigationManager, &NavigationManager::navigateToLessons);
    connect(ui->statisticsButton, &QPushButton::clicked, navigationManager, &NavigationManager::navigateToStatistics);
    connect(ui->quizzesButton, &QPushButton::clicked, navigationManager, &NavigationManager::navigateToQuizzes);
    connect(ui->multiplayerButton, &QPushButton::clicked, navigationManager, &NavigationManager::navigateToMultiplayer);
    connect(ui->freeStyleButton, &QPushButton::clicked, navigationManager, &NavigationManager::navigateToFreeStyle);
    connect(ui->exitButton, &QPushButton::clicked, this, &MainWindow::exitApplication);
    connect(ui->localMatchButton, &QPushButton::clicked, navigationManager, &NavigationManager::navigateToLocalMultiplayer);

    // Connect game mode buttons with sound effects
    connect(ui->generalGamePlay, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);
    connect(ui->multiplayerMajorMinorChords, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);
    connect(ui->lessonOneButton, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);
    connect(ui->Scales, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);
    connect(ui->identifyingMajorThird, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);
    connect(ui->Triad, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);
    connect(ui->Rhythm, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);
    connect(ui->lessonTwoButton, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);
    connect(ui->lessonThreeButton, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);
    connect(ui->lessonFourButton, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);
    connect(ui->lessonFiveButton, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);
    connect(ui->lessonSixButton, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);
    connect(ui->startButton, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);

    // Connect game mode actions
    connect(ui->generalGamePlay, &QPushButton::clicked, this, &MainWindow::startGeneralGamePlay);
    connect(ui->multiplayerMajorMinorChords, &QPushButton::clicked, this, &MainWindow::startMajorMinorGamePlay);
    connect(ui->lessonOneButton, &QPushButton::clicked, this, &MainWindow::startGeneralGamePlay);
    connect(ui->Scales, &QPushButton::clicked, this, &MainWindow::startScale);
    connect(ui->identifyingMajorThird, &QPushButton::clicked, this, &MainWindow::startMajorThirdGamePlay);
    connect(ui->Triad, &QPushButton::clicked, this, &MainWindow::startTriad);
    connect(ui->Rhythm, &QPushButton::clicked, this, &MainWindow::startRhythm);
    connect(ui->lessonTwoButton, &QPushButton::clicked, this, &MainWindow::startMajorMinorGamePlay);
    connect(ui->lessonThreeButton, &QPushButton::clicked, this, &MainWindow::startTriad);
    connect(ui->lessonFourButton, &QPushButton::clicked, this, &MainWindow::startScale);
    connect(ui->lessonFiveButton, &QPushButton::clicked, this, &MainWindow::startMajorThirdGamePlay);
    connect(ui->lessonSixButton, &QPushButton::clicked, this, &MainWindow::startRhythm);
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::startAdaptiveQuiz);

    // Connect return buttons with sound effects
    connect(ui->returnFromSettingsButton, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);
    connect(ui->returnFromLessonsButton, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);
    connect(ui->returnFromMultiplayerButton, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);
    connect(ui->returnFromQuizzesButton, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);
    connect(ui->returnFromStatisticsButton, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);
    connect(ui->returnFromFreeStyleButton, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);
    connect(ui->returnToLocalGamePlayScreen, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);
    connect(ui->returnFromLocalButton, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);
    connect(ui->ReturnToLessonsPage, &QPushButton::pressed, SoundManager::instance(), &SoundManager::playButtonClick);

    // Connect return actions
    connect(ui->returnFromSettingsButton, &QPushButton::clicked, navigationManager, &NavigationManager::navigateToMainPage);
    connect(ui->returnFromLessonsButton, &QPushButton::clicked, navigationManager, &NavigationManager::navigateToMainPage);
    connect(ui->returnFromMultiplayerButton, &QPushButton::clicked, navigationManager, &NavigationManager::navigateToMainPage);
    connect(ui->returnFromQuizzesButton, &QPushButton::clicked, navigationManager, &NavigationManager::navigateToMainPage);
    connect(ui->returnFromStatisticsButton, &QPushButton::clicked, navigationManager, &NavigationManager::navigateToMainPage);
    connect(ui->returnFromFreeStyleButton, &QPushButton::clicked, navigationManager, &NavigationManager::navigateToLessons);
    connect(ui->returnToLocalGamePlayScreen, &QPushButton::clicked, navigationManager, &NavigationManager::navigateToLocalMultiplayer);
    connect(ui->returnFromLocalButton, &QPushButton::clicked, navigationManager, &NavigationManager::navigateToMultiplayer);
    connect(ui->ReturnToLessonsPage, &QPushButton::clicked, navigationManager, &NavigationManager::navigateToLessons);

    // Connect settings actions
    connect(ui->musicVolumeSlider, &QSlider::valueChanged, this, [this](int value) {
        SoundManager::instance()->setBGMusicVolume(value);
        LoadDataManager::instance()->setBackgroundMusicLevel(value);
    });
    connect(ui->sfxVolumeSlider, &QSlider::valueChanged, this, [this](int value) {
        SoundManager::instance()->setSFXVolume(value);
        LoadDataManager::instance()->setFXSoundLevel(value);
    });
}

/**
 * @brief Starts a game with a specific topic ID
 * @param topicId The ID of the topic to load
 * @details Initializes either a lessons widget or game widget based on the current page,
 *          sets up the piano widget, and connects necessary signals.
 */
void MainWindow::startGameWithTopicId(int topicId)
{
    auto piano = PianoWidget::instance();

    if (ui->stackedWidget->currentWidget()->objectName() == "lessonsPage")
    {
        // First navigate to the lessons page screen
        navigationManager->navigateToLessonsPageScreen();


        // Clean up old widget if it exists
        if (lessonsWidget) {
            if (piano) {
                disconnect(piano, nullptr, lessonsWidget, nullptr);
            }
            lessonsWidget->deleteLater();
            lessonsWidget = nullptr;
        }

        // Create and setup new widget
        lessonsWidget = new LessonsWidget(this, topicId);
        lessonsWidget->setParent(ui->lessonsPlayPlaceHolder);
        lessonsWidget->setGeometry(ui->lessonsPlayPlaceHolder->rect());
        lessonsWidget->show();

        // Setup piano after widget is ready
        if (piano && ui->lessonsPagePianoHolder) {
            piano->attachToPlaceholder(ui->lessonsPagePianoHolder);
            connect(piano, &PianoWidget::keyPressed, lessonsWidget, &LessonsWidget::handleKeyPressed);
        }

    }
    else
    {
        // First navigate to the game play screen
        navigationManager->navigateToGamePlay();

        // Clean up old widget if it exists
        if (gameWidget) {
            if (piano) {
                disconnect(piano, nullptr, gameWidget, nullptr);
            }
            gameWidget->deleteLater();
            gameWidget = nullptr;
        }

        // Create and setup new widget
        gameWidget = new MultiplayerGameWidget(this, topicId);
        gameWidget->setParent(ui->gamePlayPlaceHolder);
        gameWidget->setGeometry(ui->gamePlayPlaceHolder->rect());
        gameWidget->show();

        // Setup piano after widget is ready
        if (piano && ui->pianoLocalPlaceholder) {
            piano->attachToPlaceholder(ui->pianoLocalPlaceholder);
            connect(piano, &PianoWidget::keyPressed, gameWidget, &MultiplayerGameWidget::handleKeyPressed);
        }
    }
}

/**
 * @brief Starts the general game mode
 * @details Initializes the game with topic ID 101.
 */
void MainWindow::startGeneralGamePlay()
{
    startGameWithTopicId(101);
}

/**
 * @brief Starts the major/minor chords game mode
 * @details Initializes the game with topic ID 102.
 */
void MainWindow::startMajorMinorGamePlay()
{
    startGameWithTopicId(102);
}

/**
 * @brief Starts the major third game mode
 * @details Initializes the game with topic ID 105.
 */
void MainWindow::startMajorThirdGamePlay()
{
    startGameWithTopicId(105);
}

/**
 * @brief Starts the scales game mode
 * @details Initializes the game with topic ID 104.
 */
void MainWindow::startScale()
{
    startGameWithTopicId(104);
}


/**
 * @brief Starts the triad game mode
 * @details Initializes the game with topic ID 103.
 */
void MainWindow::startTriad()
{
    startGameWithTopicId(103);
}

/**
 * @brief Starts the rhythm game mode
 * @details Initializes the game with topic ID 106.
 */
void MainWindow::startRhythm()
{
    startGameWithTopicId(106);
}

/**
 * @brief Starts the adaptive quiz gameplay
 * @details Initializes the QuizWidget, connects signals, and navigates to the quiz screen
 */
void MainWindow::startAdaptiveQuiz()
{
    auto piano = PianoWidget::instance();

    // First navigate to the quiz page screen (reusing the lessons page screen)
    navigationManager->navigateToLessonsPageScreen();

    // Clean up old widget if it exists
    if (quizWidget) {
        if (piano) {
            disconnect(piano, nullptr, quizWidget, nullptr);
        }
        quizWidget->deleteLater();
        quizWidget = nullptr;
    }

    // Create a new quiz widget
    quizWidget = new QuizWidget(ui->lessonsPageScreen);
    
    // Connect the quiz finished signal
    connect(quizWidget, &QuizWidget::quizFinished, navigationManager, &NavigationManager::navigateToQuizzes);

    // Explicitly attach the piano to the lessons page piano holder
    if (piano && ui->lessonsPagePianoHolder) {
        piano->attachToPlaceholder(ui->lessonsPagePianoHolder);
        connect(piano, &PianoWidget::keyPressed, quizWidget, &QuizWidget::handleKeyPressed);
        connect(piano, &PianoWidget::keyReleased, quizWidget, &QuizWidget::handleKeyReleased);
    }

    // Start the quiz
    quizWidget->startQuiz();
}

/**
 * @brief Handles page change events
 * @param newPage Pointer to the new page widget
 * @details Manages piano widget attachment/detachment and cleanup of game widgets
 *          when navigating between different pages.
 */
void MainWindow::handlePageChange(QWidget* newPage)
{
    auto piano = PianoWidget::instance();

    // First detach piano from its current location and reset its state
    if (piano) {
        piano->reset();  // Reset piano state before detaching
        piano->detach();
    }

    // Handle different page transitions
    if (newPage == ui->freeStylePage) {
        if (piano && ui->pianoPlaceholder) {
            piano->attachToPlaceholder(ui->pianoPlaceholder);
        }

        // Cleanup game widget if it exists
        if (gameWidget) {
            if (piano) {
                disconnect(piano, nullptr, gameWidget, nullptr);
            }
            gameWidget->deleteLater();
            gameWidget = nullptr;
        }
    }
    else if (newPage == ui->lessonsPageScreen) {
        // For lessons page, we don't cleanup widgets as they are managed by the lessons page
        // Just ensure piano is detached
        if (piano) {
            piano->detach();
        }
    }
    else if (newPage == ui->localGamePlayScreen) {
        // For local game screen, we don't cleanup widgets as they are managed by the game screen
        // Just ensure piano is detached
        if (piano) {
            piano->detach();
        }
    }
    else if (newPage == ui->statisticsPage) {
        // Clean up old statistics widget if it exists
        if (statisticsWidget) {
            statisticsWidget->deleteLater();
            statisticsWidget = nullptr;
        }

        // Create and setup new statistics widget
        statisticsWidget = new StatisticsWidget(ui->statisticsPage);
        statisticsWidget->setParent(ui->statisticsWidget);  // Use the frame from UI
        statisticsWidget->setGeometry(ui->statisticsWidget->rect());
        statisticsWidget->show();
    }
    else {
        // For all other pages, cleanup all widgets
        if (gameWidget) {
            if (piano) {
                disconnect(piano, nullptr, gameWidget, nullptr);
            }
            gameWidget->deleteLater();
            gameWidget = nullptr;
        }
        if (lessonsWidget) {
            if (piano) {
                disconnect(piano, nullptr, lessonsWidget, nullptr);
            }
            lessonsWidget->deleteLater();
            lessonsWidget = nullptr;
        }
        if (statisticsWidget) {
            statisticsWidget->deleteLater();
            statisticsWidget = nullptr;
        }
    }
}

/**
 * @brief Scales all UI elements based on screen resolution
 * @details Adjusts the size and position of all buttons, labels, and frames
 *          to maintain proper proportions across different screen resolutions.
 *          Handles scaling for both 4K and standard resolutions, ensuring
 *          the UI remains consistent with the original design (1920x1080).
 */
void MainWindow::scaleAllButtons()
{
    // Get the primary screen
    QScreen* screen = QGuiApplication::primaryScreen();
    QSize screenSize = screen->size();

    // Ensure we don't exceed 4K resolution
    if (screenSize.width() > 3840 || screenSize.height() > 2160) {
        double scaleX = 3840.0 / screenSize.width();
        double scaleY = 2160.0 / screenSize.height();
        double scale = qMin(scaleX, scaleY);

        screenSize = QSize(screenSize.width() * scale, screenSize.height() * scale);
    }

    // scaling parameters of the UI when it was designed
    int designWidth = 1920;
    int designHeight = 1080;

    QList<QComboBox*> boxes = {
        ui->colourblindModeBox
    };

    QList<QSlider*> sliders = {
        ui->musicVolumeSlider, ui->sfxVolumeSlider
    };

    QList<QLabel*> labels = {
        ui->selectGameModeLabel, ui->selectLesson, ui->accuracyLabel,
        ui->star1, ui->star2, ui->star3, ui->star4, ui->star5, ui->selectGameMode1, ui->descriptionLabel, ui->playerScoreLabel,
        ui->titleLabel, ui->descriptionLabelLocal, ui->currentPlayerLabelLocal, ui->player1ScoreLabelLocal, ui->player2ScoreLabelLocal,
        ui->titleLabelLocal, ui->selectQuizzesLabel, ui->settingsLabel, ui->settingsLabel
    };

    // Scale buttons only once
    QList<QPushButton*> buttons = {
        ui->lessonsButton, ui->quizzesButton, ui->statisticsButton,
        ui->settingsButton, ui->exitButton, ui->multiplayerButton,
        ui->returnFromLessonsButton, ui->returnFromSettingsButton,
        ui->returnFromMultiplayerButton, ui->returnFromQuizzesButton,
        ui->returnFromStatisticsButton, ui->onlineMatchButton,
        ui->localMatchButton, ui->freeStyleButton, ui->lessonOneButton,
        ui->lessonTwoButton, ui->lessonThreeButton, ui->lessonFourButton,
        ui->lessonFiveButton, ui->lessonSixButton, ui->returnFromFreeStyleButton,
        ui->generalGamePlay, ui->returnFromLocalButton, ui->returnToLocalGamePlayScreen,
        ui->multiplayerMajorMinorChords, ui->Scales, ui->identifyingMajorThird, ui->Triad, ui->Rhythm, ui->ReturnToLessonsPage,
        ui->resetButton
    };

    QList<QFrame*> frames = {
        ui->pianoPlaceholder, ui->pianoLocalPlaceholder, ui->gamePlayPlaceHolder, ui->lessonsPlayPlaceHolder,
        ui->lessonsPagePianoHolder, ui->settingsBackgroundFrame, ui->statisticsWidget
    };

    // Scale the stacked widget to fill the window
    ui->stackedWidget->setGeometry(0, 0, screenSize.width(), screenSize.height());

    for (QSlider* sld : sliders){
        // Get the original geometry and scale it
        QRect geo = sld->geometry();
        MathUtils::scaleWidget(sld, geo.x(), geo.y(),
                               geo.width(), geo.height(),
                               screenSize.width(), screenSize.height(),
                               designWidth, designHeight);
    }

    for (QComboBox* box : boxes){
        // Get the original geometry and scale it
        QRect geo = box->geometry();
        MathUtils::scaleWidget(box, geo.x(), geo.y(),
                               geo.width(), geo.height(),
                               screenSize.width(), screenSize.height(),
                               designWidth, designHeight);
    }

    for (QPushButton* btn : buttons) {
        // Get the original geometry and scale it
        QRect geo = btn->geometry();
        MathUtils::scaleWidget(btn, geo.x(), geo.y(),
                               geo.width(), geo.height(),
                               screenSize.width(), screenSize.height(),
                               designWidth, designHeight);
    }

    for (QLabel* lbl : labels) {
        // Get the original geometry and scale it
        QRect geo = lbl->geometry();
        MathUtils::scaleWidget(lbl, geo.x(), geo.y(),
                               geo.width(), geo.height(),
                               screenSize.width(), screenSize.height(),
                               designWidth, designHeight);
    }

    for (QFrame* frm : frames){
        QRect geo = frm->geometry();
        MathUtils::scaleWidget(frm, geo.x(), geo.y(),
                               geo.width(), geo.height(),
                               screenSize.width(), screenSize.height(),
                               designWidth, designHeight);
    }
}

/**
 * @brief Exits the application
 * @details Terminates the application by calling QCoreApplication::quit()
 */
void MainWindow::exitApplication()
{
    QCoreApplication::quit();
}

void MainWindow::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);
    // Start background music when the window is shown
    SoundManager::instance()->startBackgroundMusic();
}
