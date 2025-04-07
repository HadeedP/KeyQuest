/**
 * @file lessonspage.cpp
 * @brief Implementation of the LessonsPage class for KeyQuest
 * @author Alan Cruz
 * @details This file implements the lessons page functionality for the KeyQuest game,
 *          providing the main interface for the lessons section.
 */

#include "lessonspage.h"

/**
 * @brief Constructor for LessonsPage
 * @param parent Pointer to the parent widget
 * @details Initializes the lessons page by inheriting from LessonsbackgroundPage
 *          and setting up focus policy to receive keyboard events.
 */
LessonsPage::LessonsPage(QWidget *parent)
    : LessonsbackgroundPage(parent)
{
    setFocusPolicy(Qt::StrongFocus);
}
