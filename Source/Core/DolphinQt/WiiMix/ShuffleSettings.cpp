#include "DolphinQt/WiiMix/ShuffleSettings.h"

#include <QList>
#include "DolphinQt/WiiMix/Enums.h"

WiiMixShuffleSettings::WiiMixShuffleSettings(
    WiiMixEnums::Difficulty difficulty,
      WiiMixEnums::SaveStateBank save_state_bank,
      std::vector<WiiMixObjective> objectives,
      std::vector<WiiMixEnums::ObjectiveType> types,
      std::vector<WiiMixEnums::GameGenre> genres,
    int number_of_switches, 
    int min_time_between_switch, 
    int max_time_between_switch, 
    bool endless)
    : WiiMixCommonSettings(
        difficulty,
        save_state_bank,
        objectives
    ),
      m_number_of_switches(number_of_switches),
      m_min_time_between_switch(min_time_between_switch),
      m_max_time_between_switch(max_time_between_switch),
      m_endless(endless)
{
    #ifdef QT_GUI_LIB
        if (number_of_switches != DEFAULT_NUMBER_OF_SWITCHES) {
            m_number_of_switches = number_of_switches;
        } else if (Config::Get(Config::WIIMIX_NUMBER_OF_SWITCHES) != DEFAULT_NUMBER_OF_SWITCHES) {
            m_number_of_switches = Config::Get(Config::WIIMIX_NUMBER_OF_SWITCHES);
        } else {
            m_number_of_switches = DEFAULT_NUMBER_OF_SWITCHES;
        }
    #endif
    if (number_of_switches != DEFAULT_NUMBER_OF_SWITCHES) {
        m_number_of_switches = number_of_switches;
    } else {
        m_number_of_switches = DEFAULT_NUMBER_OF_SWITCHES;
    }

    #ifdef QT_GUI_LIB
        if (min_time_between_switch != DEFAULT_MIN_SWITCH_TIME) {
            m_min_time_between_switch = min_time_between_switch;
        } else if (Config::Get(Config::WIIMIX_MIN_TIME_BETWEEN_SWITCH) != DEFAULT_MIN_SWITCH_TIME) {
            m_min_time_between_switch = Config::Get(Config::WIIMIX_MIN_TIME_BETWEEN_SWITCH);
        } else {
            m_min_time_between_switch = DEFAULT_MIN_SWITCH_TIME;
        }
    #endif
    if (min_time_between_switch != DEFAULT_MIN_SWITCH_TIME) {
        m_min_time_between_switch = min_time_between_switch;
    } else {
        m_min_time_between_switch = DEFAULT_MIN_SWITCH_TIME;
    }
    qDebug() << "min_time_between_switch: " << m_min_time_between_switch;

    #ifdef QT_GUI_LIB
        if (max_time_between_switch != DEFAULT_MAX_SWITCH_TIME) {
            m_max_time_between_switch = max_time_between_switch;
        } else if (Config::Get(Config::WIIMIX_MAX_TIME_BETWEEN_SWITCH) != DEFAULT_MAX_SWITCH_TIME) {
            m_max_time_between_switch = Config::Get(Config::WIIMIX_MAX_TIME_BETWEEN_SWITCH);
        } else {
            m_max_time_between_switch = DEFAULT_MAX_SWITCH_TIME;
        }
    #endif

    if (max_time_between_switch != DEFAULT_MAX_SWITCH_TIME) {
        m_max_time_between_switch = max_time_between_switch;
    } else {
        m_max_time_between_switch = DEFAULT_MAX_SWITCH_TIME;
    }
    qDebug() << "max_time_between_switch: " << m_max_time_between_switch;

    #ifdef QT_GUI_LIB
        if (endless != DEFAULT_IS_ENDLESS) {
            m_endless = endless;
        } else if (Config::Get(Config::WIIMIX_IS_ENDLESS) != DEFAULT_IS_ENDLESS) {
            m_endless = Config::Get(Config::WIIMIX_IS_ENDLESS);
        } else {
            m_endless = DEFAULT_IS_ENDLESS;
        }
    #endif

    if (endless != DEFAULT_IS_ENDLESS) {
        m_endless = endless;
    } else {
        m_endless = DEFAULT_IS_ENDLESS;
    }
}

int WiiMixShuffleSettings::GetNumberOfSwitches() const
{
    return m_number_of_switches;
}

