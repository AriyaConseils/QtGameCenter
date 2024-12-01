#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Delegates/playerdelegate.h"
#include "models/tableplayerproxymodel.h"
#include "signupdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // m_gameManager = new QOnlineGameCenter("http://localhost:9999", "ws://localhost:9998/", this);
    m_gameManager = new QOnlineGameCenter("http://178.195.54.144:9999/", "ws://178.195.54.144:9998/", this);

    m_localPlayer = m_gameManager->localPlayer();

    CONNECT_PROP(Player, m_localPlayer, online, ui->widgetStatusPicture, online);
    CONNECT_PROP_TRANSFORM(Player, m_localPlayer, image, ui->labelHeaderPicture, pixmap,
                           [this](QVariant image) { return QPixmap(image.toString()); });

    setupGameManager(m_gameManager);
    setupUI(m_gameManager);

    connect(ui->widgetPlayRoom, &PlayerRoom::roomInfoChanged, m_gameManager, &QOnlineGameCenter::postTableInformation);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupGameManager(QOnlineGameCenter *gameManager)
{
    // Connects to the server to start the game. Handles sign-up
    // requests using a lambda function that opens a dialog to
    // collect credentials and returns the data if accepted, or
    // an empty object if the user cancels.
    gameManager->connectToServer([]() -> QJsonObject {
        SignUpDialog dialog;
        if (dialog.exec() == QDialog::Accepted) {
            return dialog.getInfos();
        }
        return {};
    });
}


void MainWindow::setupUI(QOnlineGameCenter *gameManager)
{
    // Sets up the data models for the views:
    // - Connected players displayed in the list view
    // - Game rooms created by online players displayed in the tree view
    auto *proxy = gameManager->getProxyFriendList();
    ui->listView->setModel(proxy);
    ui->listView->setItemDelegate(new PlayerDelegate(this));

    ui->widgetPlayRoom->setLocalPlayer(m_localPlayer);

    ui->treeViewRooms->setModel(gameManager->roomModel());
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    // Clears the current room and allows 5 ms to send any
    // remaining messages to the server before closing the application,
    // but only if the game is connected to the server.
    if (m_localPlayer->online()) {
        ui->widgetPlayRoom->clearRoom();

        QEventLoop loop;
        QTimer::singleShot(5, &loop, &QEventLoop::quit);
        loop.exec();
    }
}


void MainWindow::on_pushButtonMyRoom_clicked()
{
    // Switches to the view for creating a room, allowing the player
    // to wait for another player to join the game.
    ui->widgetPlayRoom->initRoom();
    ui->stackedWidget->setCurrentIndex(2);
}


void MainWindow::on_pushButtonJoinRoom_clicked()
{
    // To be implemented: This will allow the player to join
    // an already open room hosted by an online player.
    ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_pushButtonBack_clicked()
{
    // Manages returning from views within the stacked widget without
    // requiring a common interface. This allows each widget to implement
    // its own custom logic for handling back navigation using a "getBack()"
    // method if it exists. Otherwise, it defaults to navigating to the
    // previous view in the stacked widget.
    QWidget *currentWidget = ui->stackedWidget->currentWidget();
    if (!currentWidget) return;

    const QMetaObject *metaObject = currentWidget->metaObject();
    int methodIndex = metaObject->indexOfMethod("getBack()");
    if (methodIndex != -1) {
        QMetaMethod method = metaObject->method(methodIndex);
        if (method.returnType() == QMetaType::Bool && method.parameterCount() == 0) {
            bool result = false;
            if (method.invoke(currentWidget, Q_RETURN_ARG(bool, result)) && result) {
                return;
            }
        }
    }
    ui->stackedWidget->setCurrentIndex(0);
}
