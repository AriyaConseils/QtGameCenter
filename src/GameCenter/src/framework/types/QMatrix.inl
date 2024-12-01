#include "QMatrix.h"

template<typename T>
QMatrix<T>::QMatrix(int rows, int cols, const T& defaultValue)
    : rows_(rows), cols_(cols), data_(rows, QVector<T>(cols, defaultValue)) {}

template<typename T>
QMatrix<T>::QMatrix(const QSize& size, const T& defaultValue)
    : QMatrix(size.height(), size.width(), defaultValue) {}

template<typename T>
const T& QMatrix<T>::at(int row, int col) const {
    ensureIndexIsValid(row, col);
    return data_[row][col];
}

template<typename T>
T& QMatrix<T>::at(int row, int col) {
    ensureIndexIsValid(row, col);
    return data_[row][col];
}

template<typename T>
QVector<T> QMatrix<T>::getRow(int row) const {
    if (row < 0 || row >= rows_) {
        throw std::out_of_range("Invalid row index");
    }
    return data_[row];
}

template<typename T>
QVector<T> QMatrix<T>::getColumn(int col) const {
    if (col < 0 || col >= cols_) {
        throw std::out_of_range("Invalid column index");
    }
    QVector<T> column(rows_);
    for (int row = 0; row < rows_; ++row) {
        column[row] = data_[row][col];
    }
    return column;
}

template<typename T>
const T& QMatrix<T>::operator()(int row, int col) const {
    return at(row, col);
}

template<typename T>
T& QMatrix<T>::operator()(int row, int col) {
    return at(row, col);
}

template<typename T>
QVector<T>& QMatrix<T>::operator[](int row) {
    if (row < 0 || row >= rows_) {
        throw std::out_of_range("Row index out of range");
    }
    return data_[row];
}

template<typename T>
const QVector<T>& QMatrix<T>::operator[](int row) const {
    if (row < 0 || row >= rows_) {
        throw std::out_of_range("Row index out of range");
    }
    return data_[row];
}

template<typename T>
bool QMatrix<T>::operator==(const QMatrix<T>& other) const {
    return rows_ == other.rows_ && cols_ == other.cols_ && data_ == other.data_;
}

template<typename T>
QMatrix<T> QMatrix<T>::operator+(const QMatrix<T>& other) const {
    if (rows_ != other.rows_ || cols_ != other.cols_) {
        throw std::invalid_argument("Matrix dimensions must match for addition");
    }
    QMatrix<T> result(rows_, cols_);
    for (int row = 0; row < rows_; ++row) {
        for (int col = 0; col < cols_; ++col) {
            result.at(row, col) = this->at(row, col) + other.at(row, col);
        }
    }
    return result;
}

template<typename T>
QMatrix<T> QMatrix<T>::operator-(const QMatrix<T>& other) const {
    if (rows_ != other.rows_ || cols_ != other.cols_) {
        throw std::invalid_argument("Matrix dimensions must match for subtraction");
    }
    QMatrix<T> result(rows_, cols_);
    for (int row = 0; row < rows_; ++row) {
        for (int col = 0; col < cols_; ++col) {
            result.at(row, col) = this->at(row, col) - other.at(row, col);
        }
    }
    return result;
}

template<typename T>
QMatrix<T> QMatrix<T>::operator*(const QMatrix<T>& other) const {
    if (cols_ != other.rows_) {
        throw std::invalid_argument("Matrix dimensions must be compatible for multiplication");
    }
    QMatrix<T> result(rows_, other.cols_);
    for (int row = 0; row < rows_; ++row) {
        for (int col = 0; col < other.cols_; ++col) {
            T sum = T();
            for (int k = 0; k < cols_; ++k) {
                sum += this->at(row, k) * other.at(k, col);
            }
            result.at(row, col) = sum;
        }
    }
    return result;
}

template<typename T>
QVector<T> QMatrix<T>::operator*(const QVector<T>& vec) const {
    if (cols_ != vec.size()) {
        throw std::invalid_argument("Vector size must match the number of columns in the matrix");
    }
    QVector<T> result(rows_, T());
    for (int row = 0; row < rows_; ++row) {
        for (int col = 0; col < cols_; ++col) {
            result[row] += this->at(row, col) * vec[col];
        }
    }
    return result;
}

template<typename T>
void QMatrix<T>::resize(int rows, int cols, const T& defaultValue) {
    data_.resize(rows);
    for (auto& row : data_) {
        row.resize(cols);
        std::fill(row.begin(), row.end(), defaultValue);
    }
    rows_ = rows;
    cols_ = cols;
}

template<typename T>
void QMatrix<T>::clear(const T& defaultValue) {
    for (auto& row : data_) {
        std::fill(row.begin(), row.end(), defaultValue);
    }
}

template<typename T>
int QMatrix<T>::rows() const {
    return rows_;
}

template<typename T>
int QMatrix<T>::cols() const {
    return cols_;
}

template<typename T>
bool QMatrix<T>::isValid(int row, int col) const {
    return row >= 0 && row < rows_ && col >= 0 && col < cols_;
}

template<typename T>
void QMatrix<T>::ensureIndexIsValid(int row, int col) const {
    if (!isValid(row, col)) {
        throw std::out_of_range("Matrix index out of range");
    }
}
