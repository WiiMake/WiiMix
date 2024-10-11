#include <QGroupBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QWidget>
#include <QIcon>

#include "DolphinQt/WiiMix/ModesWidget.h"
#include "DolphinQt/WiiMix/ConfigWidget.h"
#include "DolphinQt/WiiMix/Settings.h"
#include "DolphinQt/Resources.h"

WiiMixConfigWidget::WiiMixConfigWidget(QWidget* parent) : QWidget(parent) {
    CreateLayout();
    ConnectWidgets();
}

void WiiMixConfigWidget::CreateLayout(WiiMixSettings::Mode mode) {
    // QGroupBox* m_config_box = new QGroupBox(tr("Configuration"));
    // QVBoxLayout* config_layout = new QVBoxLayout();

    // // num objectives (shuffle) - size of bingo card (bingo) - num stages in rogue (rogue)
    // QLabel* objectives_label = new QLabel(tr("Objectives:"));
    // m_objectives = new QComboBox();
    // config_layout->addWidget(objectives_label);
    // config_layout->addWidget(m_objectives);

    // // shuffle time TODO: need a custom slider to show values
    // QLabel* shuffle_time_label = new QLabel(tr("Shuffle Time:"));
    // m_shuffle_time = new QSlider(Qt::Horizontal);
    // config_layout->addWidget(shuffle_time_label);
    // config_layout->addWidget(m_shuffle_time);

    // // Difficulty
    // QLabel* difficulty_label = new QLabel(tr("Difficulty:"));
    // m_difficulty = new QLabel();
    // config_layout->addWidget(difficulty_label);
    // config_layout->addWidget(m_difficulty);

    // // Time (maybe I make this an estimate at the bottom rather than a parameter)
    // QLabel* time_label = new QLabel(tr("Time:"));
    // m_time = new QLabel();
    // config_layout->addWidget(time_label);
    // config_layout->addWidget(m_time);

    // // unverified save states (checkbox for all rounds)
    // m_unverified = new QCheckBox(tr("Unverified Save States"));
    // config_layout->addWidget(m_unverified);

    // m_config_box->setLayout(config_layout);
}

void WiiMixConfigWidget::ConnectWidgets() {
    // TODO: connect widgets here
    return;
}