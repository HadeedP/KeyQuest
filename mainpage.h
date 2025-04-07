/**
 * @file mainpage.h
 * @author Alan Cruz
 * @brief Header file for the main menu page
 * @version 1.0
 * @date 2024-03-21
 * 
 * @copyright Copyright (c) 2024
 */

#ifndef MAINPAGE_H
#define MAINPAGE_H

#include "backgroundpage.h"

/**
 * @brief Widget class for the main menu page
 * 
 * Provides the main navigation interface for the application,
 * allowing users to access lessons, multiplayer, and settings
 */
class MainPage : public BackgroundPage
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new MainPage
     * @param parent The parent widget
     */
    explicit MainPage(QWidget *parent = nullptr);
};

#endif // MAINPAGE_H 