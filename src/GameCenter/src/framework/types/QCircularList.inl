#include "QCircularList.h"

template <typename T>
QCircularList<T>::QCircularList() : currentIndex_(-1) {}

template <typename T>
void QCircularList<T>::append(const T& value) {
    list_.append(value);
    if (list_.size() == 1) {
        currentIndex_ = 0;
    }
}

template <typename T>
T QCircularList<T>::next() {
    ensureNotEmpty();
    currentIndex_ = (currentIndex_ + 1) % list_.size();
    return list_.at(currentIndex_);
}

template <typename T>
T QCircularList<T>::current() const {
    ensureNotEmpty();
    return list_.at(currentIndex_);
}

template <typename T>
T QCircularList<T>::peekNext() const {
    ensureNotEmpty();
    int nextIndex = (currentIndex_ + 1) % list_.size();
    return list_.at(nextIndex);
}

template <typename T>
T QCircularList<T>::peekPrevious() const {
    ensureNotEmpty();
    int prevIndex = (currentIndex_ - 1 + list_.size()) % list_.size();
    return list_.at(prevIndex);
}

template <typename T>
bool QCircularList<T>::contains(const T& value) const {
    return list_.contains(value);
}

template <typename T>
void QCircularList<T>::remove(const T& value) {
    ensureNotEmpty();
    int indexToRemove = list_.indexOf(value);
    if (indexToRemove == -1) {
        return;
    }
    list_.removeAt(indexToRemove);
    if (list_.isEmpty()) {
        currentIndex_ = -1;
    } else if (indexToRemove <= currentIndex_) {
        currentIndex_ = (currentIndex_ - 1 + list_.size()) % list_.size();
    }
}

template <typename T>
T QCircularList<T>::at(int index) const {
    if (index < 0 || index >= list_.size()) {
        throw std::out_of_range("Index out of range");
    }
    return list_.at(index);
}

template <typename T>
void QCircularList<T>::clear() {
    list_.clear();
    currentIndex_ = -1;
}

template <typename T>
int QCircularList<T>::size() const {
    return list_.size();
}

template <typename T>
void QCircularList<T>::ensureNotEmpty() const {
    if (list_.isEmpty()) {
        throw std::out_of_range("QCircularList is empty");
    }
}
