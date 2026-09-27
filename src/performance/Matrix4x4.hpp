#pragma once

/**
 * Matrix4x4 - SIMD-optimized 4x4 matrix operations
 * 
 * Provides optimized 4x4 matrix operations using SIMD instructions
 * for high-performance neural computations. Designed for recurrent
 * neural networks, coordinate transformations, and neural population updates.
 * 
 * Features:
 * - SIMD-accelerated operations (SSE/AVX/AVX-512)
 * - Cache-aligned memory layout
 * - Vectorized operations
 * - Type-safe interface
 * - Automatic fallback to scalar operations
 * 
 * Biological motivation:
 * - Neural population dynamics can be represented as 4x4 matrices
 * - Recurrent connections use matrix multiplication
 * - Coordinate transformations for spatial neural networks
 */

#include <cstddef>
#include <cstdint>
#include <cmath>
#include <array>
#include <type_traits>
#include <algorithm>
#include "SIMDVectorization.hpp"

namespace nlm {

/**
 * Matrix4x4 - 4x4 matrix with SIMD optimizations
 * 
 * Template parameter T specifies the scalar type (float or double).
 * Uses SIMD instructions for vectorized operations when possible.
 */
template<typename T>
class Matrix4x4 {
public:
    static constexpr size_t rows = 4;
    static constexpr size_t cols = 4;
    static constexpr size_t size = rows * cols;
    
    using ScalarType = T;
    using SIMDType = typename SIMDVectorization<T>::SIMDType;
    using VectorType = SIMDVectorization<T>;

private:
    alignas(64) std::array<T, size> data_;
    mutable bool dirty_ = true;

public:
    /**
     * Default constructor - creates identity matrix
     */
    Matrix4x4() {
        setIdentity();
    }

    /**
     * Constructor from array
     */
    explicit Matrix4x4(const T* data) {
        std::copy(data, data + size, data_.begin());
    }

    /**
     * Constructor from array of rows
     */
    Matrix4x4(const T (&matrix)[4][4]) {
        for (size_t i = 0; i < rows; ++i) {
            std::copy(matrix[i], matrix[i] + cols, data_.begin() + i * cols);
        }
    }

    /**
     * Access element at (row, col)
     */
    T& operator()(size_t row, size_t col) {
        dirty_ = true;
        return data_[row * cols + col];
    }

    /**
     * Access element at (row, col) - const version
     */
    const T& operator()(size_t row, size_t col) const {
        return data_[row * cols + col];
    }

    /**
     * Access element at index
     */
    T& operator[](size_t index) {
        dirty_ = true;
        return data_[index];
    }

    /**
     * Access element at index - const version
     */
    const T& operator[](size_t index) const {
        return data_[index];
    }

    /**
     * Set matrix from array
     */
    void set(const T* data) {
        std::copy(data, data + size, data_.begin());
        dirty_ = true;
    }

    /**
     * Get pointer to internal data
     */
    const T* data() const { return data_.data(); }

    /**
     * Get mutable pointer to internal data
     */
    T* data() { dirty_ = true; return data_.data(); }

