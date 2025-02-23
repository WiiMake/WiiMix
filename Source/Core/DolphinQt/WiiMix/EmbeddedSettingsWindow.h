#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QSlider>
#include <QCheckBox>
#include <QSpinBox>

#include "DolphinQt/WiiMix/ShuffleSettings.h"

class EmbeddedSettingsWindow final : public QWidget {
    Q_OBJECT

private:
    QVBoxLayout* m_config_layout;
    QSpinBox* m_num_players;
    QCheckBox* m_endless_mode;
    QLabel* m_min_switch_time_label;
    QLabel* m_max_switch_time_label;
    QLabel* m_num_players_label;
    QSlider* m_min_time_between_switch;
    QSlider* m_max_time_between_switch;
    int selectedSetting;
    QPushButton* startButton;
    QPushButton* returnButton;

signals:
    void backToUpperWidget();
    void StartWiiMixShuffle(WiiMixShuffleSettings *settings);

public:
    void setNumSwitches(int);
    void setEndless(bool);
    void setMinTimeBetweenSwitch(int);
    void setMaxTimeBetweenSwitch(int);
    explicit EmbeddedSettingsWindow(QWidget *);
    void keyPressEvent(QKeyEvent *) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
};