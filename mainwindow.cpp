#include <QTableWidget>
#include <QHeaderView>
#include <QMenuBar>
#include <QScrollBar>
#include <QWheelEvent>
#include <QChildEvent>
#include "mainwindow.h"
#include "profiledata.h"
#include <QApplication>
#include <QMessageBox>
#include <QDateTime>
#include <QFont>
#include <QIcon>
#include <QPixmap>
#include <QPainter>
#include <QRegularExpression>
#include <QStandardPaths>

// ─── Helpers ────────────────────────────────────────────────────────────────

static QIcon makeColorIcon(const QColor& color) {
    QPixmap pm(16, 16);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setBrush(color);
    p.setPen(Qt::NoPen);
    p.drawRect(2, 2, 11, 11);
    return QIcon(pm);
}

// SmoothScrollArea — wheel events captured regardless of which child has focus
class SmoothScrollArea : public QScrollArea {
public:
    explicit SmoothScrollArea(QWidget* parent = nullptr) : QScrollArea(parent) {
        setFocusPolicy(Qt::WheelFocus);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        setWidgetResizable(true);
    }

    void setWidget(QWidget* w) {
        QScrollArea::setWidget(w);
        if (w) installOnAll(w);
    }

private:
    void installOnAll(QObject* obj) {
        obj->installEventFilter(this);
        for (QObject* child : obj->children())
            installOnAll(child);
    }

    void doScroll(QWheelEvent* we) {
        QScrollBar* vbar = verticalScrollBar();
        int px = we->pixelDelta().y();
        if (px != 0) {
            vbar->setValue(vbar->value() - px);
        } else {
            int delta = we->angleDelta().y();
            vbar->setValue(vbar->value() - (delta * 60 + (delta >= 0 ? 60 : -60)) / 120);
        }
    }

protected:
    void wheelEvent(QWheelEvent* e) override {
        doScroll(e);
        e->accept();
    }

    bool eventFilter(QObject* obj, QEvent* e) override {
        if (e->type() == QEvent::Wheel) {
            doScroll(static_cast<QWheelEvent*>(e));
            e->accept();
            return true;
        }
        if (e->type() == QEvent::ChildAdded) {
            auto* ce = static_cast<QChildEvent*>(e);
            if (ce->child()) installOnAll(ce->child());
        }
        return QScrollArea::eventFilter(obj, e);
    }
};

// ─── tuned-adm Detection ────────────────────────────────────────────────────

bool MainWindow::isTunedInstalled() {
    return !QStandardPaths::findExecutable("tuned-adm").isEmpty();
}

void MainWindow::buildSetupTab() {
    m_setupTab = new QWidget;
    auto* layout = new QVBoxLayout(m_setupTab);
    layout->setContentsMargins(32, 32, 32, 32);
    layout->setSpacing(16);

    auto* titleLabel = new QLabel("⚠  tuned not detected");
    QFont tf = titleLabel->font();
    tf.setPointSize(13);
    tf.setBold(true);
    titleLabel->setFont(tf);

    auto* bodyLabel = new QLabel(
        "This application requires <b>tuned</b> and <b>tuned-adm</b> to function.<br><br>"
        "They were not found on your system PATH. Please install the tuned package "
        "using your distribution's package manager, then restart this application.<br><br>"
        "<b>Fedora / RHEL / CentOS:</b><br>"
        "<code>sudo dnf install tuned</code><br><br>"
        "<b>After installation, enable the service:</b><br>"
        "<code>sudo systemctl enable --now tuned</code>"
    );
    bodyLabel->setTextFormat(Qt::RichText);
    bodyLabel->setWordWrap(true);
    bodyLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addWidget(bodyLabel);
    layout->addStretch();
}

void MainWindow::applySetupMode() {
    m_tabWidget->insertTab(0, m_setupTab, "⚠ Setup");
    m_tabWidget->setCurrentIndex(0);
    for (int i = 1; i < m_tabWidget->count(); ++i)
        m_tabWidget->setTabEnabled(i, false);
    m_autoRefreshTimer->stop();
}

