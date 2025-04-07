/**
 * @file mathutils.h
 * @author Alan Cruz
 * @brief Utility functions for mathematical calculations and scaling
 * @version 1.0
 * @date 2024-03-21
 * 
 * @copyright Copyright (c) 2024
 */

#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <QWidget>

/**
 * @brief Class containing utility functions for mathematical operations
 * 
 * Provides functions for scaling UI elements based on screen dimensions
 */
class MathUtils {
public:
    // Function to scale a widget's position and size based on screen size
    /**
     * @brief Scales a widget's position and size based on screen size
     * @param widget The widget to scale
     * @param x The x-coordinate position
     * @param y The y-coordinate position
     * @param w The width of the widget
     * @param h The height of the widget
     * @param screenWidth The current screen width
     * @param screenHeight The current screen height
     * @param designWidth The design-time width
     * @param designHeight The design-time height
     */
    static void scaleWidget(QWidget *widget, int x, int y, int w, int h,
                            int screenWidth, int screenHeight,
                            int designWidth, int designHeight);
};

#endif // MATHUTILS_H
