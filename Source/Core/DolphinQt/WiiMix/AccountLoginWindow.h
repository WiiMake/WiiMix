class WiiMixAccountLoginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit WiiMixAccountLoginWindow(QWidget* parent = nullptr);
    ~WiiMixAccountLoginWindow() override;

    void CreateLayout();
    void ConnectWidgets();
    void OnLogin(QString username, QString password);

signals:
    void onLogin(QString username, QString password);

private:
    QLineEdit* m_username;
    QLineEdit* m_password;
    QPushButton* m_login_button;
};