#include "signupdialog.h"
#include "ui_signupdialog.h"
#include <QDir>
#include <QListWidgetItem>
#include <QPixmap>
#include <QIcon>
#include <QJsonObject>

SignUpDialog::SignUpDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SignUpDialog)
{
    // Initializes the sign-up dialog and its UI components.
    // Removes the window borders and prepares the avatar selection list.

    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    initAvatarListWidget();
    loadImagesToListWidget();
}


SignUpDialog::~SignUpDialog()
{
    delete ui;
}

void SignUpDialog::initAvatarListWidget()
{
    // Configures the list widget for avatar selection.
    // Sets up grid display, icon size, spacing, and disables drag functionality.

    ui->listWidgetAvatar->setViewMode(QListWidget::IconMode); // Display items in a grid
    ui->listWidgetAvatar->setIconSize(QSize(120, 120));       // Set icon size
    ui->listWidgetAvatar->setResizeMode(QListWidget::Adjust); // Adjust the layout dynamically
    ui->listWidgetAvatar->setSpacing(15);                    // Add spacing between items
    ui->listWidgetAvatar->setUniformItemSizes(true);         // Uniform cell sizes
    ui->listWidgetAvatar->setGridSize(QSize(130, 130));      // Set grid cell size
    ui->listWidgetAvatar->setDragEnabled(false);             // Disable drag-and-drop
    ui->listWidgetAvatar->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Disable vertical scrollbar
    ui->listWidgetAvatar->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Disable horizontal scrollbar
}


void SignUpDialog::loadImagesToListWidget()
{
    // Loads avatar images from a specified directory into the list widget.
    // Configures the widget for grid display and allows users to select an avatar.
    // Highlights the selected avatar when clicked.

    // Directory containing the images
    QString picturesDir = ":/Avatars/assets";
    QDir dir(picturesDir);

    // Filter for .png files
    QStringList filters;
    filters << "*.png";
    dir.setNameFilters(filters);

    // Retrieve all .png files
    QFileInfoList fileList = dir.entryInfoList(QDir::Files);

    // Add images to the QListWidget
    for (const QFileInfo &fileInfo : fileList) {
        // Load the image
        QPixmap pixmap(fileInfo.filePath());
        QPixmap scaledPixmap = pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // Create a QListWidgetItem for the image
        QListWidgetItem *item = new QListWidgetItem(QIcon(scaledPixmap), "");
        item->setData(Qt::UserRole, fileInfo.filePath());
        item->setSizeHint(QSize(120, 120));
        item->setTextAlignment(Qt::AlignCenter);

        // Add the item to the QListWidget
        ui->listWidgetAvatar->addItem(item);
    }

    // Connect the selection signal
    connect(ui->listWidgetAvatar, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        QString filePath = item->data(Qt::UserRole).toString();
        qDebug() << "Selected image:" << filePath;

        // Highlight the selected item
        for (int i = 0; i < ui->listWidgetAvatar->count(); ++i) {
            QListWidgetItem *currentItem = ui->listWidgetAvatar->item(i);
            currentItem->setBackground(Qt::NoBrush); // Reset the background of other items
        }
        item->setBackground(QColor("#0078d7")); // Highlight the selected item
    });
}





void SignUpDialog::on_pushButtonOk_clicked()
{
    // Validates the form fields to ensure all required inputs are filled.
    // Checks if the passwords match, showing a warning if they don't.
    // Accepts the dialog if all validations pass.

    // Check if passwords match
    if (ui->lineEditPassword->text() != ui->lineEditPassword_2->text()) {
        ui->labelWarningPassword->show();
        return;
    }
    ui->labelWarningPassword->hide();

    // Dynamically find all QLineEdit children
    QList<QLineEdit*> lineEdits = this->findChildren<QLineEdit*>();

    // Validate fields and apply styling for empty fields
    bool isOk = true;
    for (QLineEdit *lineEdit : lineEdits) {
        if (lineEdit->text().isEmpty()) {
            lineEdit->setStyleSheet("border: 2px solid #8b3b3b;");
            isOk = false; // Stop if any field is empty
        } else {
            // Reset styling for filled fields
            lineEdit->setStyleSheet("");
        }
    }
    if (!isOk) return;

    // All fields are valid, accept the dialog
    accept();
}


QJsonObject SignUpDialog::getInfos()
{
    // Collects user input for first name, last name, password, and avatar.
    // Ensures a default avatar is used if none is selected.
    // Returns the information as a JSON object.

    QJsonObject userInfo;

    // Retrieve information from QLineEdit fields
    userInfo["firstName"] = ui->lineEditFistName->text();
    userInfo["lastName"] = ui->lineEditLastName->text();
    userInfo["password"] = ui->lineEditPassword->text();

    // Get the path of the selected avatar
    QListWidgetItem *selectedItem = ui->listWidgetAvatar->currentItem();
    if (selectedItem) {
        QString selectedImagePath = selectedItem->data(Qt::UserRole).toString();
        userInfo["image"] = selectedImagePath;
    } else {
        userInfo["image"] = QStringLiteral(":/Avatars/assets/pere-noel.png");
    }

    return userInfo;
}

