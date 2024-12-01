#include "playerstatistics.h"
#include "ui_playerstatistics.h"

PlayerStatistics::PlayerStatistics(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlayerStatistics)
{
    ui->setupUi(this);

    CONNECT_PROP_TRANSFORM(PlayerStatistics, this, online, ui->status, styleSheet, [this](QVariant value) {
        return getStatusStyleSheet(value.toBool() ? "green" : "red");
    });

    CONNECT_PROP(PlayerStatistics, this, firstName, ui->lineEditFirstName, text);
    CONNECT_PROP(PlayerStatistics, this, lastName, ui->lineEditLastName, text);

    CONNECT_PROP_TRANSFORM(PlayerStatistics, this, gamesPlayed, ui->lcdNumberTotalGames, text, [this](QVariant text){
        updateLukoMeter();
        return text;
    });

    CONNECT_PROP_TRANSFORM(PlayerStatistics, this, gamesWon, ui->lcdNumberWonGames, text, [this](QVariant text){
        updateLukoMeter();
        return text;
    });

    CONNECT_PROP_TRANSFORM(PlayerStatistics, this, image, ui->labelMiniPicture, pixmap, [this](QVariant image) {
        return QPixmap(image.toString());
    });

    CONNECT_PROP_TRANSFORM(PlayerStatistics, this, image, ui->labelMainPicture, pixmap, [this](QVariant image) {
        return QPixmap(image.toString());
    });

    CONNECT_PROP_TRANSFORM(PlayerStatistics, this, fullSize, ui->widgetPicture, visible, [this](QVariant enable) {
        ui->labelMainPicture->setVisible(!ui->widgetPlayerInformation->isVisible());
        return enable.toBool();
    });
}

PlayerStatistics::~PlayerStatistics()
{
    delete ui;
}


void PlayerStatistics::updateLukoMeter()
{
    if(this->gamesPlayed() == 0){
        ui->progressBarLukoMeter->setValue(ui->progressBarLukoMeter->maximum());
    } else if(this->gamesWon()/this->gamesPlayed() <= ui->progressBarLukoMeter->maximum()){
        ui->progressBarLukoMeter->setValue(this->gamesWon()/this->gamesPlayed());
    } else {
        ui->progressBarLukoMeter->setValue(0);
    }
}

void PlayerStatistics::changeLayoutOrder(LayoutOrder order)
{
    // Supprimer les widgets existants du layout sans les détruire
    ui->gridLayout->removeWidget(ui->widgetPicture);
    ui->gridLayout->removeWidget(ui->widgetPlayerInformation);
    ui->gridLayout->removeItem(ui->commandLayout);

    // Réorganiser les widgets selon l'ordre spécifié
    switch (order) {
    case DefaultOrder:
        ui->gridLayout->addLayout(ui->commandLayout, 0, 1);
        ui->gridLayout->addWidget(ui->widgetPlayerInformation, 1, 1);
        ui->gridLayout->addWidget(ui->widgetPicture, 2, 1);
        break;

    case AlternateOrder:
        ui->gridLayout->addWidget(ui->widgetPicture, 0, 1);
        ui->gridLayout->addWidget(ui->widgetPlayerInformation, 1, 1);
        ui->gridLayout->addLayout(ui->commandLayout, 2, 1);
        break;
    }
}



QString PlayerStatistics::getStatusStyleSheet(const QString &color) const {
    return QString(
               "background-color: %1;"
               "border: 1px solid #e0e0e0;"
               "border-radius: 10px;"
               ).arg(color);
}

void PlayerStatistics::on_pushButtonHideInformations_clicked()
{
    ui->labelMainPicture->setVisible(fullSize());
}

