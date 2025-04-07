/**
 * @file mathutils.cpp
 * @brief Implementation of the MathUtils class for KeyQuest
 * @author Alan Cruz
 * @details This file implements utility functions for mathematical calculations
 *          used in scaling UI elements across different screen resolutions.
 */

#include "mathutils.h"

/**
 * @brief Scales a widget's position and size based on screen resolution
 * @param widget Pointer to the widget to scale
 * @param x Original x position in design resolution
 * @param y Original y position in design resolution
 * @param w Original width in design resolution
 * @param h Original height in design resolution
 * @param screenWidth Current screen width
 * @param screenHeight Current screen height
 * @param designWidth Original design width (typically 1920)
 * @param designHeight Original design height (typically 1080)
 * @details Calculates the scaled position and size of a widget based on the ratio
 *          between the current screen resolution and the original design resolution.
 *          This ensures UI elements maintain their relative positions and sizes
 *          across different screen sizes.
 */
void MathUtils::scaleWidget(QWidget *widget, int x, int y, int w, int h,
                            int screenWidth, int screenHeight,
                            int designWidth, int designHeight)
{
    // Scale position and size relative to design resolution
    int scaledX = (x * screenWidth) / designWidth;
    int scaledY = (y * screenHeight) / designHeight;
    int scaledW = (w * screenWidth) / designWidth;
    int scaledH = (h * screenHeight) / designHeight;

    widget->setGeometry(scaledX, scaledY, scaledW, scaledH);
}
