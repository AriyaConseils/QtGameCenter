#include "TreeItem.h"
#include <QDebug>

TreeItem::TreeItem(const QVariantList &data, TreeItem *parent)
    : m_itemData(data), m_parentItem(parent) {}

TreeItem::~TreeItem() {
    clearChildren();
}

void TreeItem::appendChild(std::unique_ptr<TreeItem> &&child) {
    if (child) {
        m_childItems.push_back(std::move(child));  // Utilisation de push_back et move
    }
}

TreeItem* TreeItem::child(int row) const {
    if (row >= 0 && row < static_cast<int>(m_childItems.size())) {
        return m_childItems.at(row).get();  // Retourne un pointeur brut vers l'enfant
    }
    return nullptr;
}

int TreeItem::childCount() const {
    return static_cast<int>(m_childItems.size());  // Retourne le nombre d'enfants
}


int TreeItem::columnCount() const {
    return m_itemData.count();
}

QVariant TreeItem::data(int column) const {
    if (column < 0 || column >= m_itemData.count()) {
        return QVariant();
    }
    return m_itemData.at(column);
}

TreeItem* TreeItem::parentItem() const {
    return m_parentItem;
}

int TreeItem::row() const {
    if (m_parentItem) {
        for (int i = 0; i < m_parentItem->childCount(); ++i) {
            if (m_parentItem->child(i) == this) {
                return i;
            }
        }
    }
    return 0;  // Si ce n'est pas dans un parent, on renvoie 0 par défaut
}

void TreeItem::clearChildren() {
    m_childItems.clear();  // La mémoire est automatiquement libérée grâce à unique_ptr
}

void TreeItem::removeChild(int row) {
    if (row >= 0 && row < static_cast<int>(m_childItems.size())) {
        m_childItems.erase(m_childItems.begin() + row);
    }
}

