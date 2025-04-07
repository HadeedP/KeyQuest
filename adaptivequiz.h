/**
 * @file adaptivequiz.h
 * @brief Header file for the AdaptiveQuiz class
 * @author Alan Cruz, Sarah Laharpour   
 * @details This file defines the AdaptiveQuiz class which implements a reinforcement learning
 *          based adaptive quiz system. It uses Q-learning to select appropriate questions
 *          based on the user's current skill level, tracks user progress, and dynamically
 *          adjusts difficulty across different topics including notes, chords, and scales.
 */

#pragma once
#include <map>
#include <vector>
#include <tuple>
#include <QString>
#include <unordered_set>
#include <QObject>
#include "question.h"
#include "state.h"

/**
 * @brief The adaptive quiz engine using Q-learning for question selection
 * @details This class implements a reinforcement learning algorithm (Q-learning)
 *          to dynamically select questions based on the user's performance and skill level.
 *          It tracks user progress across multiple topics (notes, chords, scales),
 *          updates skill levels based on correct/incorrect answers, and calculates
 *          appropriate rewards for the learning algorithm. The engine maintains
 *          a history of questions asked and responses, and provides methods for
 *          score and accuracy calculation.
 */
class AdaptiveQuiz : public QObject {
    Q_OBJECT
    
private:
    /// Maps each state to a map of question IDs and their Q-values
    std::map<State, std::map<int, float>> q_table;

    /// User's current skill state (notes, chords, scales)
    State state;

    /// Questions mapped by their unique ID
    std::map<int, Question> questionBankByID;

    /**
     * @brief History of quiz interactions
     * @details Each entry is a tuple containing:
     *          - User's state when question was asked
     *          - Question ID
     *          - Question description
     *          - Boolean indicating if user's response was correct
     */
    std::vector<std::tuple<State, int, QString, bool>> history;

    /// Set of question IDs that have already been asked in the current session
    std::unordered_set<int> askedQuestionsThisSession;

    /// Learning rate for Q-learning algorithm
    float lr;

    /// Discount factor for Q-learning algorithm
    float df;

    /// Threshold of correct answers needed to upgrade skill level in a topic
    const int correctThreshold;

    /// Threshold of incorrect answers that triggers a downgrade in skill level
    const int incorrectThreshold;

    /// Counters for correct answers per question ID (weighted by difficulty)
    std::map<int, int> correctCounts;

    /// Counters for incorrect answers per question ID
    std::map<int, int> incorrectCounts;

    /// Current score in the quiz
    float score;

    /// Number of correctly answered questions
    int correctAnswers;

    /// Total number of questions answered
    int totalQuestions;

public:
    /**
     * @brief Constructor for AdaptiveQuiz
     * @param questionBank Map of question IDs to Question objects
     * @param qTable The Q-table with stored learning from previous sessions
     * @param initialState The initial skill state of the user
     * @param parent Parent QObject for memory management
     * @details Initializes the adaptive quiz with a question bank, existing Q-values,
     *          and the user's initial skill state. Sets default values for learning
     *          parameters and initializes counters for tracking progress.
     */
    AdaptiveQuiz(const std::map<int, Question>& questionBank,
        const std::map<State, std::map<int, float>>& qTable,
        const State& initialState,
        QObject* parent = nullptr);

    /**
     * @brief Gets the number of correctly answered questions
     * @return Integer count of correct answers
     */
    int getCorrectAnswers() const;

    /**
     * @brief Gets the total number of questions answered
     * @return Integer count of total questions
     */
    int getTotalQuestions() const;

    /**
     * @brief Calculates the current accuracy as a percentage
     * @return Float value representing accuracy (0-100)
     * @details Calculates the percentage of correctly answered questions
     *          from the total number of questions answered.
     */
    float getAccuracy() const;

    /**
     * @brief Gets the current score
     * @return Float value representing the current score
     */
    float getScore() const;

    /**
     * @brief Gets the history of quiz interactions
     * @return Vector of tuples containing state, question ID, description, and correctness
     */
    const std::vector<std::tuple<State, int, QString, bool>>& getHistory() const;

