/**
 * @file mainpage.cpp
 * @brief Implementation of the MainPage class for KeyQuest
 * @author Alan Cruz
 * @details This file implements the main page functionality for the KeyQuest game,
 *          providing the initial interface that users see when launching the application.
 */

#include "mainpage.h"

/**
 * @brief Constructor for MainPage
 * @param parent Pointer to the parent widget
 * @details Initializes the main page by inheriting from BackgroundPage
 *          and setting up focus policy to receive keyboard events.
 */
MainPage::MainPage(QWidget *parent)
    : BackgroundPage(parent)
{
    // Enable widget to receive mouse and keyboard events directly
    setFocusPolicy(Qt::StrongFocus);
}