void MainWindow::applyNormalMode() {
    const int setupIdx = m_tabWidget->indexOf(m_setupTab);
    if (setupIdx != -1)
        m_tabWidget->removeTab(setupIdx);
    for (int i = 0; i < m_tabWidget->count(); ++i)
        m_tabWidget->setTabEnabled(i, true);
    m_tabWidget->setCurrentIndex(0);
    m_autoRefreshTimer->start();

    const QStringList profiles = readAvailableProfiles();
    populateProfileList(profiles);
    refreshStatus();
}

// ─── Constructor ────────────────────────────────────────────────────────────

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("LGL Power Profile Manager");
    setMinimumSize(640, 520);
    resize(760, 600);

    setupUi();
    setupMenuBar();
    setupTray();
    setupConnections();

    m_autoRefreshTimer = new QTimer(this);
    m_autoRefreshTimer->setInterval(5000);
    connect(m_autoRefreshTimer, &QTimer::timeout, this, [this]{ refreshStatus(false); });

    buildSetupTab();

    if (isTunedInstalled()) {
        applyNormalMode();
    } else {
        applySetupMode();
        statusBar()->showMessage("tuned not found — see Setup tab");
    }
}

MainWindow::~MainWindow() {}

// ─── UI Setup ────────────────────────────────────────────────────────────────

