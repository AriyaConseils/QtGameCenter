#ifndef SIGNUPDIALOG_H
#define SIGNUPDIALOG_H

#include <QDialog>
#include <QJsonObject>



namespace Ui {
class SignUpDialog;
}


/**
 * @class SignUpDialog
 * @brief A dialog for collecting user information to sign up or log in.
 *
 * This dialog allows users to enter their details such as first name, last name,
 * password, and choose an avatar. It validates the input and provides the collected
 * information as a JSON object.
 */
class SignUpDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a SignUpDialog.
     * @param parent The parent widget (optional).
     */
    explicit SignUpDialog(QWidget *parent = nullptr);

    /**
     * @brief Destructor for SignUpDialog.
     */
    ~SignUpDialog();

    /**
     * @brief Retrieves the user information entered in the dialog.
     *
     * Collects details such as first name, last name, password,
     * and the selected avatar, and returns them in a QJsonObject.
     *
     * @return QJsonObject containing user details.
     */
    QJsonObject getInfos();

private slots:
    /**
     * @brief Handles the OK button click event.
     *
     * Validates the input fields and checks if passwords match.
     * If validation succeeds, the dialog is accepted.
     */
    void on_pushButtonOk_clicked();

private:
    Ui::SignUpDialog *ui; ///< UI instance for the dialog

    /**
     * @brief Configures the avatar list widget.
     *
     * Sets up the grid display, icon size, spacing, and disables drag functionality
     * for the avatar selection list widget.
     */
    void initAvatarListWidget();

    /**
     * @brief Loads avatar images into the list widget.
     *
     * Populates the list widget with images from a specified directory.
     * Each image is represented as an icon and associated with its file path.
     */
    void loadImagesToListWidget();
};

#endif // SIGNUPDIALOG_H
