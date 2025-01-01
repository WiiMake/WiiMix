#include <QString>
#include <QLineEdit>
#include <QPushButton>
class WiiMixAccountLoginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit WiiMixAccountLoginWindow(QWidget* parent = nullptr);

    void CreateLayout();
    void ConnectWidgets();
    void OnLogin(QString username, QString password);

signals:
    void onLogin();

private:
    QLineEdit* m_username;
    QLineEdit* m_password;
    QPushButton* m_login_button;
};