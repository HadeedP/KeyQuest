class ScoringSystem {
private:
    float accuracy = 0.0f; /*!< The overall accuracy of the user's current session, as a percentage. Default is 0. */
    unsigned int totalKeyPresses = 0; /*!< The total number of Keyboard key presses the user has made during the current session. */
    unsigned int correctKeyPresses = 0; /*!< The total number of correct key presses the user has made during the current session. */

    const unsigned int maxNoteValue = 72; /*!< The maximum key value recognized by the program. */
    const unsigned int minNoteValue = 60; /*!< The minimum key value recognized by the program. */
public:
    /*!
     * \brief The constructor of the ScoringSystem object.
     */
    ScoringSystem();

    /*!
     * \brief Checks if the values corresponding to two notes are the same; if they are, increment 'correctKeyPresses.'
     * \param inputNote The note input by the user.
     * \param expectedNote The correct note, expected by the user.
     * \return Returns 0 if no error occurs (inputNote and expectedNote) are within the expected range (60 - 72), -1 otherwise.
     */
    int evaluate(unsigned int inputNote, unsigned int expectedNote);

    /*!
     * \brief Sets the attributes of the ScoringSystem object back to their default values.
     */
    void reset();

    /*!
     * \brief Returns the user's overall accuracy for the current session.
     * \return A float that corresponds to the user's overall accuracy during the session.
     */
    float getAccuracy();

    /*!
     * \brief Returns a counter of how many times the user has pressed a Keyboard key and played a note.
     * \return An unsigned int which corresponds to the number of times the user played a note.
     */
    unsigned int getTotalKeyPresses();

    /*!
     * \brief Returns a counter of how many times the user has pressed the correct key expected of them by the lesson/quiz.
     * \return An unsigned int which corresponds to the number of correct key presses the user has made.
     */
    unsigned int getTotalCorrectKeyPresses();

    /*!
     * \brief Calculates how many stars the user has earned for their performance during the current session.
     * \return An unsigned integer representing the number of stars the player has earned.
     */
    unsigned int rate();

    /*!
     * \brief The destructor of the ScoringSystem object.
     */
    ~ScoringSystem();
};
