// pqxx docs: https://pqxx.org/development/libpqxx/

#include "DolphinQt/WiiMix/Objective.h"

/*
-- GENRES Table
CREATE TABLE GENRES (
    id SERIAL PRIMARY KEY,
    game_genre game_genres_enum NOT NULL UNIQUE
);

-- OBJECTIVE_TYPES Table
CREATE TABLE OBJECTIVE_TYPES (
    id SERIAL PRIMARY KEY,
    objective_type objective_type_enum NOT NULL UNIQUE
);

-- OBJECTIVES Table
CREATE TABLE OBJECTIVES (
    id SERIAL PRIMARY KEY,
    title VARCHAR(100) NOT NULL,
    retroachievements_game_id INTEGER NOT NULL,
    game_id VARCHAR(6) NOT NULL,
    objective_type_id INTEGER NOT NULL REFERENCES OBJECTIVE_TYPES(id),
    objective_description TEXT NOT NULL,
    game_genre_id INTEGER NOT NULL REFERENCES GENRES(id),
    difficulty difficulty_enum NOT NULL,
    time INTEGER NOT NULL,
    creator_id INTEGER NOT NULL REFERENCES PLAYERS(id) ON DELETE SET NULL -- Links to user who created the objective
);

-- PLAYERS Table
CREATE TABLE PLAYERS (
    id SERIAL PRIMARY KEY,
    username VARCHAR(50) NOT NULL UNIQUE,
    password_hash TEXT NOT NULL -- Store securely hashed passwords
);

-- COMPLETED_OBJECTIVES Table
CREATE TABLE COMPLETED_OBJECTIVES (
    player_id INTEGER NOT NULL REFERENCES PLAYERS(id) ON DELETE CASCADE,
    objective_id INTEGER NOT NULL REFERENCES OBJECTIVES(id) ON DELETE CASCADE,
    completed_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP, -- Tracks completion time
    PRIMARY KEY (player_id, objective_id)
); 
*/
#include <pqxx/pqxx>


// I don't think there will be threading problems with this
// But if there are, looking into thread_local so each thread gets its own connection might be a good idea 

// NOTE that WiiMixDBApi handles the postgres db and manages the objective files
class WiiMixDBApi
{
public:
    explicit WiiMixDBApi();

    bool UpdateObjective(WiiMixObjective objective);
    bool AddObjective(WiiMixObjective objective, std::string state_path);
    bool DeleteObjective(WiiMixObjective objective);
    // save_state = true if you want to pull the corresponding save state as well
    pqxx::result GetObjective(WiiMixObjective objective, bool save_state = false);

    pqxx::result GetGenres(WiiMixObjective objective);
    bool RemoveGenre(WiiMixObjective objective, WiiMixEnums::GameGenre genre);
    bool AddGenre(WiiMixObjective objective, WiiMixEnums::GameGenre genre);
    pqxx::result GetObjectiveTypes(WiiMixObjective objective);
    bool RemoveObjectiveType(WiiMixObjective objective, WiiMixEnums::ObjectiveType objective_type);
    bool AddObjectiveType(WiiMixObjective objective, WiiMixEnums::ObjectiveType objective_type);
    pqxx::result GetPlayers();
    pqxx::result GetCompletedObjectives(WiiMixObjective objective);
    bool AddPlayer(std::string username, std::string password_hash);
    bool DeletePlayer(int player_id);
    bool MarkObjectiveAsCompleted(int player_id, int objective_id);
    bool MarkObjectiveAsUncompleted(int player_id, int objective_id);
    pqxx::result GetObjectiveById(int objective_id);
    pqxx::result GetObjectivesByGameId(std::string game_id);
    pqxx::result GetObjectivesByPlayerId(int player_id);
    pqxx::result GetObjectivesByGenre(int genre_id);
    pqxx::result GetObjectivesByType(int objective_type_id);
    pqxx::result GetObjectivesByDifficulty(std::string difficulty);
    pqxx::result GetObjectivesByTime(int time);
    pqxx::result GetObjectivesByCreator(int creator_id);

private:
    // pqxx::connection m_connection;
    // pqxx::work m_work;
};
