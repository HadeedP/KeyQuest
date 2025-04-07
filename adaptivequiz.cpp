/**
 * @file adaptivequiz.cpp
 * @brief Implementation of the AdaptiveQuiz class
 * @author Alan Cruz, Sarah Laharpour
 * @details This file implements the reinforcement learning based adaptive quiz system
 *          defined in the AdaptiveQuiz class. It contains the logic for Q-learning,
 *          question selection, state updates, and skill progression tracking.
 */

#include "adaptivequiz.h"
#include <iostream>
#include <map>
#include <vector>
#include <tuple>
#include <random>
#include <algorithm>
#include <unordered_set>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>



/**
 * @brief Constructor for AdaptiveQuiz
 * @param questionBank Map of question IDs to Question objects
 * @param qTable The Q-table with stored learning from previous sessions
 * @param initialState The initial skill state of the user
 * @param parent Parent QObject for memory management
 */
AdaptiveQuiz::AdaptiveQuiz(const std::map<int, Question>& questionBank,
    const std::map<State, std::map<int, float>>& qTable,
    const State& initialState,
    QObject* parent)
    : QObject(parent),
    questionBankByID(questionBank),
    q_table(qTable),
    state(initialState),
    lr(0.1f),
    df(0.9f),
    correctThreshold(4),
    incorrectThreshold(4),
    score(0.0f),
    correctAnswers(0),
    totalQuestions(0) {}



    // This method returns a list of questions that match the user's current skill level
    // allowSlightStretch = true, only matches questions that are in user's exact level
    // allowSlightStretch = false, include questions that are one level harder than user's current skill
    /**
     * @brief Gets valid questions for the current skill level
     * @param allowSlightStretch Whether to include questions slightly above user's level
     * @return Vector of question IDs that match the criteria
     */
    std::vector<int> AdaptiveQuiz::getActionsForStateLevel(bool allowSlightStretch) {
        
        std::vector<int> validQuestionIDs;
        int stretchAmount = 0;
        if (allowSlightStretch) {
            stretchAmount = 1;
        }

        for (const auto& [qid, q] : questionBankByID) {
            int topicID = q.getTopicID();
            int difficulty = q.getDifficulty();
            if ((topicID == 101 && difficulty <= state.notes + stretchAmount) ||
                    (topicID >= 102 && topicID <= 103 && difficulty <= state.chords + stretchAmount) ||
                    (topicID >= 104 && difficulty <= state.scales + stretchAmount)) {
                    validQuestionIDs.push_back(qid);
            }
        }
        return validQuestionIDs;
    }


    // If Q-table has no entry for the current state, pick a random action
    // Otherwise select the action with the highest value  for the currrent state
    /**
     * @brief Selects the next question to present to the user
     * @return Question ID of the selected question
     */
    int AdaptiveQuiz::getNextAction() {
        int avgLevel = (state.notes + state.chords + state.scales) / 3;
        float epsilon;
        if (avgLevel == 0){
            epsilon = 0.9f;
        }
        else if (avgLevel == 1){
            epsilon = 0.7f;
        }
        else{ 
            epsilon = 0.5f;
        }

        bool explore = ((float)rand() / RAND_MAX) < epsilon;

        std::vector<int> candidates = getActionsForStateLevel(explore);
    
        // Filter out already asked questions
        std::vector<int> filteredCandidates;
        for (int qid : candidates) {
            if (askedQuestionsThisSession.find(qid) == askedQuestionsThisSession.end()) {
                filteredCandidates.push_back(qid);
            }
        }

        if (filteredCandidates.empty()) {
            // All have been asked - reset session and allow repetition
            askedQuestionsThisSession.clear();
            filteredCandidates = candidates;
        }

        if (filteredCandidates.empty()) return questionBankByID.begin()->first;

        if (explore) {
            // exploring
            // pick a random question from filtered list
            std::vector<int> notesQ, chordsQ, scalesQ;
            for (int qid : filteredCandidates) {
                int tid = questionBankByID[qid].getTopicID();
                if (tid == 101) notesQ.push_back(qid);
                else if (tid >= 101 && tid <= 102) chordsQ.push_back(qid);
                else if (tid == 104) scalesQ.push_back(qid);
            }

            std::vector<int>* group = nullptr;
            int choice = rand() % 3;
            if (choice == 0 && !notesQ.empty()) group = &notesQ;
            else if (choice == 1 && !chordsQ.empty()) group = &chordsQ;
            else if (choice == 2 && !scalesQ.empty()) group = &scalesQ;

            if (group && !group->empty()) {
                return (*group)[rand() % group->size()];
            } else {
                return filteredCandidates[rand() % filteredCandidates.size()];
            }
        } else {
            // exploiting
            // pick question with highest Q-value
            int bestQID = filteredCandidates[0];
            float bestValue = -1e9;
            for (int qid : filteredCandidates) {
                float q = q_table[state][qid];
                if (q > bestValue) {
                    bestValue = q;
                    bestQID = qid;
                }
            }
            return bestQID;
        }
        std::cout << "[DEBUG] Mode: " << (explore ? "Explore" : "Exploit") << "\n";

    }

    /**
     * @brief Gets the user's current skill state
     * @return State object with current skill levels
     */
    State AdaptiveQuiz::getCurrentState() const { 
        return state; 
    }

    /**
     * @brief Retrieves a question by its ID
     * @param questionID The ID of the question to retrieve
     * @return The Question object
     */
    Question AdaptiveQuiz::getQuestion(int questionID) { 
        if (questionBankByID.find(questionID) != questionBankByID.end()) {
            return questionBankByID.at(questionID);
        } else {
            // Return default/empty question to avoid crash
            return Question();
        }
    }
    

    // if correct and skill improved, 20 points
    // if correct but no change in skill 10 points
    // if not correct -5 points
    /**
     * @brief Calculates the reward for a state transition
     * @param before State before the transition
     * @param after State after the transition
     * @param correct Whether the answer was correct
     * @return Float value representing the reward
     */
    float AdaptiveQuiz::getReward(const State& before, const State& after, bool correct) {
        if (correct && (after.notes > before.notes || after.chords > before.chords || after.scales > before.scales))
            return 5.0f;
        if (correct) return 2.5f;
        return -2.5f;
    }

    // Finds the highest Q-value from all possible actions in the current state
    /**
     * @brief Finds the maximum Q-value for a given state
     * @param s The state to evaluate
     * @return Float value of the maximum Q-value
     */
    float AdaptiveQuiz::maxQValue(const State& s) {
        if (q_table.find(s) == q_table.end()) return 0.0f;
        float maxQ = 0.0f;
        for (const auto& [_, q] : q_table[s]) {
            if (q > maxQ) maxQ = q;
        }
        return maxQ;
    }

    // Upgrade the skill level if answer was correct,
    // and lower the skill level if the answer was incorrect 
    /**
     * @brief Updates the user's skill state based on their response
     * @param questionID The ID of the answered question
     * @param correct Whether the answer was correct
     * @param currentState The state to update
     */
    void AdaptiveQuiz::updateState(int questionID, bool correct, State& currentState) {
        int difficulty = questionBankByID[questionID].getDifficulty();
        int topicID = questionBankByID[questionID].getTopicID();
        
        // Assigning points weight for each diffculty value
        int points = 1;
        if (difficulty == 1) points = 2;
        if (difficulty == 2) points = 3;

        if (correct) {
            // Adding the earned points to the correct count for that topic ID
            correctCounts[questionID] += points;

            // Since the user already answered a correct questions, we will reset this value
            incorrectCounts[questionID] = 0;

            if (correctCounts[questionID] >= correctThreshold) {
                if (topicID == 101 && currentState.notes < 2) currentState.notes++;
                if (topicID >= 102 && topicID <= 103 && currentState.chords < 2) currentState.chords++;
                if (topicID >= 104 && currentState.scales < 2) currentState.scales++;
                correctCounts[questionID] = 0;
            }
        } else {
            // If the aswer to the question was wrong
            incorrectCounts[questionID]++;
            correctCounts[questionID] = 0;

            if (incorrectCounts[questionID] >= incorrectThreshold) {
                if (topicID == 101 && currentState.notes > 0) currentState.notes--;
                if (topicID >= 102 && topicID <= 103 && currentState.chords > 0) currentState.chords--;
                if (topicID >= 104 && currentState.scales > 0) currentState.scales--;
                incorrectCounts[questionID] = 0;
            }
        }
    }

    // Performs the Q-learning formula
    /**
     * @brief Updates the Q-table using the Q-learning algorithm
     * @param currentState State before the action
     * @param questionID Question ID (action taken)
     * @param reward Reward received
     * @param nextState State after the action
     */
    void AdaptiveQuiz::updateQTable(const State& currentState, int questionID, float reward, const State& nextState) {
        float& q = q_table[currentState][questionID];
        float nextMax = maxQValue(nextState);
        q = q + lr * (reward + df * nextMax - q);
    }

    // returns Q-value for a given state-action, used in test cases
    /**
     * @brief Gets the Q-value for a specific state-action pair
     * @param s The state
     * @param questionID The question ID (action)
     * @return Float value representing the Q-value
     */
    float AdaptiveQuiz::getQValue(const State& s, int questionID) const {
        auto it = q_table.find(s);
        if (it != q_table.end()) {
            auto jt = it->second.find(questionID);
            if (jt != it->second.end()) return jt->second;
        }
        return 0.0f;
    }

    // This method will return Q-table so it can be saved in database
    /**
     * @brief Gets the current Q-table
     * @return Map representing the Q-table for all states and actions
     */
    std::map<State, std::map<int, float>> AdaptiveQuiz::getQTable() const {
        return q_table; 
    }

    /**
     * @brief Gets the number of correctly answered questions
     * @return Integer count of correct answers
     */
    int AdaptiveQuiz::getCorrectAnswers() const {
        return correctAnswers;
    }
    
    /**
     * @brief Gets the total number of questions answered
     * @return Integer count of total questions
     */
    int AdaptiveQuiz::getTotalQuestions() const {
        return totalQuestions;
    }
    
    /**
     * @brief Gets the current score
     * @return Float value representing the current score
     */
    float AdaptiveQuiz::getScore() const {
        return score;
    }
    
    /**
     * @brief Calculates the current accuracy as a percentage
     * @return Float value representing accuracy (0-100)
     */
    float AdaptiveQuiz::getAccuracy() const {
        return totalQuestions == 0 ? 0.0f : static_cast<float>(correctAnswers) / totalQuestions * 100.0f;
    }
    
    /**
     * @brief Gets the history of quiz interactions
     * @return Vector of tuples containing state, question ID, description, and correctness
     */
    const std::vector<std::tuple<State, int, QString, bool>>& AdaptiveQuiz::getHistory() const {
        return history;
    }
    


    // This method needs to get implemented in the Controller page
    // What it does?
    // Selects the best Question
    // Asks user for input
    // Calculates reward
    // Updates skill state
    // Updates Q-table
    // Saves it in history
    /**
     * @brief Processes and evaluates a user's response to a question
     * @param questionID The ID of the answered question
     * @param correct Whether the answer was correct
     */
    void AdaptiveQuiz::evaluateResponse(int questionID, bool correct) {
        // Add to history
        history.emplace_back(state, questionID, questionBankByID[questionID].getDescription(), correct);
        
        // Track total questions and correct answers
        totalQuestions++;
        if (correct) {
            correctAnswers++;
            score += 10.0f;
        } else {
            score -= 5.0f;
            if (score < 0.0f) score = 0.0f;  // Keep score non-negative
        }

        // Emit signal for highlighting keys with correctness feedback
        emit highlightKeys(correct);
        
        // Save state before update for reward calculation
        State stateBefore = state;
        
        // Update user's skill level based on response
        updateState(questionID, correct, state);
        
        // Get reward and update Q-table
        float reward = getReward(stateBefore, state, correct);
        updateQTable(stateBefore, questionID, reward, state);
        
        // Add question to asked set
        askedQuestionsThisSession.insert(questionID);
    }
