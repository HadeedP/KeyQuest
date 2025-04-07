#include "statisticswidget.h"
#include <QFont>
#include <QFontDatabase>

/**
 * @file statisticswidget.cpp
 * @author Alan Cruz
 * @brief Implementation of the StatisticsWidget class
 */

/**
 * @brief Constructs a new StatisticsWidget
 * @param parent The parent widget
 * 
 * Initializes the widget's layout components and sets up the UI.
 * After construction, the widget will display current statistics
 * for all lesson topics.
 */
StatisticsWidget::StatisticsWidget(QWidget *parent)
    : QFrame(parent)
    , mainLayout(new QVBoxLayout(this))
    , statsGrid(new QGridLayout())
{
    setupUI();
    updateStatistics();
}

/**
 * @brief Destroys the StatisticsWidget
 * 
 * Cleans up all allocated resources including layouts and labels.
 * Qt's parent-child relationship handles most of the cleanup automatically.
 */
StatisticsWidget::~StatisticsWidget()
{
}

/**
 * @brief Creates a consistently styled label for the statistics display
 * @param text The text to display in the label
 * @return QLabel* A pointer to the newly created and styled label
 * 
 * Creates a QLabel with standardized styling including:
 * - System font with 16pt size and bold weight
 * - Brown text color (rgb(103, 49, 0))
 * - Center alignment
 */
QLabel* StatisticsWidget::createStyledLabel(const QString& text)
{
    QLabel* label = new QLabel(text);
    
    // Use system font
    QFont font = QFontDatabase::systemFont(QFontDatabase::GeneralFont);
    font.setPointSize(16);
    font.setBold(true);
    
    label->setFont(font);
    label->setStyleSheet("QLabel { color: rgb(103, 49, 0); }"); // Brown color
    label->setAlignment(Qt::AlignCenter);
    
    return label;
}

/**
 * @brief Sets up the user interface components of the widget
 * 
 * Configures the widget's appearance and creates the layout structure:
 * - Sets frame style and background
 * - Creates header labels
 * - Sets up the grid layout for statistics
 * - Creates and positions labels for each lesson topic
 * - Configures layout spacing and margins
 */
void StatisticsWidget::setupUI()
{
    // Set frame style
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);
    setStyleSheet("QFrame { background-color: rgba(255, 255, 255, 0.9); border-radius: 10px; }");
    
    // Create headers
    lessonHeader = createStyledLabel("Lesson");
    accuracyHeader = createStyledLabel("Avg. Accuracy");
    scoreHeader = createStyledLabel("Avg. Score");
    attemptsHeader = createStyledLabel("Total Attempts");
    
    // Add headers to grid
    statsGrid->addWidget(lessonHeader, 0, 0);
    statsGrid->addWidget(accuracyHeader, 0, 1);
    statsGrid->addWidget(scoreHeader, 0, 2);
    statsGrid->addWidget(attemptsHeader, 0, 3);
    
    // Create and add labels for each lesson
    for(int i = 0; i < 6; ++i) {
        QString topicId = QString::number(101 + i);
        
        lessonLabels[i] = createStyledLabel(topicNames[topicId]);
        accuracyLabels[i] = createStyledLabel("0%");
        scoreLabels[i] = createStyledLabel("0");
        attemptsLabels[i] = createStyledLabel("0");
        
        statsGrid->addWidget(lessonLabels[i], i + 1, 0);
        statsGrid->addWidget(accuracyLabels[i], i + 1, 1);
        statsGrid->addWidget(scoreLabels[i], i + 1, 2);
        statsGrid->addWidget(attemptsLabels[i], i + 1, 3);
    }
    
    // Set column stretch factors
    statsGrid->setColumnStretch(0, 2); // Lesson name column wider
    statsGrid->setColumnStretch(1, 1);
    statsGrid->setColumnStretch(2, 1);
    statsGrid->setColumnStretch(3, 1);
    
    // Add some spacing
    statsGrid->setSpacing(20);
    
    // Add the grid to the main layout
    mainLayout->addLayout(statsGrid);
    mainLayout->addStretch();
    
    // Set margins for the main layout
    mainLayout->setContentsMargins(30, 30, 30, 30);
}

/**
 * @brief Updates all statistics labels with current data
 * 
 * Retrieves the latest statistics from LoadDataManager and updates
 * the display for each lesson topic. For each topic, calculates and shows:
 * - Average accuracy (as percentage)
 * - Average score
 * - Total number of attempts
 * 
 * If no data exists for a topic, displays zeros.
 */
void StatisticsWidget::updateStatistics()
{
    LoadDataManager* dataManager = LoadDataManager::instance();
    QJsonObject data = dataManager->getData();
    QJsonObject topics = data["lessons"].toObject()["topics"].toObject();
    
    // Update statistics for each topic
    for(int i = 0; i < 6; ++i) {
        QString topicId = QString::number(101 + i);
        QJsonObject topic = topics[topicId].toObject();
        QJsonObject stats = topic["statistics"].toObject();
        
        QJsonArray scores = stats["scores"].toArray();
        QJsonArray accuracies = stats["accuracy"].toArray();
        QJsonArray attempts = stats["attempts"].toArray();
        
        // Calculate averages
        double avgAccuracy = 0.0;
        double avgScore = 0.0;
        int totalAttempts = attempts.size();
        
        if (totalAttempts > 0) {
            for (const QJsonValue& acc : accuracies) {
                avgAccuracy += acc.toDouble();
            }
            avgAccuracy /= totalAttempts;
            
            for (const QJsonValue& score : scores) {
                avgScore += score.toDouble();
            }
            avgScore /= totalAttempts;
        }
        
        // Update labels
        accuracyLabels[i]->setText(QString::number(avgAccuracy, 'f', 1) + "%");
        scoreLabels[i]->setText(QString::number(avgScore, 'f', 1));
        attemptsLabels[i]->setText(QString::number(totalAttempts));
    }
} 