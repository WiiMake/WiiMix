

#include <QObject>
#include <QFrame>


struct BingoItem {
    std::string description;
    std::string name;
    std::string icon;
};
namespace BingoGame {
    void toolTip(QWidget *context);
    QFrame *createObjectiveWindow(std::vector<BingoItem> items);
}

