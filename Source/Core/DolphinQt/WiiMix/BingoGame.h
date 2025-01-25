

#include <QObject>
#include <QFrame>
#include <QPushButton>
#include "Objective.h"

namespace BingoGame {
    QFrame *createObjectiveWindow(std::vector<WiiMixObjective> items);
}
class BingoButton : public QPushButton{
    typedef QPushButton inherited;
public:
    std::string icon;
    std::string name;
    std::string description;
    void toolTip(QWidget *context);
    bool eventFilter(QObject* obj, QEvent* event);
    virtual ~BingoButton() {
        icon = "";
        name = "";
        description = "";
        inherited::~QPushButton();
    }
};

