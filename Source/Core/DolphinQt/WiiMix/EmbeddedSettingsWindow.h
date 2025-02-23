#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QSlider>
#include <QCheckBox>

class EmbeddedSettingsWindow final : public QWidget {
private:
    QVBoxLayout* m_config_layout;
    QLineEdit* m_num_switches;
    QCheckBox* m_endless_mode;
    QLabel* m_min_switch_time_label;
    QLabel* m_max_switch_time_label;
    QSlider* m_min_time_between_switch;
    QSlider* m_max_time_between_switch;
    int selectedSetting;
    QLabel* num_switches_label;
    QPushButton* startButton;
public:
    void setNumSwitches(int);
    void setEndless(bool);
    void setMinTimeBetweenSwitch(int);
    void setMaxTimeBetweenSwitch(int);
    explicit EmbeddedSettingsWindow(QWidget *);
    void keyPressEvent(QKeyEvent *) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
};