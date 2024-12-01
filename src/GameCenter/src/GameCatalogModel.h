#include <QAbstractListModel>
#include <functional>
#include <memory>
#include <QVariant>

#include "games/TicTacToeGames.h"
#include "games/GoGames.h"
#include "games/CheckersGame.h"

// Typedef pour les constructeurs de jeux
using GameFactory = std::function<std::unique_ptr<AbstractTableGame>()>;

class GameCatalogModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit GameCatalogModel(QObject *parent = nullptr) : QAbstractListModel(parent) {
        initializeGames();
    }

    /**
     * @brief Adds a new game to the catalog.
     * @param name The name of the game (for display).
     * @param factory A function to create instances of the game.
     */
    void addGame(const QString &name, GameFactory factory) {
        beginInsertRows(QModelIndex(), m_games.size(), m_games.size());
        m_games.append({name, factory});
        endInsertRows();
    }

    /**
     * @brief Returns the number of rows in the model.
     * @param parent The parent index (unused).
     * @return The total number of games in the catalog.
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return m_games.size();
    }

    /**
     * @brief Provides data for views.
     * @param index The index of the item.
     * @param role The role of the requested data (DisplayRole or UserRole).
     * @return The requested data or an empty QVariant if invalid.
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= m_games.size())
            return {};

        const auto &game = m_games[index.row()];
        if (role == Qt::DisplayRole) {
            return game.name; // Display game name
        } else if (role == Qt::UserRole) {
            // Store the factory function in UserRole
            return QVariant::fromValue<GameFactory>(game.factory);
        }
        return {};
    }

private:
    struct Game {
        QString name;
        GameFactory factory;
    };

    QList<Game> m_games;

    void initializeGames() {
        addGame("Go", []() { return std::make_unique<GoGames>(); });
        addGame("Tic Tac Toe", []() { return std::make_unique<TicTacToeGames>(); });
        addGame("Checkers", []() { return std::make_unique<CheckersGame>(); });
    }
};