void MainWindow::setupUi() {
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    auto* rootLayout = new QVBoxLayout(m_centralWidget);
    rootLayout->setContentsMargins(12, 12, 12, 8);
    rootLayout->setSpacing(8);

    // ── Header bar ──────────────────────────────────────────────────────────
    auto* headerFrame = new QFrame;
    headerFrame->setObjectName("headerFrame");
    auto* headerLayout = new QHBoxLayout(headerFrame);
    headerLayout->setContentsMargins(12, 8, 12, 8);

    auto* titleLabel = new QLabel("⚡ LGL Power Profile Manager");
    titleLabel->setObjectName("titleLabel");
    QFont tf = titleLabel->font();
    tf.setPointSize(14);
    tf.setBold(true);
    titleLabel->setFont(tf);

    m_statusDot = new QLabel("●");
    m_statusDot->setToolTip("tuned service status");
    QFont df = m_statusDot->font();
    df.setPointSize(16);
    m_statusDot->setFont(df);

    m_statusLabel = new QLabel("Unknown");

    m_refreshBtn = new QPushButton("↻ Refresh");
    m_refreshBtn->setMinimumWidth(120);
    m_refreshBtn->setFixedHeight(34);
    m_refreshBtn->setObjectName("secondaryBtn");

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_statusDot);
    headerLayout->addWidget(m_statusLabel);
    headerLayout->addSpacing(8);
    headerLayout->addWidget(m_refreshBtn);

    rootLayout->addWidget(headerFrame);

    // ── Tabs ────────────────────────────────────────────────────────────────
    m_tabWidget = new QTabWidget;
    m_tabWidget->setObjectName("mainTabs");

    // ── Tab 1: Status ────────────────────────────────────────────────────────
    auto* statusTab = new QWidget;
    auto* statusLayout = new QVBoxLayout(statusTab);
    statusLayout->setContentsMargins(12, 12, 12, 12);
    statusLayout->setSpacing(10);

    auto* profileGroup = new QGroupBox("Active Profile");
    auto* profileGrid  = new QGridLayout(profileGroup);
    profileGrid->setSpacing(8);

    auto addInfoRow = [&](QGridLayout* g, int row, const QString& label, QLabel*& valLabel) {
        auto* lbl = new QLabel(label);
        lbl->setObjectName("infoKey");
        valLabel = new QLabel("—");
        valLabel->setObjectName("infoVal");
        g->addWidget(lbl, row, 0);
        g->addWidget(valLabel, row, 1);
    };

    addInfoRow(profileGrid, 0, "Profile:", m_activeProfileLabel);
    addInfoRow(profileGrid, 1, "Service:", m_serviceStatusLabel);
    profileGrid->setColumnStretch(1, 1);

    statusLayout->addWidget(profileGroup);
    // ── power-profiles-daemon note ───────────────────────────────────────────
    auto* noteFrame = new QFrame;
    noteFrame->setFrameShape(QFrame::StyledPanel);
    auto* noteLayout = new QVBoxLayout(noteFrame);
    noteLayout->setContentsMargins(10, 8, 10, 8);
    noteLayout->setSpacing(4);

    auto* noteLabel = new QLabel(
        "<b>Note:</b> When a tuned profile is applied it will override power-profiles-daemon "
        "settings. If you notice your profile being reverted, you may need to disable "
        "power-profiles-daemon.<br><br>"
        "To disable:&nbsp;&nbsp;<code>sudo systemctl mask power-profiles-daemon</code><br>"
        "To re-enable: <code>sudo systemctl unmask power-profiles-daemon</code>"
    );
    noteLabel->setTextFormat(Qt::RichText);
    noteLabel->setWordWrap(true);
    noteLayout->addWidget(noteLabel);

    statusLayout->addWidget(noteFrame);
    statusLayout->addStretch();

    m_tabWidget->addTab(statusTab, "Status");

    // ── Tab 2: Profiles ──────────────────────────────────────────────────────
    auto* profilesTab    = new QWidget;
    auto* profilesLayout = new QVBoxLayout(profilesTab);
    profilesLayout->setContentsMargins(12, 12, 12, 12);
    profilesLayout->setSpacing(8);

    m_profileScroll = new SmoothScrollArea;
    m_profileScroll->setFrameShape(QFrame::NoFrame);
    profilesLayout->addWidget(m_profileScroll, 1);

    auto* applyRow = new QHBoxLayout;
    m_applyBtn = new QPushButton("▶  Apply Profile");
    m_applyBtn->setObjectName("primaryBtn");
    m_applyBtn->setFixedHeight(36);
    m_applyBtn->setMinimumWidth(160);
    applyRow->addWidget(m_applyBtn);
    applyRow->addStretch();
    profilesLayout->addLayout(applyRow);

    m_tabWidget->addTab(profilesTab, "Profiles");

    // ── Tab 3: Log ───────────────────────────────────────────────────────────
    auto* logTab    = new QWidget;
    auto* logLayout = new QVBoxLayout(logTab);
    logLayout->setContentsMargins(8, 8, 8, 8);
    logLayout->setSpacing(6);

    m_logView = new QTextEdit;
    m_logView->setReadOnly(true);
    m_logView->setObjectName("logView");
    QFont monof;
    monof.setFamily("Monospace");
    monof.setPointSize(9);
    m_logView->setFont(monof);

    m_clearLogBtn = new QPushButton("Clear Log");
    m_clearLogBtn->setObjectName("secondaryBtn");
    m_clearLogBtn->setFixedWidth(100);

    auto* logBtnRow = new QHBoxLayout;
    logBtnRow->addStretch();
    logBtnRow->addWidget(m_clearLogBtn);

    logLayout->addWidget(m_logView);
    logLayout->addLayout(logBtnRow);

    m_tabWidget->addTab(logTab, "Log");

    // ── Tab 4: Reference ─────────────────────────────────────────────────────
    auto* refTab    = new QWidget;
    auto* refLayout = new QVBoxLayout(refTab);
    refLayout->setContentsMargins(10, 10, 10, 10);
    refLayout->setSpacing(8);

    auto* refTitle = new QLabel("Profile Reference");
    refTitle->setObjectName("refTitle");
    QFont rtf = refTitle->font();
    rtf.setPointSize(11);
    rtf.setBold(true);
    refTitle->setFont(rtf);

    auto* refSubtitle = new QLabel("Use this table to choose the right tuned profile for your workload.");
    refSubtitle->setObjectName("infoKey");

    auto* table = new QTableWidget(refTab);
    table->setObjectName("refTable");
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({"Profile", "Label", "Description"});
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    table->verticalHeader()->setVisible(false);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->setShowGrid(false);
    table->setWordWrap(true);
    table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    const auto& profiles = builtinProfiles();
    table->setRowCount(profiles.size());
    for (int i = 0; i < profiles.size(); ++i) {
        const auto& p = profiles[i];

        auto* nameItem = new QTableWidgetItem(p.name);
        nameItem->setFont(QFont("Monospace", 9));
        nameItem->setForeground(QColor("#ff9900"));
        nameItem->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
        table->setItem(i, 0, nameItem);

        for (const auto& [col, text] : {
                 std::pair{1, p.label},
                 std::pair{2, p.description}}) {
            auto* item = new QTableWidgetItem(text);
            item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
            table->setItem(i, col, item);
        }
    }
    QTimer::singleShot(0, table, [table]() { table->resizeRowsToContents(); });

    refLayout->addWidget(refTitle);
    refLayout->addWidget(refSubtitle);
    refLayout->addWidget(table);
    m_refTableWidget = table;

    m_tabWidget->addTab(refTab, "Reference");

    rootLayout->addWidget(m_tabWidget);

    statusBar()->showMessage("Ready");
}

