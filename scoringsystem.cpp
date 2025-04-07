#include "scoringsystem.h"

/*!
 * \brief The constructor of the ScoringSystem object.
 */
ScoringSystem::ScoringSystem() {}

/*!
 * \brief The destructor of the ScoringSystem object.
 */
ScoringSystem::~ScoringSystem() {}

/*!
 * \brief Compares two notes together and checks if they are equal. If they are, increment the number of correct inputs.
 * \param inputNote The note input by the user through the Keyboard object.
 * \param expectedNote The note currently expected by the lesson/quiz.
 * \return Returns 0 if no error occurs (inputNote and expectedNote) are within the expected range (60 - 72), -1 otherwise.
 */
int ScoringSystem::evaluate(unsigned int inputNote, unsigned int expectedNote) {
    // Checking that inputNote and expectedNote are valid notes with the expected range of note values. This will help catch errors.
    if (inputNote < minNoteValue || inputNote > maxNoteValue) return -1;
    if (expectedNote < minNoteValue || expectedNote > maxNoteValue) return -1;

    // Updating key press counters, based on whether the input and expected keys are the same or not.
    totalKeyPresses++;
    if (inputNote == expectedNote) correctKeyPresses++;

    // Calculating the new overall accuracy.
    accuracy = (float)correctKeyPresses / totalKeyPresses;

    return 0;
}

/*!
 * \brief Returns the accuracy of the user's current session.
 * \return The overall accuracy stored in the ScoringSystem object.
 */
float ScoringSystem::getAccuracy() {
    return accuracy;
}

/*!
 * \brief Returns a counter of how many times the user has pressed a Keyboard key and played a note.
 * \return An unsigned int which corresponds to the number of times the user played a note.
 */
unsigned int ScoringSystem::getTotalKeyPresses() {
    return totalKeyPresses;
}

/*!
 * \brief Returns a counter of how many times the user has pressed the correct key expected of them by the lesson/quiz.
 * \return An unsigned int which corresponds to the number of correct key presses the user has made.
 */
unsigned int ScoringSystem::getTotalCorrectKeyPresses() {
    return correctKeyPresses;
}

/*!
 * \brief Sets the attributes of the ScoringSystem object back to its default values.
 */
void ScoringSystem::reset() {
    accuracy = 0.0f;
    totalKeyPresses = 0;
    correctKeyPresses = 0;
}

/*!
 * \brief Calculates the number of stars the user's performance has earned, depending on their accuracy.
 * \return The number of stars that should be displayed.
 */
unsigned int ScoringSystem::rate() {
    for (unsigned short starLevel = 0; starLevel <= 5; starLevel++) {
        if (starLevel == 5) return starLevel;   // If the starLevel is 5, the algorithm cannot go any higher and ends immediately.
        float minGrade = (20 * starLevel) / 100;
        float maxGrade = minGrade + 20;
        if (accuracy >= minGrade && accuracy < maxGrade) {
            return starLevel;
        }
    }
    return 0;   // If, for some reason, the algorithm cannot calculate a rating, the method returns a default of zero stars.
}
