
#include "BingoGame.h"
#include "Common/FileUtil.h"
#include <QToolTip>
#include <QGridLayout>
#include "DolphinQt/Resources.h"
#include <QEvent>

QFrame* BingoGame::createObjectiveWindow(std::vector<BingoItem> items) {
    double sqrtSize = round(sqrt(items.size()));
    if (sqrtSize * sqrtSize != items.size()) {
        printf("incorrect items size for bingo window, aborting drawing");
    }
    QFrame* frame = new QFrame();
    frame->setStyleSheet(QStringLiteral("QToolTip {border-radius 5px; background-color: #1B2840; border-color: black}"));
    frame->setMouseTracking(true);
    frame->setAttribute(Qt::WA_Hover, true);
    QGridLayout* gridLayout = new QGridLayout();
    for (int i = 0; i < sqrtSize; i++) {
        for (int j = 0; j < sqrtSize; j++) {
            BingoButton* button = new BingoButton();
            button->setIcon(Resources::GetResourceIcon(items[i * sqrtSize + j].icon));
            button->installEventFilter(gridLayout); // To capture mouse events
            button->setAttribute(Qt::WA_Hover, true);
            button->icon = items[i * sqrtSize + j].iconFull;
            button->name = items[i * sqrtSize + j].name;
            button->description = items[i * sqrtSize + j].description;
            button->toolTip(frame); // TODO: set this to happen on hover
            gridLayout->addWidget(button, i, j);
        }
    }
    gridLayout->setSpacing(5);
    frame->setLayout(gridLayout);
    frame->show();
    return frame;
}
void BingoButton::toolTip(QWidget* context) {
    char toolTipString[1000];
    std::ifstream t("toolTip.html");
    std::stringstream buffer;
    buffer << t.rdbuf();
    snprintf(toolTipString, 999, buffer.str().data(), icon.data(), name.data(), description.data());
    //snprintf(toolTipString, 999, "<style> @keyframes appearance {0%% {opacity: 0} 60%% {opacity: 0.6} 100%% {opacity: 1}} .container{display: flex;} img {max-width: 25%%; max-height:15%%; float: left;} .text {font-size: 20px; padding-left: 20px; padding-top: 20%%; float: left; } div {animation-duration: 5s; animation-timing-function: ease-in; animation-name: appearance}</style> <div class='container'> <div class='image'> <img src='%s'> </div> <div class='text'> <h2>%s</h2> </div> </div>\n%s", icon.data(), name.data(), description.data());
    QToolTip::showText(QCursor::pos(), QString::fromStdString(toolTipString), context, QRect(), 100000);
}
bool BingoButton::eventFilter(QObject *obj, QEvent* event) {
    if ((event->type() == QEvent::HoverEnter)) {
        printf("Hover event on bingoButton\n");
        toolTip((BingoButton*) obj);
        return true;
    }
    return QWidget::eventFilter(obj, event);
}
