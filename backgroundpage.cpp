/**
 * @file backgroundpage.cpp
 * @brief Implementation of the BackgroundPage class for KeyQuest
 * @author Alan Cruz
 * @details This file implements the background page functionality for the KeyQuest game,
 *          handling the custom background image display and widget behavior.
 */

#include "backgroundpage.h"

/**
 * @brief Constructor for BackgroundPage
 * @param parent Pointer to the parent widget
 * @details Initializes the BackgroundPage widget and sets up focus policy
 *          to receive mouse and keyboard events.
 */
BackgroundPage::BackgroundPage(QWidget *parent)
    : QWidget(parent)
{
    // Enable widget to receive mouse and keyboard events directly
    setFocusPolicy(Qt::StrongFocus);
}

/**
 * @brief Handles the painting of the background page
 * @param event The paint event that triggered this function
 * @details This method draws the custom background image for the KeyQuest game.
 *          It scales the background to fill the widget while maintaining aspect ratio
 *          and centers the image within the widget.
 */
void BackgroundPage::paintEvent(QPaintEvent *event)
{
    // Handle the base widget painting first (for stylesheet support)
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

    // draws custom background
    QPixmap background(":/resources/KeyQuest.png");
    if (!background.isNull()) {
        // Get the widget size
        QSize widgetSize = size();
        
        // Scale the background to fill the entire widget while maintaining aspect ratio
        QSize scaledSize = background.size().scaled(widgetSize, Qt::KeepAspectRatioByExpanding);
        
        // Calculate position to center the image
        int x = (widgetSize.width() - scaledSize.width()) / 2;
        int y = (widgetSize.height() - scaledSize.height()) / 2;
        
        // Draw the background
        p.drawPixmap(x, y, scaledSize.width(), scaledSize.height(), background);
    }
} 
