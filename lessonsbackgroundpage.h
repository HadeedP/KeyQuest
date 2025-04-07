/**
 * @file lessonsbackgroundpage.h
 * @author Alan Cruz
 * @brief Header file for the lessons background page widget
 * @version 1.0
 * @date 2024-03-21
 * 
 * @copyright Copyright (c) 2024
 */

#ifndef LESSONSBACKGROUNDPAGE_H
#define LESSONSBACKGROUNDPAGE_H

#include <QWidget>
#include <QPainter>
#include <QStyleOption>

/**
 * @brief Widget class for displaying the lessons background page
 * 
 * Provides a specialized background page for the lessons section
 */
class LessonsbackgroundPage: public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief Constructs a new LessonsbackgroundPage object
     * @param parent The parent widget
     */
    explicit LessonsbackgroundPage(QWidget *parent = nullptr);

protected:
    /**
     * @brief Handles paint events for the widget
     * @param event The paint event
     */
    void paintEvent(QPaintEvent *event) override;
};

#endif // LESSONSBACKGROUNDPAGE_H