void MainWindow::populateProfileList(const QStringList& profiles) {
    // Replace container widget inside the scroll area
    auto* container = new QWidget;
    auto* layout    = new QVBoxLayout(container);
    layout->setSpacing(0);
    layout->setContentsMargins(4, 4, 4, 4);

    delete m_profileGroup;
    m_profileGroup = new QButtonGroup(this);

    for (const QString& name : profiles) {
        const ProfileInfo info = findProfile(name);

        auto* row    = new QWidget(container);
        auto* rowLay = new QVBoxLayout(row);
        rowLay->setContentsMargins(4, 6, 4, 6);
        rowLay->setSpacing(2);

        auto* rb = new QRadioButton(name, row);
        m_profileGroup->addButton(rb);
        rowLay->addWidget(rb);

        if (!info.label.isEmpty() || !info.description.isEmpty()) {
            const QString text = info.label.isEmpty()
                ? info.description
                : QString("<b>%1</b> - %2").arg(info.label, info.description);
            auto* desc = new QLabel("    " + text, row);
            desc->setTextFormat(Qt::RichText);
            desc->setWordWrap(true);
            auto pal = desc->palette();
            auto c   = pal.color(QPalette::WindowText);
            c.setAlphaF(0.55f);
            pal.setColor(QPalette::WindowText, c);
            desc->setPalette(pal);
            rowLay->addWidget(desc);
        }

        layout->addWidget(row);

        auto* sep = new QFrame(container);
        sep->setFrameShape(QFrame::HLine);
        sep->setFrameShadow(QFrame::Sunken);
        layout->addWidget(sep);
    }

    layout->addStretch();
    m_profileContainer = container;
    m_profileScroll->setWidget(container);
}

void MainWindow::setupMenuBar() {
    auto* aboutMenu   = menuBar()->addMenu("About");
    auto* aboutAction = aboutMenu->addAction("About LGL Power Profile Manager");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
}

void MainWindow::setupTray() {
    m_trayIcon = new QSystemTrayIcon(makeColorIcon(Qt::gray), this);
    m_trayMenu = new QMenu(this);

    m_trayStatusAction = m_trayMenu->addAction("Profile: Unknown");
    m_trayStatusAction->setEnabled(false);
    m_trayMenu->addSeparator();
    m_trayMenu->addAction("Show Window", this, [this] { show(); raise(); activateWindow(); });
    m_trayQuitAction = m_trayMenu->addAction("Quit", qApp, &QApplication::quit);

    m_trayIcon->setContextMenu(m_trayMenu);
    m_trayIcon->setToolTip("LGL Power Profile Manager");
    m_trayIcon->show();

    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayActivated);
}

void MainWindow::setupConnections() {
    connect(m_refreshBtn,  &QPushButton::clicked, this, &MainWindow::refreshStatus);
    connect(m_applyBtn,    &QPushButton::clicked, this, &MainWindow::applyProfile);
    connect(m_clearLogBtn, &QPushButton::clicked, this, &MainWindow::clearLog);
}

