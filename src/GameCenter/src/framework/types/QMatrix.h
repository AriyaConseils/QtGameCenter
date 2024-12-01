#ifndef QMATRIX_H
#define QMATRIX_H

#include <QVector>
#include <stdexcept>
#include <QSize>

/**
 * @brief A generic matrix class implemented using `QVector`.
 *
 * This class provides various operations and utilities for matrix manipulation,
 * including resizing, element access, and mathematical operations like addition,
 * subtraction, and multiplication.
 *
 * @tparam T The type of elements stored in the matrix.
 */
template<typename T>
class QMatrix {
public:
    /**
     * @brief Default constructor for the matrix.
     *
     * @param rows Number of rows in the matrix (default is 0).
     * @param cols Number of columns in the matrix (default is 0).
     * @param defaultValue The default value to initialize the matrix elements with.
     */
    QMatrix(int rows = 0, int cols = 0, const T& defaultValue = T());

    /**
     * @brief Constructor using a QSize object.
     *
     * @param size The size of the matrix (rows and columns).
     * @param defaultValue The default value to initialize the matrix elements with.
     */
    QMatrix(const QSize& size, const T& defaultValue = T());

    /**
     * @brief Access an element at a specific position (read-only).
     *
     * @param row The row index of the element.
     * @param col The column index of the element.
     * @return The value at the specified position.
     * @throws std::out_of_range If the specified position is invalid.
     */
    const T& at(int row, int col) const;

    /**
     * @brief Access an element at a specific position (read/write).
     *
     * @param row The row index of the element.
     * @param col The column index of the element.
     * @return A reference to the value at the specified position.
     * @throws std::out_of_range If the specified position is invalid.
     */
    T& at(int row, int col);

    /**
     * @brief Get a specific row from the matrix.
     *
     * @param row The index of the row to retrieve.
     * @return A QVector representing the row.
     * @throws std::out_of_range If the specified row index is invalid.
     */
    QVector<T> getRow(int row) const;

    /**
     * @brief Get a specific column from the matrix.
     *
     * @param col The index of the column to retrieve.
     * @return A QVector representing the column.
     * @throws std::out_of_range If the specified column index is invalid.
     */
    QVector<T> getColumn(int col) const;

    /**
     * @brief Resize the matrix to new dimensions.
     *
     * @param rows The new number of rows.
     * @param cols The new number of columns.
     * @param defaultValue The value to initialize new elements with.
     */
    void resize(int rows, int cols, const T& defaultValue = T());

    /**
     * @brief Clear the matrix and reset all elements to a default value.
     *
     * @param defaultValue The value to reset all elements to.
     */
    void clear(const T& defaultValue = T());

    /**
     * @brief Get the number of rows in the matrix.
     *
     * @return The number of rows.
     */
    int rows() const;

    /**
     * @brief Get the number of columns in the matrix.
     *
     * @return The number of columns.
     */
    int cols() const;

    /**
     * @brief Check if a given position is valid within the matrix.
     *
     * @param row The row index to check.
     * @param col The column index to check.
     * @return True if the position is valid, otherwise false.
     */
    bool isValid(int row, int col) const;

    /**
     * @brief Access an element at a specific position using parentheses (read-only).
     *
     * @param row The row index of the element.
     * @param col The column index of the element.
     * @return The value at the specified position.
     */
    const T& operator()(int row, int col) const;

    /**
     * @brief Access an element at a specific position using parentheses (read/write).
     *
     * @param row The row index of the element.
     * @param col The column index of the element.
     * @return A reference to the value at the specified position.
     */
    T& operator()(int row, int col);

    /**
     * @brief Access a row of the matrix using the index operator (read/write).
     *
     * @param row The index of the row to access.
     * @return A reference to the QVector representing the row.
     * @throws std::out_of_range If the specified row index is invalid.
     */
    QVector<T>& operator[](int row);

    /**
     * @brief Access a row of the matrix using the index operator (read-only).
     *
     * @param row The index of the row to access.
     * @return A reference to the QVector representing the row.
     * @throws std::out_of_range If the specified row index is invalid.
     */
    const QVector<T>& operator[](int row) const;

    /**
     * @brief Compare two matrices for equality.
     *
     * @param other The matrix to compare with.
     * @return True if the matrices are equal, otherwise false.
     */
    bool operator==(const QMatrix<T>& other) const;

    /**
     * @brief Add two matrices.
     *
     * @param other The matrix to add.
     * @return A new matrix representing the result of the addition.
     * @throws std::invalid_argument If the dimensions of the matrices do not match.
     */
    QMatrix<T> operator+(const QMatrix<T>& other) const;

    /**
     * @brief Subtract one matrix from another.
     *
     * @param other The matrix to subtract.
     * @return A new matrix representing the result of the subtraction.
     * @throws std::invalid_argument If the dimensions of the matrices do not match.
     */
    QMatrix<T> operator-(const QMatrix<T>& other) const;

    /**
     * @brief Perform matrix multiplication.
     *
     * @param other The matrix to multiply with.
     * @return A new matrix representing the result of the multiplication.
     * @throws std::invalid_argument If the dimensions of the matrices are incompatible.
     */
    QMatrix<T> operator*(const QMatrix<T>& other) const;

    /**
     * @brief Perform matrix-vector multiplication.
     *
     * @param vec The vector to multiply with.
     * @return A QVector representing the result of the multiplication.
     * @throws std::invalid_argument If the vector size does not match the number of columns.
     */
    QVector<T> operator*(const QVector<T>& vec) const;

private:
    int rows_;                    ///< Number of rows in the matrix.
    int cols_;                    ///< Number of columns in the matrix.
    QVector<QVector<T>> data_;    ///< Data storage for the matrix.

    /**
     * @brief Ensure that a given index is valid within the matrix.
     *
     * @param row The row index to validate.
     * @param col The column index to validate.
     * @throws std::out_of_range If the index is invalid.
     */
    void ensureIndexIsValid(int row, int col) const;
};

// Include implementation
#include "QMatrix.inl"

#endif // QMATRIX_H
