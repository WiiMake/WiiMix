#include <QWidget>
#include <QVBoxLayout>

class EmbeddedSettingsWindow final : public QWidget {
private:
    QVBoxLayout* m_config_layout;
    QLineEdit* m_num_switches;
    QCheckBox* m_endless_mode;
    QLabel* m_min_switch_time_label;
    QLabel* m_max_switch_time_label;
    QSlider* m_min_time_between_switch;
    QSlider* m_max_time_between_switch;
public:
    void setNumSwitches(int);
    void setEndless(bool);
    void setMinTimeBetweenSwitch(int);
    void setMaxTimeBetweenSwitch(int);
    EmbeddedSettingsWindow(QWidget *);
    void keyPressEvent(QKeyEvent *) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
};