#ifndef PLAYER_H
#define PLAYER_H

#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include "framework/helpers.h"
#include <QUrl>

/**
 * @namespace PlayerEnums
 * @brief Namespace containing enumerations related to the Player class.
 */
namespace PlayerEnums {
/**
     * @enum PlayerPosition
     * @brief Represents possible positions of players.
     */
enum PlayerPosition {
    Player1 = 0, ///< The first player
    Player2,     ///< The second player
    Player3,     ///< The third player
    Player4,     ///< The fourth player
    Player5,     ///< The fifth player
    Unknown      ///< Undefined or unknown position
};
}


class Player : public QObject{
    Q_OBJECT
    DECLARE_PROPERTY(PlayerEnums::PlayerPosition, playerPosition, setPlayerPosition, PlayerEnums::Unknown)
    DECLARE_PROPERTY(QString, playerId, setPlayerId, QStringLiteral("Unknown"))
    DECLARE_PROPERTY_CB(QString, lastName, setLastName, QStringLiteral("Unknown")) {
        Q_UNUSED(_lastName)
        emit statsChanged();
    }
    DECLARE_PROPERTY_CB(QString, firstName, setFirstName, QStringLiteral("Unknown")) {
        Q_UNUSED(_firstName)
        emit statsChanged();
    }
    DECLARE_PROPERTY_CB(int, gamesPlayed, setGamesPlayed, 0) {
        Q_UNUSED(_gamesPlayed)
        emit statsChanged();
    }
    DECLARE_PROPERTY_CB(int, gamesWon, setGamesWon, 0){
        Q_UNUSED(_gamesWon)
        emit statsChanged();
    }

    DECLARE_PROPERTY(QString, image, setImage, QStringLiteral(""))
    DECLARE_PROPERTY(bool, online, setOnline, false)

    Q_PROPERTY(QString stats READ getStats NOTIFY statsChanged)

public:



    // Constructeur par défaut
    Player(QObject* parent = nullptr)
        : QObject(parent)
    {

    }
    // Constructeur avec paramètres
    Player(const QString& playerId)
        : m_playerId(playerId), m_firstName(QStringLiteral("Unknown")), m_lastName(QStringLiteral("Unknown")), m_gamesPlayed(0), m_gamesWon(0), m_playerPosition(PlayerEnums::Unknown){}

    // Constructeur avec paramètres
    Player(const QString& firstName, const QString& lastName)
        : m_firstName(firstName), m_lastName(lastName), m_gamesPlayed(0), m_gamesWon(0), m_playerPosition(PlayerEnums::Unknown){}

    // Constructeur de copie
    Player(const Player& other) {
        m_playerId = other.m_playerId;       // Copie explicite du prénom
        m_firstName = other.m_firstName;       // Copie explicite du prénom
        m_lastName = other.m_lastName;         // Copie explicite du nom de famille
        m_gamesPlayed = other.m_gamesPlayed;   // Copie explicite du nombre de parties jouées
        m_gamesWon = other.m_gamesWon;         // Copie explicite du nombre de parties gagnées
        m_playerPosition = other.m_playerPosition; // Copie explicite de la position du joueur
        m_image = other.m_image;
        m_online = other.m_online;
    }

    // Opérateur d'affectation (copie)
    Player& operator=(const Player& other) {
        if (this != &other) {
            m_playerId = other.m_playerId;
            emit playerIdChanged();

            m_firstName = other.m_firstName;
            emit firstNameChanged();

            m_lastName = other.m_lastName;
            emit lastNameChanged();

            m_gamesPlayed = other.m_gamesPlayed;
            emit gamesPlayedChanged();

            m_gamesWon = other.m_gamesWon;
            emit gamesWonChanged();

            m_playerPosition = other.m_playerPosition;
            emit playerPositionChanged();

            m_image = other.m_image;
            emit imageChanged();
        }
        return *this;
    }

    // Constructeur de déplacement
    Player(Player&& other) noexcept
        : m_firstName(std::move(other.m_firstName)), m_lastName(std::move(other.m_lastName)), m_playerId(std::move(other.m_playerId)),
        m_gamesPlayed(other.m_gamesPlayed), m_gamesWon(other.m_gamesWon), m_playerPosition(other.m_playerPosition){
        other.m_gamesPlayed = 0;
        other.m_gamesWon = 0;
        m_image = other.m_image;
        m_online = other.m_online;
    }


