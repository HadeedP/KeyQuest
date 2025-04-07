/**
 * @file state.h
 * @brief Header file for the State structure
 * @author Alan Cruz, Sarah Lahourpour
 * @details This file defines the State structure which represents a user's
 *          current skill levels across different musical domains (notes, chords,
 *          and scales). It provides comparison operators, JSON serialization,
 *          and string conversion methods for persistence and lookup operations.
 */

#pragma once
#include <tuple>
#include <QJsonObject>

/**
 * @brief Structure representing a user's skill levels
 * @details Encapsulates a user's proficiency levels in three musical domains:
 *          notes, chords, and scales. Each domain has an integer skill level,
 *          typically ranging from 0 (beginner) to 2 (advanced). This structure
 *          is used by the adaptive quiz engine to track user progress and select
 *          appropriate questions based on current abilities.
 *          
 *          The structure provides methods for:
 *          - Comparison (for use as map keys)
 *          - JSON serialization/deserialization for persistence
 *          - String conversion for debugging and storage
 */
struct State {
    int notes;   ///< Skill level for notes (0=beginner, 1=intermediate, 2=advanced)
    int chords;  ///< Skill level for chords (0=beginner, 1=intermediate, 2=advanced)
    int scales;  ///< Skill level for scales (0=beginner, 1=intermediate, 2=advanced)

    /**
     * @brief Comparison operator for ordering States
     * @param other The State to compare with
     * @return true if this State is "less than" the other State
     * @details Compares States in lexicographical order: first by notes,
     *          then by chords, and finally by scales. This enables State
     *          to be used as a key in ordered containers like std::map.
     */
    bool operator<(const State& other) const {
        if (notes < other.notes) return true;
        if (notes > other.notes) return false;

        if (chords < other.chords) return true;
        if (chords > other.chords) return false;

        return scales < other.scales;
    }
    
    /**
     * @brief Converts the State to a JSON object
     * @return QJsonObject representation of the State
     * @details Serializes all skill levels of the State into a JSON object
     *          for storage in data files or transmission.
     */
    QJsonObject toJson() const {
        QJsonObject json;
        json["notes"] = notes;
        json["chords"] = chords;
        json["scales"] = scales;
        return json;
    }
    
    /**
     * @brief Populates the State from a JSON object
     * @param json QJsonObject containing State data
     * @details Deserializes data from a JSON object to populate the State's properties.
     *          Extracts skill levels for notes, chords, and scales if present.
     */
    void fromJson(const QJsonObject &json) {
        if (json.contains("notes")) notes = json["notes"].toInt();
        if (json.contains("chords")) chords = json["chords"].toInt();
        if (json.contains("scales")) scales = json["scales"].toInt();
    }
    
    /**
     * @brief Converts the State to a string representation
     * @return QString containing a bracketed, comma-separated list of skill levels
     * @details Creates a string in the format "[notes,chords,scales]" that can be
     *          used as a unique key for the State, useful for debugging and
     *          alternative storage mechanisms.
     */
    QString toKey() const {
        return QString("[%1,%2,%3]").arg(notes).arg(chords).arg(scales);
    }
    
    /**
     * @brief Creates a State from its string representation
     * @param key QString in the format "[notes,chords,scales]"
     * @return State object with the parsed skill levels
     * @details Parses a string created by toKey() back into a State object,
     *          extracting the skill levels for notes, chords, and scales.
     *          If the string format is invalid, returns a State with default values.
     */
    static State fromKey(const QString &key) {
        State state;
        QString temp = key;
        temp = temp.remove('[').remove(']');
        QStringList parts = temp.split(',');
        if (parts.size() >= 3) {
            state.notes = parts[0].toInt();
            state.chords = parts[1].toInt();
            state.scales = parts[2].toInt();
        }
        return state;
    }
};
