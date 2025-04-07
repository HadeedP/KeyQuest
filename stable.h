/**
 * @file stable.h
 * @brief Precompiled header file for the KeyQuest application
 * @author Alan Cruz
 * @date 2024
 * 
 * @details
 * This is a precompiled header file that includes commonly used Qt headers
 * to improve compilation speed. The headers are organized into categories:
 * - Qt Core: Basic Qt functionality
 * - Qt GUI: Graphical user interface components
 * - Qt Widgets: Widget-based user interface elements
 * - Qt Multimedia: Audio and media playback functionality
 * 
 * Using this precompiled header significantly reduces compilation time by:
 * 1. Preprocessing these headers only once
 * 2. Caching the compiled headers
 * 3. Reusing the cached version in subsequent compilations
 * 
 * @note This header should only include stable headers that rarely change
 * to maintain the benefits of precompilation.
 */

#ifndef STABLE_H
#define STABLE_H

/**
 * @defgroup QtCore Qt Core Components
 * @brief Essential Qt functionality and data structures
 */
// Qt Core
#include <QObject>      ///< Base class for Qt's object hierarchy
#include <QString>      ///< Unicode string class
#include <QList>        ///< Template-based container class
#include <QDebug>       ///< Debug stream functionality
#include <QTimer>       ///< High-level programming interface for timers

/**
 * @defgroup QtGUI Qt GUI Components
 * @brief Graphical user interface elements
 */
// Qt GUI
#include <QWidget>          ///< Base class of all user interface objects
#include <QPushButton>      ///< Command button
#include <QLabel>           ///< Text or image display
#include <QFrame>           ///< Base class for widgets with frame
#include <QScreen>          ///< Screen information and geometry
#include <QGuiApplication>  ///< Manages GUI application control flow

/**
 * @defgroup QtWidgets Qt Widgets Components
 * @brief Widget-based user interface components
 */
// Qt Widgets
#include <QMainWindow>      ///< Main application window
#include <QStackedWidget>   ///< Stack of widgets where only one is visible
#include <QVBoxLayout>      ///< Vertical box layout manager
#include <QGridLayout>      ///< Grid layout manager
#include <QMessageBox>      ///< Modal dialog for showing messages

/**
 * @defgroup QtMultimedia Qt Multimedia Components
 * @brief Audio and media playback functionality
 */
// Qt Multimedia
#include <QMediaPlayer>     ///< Media playback functionality
#include <QAudioOutput>     ///< Audio output management
#include <QSoundEffect>     ///< Low-latency sound effect playback

#endif // STABLE_H 