    /**
     * @brief Gets the current Q-table
     * @return Map representing the Q-table for all states and actions
     * @details Returns the entire Q-table which maps states to question IDs and
     *          their associated Q-values. Used for saving learning progress.
     */
    std::map<State, std::map<int, float>> getQTable() const;

    /**
     * @brief Gets valid questions for the current skill level
     * @param allowSlightStretch Whether to include questions slightly above user's level
     * @return Vector of question IDs that match the criteria
     * @details Filters the question bank to find questions that match the user's
     *          current skill level in each topic. If allowSlightStretch is true,
     *          includes questions one level above the user's current skill.
     */
    std::vector<int> getActionsForStateLevel(bool allowSlightStretch = false);

    /**
     * @brief Selects the next question to present to the user
     * @return Question ID of the selected question
     * @details Uses an epsilon-greedy strategy to balance exploration and exploitation.
     *          Explores (random selection) or exploits (highest Q-value) based on
     *          the user's average skill level. Avoids repeating questions within
     *          the same session when possible.
     */
    int getNextAction();

    /**
     * @brief Gets the user's current skill state
     * @return State object with current skill levels
     */
    State getCurrentState() const;

    /**
     * @brief Retrieves a question by its ID
     * @param questionID The ID of the question to retrieve
     * @return The Question object
     * @details Looks up a question in the question bank by its ID.
     *          Returns an empty question if the ID is not found.
     */
    Question getQuestion(int questionID);

    /**
     * @brief Calculates the reward for a state transition
     * @param before State before the transition
     * @param after State after the transition
     * @param correct Whether the answer was correct
     * @return Float value representing the reward
     * @details Determines the reward based on whether the answer was correct
     *          and whether the user's skill level improved.
     */
    float getReward(const State& before, const State& after, bool correct);

    /**
     * @brief Finds the maximum Q-value for a given state
     * @param s The state to evaluate
     * @return Float value of the maximum Q-value
     * @details Searches all actions (question IDs) in the Q-table for the
     *          given state and returns the highest Q-value found.
     */
    float maxQValue(const State& s);

    /**
     * @brief Updates the user's skill state based on their response
     * @param questionID The ID of the answered question
     * @param correct Whether the answer was correct
     * @param currentState The state to update
     * @details Increases or decreases skill levels in the appropriate topic
     *          based on accumulated correct or incorrect answers. Factors in
     *          question difficulty when updating counters.
     */
    void updateState(int questionID, bool correct, State& currentState);

    /**
     * @brief Updates the Q-table using the Q-learning algorithm
     * @param currentState State before the action
     * @param questionID Question ID (action taken)
     * @param reward Reward received
     * @param nextState State after the action
     * @details Applies the Q-learning update formula:
     *          Q(s,a) = Q(s,a) + lr * (reward + df * max(Q(s',a')) - Q(s,a))
     */
    void updateQTable(const State& currentState, int questionID, float reward, const State& nextState);

    /**
     * @brief Gets the Q-value for a specific state-action pair
     * @param s The state
     * @param questionID The question ID (action)
     * @return Float value representing the Q-value
     */
    float getQValue(const State& s, int questionID) const;

    /**
     * @brief Processes and evaluates a user's response to a question
     * @param questionID The ID of the answered question
     * @param correct Whether the answer was correct
     * @details Updates score, tracks history, updates user's skill state,
     *          calculates rewards, updates the Q-table, and handles all
     *          necessary bookkeeping for the adaptive quiz system.
     */
    void evaluateResponse(int questionID, bool correct);
    
signals:
    /**
     * @brief Signal emitted when the UI needs to be updated
     * @param score The current score
     * @param title The current question title
     * @param description The current question description
     * @param accuracy The current accuracy percentage
     */
    void updateUI(int score, QString title, QString description, double accuracy);

    /**
     * @brief Signal emitted when the quiz ends
     * @param score The final score
     * @param accuracy The final accuracy percentage
     */
    void quizOver(int score, double accuracy);

    /**
     * @brief Signal emitted for visual feedback on answer correctness
     * @param isCorrect True if the answer was correct, false otherwise
     */
    void highlightKeys(bool isCorrect);
};