void WiiMixShuffleSettings::SetNumberOfSwitches(int value)
{
    m_number_of_switches = value;
}

int WiiMixShuffleSettings::GetMinTimeBetweenSwitch() const
{
    return m_min_time_between_switch;
}

void WiiMixShuffleSettings::SetMinTimeBetweenSwitch(int value)
{
    m_min_time_between_switch = value;
}

int WiiMixShuffleSettings::GetMaxTimeBetweenSwitch() const
{
    return m_max_time_between_switch;
}

void WiiMixShuffleSettings::SetMaxTimeBetweenSwitch(int value)
{
    m_max_time_between_switch = value;
}

bool WiiMixShuffleSettings::GetEndless() const
{
    return m_endless;
}

void WiiMixShuffleSettings::SetEndless(bool value)
{
    m_endless = value;
}

void WiiMixShuffleSettings::SetDifficulty(WiiMixEnums::Difficulty difficulty) {
    m_difficulty = difficulty;
    #ifdef QT_GUI_LIB
        Config::Set(Config::LayerType::Base, Config::WIIMIX_SHUFFLE_DIFFICULTY, difficulty);
    #endif
    // emit SettingsChanged(difficulty);
}

void WiiMixShuffleSettings::SetSaveStateBank(WiiMixEnums::SaveStateBank save_state_bank) {
    m_save_state_bank = save_state_bank;
    #ifdef QT_GUI_LIB
        Config::Set(Config::LayerType::Base, Config::WIIMIX_SHUFFLE_SAVE_STATE_BANK, save_state_bank);
    #endif
    // emit SettingsChanged(save_state_bank);
}

void WiiMixShuffleSettings::SetObjectiveTypes(std::vector<WiiMixEnums::ObjectiveType> types) {
    m_objective_types = types;
    // Convert std::vector to QList
    // QList<WiiMixEnums::ObjectiveType> qlist_types = QList<WiiMixEnums::ObjectiveType>::fromVector(QVector<WiiMixEnums::ObjectiveType>::fromStdVector(types));

    // Wrap in Config::Info if needed
    // Config::Info<std::vector<WiiMixEnums::ObjectiveType>> info(types);
    #ifdef QT_GUI_LIB
        Config::Set(Config::LayerType::Base, Config::WIIMIX_SHUFFLE_OBJECTIVE_TYPES, WiiMixEnums::ObjectiveTypesToString(types));
    #endif
    // emit SettingsChanged(types);
}

void WiiMixShuffleSettings::SetGameGenres(std::vector<WiiMixEnums::GameGenre> game_genres) {
    m_game_genres = game_genres;
    #ifdef QT_GUI_LIB
        Config::Set(Config::LayerType::Base, Config::WIIMIX_SHUFFLE_GAME_GENRES, WiiMixEnums::GameGenresToString(game_genres));
    #endif
    // emit SettingsChanged(game_genres);
}

QJsonDocument WiiMixShuffleSettings::ToJson() {
    QJsonObject obj = ToJsonCommon();
    obj[QStringLiteral(SHUFFLE_SETTINGS_NUMBER_OF_SWITCHES)] = m_number_of_switches;
    obj[QStringLiteral(SHUFFLE_SETTINGS_MIN_TIME_BETWEEN_SWITCH)] = m_min_time_between_switch;
    obj[QStringLiteral(SHUFFLE_SETTINGS_MAX_TIME_BETWEEN_SWITCH)] = m_max_time_between_switch;
    obj[QStringLiteral(SHUFFLE_SETTINGS_IS_ENDLESS)] = m_endless;
    return QJsonDocument(obj);
}

void WiiMixShuffleSettings::FromJson(QJsonDocument json) {
    FromJsonCommon(json);
    QJsonObject obj = json.object();
    m_number_of_switches = obj[QStringLiteral(SHUFFLE_SETTINGS_NUMBER_OF_SWITCHES)].toInt();
    m_min_time_between_switch = obj[QStringLiteral(SHUFFLE_SETTINGS_MIN_TIME_BETWEEN_SWITCH)].toInt();
    m_max_time_between_switch = obj[QStringLiteral(SHUFFLE_SETTINGS_MAX_TIME_BETWEEN_SWITCH)].toInt();
    m_endless = obj[QStringLiteral(SHUFFLE_SETTINGS_IS_ENDLESS)].toBool();
}