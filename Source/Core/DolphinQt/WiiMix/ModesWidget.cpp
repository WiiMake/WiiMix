#include <QGroupBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QWidget>
#include <QIcon>
#include <QResizeEvent>

#include "DolphinQt/WiiMix/ModesWidget.h"
#include "DolphinQt/WiiMix/Settings.h"

WiiMixModesWidget::WiiMixModesWidget(QWidget* parent) : QWidget(parent) {
    CreateLayout();
    ConnectWidgets();
}

void WiiMixModesWidget::CreateLayout() {
    m_mode_box = new QGroupBox(tr("Modes"));
    m_mode_layout = new QHBoxLayout();
    m_mode_box->setLayout(m_mode_layout);
    m_mode_selectors = {};
    for (int i = 0; i < static_cast<int>(WiiMixSettings::Mode::END); i++) {
        m_mode_selectors[i] = new QVBoxLayout();
    }

    // Create selectors
    QFont titleFont = QFont();
    titleFont.setPointSize(42);
    titleFont.setBold(true);

    QFont descriptionFont = QFont();
    descriptionFont.setPointSize(16);

    for (int i = 0; i < static_cast<int>(WiiMixSettings::Mode::END); i++) {
        QString titleText = WiiMixSettings::ModeToTitle(WiiMixSettings::Mode(i));
        if (titleText == QStringLiteral("")) {
            continue;
        }

        // Title
        QLabel* title = new QLabel(titleText);
        title->setFont(titleFont);
        title->setAlignment(Qt::AlignHCenter);

        // Icon
        QIcon icon = WiiMixSettings::ModeToIcon(WiiMixSettings::Mode(i));
        if (icon.isNull()) {
            continue;
        }
        QLabel* iconLabel = new QLabel();
        iconLabel->setPixmap(icon.pixmap(64, 64));  // Set initial size, resizable later
        iconLabel->setAlignment(Qt::AlignHCenter);

        // Description
        QString descriptionText = WiiMixSettings::ModeToDescription(WiiMixSettings::Mode(i));
        if (descriptionText == QStringLiteral("")) {
            continue;
        }
        QLabel* description = new QLabel(descriptionText);
        description->setFont(descriptionFont);
        description->setWordWrap(true);  // Ensure text wraps vertically
        description->setAlignment(Qt::AlignHCenter);

        // Create a QFrame for the selector
        QFrame* mode_selector_frame = new QFrame();
        mode_selector_frame->setFrameShape(QFrame::StyledPanel);
        mode_selector_frame->setStyleSheet(QStringLiteral("QFrame { border: 2px solid transparent; }"));
        mode_selector_frame->setCursor(Qt::PointingHandCursor);
        mode_selector_frame->installEventFilter(this); // To capture mouse events

        QVBoxLayout* layout = new QVBoxLayout();
        layout->addWidget(title);
        layout->addWidget(iconLabel, 0);
        layout->addWidget(description, 1);

        mode_selector_frame->setLayout(layout);

        m_mode_layout->addWidget(mode_selector_frame);
    }

    setLayout(m_mode_layout);

    // for (int i = 0; i < static_cast<int>(WiiMixSettings::Mode::END); i++) {
    //     qDebug() << "Creating layout for mode " << Mode(i);
    //     QString titleText = WiiMixSettings::ModeToTitle(Mode(i));
    //     qDebug() << "TitleText: " << titleText;
    //     if (titleText == QStringLiteral("")) {
    //         continue;
    //     }
    //     QLabel* title = new QLabel(titleText);
    //     title->setFont(titleFont);
    //     title->setAlignment(Qt::AlignHCenter);

    //     QIcon icon = WiiMixSettings::ModeToIcon(Mode(i));
    //     if (icon.isNull()) {
    //         continue;
    //     }
    //     QLabel* iconLabel = new QLabel();
    //     iconLabel->setPixmap(icon.pixmap(32, 32)); // Adjust the size as needed
    //     iconLabel->setAlignment(Qt::AlignHCenter);

    //     QString descriptionText = WiiMixSettings::ModeToDescription(Mode(i));
    //     if (descriptionText == QStringLiteral("")) {
    //         continue;
    //     }
    //     QLabel* description = new QLabel(descriptionText);
    //     description->setFont(descriptionFont);
    //     description->setWordWrap(true);
    //     description->setAlignment(Qt::AlignHCenter);

    //     QWidget* container = new QWidget();
    //     container->setLayout(m_mode_selectors[i]);
    //     container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    //     m_mode_layout->addWidget(container);

    //     m_mode_selectors[i]->addWidget(title);
    //     m_mode_selectors[i]->addWidget(iconLabel);
    //     m_mode_selectors[i]->addWidget(description);
    //     m_mode_layout->addLayout(m_mode_selectors[i]);
    // }

    // setLayout(m_mode_layout);
}

// bool WiiMixModesWidget::eventFilter(QObject* obj, QEvent* event) {
//     if (event->type() == QEvent::MouseButtonPress) {
//         QFrame* frame = qobject_cast<QFrame*>(obj);
//         if (frame) {
//             // Deselect all other frames
//             for (int i = 0; i < WiiMixSettings::Mode::END; i++) {
//                 QFrame* other_frame = qobject_cast<QFrame*>(m_mode_layout->itemAt(i)->widget());
//                 if (other_frame) {
//                     other_frame->setStyleSheet("QFrame { border: 2px solid transparent; }");
//                 }
//             }

//             // Highlight the selected frame
//             frame->setStyleSheet("QFrame { border: 2px solid blue; }");

//             // Trigger configuration update for this mode
//             Mode selected_mode = static_cast<Mode>(m_mode_layout->indexOf(frame));
//             emit ModeSelected(selected_mode);  // You would need to define this signal

//             return true;
//         }
//     }
//     return QWidget::eventFilter(obj, event);
// }

void WiiMixModesWidget::ConnectWidgets() {
    // TODO: connect widgets here
    return;
}