// ─── tuned-adm runner ────────────────────────────────────────────────────────

void MainWindow::runTunedAdm(const QStringList& args, const QString& description) {
    const QString desc = description.isEmpty() ? ("tuned-adm " + args.join(" ")) : description;
    appendLog(QString("[%1] Running: pkexec tuned-adm %2")
                  .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                  .arg(args.join(" ")),
              "#00ff00");

    m_pendingOperation = args.value(0);
    auto* proc = new QProcess(this);

    connect(proc, &QProcess::readyReadStandardOutput, this, [this, proc]() {
        appendLog(QString::fromUtf8(proc->readAllStandardOutput()).trimmed(), "#00ff00");
    });
    connect(proc, &QProcess::readyReadStandardError, this, [this, proc]() {
        appendLog(QString::fromUtf8(proc->readAllStandardError()).trimmed(), "#ff4444");
    });
    connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            &MainWindow::onProcessFinished);
    connect(proc, &QProcess::errorOccurred, this, &MainWindow::onProcessError);

    proc->start("pkexec", QStringList{"tuned-adm"} + args);
    m_applyBtn->setEnabled(false);
    statusBar()->showMessage(desc + "…");
}

void MainWindow::onProcessFinished(int exitCode, QProcess::ExitStatus) {
    auto* proc = qobject_cast<QProcess*>(sender());
    if (proc) proc->deleteLater();

    m_applyBtn->setEnabled(true);

    if (exitCode == 0) {
        statusBar()->showMessage("Done ✓", 3000);
        refreshStatus();
    } else if (exitCode == 126 || exitCode == 127) {
        const QString msg = (exitCode == 126)
            ? "Authorisation cancelled — operation aborted."
            : "pkexec not found — is polkit installed?";
        appendLog(msg, "#ff9900");
        statusBar()->showMessage(msg, 5000);
    } else {
        appendLog(QString("tuned-adm exited with code %1").arg(exitCode), "#ff4444");
        statusBar()->showMessage(QString("Error (exit %1)").arg(exitCode), 4000);
    }
}

void MainWindow::onProcessError(QProcess::ProcessError err) {
    auto* proc = qobject_cast<QProcess*>(sender());
    if (proc) proc->deleteLater();

    m_applyBtn->setEnabled(true);

    QString msg;
    switch (err) {
        case QProcess::FailedToStart:
            msg = "Failed to start tuned-adm (is tuned installed? is polkit running?)";
            break;
        case QProcess::Crashed: msg = "tuned-adm crashed"; break;
        default:                msg = "Process error"; break;
    }
    appendLog(msg, "#ff4444");
    statusBar()->showMessage(msg, 5000);
}

// ─── Actions ─────────────────────────────────────────────────────────────────

void MainWindow::refreshStatus(bool syncSelection) {
    auto* proc = new QProcess(this);

    connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
            [this, proc, syncSelection](int exitCode, QProcess::ExitStatus) {
                const QString out = QString::fromUtf8(proc->readAllStandardOutput()).trimmed();
                proc->deleteLater();

                if (exitCode != 0 || out.isEmpty()) {
                    updateStatusDisplay({}, syncSelection);
                    return;
                }

                // "Current active profile: balanced"
                static const QRegularExpression re(R"(Current active profile:\s*(\S+))");
                const auto m = re.match(out);
                updateStatusDisplay(m.hasMatch() ? m.captured(1) : QString{}, syncSelection);
            });

    proc->start("tuned-adm", {"active"});
}

void MainWindow::applyProfile() {
    QAbstractButton* checked = m_profileGroup ? m_profileGroup->checkedButton() : nullptr;
    if (!checked) {
        statusBar()->showMessage("No profile selected.", 3000);
        return;
    }

    const QString profile = checked->text();

    // Validate before passing to pkexec — defence in depth
    static const QRegularExpression kSafe(R"(^[a-z][a-z0-9_-]{1,63}$)");
    if (!kSafe.match(profile).hasMatch()) {
        appendLog("Invalid profile name: " + profile, "#ff4444");
        return;
    }

    runTunedAdm({"profile", profile},
                QString("Applying profile \"%1\"").arg(profile));
}

