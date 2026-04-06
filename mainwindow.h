#pragma once

#include <QMainWindow>
#include <QProcess>
#include <QTimer>
#include <QLabel>
#include <QRadioButton>
#include <QPushButton>
#include <QTextEdit>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QStatusBar>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QTabWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QScrollArea>
#include <QFrame>
#include <QButtonGroup>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void refreshStatus(bool syncSelection = true);
    void applyProfile();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessError(QProcess::ProcessError error);
    void onTrayActivated(QSystemTrayIcon::ActivationReason reason);
    void showAbout();
    void clearLog();

private:
    void setupUi();
    void setupMenuBar();
    void setupTray();
    void setupConnections();
    void populateProfileList(const QStringList &profiles);
    void runTunedAdm(const QStringList &args, const QString &description = QString());
    void appendLog(const QString &text, const QString &color = QString());
    void updateStatusDisplay(const QString &activeProfile, bool syncSelection = true);

    // tuned-adm detection
    bool isTunedInstalled();
    void buildSetupTab();
    void applySetupMode();
    void applyNormalMode();

    // Reads available profiles and current active profile (no root needed)
    static QStringList readAvailableProfiles();
    static QString readCurrentProfile();

    // State
    QString     m_currentProfile;
    QString     m_pendingOperation;
    QTimer     *m_autoRefreshTimer;

    // UI
    QWidget    *m_centralWidget;
    QTabWidget *m_tabWidget;

    // Header
    QLabel      *m_statusDot;
    QLabel      *m_statusLabel;
    QPushButton *m_refreshBtn;

    // Status tab
    QLabel *m_activeProfileLabel;
    QLabel *m_serviceStatusLabel;

    // Profiles tab
    QScrollArea  *m_profileScroll;
    QWidget      *m_profileContainer = nullptr;
    QButtonGroup *m_profileGroup     = nullptr;
    QPushButton  *m_applyBtn;

    // Log tab
    QTextEdit   *m_logView;
    QPushButton *m_clearLogBtn;

    // Reference tab
    QTableWidget *m_refTableWidget = nullptr;

    // Tray
    QSystemTrayIcon *m_trayIcon;
    QMenu           *m_trayMenu;
    QAction         *m_trayStatusAction;
    QAction         *m_trayQuitAction;

    // Setup tab — shown when tuned-adm is not found
    QWidget *m_setupTab = nullptr;

protected:
    void resizeEvent(QResizeEvent *event) override {
        QMainWindow::resizeEvent(event);
        if (m_refTableWidget) m_refTableWidget->resizeRowsToContents();
    }
};
