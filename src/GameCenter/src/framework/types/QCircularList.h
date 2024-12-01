#ifndef QCIRCULARLIST_H
#define QCIRCULARLIST_H

#include <QList>
#include <QDebug>
#include <stdexcept>

/**
 * @class QCircularList
 * @brief A template class implementing a circular linked list.
 *
 * This class provides a circular list where the current index loops back to the start
 * after reaching the end. It supports basic operations like adding, removing, and navigating elements.
 *
 * @tparam T The type of elements in the circular list.
 */
template <typename T>
class QCircularList {
public:
    /**
     * @brief Default constructor.
     */
    QCircularList();

    /**
     * @brief Adds an element to the list.
     * @param value The value to append.
     */
    void append(const T& value);

    /**
     * @brief Moves to the next element in the list in a circular manner.
     * @return The next element.
     * @throws std::out_of_range If the list is empty.
     */
    T next();

    /**
     * @brief Returns the current element without changing the current index.
     * @return The current element.
     * @throws std::out_of_range If the list is empty.
     */
    T current() const;

    /**
     * @brief Peeks at the next element without changing the current index.
     * @return The next element.
     * @throws std::out_of_range If the list is empty.
     */
    T peekNext() const;

    /**
     * @brief Peeks at the previous element without changing the current index.
     * @return The previous element.
     * @throws std::out_of_range If the list is empty.
     */
    T peekPrevious() const;

    /**
     * @brief Checks if the list contains a specific value.
     * @param value The value to check for.
     * @return True if the value exists, false otherwise.
     */
    bool contains(const T& value) const;

    /**
     * @brief Removes a specific value from the list.
     * @param value The value to remove.
     * If the value does not exist, the method does nothing.
     */
    void remove(const T& value);

    /**
     * @brief Returns the element at a specific index.
     * @param index The index to access.
     * @return The element at the specified index.
     * @throws std::out_of_range If the index is out of range.
     */
    T at(int index) const;

    /**
     * @brief Clears all elements from the list.
     */
    void clear();

    /**
     * @brief Returns the size of the list.
     * @return The number of elements in the list.
     */
    int size() const;

    /**
     * @brief Overloaded << operator for debugging and displaying the list.
     * @param dbg The QDebug stream.
     * @param list The QCircularList to display.
     * @return The modified QDebug stream.
     */
    friend QDebug operator<<(QDebug dbg, const QCircularList<T>& list) {
        dbg.nospace() << "QCircularList: { ";
        for (int i = 0; i < list.list_.size(); ++i) {
            if (i == list.currentIndex_) {
                dbg.nospace() << "[" << list.list_.at(i) << "] ";
            } else {
                dbg.nospace() << list.list_.at(i) << " ";
            }
        }
        dbg.nospace() << "}";
        return dbg.maybeSpace();
    }

private:
    QList<T> list_;    ///< Underlying list storing the elements.
    int currentIndex_; ///< Index of the current element.

    /**
     * @brief Ensures that the list is not empty.
     * @throws std::out_of_range If the list is empty.
     */
    void ensureNotEmpty() const;
};

#include "QCircularList.inl"
#endif // QCIRCULARLIST_H
