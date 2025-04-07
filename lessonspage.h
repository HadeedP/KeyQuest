/**
 * @file lessonspage.h
 * @author Alan Cruz
 * @brief Header file for the lessons menu page
 * @version 1.0
 * @date 2024-03-21
 * 
 * @copyright Copyright (c) 2024
 */

#ifndef LESSONSPAGE_H
#define LESSONSPAGE_H

#include "lessonsbackgroundpage.h"

/**
 * @brief Widget class for the lessons menu page
 * 
 * Provides the interface for selecting and starting different lessons,
 * including general music theory, chords, scales, and melody topics
 */
class LessonsPage : public LessonsbackgroundPage
{
    Q_OBJECT
public:
    /**
     * @brief Constructs a new LessonsPage
     * @param parent The parent widget
     */
    explicit LessonsPage(QWidget *parent = nullptr);
};

#endif // LESSONSPAGE_H
