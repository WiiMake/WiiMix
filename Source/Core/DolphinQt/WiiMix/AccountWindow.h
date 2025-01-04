#include <QLabel>
#include <QPushButton>
#include "DolphinQt/WiiMix/Player.h"

class WiiMixAccountWindow : public QWidget
{
    Q_OBJECT

public:
    explicit WiiMixAccountWindow(QWidget* parent = nullptr, WiiMixPlayer* player = nullptr);

    void CreateLayout();
    void ConnectWidgets();
    void OnLogout();

signals:
    void onLogout();

// NOTE that if an errror occurs, the data on the client will be wrong
// but that you can always refresh the data by logging out and logging in again

private:
    WiiMixPlayer* m_player;
    QLabel* m_username;
    QPushButton* m_logout_button;
    QLabel* m_num_objectives_completed;
    QLabel* m_num_unique_objectives_completed;
    QLabel* m_num_objectives_attempted;
    QLabel* m_num_objectives_created;
    // TODOx: build in a UI for objective history at some point
};;