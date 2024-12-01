#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVariant>
#include <QList>
#include <memory>

class TreeItem
{
public:
    explicit TreeItem(const QVariantList &data, TreeItem *parent = nullptr);
    ~TreeItem();

    void appendChild(std::unique_ptr<TreeItem> &&child);
    TreeItem* child(int row) const;
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    TreeItem* parentItem() const;
    int row() const;
    void clearChildren();

    void setItemData(const QVariantList &data) {
        m_itemData = data;
    }


    void removeChild(int row);

private:
    QVariantList m_itemData;
    TreeItem* m_parentItem;
    std::vector<std::unique_ptr<TreeItem>> m_childItems;
};

#endif // TREEITEM_H
