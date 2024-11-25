

#include <QObject>
#include <QFrame>
#include <QPushButton>

namespace BingoGame {
    struct BingoItem {
        std::string description;
        std::string name;
        std::string icon;
        std::string iconFull;
    };
    QFrame *createObjectiveWindow(std::vector<BingoItem> items);
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

