/**
 * @file backgroundpage.h
 * @author Alan Cruz
 * @brief Header file for the background page widget
 * @version 1.0
 * @date 2024-03-21
 * 
 * @copyright Copyright (c) 2024
 */

#ifndef BACKGROUNDPAGE_H
#define BACKGROUNDPAGE_H

#include <QWidget>
#include <QPainter>
#include <QStyleOption>

/**
 * @brief Widget class for displaying background pages
 * 
 * Provides a base widget for pages that display background content
 */
class BackgroundPage : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a new BackgroundPage object
     * @param parent The parent widget
     */
    explicit BackgroundPage(QWidget *parent = nullptr);

protected:
    /**
     * @brief Handles paint events for the widget
     * @param event The paint event
     */
    void paintEvent(QPaintEvent *event) override;
};

#endif // BACKGROUNDPAGE_H 