    // Opérateur d'affectation (déplacement)
    Player& operator=(Player&& other) noexcept {
        if (this != &other) {
            m_playerId = std::move(other.m_playerId);
            emit playerIdChanged();

            m_firstName = std::move(other.m_firstName);
            emit firstNameChanged();

            m_lastName = std::move(other.m_lastName);
            emit lastNameChanged();

            m_gamesPlayed = other.m_gamesPlayed;
            emit gamesPlayedChanged();

            m_gamesWon = other.m_gamesWon;
            emit gamesWonChanged();

            m_playerPosition = other.m_playerPosition;
            emit playerPositionChanged();

            m_image = std::move(other.m_image);
            emit imageChanged();

            other.m_gamesPlayed = 0;
            other.m_gamesWon = 0;
        }
        return *this;
    }

    // Accesseurs pour le nom
    // Accesseurs pour le nombre de parties jouées
    void incrementGamesPlayed() { setGamesPlayed(gamesPlayed() + 1); }
    void incrementGamesWon() { setGamesWon(gamesWon() + 1); }


    // Retourne le ratio de victoire
    double getWinRate() const {
        if (m_gamesPlayed == 0) {
            return 0.0;
        }
        return static_cast<double>(m_gamesWon) / m_gamesPlayed;
    }

    // Méthode pour récupérer les statistiques sous forme de texte
    QString getStats() const {
        return QString("Player: %1 %2\nGames Played: %3\nGames Won: %4\nWin Rate: %5%")
            .arg(m_firstName)
            .arg(m_lastName)
            .arg(m_gamesPlayed)
            .arg(m_gamesWon)
            .arg(getWinRate() * 100.0, 0, 'f', 2);
    }

    // Sérialise les données du joueur dans un QJsonObject
    QJsonObject toJson() const {
        QJsonObject json;
        json["playerId"] = m_playerId;
        json["firstName"] = m_firstName;
        json["lastName"] = m_lastName;
        json["gamesPlayed"] = m_gamesPlayed;
        json["gamesWon"] = m_gamesWon;
        json["playerPosition"] = static_cast<int>(m_playerPosition);
        json["image"] = m_image;
        json["online"] = m_online;
        return json;
    }


    // Désérialise les données d'un joueur à partir d'un QJsonObject
    static Player fromJson(const QJsonObject& json) {
        Player player;
        player.setPlayerId(json.value("playerId").toString(QStringLiteral("Unknown")));
        player.setFirstName(json.value("firstName").toString(QStringLiteral("Unknown")));
        player.setLastName(json.value("lastName").toString(QStringLiteral("Unknown")));
        player.setGamesPlayed(json.value("gamesPlayed").toInt(0));
        player.setGamesWon(json.value("gamesWon").toInt(0));
        player.setPlayerPosition(static_cast<PlayerEnums::PlayerPosition>(json.value("playerPosition").toInt(static_cast<int>(PlayerEnums::Unknown))));
        player.setImage(json.value("image").toString(""));
        player.setOnline(json.value("online").toBool(false));
        if(player.playerId() == QStringLiteral("Unknown")){
            player.setPlayerId(json.value("firstName").toString()+json.value("lastName").toString());
        }
        return player;
    }

    // Surcharge de l'opérateur == pour comparer deux joueurs
    bool operator==(const Player& other) const {
        return m_playerId == other.m_playerId &&
               m_firstName == other.m_firstName &&
               m_lastName == other.m_lastName &&
               m_gamesPlayed == other.m_gamesPlayed &&
               m_gamesWon == other.m_gamesWon;
    }

    // Surcharge de l'opérateur != pour comparer deux joueurs
    bool operator!=(const Player& other) const {
        return !(*this == other);
    }

    // Surcharge de l'opérateur << pour le stream debug
    friend QDebug operator<<(QDebug dbg, const Player& player) {
        dbg.nospace() << "Player(" << player.m_firstName << " " << player.m_lastName
                      << ", Games Played: " << player.m_gamesPlayed
                      << ", Games Won: " << player.m_gamesWon
                      << ", Win Rate: " << player.getWinRate() * 100.0 << "%)";
        return dbg.maybeSpace();
    }

signals:
    void playerPositionChanged();
    void lastNameChanged();
    void firstNameChanged();
    void gamesPlayedChanged();
    void gamesWonChanged();
    void imageChanged();
    void onlineChanged();
    void playerIdChanged();

    void statsChanged();

private:

};


#endif // PLAYER_H