void MainWindow::updateStatusDisplay(const QString& activeProfile, bool syncSelection) {
    if (activeProfile.isEmpty()) {
        m_statusDot->setStyleSheet("color: gray;");
        m_statusLabel->setText("Unknown");
        m_activeProfileLabel->setText("—");
        m_serviceStatusLabel->setText("—");
        m_trayIcon->setIcon(makeColorIcon(Qt::gray));
        m_trayStatusAction->setText("Profile: Unknown");
    } else {
        m_statusDot->setStyleSheet("color: #3db03d;");
        m_statusLabel->setText("Active");
        m_activeProfileLabel->setText(activeProfile);
        m_serviceStatusLabel->setText("Running");
        m_trayIcon->setIcon(makeColorIcon(QColor("#3db03d")));
        m_trayStatusAction->setText("Profile: " + activeProfile);
        m_trayIcon->setToolTip("LGL Power Profile Manager — " + activeProfile);

        m_currentProfile = activeProfile;

        // Only sync radio buttons when explicitly requested (not on auto-refresh)
        // so the user's pending selection is not overwritten by the timer.
        if (syncSelection && m_profileGroup) {
            for (QAbstractButton* btn : m_profileGroup->buttons()) {
                if (btn->text() == activeProfile) {
                    btn->setChecked(true);
                    break;
                }
            }
        }
    }
}

void MainWindow::appendLog(const QString& text, const QString& color) {
    if (text.isEmpty()) return;
    const QString html = color.isEmpty()
        ? text.toHtmlEscaped()
        : QString("<span style='color:%1'>%2</span>").arg(color, text.toHtmlEscaped());
    m_logView->append(html);
}

void MainWindow::clearLog() {
    m_logView->clear();
}

void MainWindow::onTrayActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger) {
        if (isVisible()) {
            hide();
        } else {
            show();
            raise();
            activateWindow();
        }
    }
}

void MainWindow::showAbout() {
    QMessageBox::about(this, "About LGL Power Profile Manager",
        "<b>LGL Power Profile Manager</b> v1.0.0<br><br>"
        "A Qt6 GUI for managing tuned performance profiles.<br><br>"
        "Built for Fedora and RHEL-based systems.<br>"
        "© LinuxGamerLife");
}

// ─── Static helpers ──────────────────────────────────────────────────────────

QStringList MainWindow::readAvailableProfiles() {
    QProcess p;
    p.start("tuned-adm", {"list"});
    if (!p.waitForFinished(5000)) p.kill();

    QStringList profiles;
    if (p.exitCode() == 0) {
        const QString out = QString::fromUtf8(p.readAllStandardOutput());
        static const QRegularExpression re(R"(^-\s+(\S+))",
                                           QRegularExpression::MultilineOption);
        QRegularExpressionMatchIterator it = re.globalMatch(out);
        while (it.hasNext())
            profiles << it.next().captured(1);
    }

    if (profiles.isEmpty()) {
        for (const ProfileInfo& info : builtinProfiles())
            profiles << info.name;
        return profiles;
    }

    // Re-order to match builtinProfiles() order: known profiles first in the
    // defined order, then any custom/unknown profiles appended at the end.
    QStringList ordered;
    for (const ProfileInfo& info : builtinProfiles()) {
        if (profiles.contains(info.name))
            ordered << info.name;
    }
    for (const QString& name : profiles) {
        if (!ordered.contains(name))
            ordered << name;
    }
    return ordered;
}

QString MainWindow::readCurrentProfile() {
    QProcess p;
    p.start("tuned-adm", {"active"});
    if (!p.waitForFinished(3000)) { p.kill(); return {}; }

    const QString out = QString::fromUtf8(p.readAllStandardOutput()).trimmed();
    static const QRegularExpression re(R"(Current active profile:\s*(\S+))");
    const auto m = re.match(out);
    return m.hasMatch() ? m.captured(1) : QString{};
}
