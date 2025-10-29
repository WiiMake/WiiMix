#include "DolphinQt/WiiMix/StateSendMenu.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
// #include <QRegExp> //TODOx: @gyoder figure out why this will not build on windows qt stuff
#include <QRegularExpressionValidator>
#include <QListWidgetItem>

#include "Core/ConfigManager.h"
#include "DolphinQt/WiiMix/CommonSettings.h"
#include "DolphinQt/WiiMix/GlobalSettings.h"
#include "DolphinQt/WiiMix/Client.h"
#include "DolphinQt/WiiMix/WebAPI.h"

WiiMixStateSendMenu::WiiMixStateSendMenu() {
    CreateLayout();
    ConnectWidgets();
}

void WiiMixStateSendMenu::CreateLayout() {
    m_title = new QLineEdit();
    m_title->setValidator(new QRegularExpressionValidator(QRegularExpression(QStringLiteral(".+")), this));

    m_description = new QTextEdit();

    m_achievement_id = new QSpinBox();
    m_achievement_id->setMinimum(1); // Ensure a valid achievement ID
    m_achievement_id->setMaximum(999999); // Arbitrary upper bound
    m_achievement_id->setValue(1); // Default to 1

    m_objective_types = new QListWidget();
    m_objective_types->setSelectionMode(QAbstractItemView::MultiSelection);

    for (int i = 0; i < static_cast<int>(WiiMixEnums::ObjectiveType::END); i++) {
        m_objective_types->addItem(new QListWidgetItem(QString::fromStdString(WiiMixEnums::ObjectiveTypeToString(static_cast<WiiMixEnums::ObjectiveType>(i)))));
    }

    m_game_genres = new QListWidget();
    m_game_genres->setSelectionMode(QAbstractItemView::MultiSelection);

    for (int i = 0; i < static_cast<int>(WiiMixEnums::GameGenre::END); i++) {
        m_game_genres->addItem(new QListWidgetItem(QString::fromStdString(WiiMixEnums::GameGenreToString(static_cast<WiiMixEnums::GameGenre>(i)))));
    }

    m_difficulty = new QComboBox();
    m_difficulty->setCurrentIndex(-1); // No default selection

    for (int i = 0; i < static_cast<int>(WiiMixEnums::Difficulty::END); i++) {
        m_difficulty->addItem(WiiMixCommonSettings::DifficultyToString(static_cast<WiiMixEnums::Difficulty>(i)));
    }

    m_time_length = new QSpinBox();
    m_time_length->setMinimum(1); // Ensure a positive time length

    m_send_button = new QPushButton(QStringLiteral("Send"));

    QVBoxLayout* layout = new QVBoxLayout();

    // General Information Group
    QGroupBox* generalGroup = new QGroupBox(QStringLiteral("General"));
    QFormLayout* generalLayout = new QFormLayout();
    generalLayout->addRow(new QLabel(QStringLiteral("Title:")), m_title);
    generalLayout->addRow(new QLabel(QStringLiteral("Description:")), m_description);
    generalLayout->addRow(new QLabel(QStringLiteral("Achievement ID:")), m_achievement_id);
    generalGroup->setLayout(generalLayout);
    layout->addWidget(generalGroup);
    
    // Categorization Group
    QGroupBox* categoriesGroup = new QGroupBox(QStringLiteral("Categories"));
    QFormLayout* objectivesLayout = new QFormLayout();
    objectivesLayout->addRow(new QLabel(QStringLiteral("Objective Types:")), m_objective_types);
    objectivesLayout->addRow(new QLabel(QStringLiteral("Game Genres:")), m_game_genres);
    objectivesLayout->addRow(new QLabel(QStringLiteral("Difficulty:")), m_difficulty);
    objectivesLayout->addRow(new QLabel(QStringLiteral("Length (minutes):")), m_time_length);
    categoriesGroup->setLayout(objectivesLayout);
    layout->addWidget(categoriesGroup);

    layout->addWidget(m_send_button, 0, Qt::AlignCenter);
    
    layout->addStretch();

    setLayout(layout);
}

void WiiMixStateSendMenu::ConnectWidgets() {
    connect(m_send_button, &QPushButton::clicked, this, [this]() {
        // Turn the send button into a progress bar
        m_send_button->setEnabled(false);
        m_send_button->setText(QStringLiteral("Sending 0% complete"));

        std::vector<WiiMixEnums::ObjectiveType> selected_objective_types;
        for (QListWidgetItem* item : m_objective_types->selectedItems()) {
            selected_objective_types.push_back(static_cast<WiiMixEnums::ObjectiveType>(m_objective_types->row(item)));
        }

        std::vector<WiiMixEnums::GameGenre> selected_game_genres;
        for (QListWidgetItem* item : m_game_genres->selectedItems()) {
            selected_game_genres.push_back(static_cast<WiiMixEnums::GameGenre>(m_game_genres->row(item)));
        }

        WiiMixObjective objective(
            -1,
            "",
            m_title->text().toStdString(),
            WiiMixWebAPI::instance()->getRetroAchievementsGameID(m_achievement_id->value()),
            SConfig::GetInstance().GetGameID(),
            m_achievement_id->value(),
            selected_objective_types,
            m_description->toPlainText().toStdString(),
            selected_game_genres,
            static_cast<WiiMixEnums::Difficulty>(m_difficulty->currentIndex()),
            m_time_length->value(),
            WiiMixGlobalSettings::instance()->GetPlayer() != nullptr ? WiiMixGlobalSettings::instance()->GetPlayer()->GetUsername() : ""
        );

        qDebug() << "achievement id objective: " << objective.GetAchievementId();
        qDebug() << "difficulty objective: " << QString::fromStdString(WiiMixEnums::DifficultyToString(objective.GetDifficulty()));

        emit SendObjective(objective);
    });
}

QPushButton* WiiMixStateSendMenu::GetSendButton() {
    return m_send_button;
}

void WiiMixStateSendMenu::SetProgressText(QString progress_text) {
    m_send_button->setText(progress_text);
}
