#ifndef STATISTICSWIDGET_H
#define STATISTICSWIDGET_H

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include "loaddatamanager.h"

/**
 * @file statisticswidget.h
 * @author Alan Cruz
 * @brief Header file for the StatisticsWidget class
 */

/**
 * @brief The StatisticsWidget class displays lesson statistics in a grid layout
 * 
 * This widget shows statistics for each lesson topic including:
 * - Average accuracy
 * - Average score
 * - Total number of attempts
 * 
 * The statistics are automatically loaded from the LoadDataManager and displayed
 * in a formatted grid with headers and styled labels.
 */
class StatisticsWidget : public QFrame
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a StatisticsWidget
     * @param parent The parent widget
     */
    explicit StatisticsWidget(QWidget *parent = nullptr);

    /**
     * @brief Destroys the StatisticsWidget
     */
    ~StatisticsWidget();

private:
    /**
     * @brief Sets up the user interface components
     * 
     * Creates and arranges all labels and layouts for displaying statistics.
     * Configures the visual style of the widget including fonts and colors.
     */
    void setupUI();

    /**
     * @brief Updates the statistics display with current data
     * 
     * Loads the latest statistics from LoadDataManager and updates
     * all labels with the current values for each lesson topic.
     */
    void updateStatistics();

    /**
     * @brief Creates a styled QLabel with consistent formatting
     * @param text The text to display in the label
     * @return A pointer to the newly created and styled QLabel
     */
    QLabel* createStyledLabel(const QString& text);
    
    // Layout components
    QVBoxLayout* mainLayout;    ///< Main vertical layout
    QGridLayout* statsGrid;     ///< Grid layout for statistics

    // Header labels
    QLabel* lessonHeader;       ///< Header for lesson names column
    QLabel* accuracyHeader;     ///< Header for accuracy column
    QLabel* scoreHeader;        ///< Header for score column
    QLabel* attemptsHeader;     ///< Header for attempts column
    
    // Arrays to store statistics labels
    QLabel* lessonLabels[6];    ///< Labels for lesson names (101-106)
    QLabel* accuracyLabels[6];  ///< Labels for average accuracy
    QLabel* scoreLabels[6];     ///< Labels for average scores
    QLabel* attemptsLabels[6];  ///< Labels for total attempts
    
    /** @brief Mapping of topic IDs to their display names */
    const QMap<QString, QString> topicNames = {
        {"101", "Basic Notes"},
        {"102", "Major/Minor Chords"},
        {"103", "Triads"},
        {"104", "Scales"},
        {"105", "Major Third"},
        {"106", "Rhythm"}
    };
};

#endif // STATISTICSWIDGET_H 