#ifndef QUERY_BUILDER_H
#define QUERY_BUILDER_H

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QString>

// Make sure to include your headers that define these constants
// (Or redefine them here for the client)
#include "DolphinQt/WiiMix/Enums.h" 

/**
 * @brief A client-side helper class to build complex JSON queries for the server.
 *
 * This class uses a "builder pattern" so you can chain methods together
 * to create a valid query JSON object.
 */
class QueryBuilder
{
public:
    /**
     * @brief Starts a new query with the required server action.
     * @param action The Action enum (e.g., Action::GET_PLAYER)
     */
    QueryBuilder(WiiMixEnums::Action action = WiiMixEnums::Action::END, WiiMixEnums::Response response = WiiMixEnums::Response::END);

    /**
     * @brief Adds a simple, exact filter (e.g., "username" == "MyName").
     * @param column The database column to check (e.g., PLAYER_USERNAME).
     * @param value The value to match.
     * @return A reference to this builder for chaining.
     */
    QueryBuilder& selectWhere(const QString& column, const QJsonValue& value);

    /**
     * @brief Adds a "greater than" filter (e.g., "score" > 10).
     * @param column The database column to check (e.g., PLAYER_NUM_OBJECTIVES_COMPLETED).
     * @param value The value to compare against.
     * @return A reference to this builder for chaining.
     */
    QueryBuilder& filterGreaterThan(const QString& column, const QJsonValue& value);

    /**
     * @brief Adds a "less than" filter (e.g., "time" < 1000).
     * @param column The database column to check.
     * @param value The value to compare against.
     * @return A reference to this builder for chaining.
     */
    QueryBuilder& filterLessThan(const QString& column, const QJsonValue& value);

    /**
     * @brief Adds a filter to check if a column (array type) includes a value (e.g., "tags" includes "fun").
     * @param column The database column (should be an array type).
     * @param value The value to check for inclusion.
     * @return A reference to this builder for chaining.
     */
    QueryBuilder& filterIncludes(const QString& column, const QJsonValue& value);

    /**
     * @brief Adds a filter to check if a column (array type) includes a value (e.g., "tags" includes "fun").
     * @param column The database column (should be an array type).
     * @param value The value to check for inclusion.
     * @return A reference to this builder for chaining.
     */
    QueryBuilder& filterOrIncludes(const QString& column, const QJsonValue& value);

    /**
     * @brief Adds a column to sort by in ascending (A-Z, 0-9) order.
     * @param column The database column to sort by.
     * @return A reference to this builder for chaining.
     */
    QueryBuilder& sortByAsc(const QString& column);

    /**
     * @brief Adds a column to sort by in descending (Z-A, 9-0) order.
     * @param column The database column to sort by.
     * @return A reference to this builder for chaining.
     */
    QueryBuilder& sortByDesc(const QString& column);

    /**
     * @brief Limits the number of results the server should return.
     * @param count The maximum number of rows.
     * @return A reference to this builder for chaining.
     */
    QueryBuilder& limit(int count);

    /**
     * @brief Tells the server to return a count of matching rows, not the rows themselves.
     * @return A reference to this builder for chaining.
     */
    QueryBuilder& asCount();

    /**
     * @brief Adds any other specific key/value pair to the request.
     * Useful for keys like OBJECTIVE_HISTORY_PLAYER_ID.
     */
    QueryBuilder& with(const QString& key, const QJsonValue& value);

    /**
     * @brief Adds a randomization flag to the query, telling the server to return results in random order.
     * @return A reference to this builder for chaining.
     */
    QueryBuilder& random();

    /**
     * @brief Returns the final, completed QJsonObject.
     */
    QJsonObject build() const;

private:
    QJsonObject m_json;
};

/*
---------------------------------------------------------------------------
--  HOW TO USE THIS BUILDER:
---------------------------------------------------------------------------

// Include the builder
#include "QueryBuilder.h"

// --- Example 1: Get a player by username ---
QJsonObject query1 = QueryBuilder(Action::GET_PLAYER)
                        .selectWhere(PLAYER_USERNAME, "MyName")
                        .build();
// Resulting JSON:
// { "ACTION": ..., "SELECT": "username", "WHERE": "MyName" }


// --- Example 2: Get the Top 10 players by score ---
QJsonObject query2 = QueryBuilder(Action::GET_PLAYER)
                        .sortByDesc(PLAYER_NUM_OBJECTIVES_COMPLETED)
                        .limit(10)
                        .build();
// Resulting JSON:
// { "ACTION": ..., "SORT_DESC": ["num_objectives_completed"], "NUM_ROWS": 10 }


// --- Example 3: Get a COUNT of all "Hard" objectives ---
QJsonObject query3 = QueryBuilder(Action::GET_OBJECTIVE)
                        .selectWhere(OBJECTIVE_DIFFICULTY, "HARD")
                        .asCount()
                        .build();
// Resulting JSON:
// { "ACTION": ..., "SELECT": "difficulty", "WHERE": "HARD", "COUNT": true }


// --- Example 4: A complex query ---
// Get the 5 most recent objectives that are "HARD" and have more than 0 players
QJsonObject query4 = QueryBuilder(Action::GET_OBJECTIVE)
                        .selectWhere(OBJECTIVE_DIFFICULTY, "HARD")
                        .filterGreaterThan(OBJECTIVE_NUM_PLAYERS, 0)
                        .sortByDesc(OBJECTIVE_ID) // Assuming higher ID is newer
                        .limit(5)
                        .build();
// Resulting JSON:
// { 
//   "ACTION": ..., 
//   "SELECT": "difficulty", 
//   "WHERE": "HARD",
//   "FILTER_GREATER_THAN": { "num_players": 0 },
//   "SORT_DESC": ["id"],
//   "NUM_ROWS": 5
// }

*/

#endif // QUERY_BUILDER_H