    /**
     * Set to identity matrix
     */
    void setIdentity() {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                (*this)(i, j) = (i == j) ? static_cast<T>(1.0) : static_cast<T>(0.0);
            }
        }
    }

    /**
     * Set to zero matrix
     */
    void setZero() {
        std::fill(data_.begin(), data_.end(), static_cast<T>(0.0));
    }

    /**
     * Transpose the matrix
     */
    Matrix4x4 transpose() const {
        Matrix4x4 result;
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result(i, j) = (*this)(j, i);
            }
        }
        return result;
    }

    /**
     * Multiply by scalar
     */
    Matrix4x4 multiplyScalar(T scalar) const {
        Matrix4x4 result;
        VectorType::vectorMultiplyScalar(result.data(), data_.data(), scalar, size);
        return result;
    }

    /**
     * Add another matrix
     */
    Matrix4x4 add(const Matrix4x4& other) const {
        Matrix4x4 result;
        VectorType::vectorAdd(result.data(), data_.data(), other.data(), size);
        return result;
    }

    /**
     * Subtract another matrix
     */
    Matrix4x4 subtract(const Matrix4x4& other) const {
        Matrix4x4 result;
        VectorType::vectorSubtract(result.data(), data_.data(), other.data(), size);
        return result;
    }

    /**
     * Multiply by another matrix (matrix multiplication)
     */
    Matrix4x4 multiply(const Matrix4x4& other) const {
        Matrix4x4 result;
        Matrix4x4::matrixMultiply(result.data(), data_, other.data_);
        return result;
    }

    /**
     * Multiply by vector (4D)
     */
    std::array<T, 4> multiply(const std::array<T, 4>& vec) const {
        std::array<T, 4> result;
        Matrix4x4::matrixVectorMultiply(result.data(), data_, vec.data());
        return result;
    }

    /**
     * Check if matrix is identity
     */
    bool isIdentity() const {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                T expected = (i == j) ? static_cast<T>(1.0) : static_cast<T>(0.0);
                if (std::abs((*this)(i, j) - expected) > static_cast<T>(1e-6)) {
                    return false;
                }
            }
        }
        return true;
    }

    /**
     * Check if matrix is symmetric
     */
    bool isSymmetric() const {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = i + 1; j < cols; ++j) {
                if (std::abs((*this)(i, j) - (*this)(j, i)) > static_cast<T>(1e-6)) {
                    return false;
                }
            }
        }
        return true;
    }

    /**
     * Compute determinant
     */
    T determinant() const {
        // Simplified 4x4 determinant calculation using cofactor expansion
        T det = static_cast<T>(0.0);
        
        // First row expansion
        for (size_t col = 0; col < cols; ++col) {
            Matrix4x4<T> submatrix;
            size_t subIndex = 0;
            
            // Build 3x3 submatrix excluding first row and current column
            for (size_t i = 1; i < rows; ++i) {
                size_t colIndex = 0;
                for (size_t j = 0; j < cols; ++j) {
                    if (j != col) {
                        submatrix(subIndex, colIndex++) = (*this)(i, j);
                    }
                }
                subIndex++;
            }
            
            T cofactor = submatrix.determinant3x3();
            if (col % 2 == 0) {
                det += (*this)(0, col) * cofactor;
            } else {
                det -= (*this)(0, col) * cofactor;
            }
        }
        
        return det;
    }

    /**
     * Compute 3x3 determinant (helper function)
     */
    T determinant3x3() const {
        // Compute determinant of 3x3 submatrix
        // Assuming matrix is at least 3x3
        T det = static_cast<T>(0.0);
        
        // First row expansion
        for (size_t col = 0; col < 3; ++col) {
            Matrix4x4<T> submatrix;
            size_t subIndex = 0;
            
            // Build 2x2 submatrix
            for (size_t i = 0; i < 3; ++i) {
                size_t colIndex = 0;
                for (size_t j = 0; j < 3; ++j) {
                    if (j != col) {
                        submatrix(subIndex, colIndex++) = (*this)(i + 1, j);
                    }
                }
                subIndex++;
            }
            
            T cofactor = submatrix.determinant2x2();
            if (col % 2 == 0) {
                det += (*this)(1, col) * cofactor;
            } else {
                det -= (*this)(1, col) * cofactor;
            }
        }
        
        return det;
    }

    /**
     * Compute 2x2 determinant (helper function)
     */
    T determinant2x2() const {
        return (*this)(0, 0) * (*this)(1, 1) - (*this)(0, 1) * (*this)(1, 0);
    }

    /**
     * Invert the matrix
     */
    Matrix4x4 invert() const {
        Matrix4x4 result;
        Matrix4x4::matrixInvert(result.data(), data_);
        return result;
    }

    /**
     * Compute matrix trace
     */
    T trace() const {
        T trace = static_cast<T>(0.0);
        for (size_t i = 0; i < std::min(rows, cols); ++i) {
            trace += (*this)(i, i);
        }
        return trace;
    }

    /**
     * Compute matrix Frobenius norm
     */
    T norm() const {
        T sum = static_cast<T>(0.0);
        for (size_t i = 0; i < size; ++i) {
            sum += data_[i] * data_[i];
        }
        return std::sqrt(sum);
    }

    /**
     * Normalize matrix to unit Frobenius norm
     */
    Matrix4x4 normalized() const {
        T norm = this->norm();
        if (norm > static_cast<T>(0.0)) {
            return multiplyScalar(static_cast<T>(1.0) / norm);
        }
        return *this;
    }

    /**
     * Exponential of matrix (for neural dynamics)
     */
    Matrix4x4 exponential() const {
        Matrix4x4 result;
        Matrix4x4::matrixExponential(result.data(), data_);
        return result;
    }

    /**
     * Set to matrix exponential (in-place)
     */
    void setExponential() {
        Matrix4x4::matrixExponential(data_, data_);
    }

    /**
     * Check if matrix is valid (not NaN, not infinite)
     */
    bool isValid() const {
        for (size_t i = 0; i < size; ++i) {
            if (std::isnan(data_[i]) || std::isinf(data_[i])) {
                return false;
            }
        }
        return true;
    }

    /**
     * Print matrix to stream
     */
    void print(std::ostream& os) const {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                os << (*this)(i, j);
                if (j < cols - 1) os << " ";
            }
            if (i < rows - 1) os << "\n";
        }
    }

    /**
     * Static methods for optimized operations
     */
    static void matrixMultiply(T* result, const T* a, const T* b) {
        // Optimized matrix multiplication using SIMD
        VectorType::matrixMultiply4x4(result, a, b);
    }

    static void matrixVectorMultiply(T* result, const T* matrix, const T* vector) {
        // Optimized matrix-vector multiplication using SIMD
        VectorType::matrixVectorMultiply4x4(result, matrix, vector);
    }

    static void matrixInvert(T* result, const T* matrix) {
        // Optimized matrix inversion using LU decomposition
        VectorType::matrixInvert4x4(result, matrix);
    }

    static void matrixExponential(T* result, const T* matrix) {
        // Optimized matrix exponential using Taylor series
        VectorType::matrixExponential4x4(result, matrix);
    }

    /**
     * Get matrix as array of rows
     */
    std::array<std::array<T, cols>, rows> asArray() const {
        std::array<std::array<T, cols>, rows> result;
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result[i][j] = (*this)(i, j);
            }
        }
        return result;
    }

    /**
     * Set from array of rows
     */
    void fromArray(const std::array<std::array<T, cols>, rows>& matrix) {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                (*this)(i, j) = matrix[i][j];
            }
        }
    }

    /**
     * Apply to multiple matrices (batch operation)
     */
    static void batchMultiply(T* results, const T* matrices, size_t count,
                              const T* other) {
        // Batch matrix multiplication
        for (size_t i = 0; i < count; ++i) {
            Matrix4x4::matrixMultiply(
                results + i * size,
                matrices + i * size,
                other
            );
        }
    }

    /**
     * Apply to multiple matrices (batch exponential)
     */
    static void batchExponential(T* results, const T* matrices, size_t count) {
        // Batch matrix exponential
        for (size_t i = 0; i < count; ++i) {
            Matrix4x4::matrixExponential(
                results + i * size,
                matrices + i * size
            );
        }
    }

    /**
     * Multiply and accumulate (GEMM operation)
     */
    static void gemm(T* result, const T* a, const T* b, T alpha = static_cast<T>(1.0),
                     T beta = static_cast<T>(0.0)) {
        // General matrix multiplication with accumulation
        VectorType::gemm4x4(result, a, b, alpha, beta);
    }

    /**
     * Compute LU decomposition
     */
    static void luDecompose(T* lu, const T* a) {
        // LU decomposition with partial pivoting
        VectorType::luDecompose4x4(lu, a);
    }

    /**
     * Solve linear system Ax = b
     */
    static void solve(T* x, const T* a, const T* b) {
        // Solve linear system using LU decomposition
        VectorType::solveLinearSystem4x4(x, a, b);
    }

    /**
     * Compute eigenvalues and eigenvectors
     */
    static void eigen(T* eigenvalues, T* eigenvectors, const T* matrix) {
        // Compute eigenvalues and eigenvectors
        VectorType::eigen4x4(eigenvalues, eigenvectors, matrix);
    }

    /**
     * Rotate matrix around axis
     */
    static Matrix4x4 rotationX(T angle) {
        Matrix4x4 result;
        T c = std::cos(angle);
        T s = std::sin(angle);
        
        result(0, 0) = static_cast<T>(1.0);
        result(1, 1) = c;
        result(1, 2) = -s;
        result(2, 1) = s;
        result(2, 2) = c;
        result(3, 3) = static_cast<T>(1.0);
        
        return result;
    }

    static Matrix4x4 rotationY(T angle) {
        Matrix4x4 result;
        T c = std::cos(angle);
        T s = std::sin(angle);
        
        result(0, 0) = c;
        result(0, 2) = s;
        result(1, 1) = static_cast<T>(1.0);
        result(2, 0) = -s;
        result(2, 2) = c;
        result(3, 3) = static_cast<T>(1.0);
        
        return result;
    }

    static Matrix4x4 rotationZ(T angle) {
        Matrix4x4 result;
        T c = std::cos(angle);
        T s = std::sin(angle);
        
        result(0, 0) = c;
        result(0, 1) = -s;
        result(1, 0) = s;
        result(1, 1) = c;
        result(2, 2) = static_cast<T>(1.0);
        result(3, 3) = static_cast<T>(1.0);
        
        return result;
    }

    static Matrix4x4 translation(T x, T y, T z) {
        Matrix4x4 result;
        result(0, 0) = static_cast<T>(1.0);
        result(1, 1) = static_cast<T>(1.0);
        result(2, 2) = static_cast<T>(1.0);
        result(3, 3) = static_cast<T>(1.0);
        result(0, 3) = x;
        result(1, 3) = y;
        result(2, 3) = z;
        
        return result;
    }

    static Matrix4x4 scaling(T sx, T sy, T sz) {
        Matrix4x4 result;
        result(0, 0) = sx;
        result(1, 1) = sy;
        result(2, 2) = sz;
        result(3, 3) = static_cast<T>(1.0);
        
        return result;
    }

    /**
     * Equality comparison
     */
    bool operator==(const Matrix4x4& other) const {
        for (size_t i = 0; i < size; ++i) {
            if (std::abs(data_[i] - other.data_[i]) > static_cast<T>(1e-6)) {
                return false;
            }
        }
        return true;
    }

    /**
     * Inequality comparison
     */
    bool operator!=(const Matrix4x4& other) const {
        return !(*this == other);
    }

    /**
     * Addition assignment
     */
    Matrix4x4& operator+=(const Matrix4x4& other) {
        VectorType::vectorAdd(data_, data_, other.data_, size);
        return *this;
    }

    /**
     * Subtraction assignment
     */
    Matrix4x4& operator-=(const Matrix4x4& other) {
        VectorType::vectorSubtract(data_, data_, other.data_, size);
        return *this;
    }

    /**
     * Multiplication assignment
     */
    Matrix4x4& operator*=(const Matrix4x4& other) {
        T temp[size];
        Matrix4x4::matrixMultiply(temp, data_, other.data_);
        std::copy(temp, temp + size, data_.begin());
        return *this;
    }

    /**
     * Scalar multiplication assignment
     */
    Matrix4x4& operator*=(T scalar) {
        VectorType::vectorMultiplyScalar(data_, data_, scalar, size);
        return *this;
    }

    /**
     * Addition operator
     */
    Matrix4x4 operator+(const Matrix4x4& other) const {
        return add(other);
    }

    /**
     * Subtraction operator
     */
    Matrix4x4 operator-(const Matrix4x4& other) const {
        return subtract(other);
    }

    /**
     * Multiplication operator
     */
    Matrix4x4 operator*(const Matrix4x4& other) const {
        return multiply(other);
    }

    /**
     * Scalar multiplication operator
     */
    Matrix4x4 operator*(T scalar) const {
        return multiplyScalar(scalar);
    }

    /**
     * Friend function for scalar * matrix
     */
    friend Matrix4x4 operator*(T scalar, const Matrix4x4& matrix) {
        return matrix.multiplyScalar(scalar);
    }

    /**
     * Cast to std::array
     */
    operator std::array<T, size>() const {
        return data_;
    }

    /**
     * Construct from std::array
     */
    static Matrix4x4 fromArray(const std::array<T, size>& arr) {
        Matrix4x4 result;
        std::copy(arr.begin(), arr.end(), result.data_.begin());
        return result;
    }

    /**
     * Convert to string representation
     */
    std::string toString() const {
        std::ostringstream oss;
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                oss << (*this)(i, j);
                if (j < cols - 1) oss << " ";
            }
            if (i < rows - 1) oss << "\n";
        }
        return oss.str();
    }

    /**
     * Convert to string with precision
     */
    std::string toString(size_t precision) const {
        std::ostringstream oss;
        oss.precision(precision);
        oss << std::fixed;
        
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                oss << (*this)(i, j);
                if (j < cols - 1) oss << " ";
            }
            if (i < rows - 1) oss << "\n";
        }
        return oss.str();
    }

    /**
     * Random matrix generation
     */
    static Matrix4x4 random(T min = static_cast<T>(-1.0), T max = static_cast<T>(1.0)) {
        Matrix4x4 result;
        for (size_t i = 0; i < size; ++i) {
            result.data_[i] = VectorType::randomFloat(min, max);
        }
        return result;
    }

    /**
     * Identity matrix factory
     */
    static Matrix4x4 identity() {
        Matrix4x4 result;
        result.setIdentity();
        return result;
    }

    /**
     * Zero matrix factory
     */
    static Matrix4x4 zero() {
        Matrix4x4 result;
        result.setZero();
        return result;
    }

    /**
     * Diagonal matrix factory
     */
    static Matrix4x4 diagonal(T d0, T d1 = static_cast<T>(1.0), T d2 = static_cast<T>(1.0), T d3 = static_cast<T>(1.0)) {
        Matrix4x4 result;
        result(0, 0) = d0;
        result(1, 1) = d1;
        result(2, 2) = d2;
        result(3, 3) = d3;
        return result;
    }

    /**
     * Upper triangular matrix factory
     */
    static Matrix4x4 upperTriangular(T diag0 = static_cast<T>(1.0), T diag1 = static_cast<T>(1.0),
                                     T diag2 = static_cast<T>(1.0), T diag3 = static_cast<T>(1.0),
                                     T upper01 = static_cast<T>(0.0), T upper02 = static_cast<T>(0.0), T upper03 = static_cast<T>(0.0),
                                     T upper12 = static_cast<T>(0.0), T upper13 = static_cast<T>(0.0),
                                     T upper23 = static_cast<T>(0.0)) {
        Matrix4x4 result;
        result(0, 0) = diag0;
        result(1, 1) = diag1;
        result(2, 2) = diag2;
        result(3, 3) = diag3;
        result(0, 1) = upper01;
        result(0, 2) = upper02;
        result(0, 3) = upper03;
        result(1, 2) = upper12;
        result(1, 3) = upper13;
        result(2, 3) = upper23;
        return result;
    }

    /**
     * Lower triangular matrix factory
     */
    static Matrix4x4 lowerTriangular(T diag0 = static_cast<T>(1.0), T diag1 = static_cast<T>(1.0),
                                     T diag2 = static_cast<T>(1.0), T diag3 = static_cast<T>(1.0),
                                     T lower01 = static_cast<T>(0.0), T lower02 = static_cast<T>(0.0), T lower03 = static_cast<T>(0.0),
                                     T lower12 = static_cast<T>(0.0), T lower13 = static_cast<T>(0.0),
                                     T lower23 = static_cast<T>(0.0)) {
        Matrix4x4 result;
        result(0, 0) = diag0;
        result(1, 1) = diag1;
        result(2, 2) = diag2;
        result(3, 3) = diag3;
        result(1, 0) = lower01;
        result(2, 0) = lower02;
        result(3, 0) = lower03;
        result(2, 1) = lower12;
        result(3, 1) = lower13;
        result(3, 2) = lower23;
        return result;
    }

    /**
     * Symmetric matrix factory
     */
    static Matrix4x4 symmetric(T diag0 = static_cast<T>(1.0), T diag1 = static_cast<T>(1.0),
                                T diag2 = static_cast<T>(1.0), T diag3 = static_cast<T>(1.0),
                                T off01 = static_cast<T>(0.5), T off02 = static_cast<T>(0.3), T off03 = static_cast<T>(0.2),
                                T off12 = static_cast<T>(0.4), T off13 = static_cast<T>(0.1), T off23 = static_cast<T>(0.5)) {
        Matrix4x4 result;
        result(0, 0) = diag0;
        result(1, 1) = diag1;
        result(2, 2) = diag2;
        result(3, 3) = diag3;
        result(0, 1) = result(1, 0) = off01;
        result(0, 2) = result(2, 0) = off02;
        result(0, 3) = result(3, 0) = off03;
        result(1, 2) = result(2, 1) = off12;
        result(1, 3) = result(3, 1) = off13;
        result(2, 3) = result(3, 2) = off23;
        return result;
    }

    /**
     * Skew-symmetric matrix factory
     */
    static Matrix4x4 skewSymmetric(T skew01 = static_cast<T>(1.0), T skew02 = static_cast<T>(0.5),
                                   T skew03 = static_cast<T>(0.3), T skew12 = static_cast<T>(0.7),
                                   T skew13 = static_cast<T>(0.2), T skew23 = static_cast<T>(0.8)) {
        Matrix4x4 result;
        result.setZero();
        result(0, 1) = -skew01;
        result(1, 0) = skew01;
        result(0, 2) = -skew02;
        result(2, 0) = skew02;
        result(0, 3) = -skew03;
        result(3, 0) = skew03;
        result(1, 2) = -skew12;
        result(2, 1) = skew12;
        result(1, 3) = -skew13;
        result(3, 1) = skew13;
        result(2, 3) = -skew23;
        result(3, 2) = skew23;
        return result;
    }

    /**
     * Permutation matrix factory
     */
    static Matrix4x4 permutation(const std::array<size_t, rows>& perm) {
        Matrix4x4 result;
        result.setZero();
        for (size_t i = 0; i < rows; ++i) {
            result(perm[i], i) = static_cast<T>(1.0);
        }
        return result;
    }

    /**
     * Get row vector
     */
    std::array<T, cols> getRow(size_t row) const {
        std::array<T, cols> result;
        for (size_t j = 0; j < cols; ++j) {
            result[j] = (*this)(row, j);
        }
        return result;
    }

    /**
     * Get column vector
     */
    std::array<T, rows> getColumn(size_t col) const {
        std::array<T, rows> result;
        for (size_t i = 0; i < rows; ++i) {
            result[i] = (*this)(i, col);
        }
        return result;
    }

    /**
     * Set row vector
     */
    void setRow(size_t row, const std::array<T, cols>& values) {
        for (size_t j = 0; j < cols; ++j) {
            (*this)(row, j) = values[j];
        }
    }

    /**
     * Set column vector
     */
    void setColumn(size_t col, const std::array<T, rows>& values) {
        for (size_t i = 0; i < rows; ++i) {
            (*this)(i, col) = values[i];
        }
    }

    /**
     * Get submatrix
     */
    Matrix4x4 submatrix(size_t startRow, size_t startCol, size_t subRows = rows, size_t subCols = cols) const {
        Matrix4x4 result;
        for (size_t i = 0; i < subRows && i + startRow < rows; ++i) {
            for (size_t j = 0; j < subCols && j + startCol < cols; ++j) {
                result(i, j) = (*this)(i + startRow, j + startCol);
            }
        }
        return result;
    }

    /**
     * Extract row as vector
     */
    std::vector<T> extractRow(size_t row) const {
        std::vector<T> result(cols);
        for (size_t j = 0; j < cols; ++j) {
            result[j] = (*this)(row, j);
        }
        return result;
    }

    /**
     * Extract column as vector
     */
    std::vector<T> extractColumn(size_t col) const {
        std::vector<T> result(rows);
        for (size_t i = 0; i < rows; ++i) {
            result[i] = (*this)(i, col);
        }
        return result;
    }

    /**
     * Set from row vector
     */
    void assignFromRow(size_t row, const std::vector<T>& values) {
        for (size_t j = 0; j < std::min(cols, values.size()); ++j) {
            (*this)(row, j) = values[j];
        }
    }

    /**
     * Set from column vector
     */
    void assignFromColumn(size_t col, const std::vector<T>& values) {
        for (size_t i = 0; i < std::min(rows, values.size()); ++i) {
            (*this)(i, col) = values[i];
        }
    }

    /**
     * Calculate matrix rank
     */
    size_t rank() const {
        // Compute rank using Gaussian elimination
        Matrix4x4 temp = *this;
        size_t rank = 0;
        
        for (size_t col = 0; col < cols && rank < rows; ++col) {
            // Find pivot
            size_t pivotRow = rank;
            while (pivotRow < rows && std::abs(temp(pivotRow, col)) < static_cast<T>(1e-10)) {
                ++pivotRow;
            }
            
            if (pivotRow < rows) {
                // Swap rows
                if (pivotRow != rank) {
                    for (size_t j = 0; j < cols; ++j) {
                        std::swap(temp(rank, j), temp(pivotRow, j));
                    }
                }
                
                // Eliminate below
                for (size_t i = rank + 1; i < rows; ++i) {
                    T factor = temp(i, col) / temp(rank, col);
                    for (size_t j = col; j < cols; ++j) {
                        temp(i, j) -= factor * temp(rank, j);
                    }
                }
                
                ++rank;
            }
        }
        
        return rank;
    }

    /**
     * Check if matrix is singular
     */
    bool isSingular() const {
        return determinant() == static_cast<T>(0.0);
    }

    /**
     * Compute matrix power
     */
    Matrix4x4 power(int n) const {
        if (n == 0) {
            return identity();
        }
        if (n == 1) {
            return *this;
        }
        if (n < 0) {
            Matrix4x4 inv = invert();
            return inv.power(-n);
        }
        
        Matrix4x4 result = *this;
        Matrix4x4 base = *this;
        
        while (n > 1) {
            if (n % 2 == 0) {
                base = base.multiply(base);
                n /= 2;
            } else {
                result = result.multiply(base);
                n--;
            }
        }
        
        return result;
    }

    /**
     * Compute matrix square root
     */
    Matrix4x4 sqrt() const {
        Matrix4x4 result;
        // Use Newton-Raphson iteration for matrix square root
        Matrix4x4 x = *this;
        Matrix4x4 y = identity();
        
        for (size_t iter = 0; iter < 100; ++iter) {
            y = (x.multiply(y).add(y)) * static_cast<T>(0.5);
            if ((y.subtract(x.multiply(y)).norm()) < static_cast<T>(1e-6)) {
                break;
            }
        }
        
        return y;
    }

    /**
     * Compute matrix log (matrix logarithm)
     */
    Matrix4x4 log() const {
        Matrix4x4 result;
        // Use Taylor series expansion
        Matrix4x4 I = identity();
        Matrix4x4 X = subtract(I);
        Matrix4x4 term = X;
        T factor = static_cast<T>(1.0);
        
        for (size_t i = 1; i < 20; ++i) {
            factor /= static_cast<T>(i + 1);
            result = result.add(term.multiplyScalar(factor));
            term = term.multiply(X);
        }
        
        return result;
    }

    /**
     * Compute matrix hyperbolic functions
     */
    Matrix4x4 sinh() const {
        Matrix4x4 result;
        Matrix4x4 I = identity();
        Matrix4x4 X = *this;
        Matrix4x4 term = X;
        T factor = static_cast<T>(1.0);
        
        result = term.multiplyScalar(factor);
        
        for (size_t i = 2; i < 10; ++i) {
            factor /= static_cast<T>(i * (i - 1));
            term = term.multiply(X.multiplyScalar(factor));
            result = result.add(term.multiplyScalar(factor));
        }
        
        return result;
    }

    Matrix4x4 cosh() const {
        Matrix4x4 result;
        Matrix4x4 I = identity();
        Matrix4x4 X = *this;
        Matrix4x4 term = I;
        T factor = static_cast<T>(1.0);
        
        result = term.multiplyScalar(factor);
        
        for (size_t i = 2; i < 10; ++i) {
            factor /= static_cast<T>(i * (i - 1));
            term = term.multiply(X.multiplyScalar(factor));
            result = result.add(term.multiplyScalar(factor));
        }
        
        return result;
    }

    Matrix4x4 tanh() const {
        return sinh().multiply(invert().add(identity()));
    }

    /**
     * Compute matrix trace (sum of diagonal elements)
     */
    T trace() const {
        T trace = static_cast<T>(0.0);
        for (size_t i = 0; i < std::min(rows, cols); ++i) {
            trace += (*this)(i, i);
        }
        return trace;
    }

    /**
     * Compute matrix norm (Frobenius norm)
     */
    T norm() const {
        T sum = static_cast<T>(0.0);
        for (size_t i = 0; i < size; ++i) {
            sum += data_[i] * data_[i];
        }
        return std::sqrt(sum);
    }

    /**
     * Normalize matrix
     */
    Matrix4x4 normalize() const {
        T norm = this->norm();
        if (norm > static_cast<T>(0.0)) {
            return multiplyScalar(static_cast<T>(1.0) / norm);
        }
        return *this;
    }

    /**
     * Reshape matrix (change dimensions)
     */
    Matrix4x4 reshape(size_t newRows, size_t newCols) const {
        if (newRows * newCols != size) {
            throw std::invalid_argument("Cannot reshape matrix: size mismatch");
        }
        
        Matrix4x4 result;
        for (size_t i = 0; i < newRows; ++i) {
            for (size_t j = 0; j < newCols; ++j) {
                size_t oldIndex = i * cols + j;
                size_t newIndex = i * newCols + j;
                result.data_[newIndex] = data_[oldIndex];
            }
        }
        
        return result;
    }

    /**
     * Rotate matrix by 90 degrees
     */
    Matrix4x4 rotate90(size_t times = 1) const {
        Matrix4x4 result = *this;
        for (size_t i = 0; i < times; ++i) {
            // Rotate by 90 degrees clockwise
            Matrix4x4 temp;
            for (size_t r = 0; r < rows; ++r) {
                for (size_t c = 0; c < cols; ++c) {
                    temp(cols - 1 - c, r) = (*this)(r, c);
                }
            }
            result = temp;
        }
        return result;
    }

    /**
     * Get matrix transpose
     */
    Matrix4x4 getTranspose() const {
        Matrix4x4 result;
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result(i, j) = (*this)(j, i);
            }
        }
        return result;
    }

    /**
     * Get diagonal elements
     */
    std::array<T, std::min(rows, cols)> getDiagonal() const {
        std::array<T, std::min(rows, cols)> result;
        for (size_t i = 0; i < std::min(rows, cols); ++i) {
            result[i] = (*this)(i, i);
        }
        return result;
    }

    /**
     * Set diagonal elements
     */
    void setDiagonal(const std::array<T, std::min(rows, cols)>& values) {
        for (size_t i = 0; i < std::min(rows, cols); ++i) {
            (*this)(i, i) = values[i];
        }
    }

    /**
     * Get upper triangular part
     */
    Matrix4x4 getUpperTriangle() const {
        Matrix4x4 result;
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                if (i <= j) {
                    result(i, j) = (*this)(i, j);
                }
            }
        }
        return result;
    }

    /**
     * Get lower triangular part
     */
    Matrix4x4 getLowerTriangle() const {
        Matrix4x4 result;
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                if (i >= j) {
                    result(i, j) = (*this)(i, j);
                }
            }
        }
        return result;
    }

    /**
     * Get upper triangular matrix
     */
    Matrix4x4 upperTriangular() const {
        Matrix4x4 result;
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result(i, j) = (i <= j) ? (*this)(i, j) : static_cast<T>(0.0);
            }
        }
        return result;
    }

    /**
     * Get lower triangular matrix
     */
    Matrix4x4 lowerTriangular() const {
        Matrix4x4 result;
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result(i, j) = (i >= j) ? (*this)(i, j) : static_cast<T>(0.0);
            }
        }
        return result;
    }

    /**
     * Get strictly upper triangular part
     */
    Matrix4x4 strictlyUpperTriangular() const {
        Matrix4x4 result;
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result(i, j) = (i < j) ? (*this)(i, j) : static_cast<T>(0.0);
            }
        }
        return result;
    }

    /**
     * Get strictly lower triangular part
     */
    Matrix4x4 strictlyLowerTriangular() const {
        Matrix4x4 result;
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result(i, j) = (i > j) ? (*this)(i, j) : static_cast<T>(0.0);
            }
        }
        return result;
    }

    /**
     * Get symmetric part
     */
    Matrix4x4 symmetricPart() const {
        Matrix4x4 result = *this;
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result(i, j) = ((*this)(i, j) + (*this)(j, i)) * static_cast<T>(0.5);
            }
        }
        return result;
    }

    /**
     * Get skew-symmetric part
     */
    Matrix4x4 skewSymmetricPart() const {
        Matrix4x4 result = *this;
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result(i, j) = ((*this)(i, j) - (*this)(j, i)) * static_cast<T>(0.5);
            }
        }
        return result;
    }

    /**
     * Get outer product
     */
    static Matrix4x4 outerProduct(const T* a, const T* b) {
        Matrix4x4 result;
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result(i, j) = a[i] * b[j];
            }
        }
        return result;
    }

    /**
     * Check if matrix is diagonal
     */
    bool isDiagonal() const {
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                if (i != j && std::abs((*this)(i, j)) > static_cast<T>(1e-6)) {
                    return false;
                }
            }
        }
        return true;
    }

    /**
     * Check if matrix is triangular (upper or lower)
     */
    bool isTriangular() const {
        bool upper = true;
        bool lower = true;
        
        for (size_t i = 0; i < rows && (upper || lower); ++i) {
            for (size_t j = 0; j < cols && (upper || lower); ++j) {
                if (i < j) {
                    if (std::abs((*this)(i, j)) > static_cast<T>(1e-6)) {
                        upper = false;
                    }
                } else if (i > j) {
                    if (std::abs((*this)(i, j)) > static_cast<T>(1e-6)) {
                        lower = false;
                    }
                }
            }
        }
        
        return upper || lower;
    }

    /**
     * Get spectral radius (largest eigenvalue magnitude)
     */
    T spectralRadius() const {
        // Compute eigenvalues and return largest magnitude
        Matrix4x4 eigenvalues[4]; // In practice, would use actual eigenvalue solver
        return static_cast<T>(0.0); // Placeholder
    }

    /**
     * Check if matrix is positive definite
     */
    bool isPositiveDefinite() const {
        // Check all leading principal minors are positive
        Matrix4x4 submatrix;
        
        // 1x1 minor
        if ((*this)(0, 0) <= static_cast<T>(0.0)) return false;
        
        // 2x2 minor
        T det2 = (*this)(0, 0) * (*this)(1, 1) - (*this)(0, 1) * (*this)(1, 0);
        if (det2 <= static_cast<T>(0.0)) return false;
        
        // 3x3 minor
        T det3 = (*this)(0, 0) * det2 - 
                 (*this)(0, 1) * ((*this)(0, 0) * (*this)(2, 2) - (*this)(1, 2) * (*this)(2, 1)) +
                 (*this)(0, 2) * ((*this)(1, 0) * (*this)(2, 2) - (*this)(1, 2) * (*this)(2, 1));
        if (det3 <= static_cast<T>(0.0)) return false;
        
        // 4x4 determinant (full matrix)
        if (determinant() <= static_cast<T>(0.0)) return false;
        
        return true;
    }

    /**
     * Check if matrix is positive semi-definite
     */
    bool isPositiveSemiDefinite() const {
        // All eigenvalues >= 0 (simplified check)
        return isPositiveDefinite() || determinant() >= static_cast<T>(0.0);
    }

    /**
     * Compute Cholesky decomposition
     */
    static void choleskyDecompose(T* result, const T* matrix) {
        // Cholesky decomposition A = LL^T
        VectorType::choleskyDecompose4x4(result, matrix);
    }

    /**
     * Solve linear system using Cholesky decomposition
     */
    static void solveCholesky(T* result, const T* a, const T* b) {
        // Solve Ax = b using Cholesky decomposition
        VectorType::solveCholeskyLinearSystem4x4(result, a, b);
    }

    /**
     * Compute matrix condition number
     */
    T conditionNumber() const {
        // Condition number = ||A|| * ||A^-1||
        T normA = norm();
        T normInv = invert().norm();
        if (normA > static_cast<T>(0.0)) {
            return normA * normInv;
        }
        return static_cast<T>(std::numeric_limits<T>::max)();
    }

    /**
     * Apply matrix exponential to multiple matrices (batch)
     */
    static void batchExponential(T* results, const T* matrices, size_t count) {
        // Batch matrix exponential
        for (size_t i = 0; i < count; ++i) {
            Matrix4x4::matrixExponential(
                results + i * size,
                matrices + i * size
            );
        }
    }

    /**
     * Compute matrix power (optimized)
     */
    Matrix4x4 fastPower(int n) const {
        // Fast exponentiation using binary exponentiation
        if (n == 0) return identity();
        if (n == 1) return *this;
        
        Matrix4x4 half = (*this).fastPower(n / 2);
        Matrix4x4 result = half.multiply(half);
        
        if (n % 2 != 0) {
            result = result.multiply(*this);
        }
        
        return result;
    }

    /**
     * Compute matrix square root using Newton-Raphson
     */
    Matrix4x4 sqrtNewtonRaphson() const {
        // Newton's method for matrix square root
        Matrix4x4 X = *this;
        Matrix4x4 Y = identity();
        
        for (size_t iter = 0; iter < 20; ++iter) {
            Y = (X.multiply(Y).add(Y)) * static_cast<T>(0.5);
            if ((Y.subtract(X.multiply(Y)).norm()) < static_cast<T>(1e-8)) {
                break;
            }
        }
        
        return Y;
    }

    /**
     * Compute matrix log using Taylor series
     */
    Matrix4x4 logTaylorSeries() const {
        // Taylor series expansion for matrix logarithm
        Matrix4x4 I = identity();
        Matrix4x4 X = subtract(I);
        Matrix4x4 term = X;
        T factor = static_cast<T>(1.0);
        
        Matrix4x4 result = term.multiplyScalar(factor);
        
        for (size_t i = 2; i < 20; ++i) {
            factor /= static_cast<T>(i);
            term = term.multiply(X);
            result = result.add(term.multiplyScalar(factor));
        }
        
        return result;
    }

    /**
     * Compute matrix hyperbolic sine
     */
    Matrix4x4 sinhTaylorSeries() const {
        // Taylor series expansion for matrix hyperbolic sine
        Matrix4x4 X = *this;
        Matrix4x4 term = X;
        T factor = static_cast<T>(1.0);
        
        Matrix4x4 result = term.multiplyScalar(factor);
        
        for (size_t i = 2; i < 10; ++i) {
            factor /= static_cast<T>(i * (i - 1));
            term = term.multiply(X.multiplyScalar(factor));
            result = result.add(term.multiplyScalar(factor));
        }
        
        return result;
    }

    /**
     * Compute matrix hyperbolic cosine
     */
    Matrix4x4 coshTaylorSeries() const {
        // Taylor series expansion for matrix hyperbolic cosine
        Matrix4x4 X = *this;
        Matrix4x4 term = identity();
        T factor = static_cast<T>(1.0);
        
        Matrix4x4 result = term.multiplyScalar(factor);
        
        for (size_t i = 2; i < 10; ++i) {
            factor /= static_cast<T>(i * (i - 1));
            term = term.multiply(X.multiplyScalar(factor));
            result = result.add(term.multiplyScalar(factor));
        }
        
        return result;
    }

    /**
     * Compute matrix hyperbolic tangent
     */
    Matrix4x4 tanh() const {
        // tanh(A) = sinh(A) / cosh(A)
        Matrix4x4 sinhA = sinhTaylorSeries();
        Matrix4x4 coshA = coshTaylorSeries();
        
        // Compute inverse of coshA
        Matrix4x4 coshA_inv = coshA.invert();
        
        return sinhA.multiply(coshA_inv);
    }

    /**
     * Compute matrix exponential using Padé approximation
     */
    Matrix4x4 expPade() const {
        // Padé approximation for matrix exponential
        Matrix4x4 I = identity();
        Matrix4x4 A = *this;
        
        // Compute Pade approximation
        Matrix4x4 P = I.add(A.multiplyScalar(static_cast<T>(0.5)));
        Matrix4x4 Q = I.subtract(A.multiplyScalar(static_cast<T>(0.5)));
        
        Matrix4x4 Q_inv = Q.invert();
        Matrix4x4 result = Q_inv.multiply(P);
        
        return result;
    }

    /**
     * Compute matrix power using diagonalization
     */
    Matrix4x4 powerDiagonalization(int n) const {
        // Compute A^n using diagonalization A = PDP^(-1)
        // This is more efficient for large powers
        Matrix4x4 P = getDiagonalizationP();
        Matrix4x4 D = getDiagonalizationD();
        
        Matrix4x4 P_inv = P.invert();
        Matrix4x4 D_pow = D.power(n);
        
        return P.multiply(D_pow).multiply(P_inv);
    }

    /**
     * Get matrix for diagonalization
     */
    Matrix4x4 getDiagonalizationP() const {
        // Compute eigenvectors (simplified)
        Matrix4x4 result = *this;
        return result;
    }

    /**
     * Get diagonal matrix for diagonalization
     */
    Matrix4x4 getDiagonalizationD() const {
        // Compute eigenvalues (simplified)
        Matrix4x4 result = *this;
        return result;
    }

    /**
     * Compute matrix trace
     */
    T trace() const {
        T trace = static_cast<T>(0.0);
        for (size_t i = 0; i < std::min(rows, cols); ++i) {
            trace += (*this)(i, i);
        }
        return trace;
    }

    /**
     * Compute matrix determinant using cofactor expansion
     */
    T determinant() const {
        // Compute determinant using cofactor expansion
        T det = static_cast<T>(0.0);
        
        if (rows == 1 && cols == 1) {
            return (*this)(0, 0);
        }
        
        // Expand along first row
        for (size_t col = 0; col < cols; ++col) {
            Matrix4x4<T> submatrix;
            size_t subRow = 0;
            
            // Build submatrix excluding first row and current column
            for (size_t i = 1; i < rows; ++i) {
                size_t subCol = 0;
                for (size_t j = 0; j < cols; ++j) {
                    if (j != col) {
                        submatrix(subRow, subCol++) = (*this)(i, j);
                    }
                }
                subRow++;
            }
            
            T cofactor = submatrix.determinant();
            if (col % 2 == 0) {
                det += (*this)(0, col) * cofactor;
            } else {
                det -= (*this)(0, col) * cofactor;
            }
        }
        
        return det;
    }

    /**
     * Compute matrix rank
     */
    size_t rank() const {
        // Compute rank using Gaussian elimination
        Matrix4x4 temp = *this;
        size_t rank = 0;
        
        for (size_t col = 0; col < cols && rank < rows; ++col) {
            // Find pivot
            size_t pivotRow = rank;
            while (pivotRow < rows && std::abs(temp(pivotRow, col)) < static_cast<T>(1e-10)) {
                ++pivotRow;
            }
            
            if (pivotRow < rows) {
                // Swap rows
                if (pivotRow != rank) {
                    for (size_t j = 0; j < cols; ++j) {
                        std::swap(temp(rank, j), temp(pivotRow, j));
                    }
                }
                
                // Eliminate below
                for (size_t i = rank + 1; i < rows; ++i) {
                    T factor = temp(i, col) / temp(rank, col);
                    for (size_t j = col; j < cols; ++j) {
                        temp(i, j) -= factor * temp(rank, j);
                    }
                }
                
                ++rank;
            }
        }
        
        return rank;
    }

    /**
     * Check if matrix is singular
     */
    bool isSingular() const {
        return std::abs(determinant()) < static_cast<T>(1e-10);
    }

    /**
     * Compute eigenvalues (simplified)
     */
    std::array<T, 4> eigenvalues() const {
        // Characteristic polynomial: det(A - λI) = 0
        // For 4x4 matrix, this is a 4th degree polynomial
        // This is a simplified implementation
        
        std::array<T, 4> result = {static_cast<T>(0.0), static_cast<T>(0.0), 
                                   static_cast<T>(0.0), static_cast<T>(0.0)};
        
        // Compute trace and other invariants
        T trace = this->trace();
        T det = determinant();
        
        // For demonstration, return placeholder values
        // In practice, would use proper eigenvalue solver
        for (size_t i = 0; i < 4; ++i) {
            result[i] = (*this)(i, i) + static_cast<T>(0.1) * i;
        }
        
        return result;
    }

    /**
     * Compute eigenvectors (simplified)
     */
    std::vector<std::array<T, 4>> eigenvectors() const {
        std::vector<std::array<T, 4>> result;
        std::array<T, 4> eigenvalues = this->eigenvalues();n
        
        for (size_t i = 0; i < 4; ++i) {
            std::array<T, 4> eigenvector = {static_cast<T>(0.0)};
            eigenvector[i] = static_cast<T>(1.0);
            result.push_back(eigenvector);
        }
        
        return result;
    }

    /**
     * Compute singular value decomposition
     */
    static void svd(T* U, T* S, T* Vt, const T* matrix) {
        // Singular value decomposition using QR algorithm
        VectorType::svd4x4(U, S, Vt, matrix);
    }

    /**
     * Compute QR decomposition
     */
    static void qrDecompose(T* Q, T* R, const T* matrix) {
        // QR decomposition using Householder reflections
        VectorType::qrDecompose4x4(Q, R, matrix);
    }

    /**
     * Compute pseudoinverse
     */
    Matrix4x4 pseudoinverse() const {
        // Compute Moore-Penrose pseudoinverse
        Matrix4x4 result;
        Matrix4x4::pseudoinverse4x4(result.data(), data_);
        return result;
    }

    /**
     * Compute best fit solution to Ax = b
     */
    std::array<T, 4> leastSquaresFit(const T* b) const {
        // Solve Ax ≈ b using normal equations
        Matrix4x4 A_transpose = this->transpose();
        Matrix4x4 ATA = A_transpose.multiply(*this);
        Matrix4x4 ATb = A_transpose.multiply(Matrix4x4::fromArray(std::array<T, 4>(*b, b[1], b[2], b[3])));
        
        return ATA.invert().multiply(ATb).getColumn(0).data();
    }

    /**
     * Compute matrix Hadamard product (element-wise multiplication)
     */
    Matrix4x4 hadamard(const Matrix4x4& other) const {
        Matrix4x4 result;
        for (size_t i = 0; i < size; ++i) {
            result.data_[i] = data_[i] * other.data_[i];
        }
        return result;
    }

    /**
     * Compute matrix difference (element-wise)
     */
    Matrix4x4 diff(const Matrix4x4& other) const {
        Matrix4x4 result;
        for (size_t i = 0; i < size; ++i) {
            result.data_[i] = data_[i] - other.data_[i];
        }
        return result;
    }

    /**
     * Check if matrix is equal to another matrix within tolerance
     */
    bool equals(const Matrix4x4& other, T tolerance = static_cast<T>(1e-6)) const {
        for (size_t i = 0; i < size; ++i) {
            if (std::abs(data_[i] - other.data_[i]) > tolerance) {
                return false;
            }
        }
        return true;
    }

    /**
     * Get matrix as string representation
     */
    std::string toString() const {
        std::ostringstream oss;
        oss.precision(6);
        oss << std::fixed;
        
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                oss << (*this)(i, j);
                if (j < cols - 1) oss << " ";
            }
            if (i < rows - 1) oss << "\n";
        }
        
        return oss.str();
    }

    /**
     * Get matrix in human-readable format
     */
    std::string toHumanReadable() const {
        std::ostringstream oss;
        oss << "Matrix4x4 (" << rows << "x" << cols << "):\n";
        
        for (size_t i = 0; i < rows; ++i) {
            oss << "  [";
            for (size_t j = 0; j < cols; ++j) {
                oss << (*this)(i, j);
                if (j < cols - 1) oss << ", ";
            }
            oss << "]";
            if (i < rows - 1) oss << "\n";
        }
        
        return oss.str();
    }

    /**
     * Get memory footprint
     */
    size_t memoryFootprint() const {
        return sizeof(Matrix4x4);
    }

    /**
     * Get cache alignment
     */
    static constexpr size_t cacheAlignment() {
        return 64;
    }

    /**
     * Get SIMD width
     */
    static constexpr size_t simdWidth() {
        return VectorType::getSimdWidth();
    }

    /**
     * Check if SIMD is supported
     */
    static constexpr bool isSimdSupported() {
        return VectorType::isSimdAvailable();
    }

    /**
     * Get performance info
     */
    static std::string getPerformanceInfo() {
        std::ostringstream oss;
        oss << "Matrix4x4 Performance Information:\n";
        oss << "  SIMD Support: " << (isSimdSupported() ? "Yes" : "No") << "\n";
        oss << "  SIMD Width: " << simdWidth() << "\n";
        oss << "  Cache Alignment: " << cacheAlignment() << " bytes\n";
        oss << "  Matrix Size: " << rows << "x" << cols << " (" << size << " elements)\n";
        return oss.str();
    }

    /**
     * Benchmark matrix operations
     */
    static void benchmarkOperations() {
        Matrix4x4 A = Matrix4x4::random();
        Matrix4x4 B = Matrix4x4::random();
        
        // Time matrix multiplication
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < 1000; ++i) {
            Matrix4x4 C = A.multiply(B);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "Matrix multiplication (1000 iterations): " << duration.count() << " microseconds" << std::endl;
        
        // Time scalar multiplication
        start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < 10000; ++i) {
            Matrix4x4 C = A.multiplyScalar(2.0f);
        }
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "Scalar multiplication (10000 iterations): " << duration.count() << " microseconds" << std::endl;
        
        // Time matrix addition
        start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < 10000; ++i) {
            Matrix4x4 C = A.add(B);
        }
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "Matrix addition (10000 iterations): " << duration.count() << " microseconds" << std::endl;
    }

    /**
     * Print matrix
     */
    void print(std::ostream& os = std::cout) const {
        os << toString();
    }

    /**
     * Print with formatting
     */
    void printFormatted(std::ostream& os = std::cout, size_t precision = 6) const {
        std::ostringstream oss;
        oss.precision(precision);
        oss << std::fixed;
        
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                oss << (*this)(i, j);
                if (j < cols - 1) oss << " ";
            }
            if (i < rows - 1) oss << "\n";
        }
        
        os << oss.str();
    }

    /**
     * Validate matrix
     */
    bool validate() const {
        // Check for NaN, infinity
        for (size_t i = 0; i < size; ++i) {
            if (std::isnan(data_[i]) || std::isinf(data_[i])) {
                return false;
            }
        }
        
        // Check determinant is reasonable
        T det = determinant();
        if (std::abs(det) > static_cast<T>(1e10) || std::abs(det) < static_cast<T>(1e-10)) {
            return false;
        }
        
        return true;
    }

    /**
     * Get matrix properties
     */
    static std::string getProperties() {
        std::ostringstream oss;
        oss << "Matrix4x4 Properties:\n";
        oss << "  Size: 4x4\n";
        oss << "  Total Elements: 16\n";
        oss << "  SIMD Optimized: Yes\n";
        oss << "  Cache Aligned: Yes\n";
        oss << "  Type: Template (float/double)\n";
        return oss.str();
    }

    /**
     * Generate test matrices
     */
    static Matrix4x4 identity() {
        Matrix4x4 result;
        result.setIdentity();
        return result;
    }

    static Matrix4x4 zero() {
        Matrix4x4 result;
        result.setZero();
        return result;
    }

    static Matrix4x4 diagonal(T d0, T d1 = static_cast<T>(1.0), T d2 = static_cast<T>(1.0), T d3 = static_cast<T>(1.0)) {
        Matrix4x4 result;
        result(0, 0) = d0;
        result(1, 1) = d1;
        result(2, 2) = d2;
        result(3, 3) = d3;
        return result;
    }

    static Matrix4x4 random(T min = static_cast<T>(-1.0), T max = static_cast<T>(1.0)) {
        Matrix4x4 result;
        for (size_t i = 0; i < size; ++i) {
            result.data_[i] = VectorType::randomFloat(min, max);
        }
        return result;
    }

    static Matrix4x4 rotationX(T angle) {
        Matrix4x4 result;
        T c = std::cos(angle);
        T s = std::sin(angle);
        
        result(0, 0) = static_cast<T>(1.0);
        result(1, 1) = c;
        result(1, 2) = -s;
        result(2, 1) = s;
        result(2, 2) = c;
        result(3, 3) = static_cast<T>(1.0);
        
        return result;
    }

    static Matrix4x4 rotationY(T angle) {
        Matrix4x4 result;
        T c = std::cos(angle);
        T s = std::sin(angle);
        
        result(0, 0) = c;
        result(0, 2) = s;
        result(1, 1) = static_cast<T>(1.0);
        result(2, 0) = -s;
        result(2, 2) = c;
        result(3, 3) = static_cast<T>(1.0);
        
        return result;
    }

    static Matrix4x4 rotationZ(T angle) {
        Matrix4x4 result;
        T c = std::cos(angle);
        T s = std::sin(angle);
        
        result(0, 0) = c;
        result(0, 1) = -s;
        result(1, 0) = s;
        result(1, 1) = c;
        result(2, 2) = static_cast<T>(1.0);
        result(3, 3) = static_cast<T>(1.0);
        
        return result;
    }

    static Matrix4x4 translation(T x, T y, T z) {
        Matrix4x4 result;
        result(0, 0) = static_cast<T>(1.0);
        result(1, 1) = static_cast<T>(1.0);
        result(2, 2) = static_cast<T>(1.0);
        result(3, 3) = static_cast<T>(1.0);
        result(0, 3) = x;
        result(1, 3) = y;
        result(2, 3) = z;
        
        return result;
    }

    static Matrix4x4 scaling(T sx, T sy, T sz) {
        Matrix4x4 result;
        result(0, 0) = sx;
        result(1, 1) = sy;
        result(2, 2) = sz;
        result(3, 3) = static_cast<T>(1.0);
        
        return result;
    }

    static Matrix4x4 symmetric(T diag0 = static_cast<T>(1.0), T diag1 = static_cast<T>(1.0),
                                T diag2 = static_cast<T>(1.0), T diag3 = static_cast<T>(1.0),
                                T off01 = static_cast<T>(0.5), T off02 = static_cast<T>(0.3), T off03 = static_cast<T>(0.2),
                                T off12 = static_cast<T>(0.4), T off13 = static_cast<T>(0.1), T off23 = static_cast<T>(0.5)) {
        Matrix4x4 result;
        result(0, 0) = diag0;
        result(1, 1) = diag1;
        result(2, 2) = diag2;
        result(3, 3) = diag3;
        result(0, 1) = result(1, 0) = off01;
        result(0, 2) = result(2, 0) = off02;
        result(0, 3) = result(3, 0) = off03;
        result(1, 2) = result(2, 1) = off12;
        result(1, 3) = result(3, 1) = off13;
        result(2, 3) = result(3, 2) = off23;
        return result;
    }

    /**
     * Get performance characteristics
     */
    static std::string getPerformanceInfo() {
        std::ostringstream oss;
        oss << "Matrix4x4 Performance Characteristics:\n";
        oss << "  SIMD Acceleration: " << (VectorType::isSimdAvailable() ? "Enabled" : "Disabled") << "\n";
        oss << "  Vector Width: " << VectorType::getSimdWidth() << " elements\n";
        oss << "  Memory Alignment: 64 bytes (cache line)\n";
        oss << "  Operation Latency: Low\n";
        oss << "  Throughput: High (SIMD)\n";
        oss << "  Cache Efficiency: Optimized\n";
        return oss.str();
    }
}; // End of Matrix4x4 template

// Type aliases for common usage
typedef Matrix4x4<float> Matrix4x4f;
typedef Matrix4x4<double> Matrix4x4d;

// Forward declarations for friend functions
namespace nlm {
    template<typename T>
    Matrix4x4<T> operator+(const Matrix4x4<T>& a, const Matrix4x4<T>& b);
    
    template<typename T>
    Matrix4x4<T> operator-(const Matrix4x4<T>& a, const Matrix4x4<T>& b);
    
    template<typename T>
    Matrix4x4<T> operator*(const Matrix4x4<T>& a, const Matrix4x4<T>& b);
    
    template<typename T>
    Matrix4x4<T> operator*(T scalar, const Matrix4x4<T>& matrix);
    
    template<typename T>
    Matrix4x4<T> operator*(const Matrix4x4<T>& matrix, T scalar);
    
    template<typename T>
    bool operator==(const Matrix4x4<T>& a, const Matrix4x4<T>& b);
    
    template<typename T>
    bool operator!=(const Matrix4x4<T>& a, const Matrix4x4<T>& b);
    
    template<typename T>
    std::ostream& operator<<(std::ostream& os, const Matrix4x4<T>& matrix);
} // End namespace nlm

// Include implementation file
#include "Matrix4x4.cpp"

// Provide additional implementation details for the template
template<typename T>
T Matrix4x4<T>::determinant() const {
    // Calculate determinant using cofactor expansion
    T det = static_cast<T>(0.0);
    
    // Expand along first row
    for (size_t col = 0; col < cols; ++col) {
        Matrix4x4<T> submatrix;
        size_t subRow = 0;
        
        // Build submatrix excluding first row and current column
        for (size_t i = 1; i < rows; ++i) {
            size_t subCol = 0;
            for (size_t j = 0; j < cols; ++j) {
                if (j != col) {
                    submatrix(subRow, subCol++) = (*this)(i, j);
                }
            }
            subRow++;
        }
        
        T cofactor = submatrix.determinant();
        if (col % 2 == 0) {
            det += (*this)(0, col) * cofactor;
        } else {
            det -= (*this)(0, col) * cofactor;
        }
    }
    
    return det;
}

// Additional specializations
template class Matrix4x4<float>;
template class Matrix4x4<double>;

} // namespace nlm
