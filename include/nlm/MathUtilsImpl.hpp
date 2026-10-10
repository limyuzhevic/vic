#pragma once

#include "Strategy.hpp"
#include "RandomGenerator.hpp"
#include "MathUtils.hpp"
#include <cmath>

namespace nlm {

double MathUtils::sigmoid(MembranePotential x) {
    return 1.0 / (1.0 + std::exp(-x));
}

double MathUtils::tanh(MembranePotential x) {
    return std::tanh(x);
}

double MathUtils::relu(MembranePotential x) {
    return std::max(0.0, x);
}

double MathUtils::leakyRelu(MembranePotential x, MembranePotential alpha) {
    return x > 0 ? x : alpha * x;
}

double MathUtils::elu(MembranePotential x, MembranePotential alpha) {
    return x > 0 ? x : alpha * (std::exp(x) - 1);
}

double MathUtils::selu(MembranePotential x, MembranePotential alpha, MembranePotential scale) {
    return x > 0 ? x : scale * (std::exp(x) - 1);
}

double MathUtils::softplus(MembranePotential x) {
    return std::log(1.0 + std::exp(x));
}

double MathUtils::softsign(MembranePotential x) {
    return x / (1.0 + std::abs(x));
}

double MathUtils::swish(MembranePotential x) {
    return x / (1.0 + std::exp(-x));
}

double MathUtils::mish(MembranePotential x) {
    return x * std::tanh(std::log(1.0 + std::exp(x)));
}

double MathUtils::sigmoidDerivative(MembranePotential x) {
    double s = sigmoid(x);
    return s * (1.0 - s);
}

double MathUtils::tanhDerivative(MembranePotential x) {
    return 1.0 - tanh(x) * tanh(x);
}

double MathUtils::reluDerivative(MembranePotential x) {
    return x > 0 ? 1.0 : 0.0;
}

double MathUtils::leakyReluDerivative(MembranePotential x, MembranePotential alpha) {
    return x > 0 ? 1.0 : alpha;
}

double MathUtils::eluDerivative(MembranePotential x, MembranePotential alpha) {
    return x > 0 ? 1.0 : x + alpha * std::exp(x);
}

double MathUtils::seluDerivative(MembranePotential x, MembranePotential alpha, MembranePotential scale) {
    return x > 0 ? 1.0 : scale * x + alpha * std::exp(x);
}

double MathUtils::softplusDerivative(MembranePotential x) {
    return 1.0 / (1.0 + std::exp(-x));
}

double MathUtils::softsignDerivative(MembranePotential x) {
    double ss = softsign(x);
    return 1.0 / std::pow(1.0 + std::abs(x), 2);
}

double MathUtils::swishDerivative(MembranePotential x) {
    double s = swish(x);
    return s + x * s * (1.0 - s);
}

double MathUtils::mishDerivative(MembranePotential x) {
    double m = mish(x);
    double softplus = softplus(x);
    return m + x * (1.0 - std::tanh(std::log(1.0 + std::exp(x)))) * softplusDerivative(x);
}

double MathUtils::mean(const std::vector<MembranePotential>& values) {
    if (values.empty()) return 0.0;
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double MathUtils::variance(const std::vector<MembranePotential>& values) {
    if (values.size() < 2) return 0.0;
    double m = mean(values);
    double sqSum = 0.0;
    for (double v : values) {
        sqSum += (v - m) * (v - m);
    }
    return sqSum / (values.size() - 1);
}

double MathUtils::standardDeviation(const std::vector<MembranePotential>& values) {
    return std::sqrt(variance(values));
}

double MathUtils::median(std::vector<MembranePotential> values) {
    if (values.empty()) return 0.0;
    std::sort(values.begin(), values.end());
    size_t n = values.size();
    if (n % 2 == 0) {
        return (values[n/2 - 1] + values[n/2]) / 2.0;
    } else {
        return values[n/2];
    }
}

double MathUtils::quantile(std::vector<MembranePotential> values, double q) {
    if (values.empty()) return 0.0;
    if (q < 0.0 || q > 1.0) throw std::invalid_argument("Quantile must be between 0 and 1");
    
    std::sort(values.begin(), values.end());
    double pos = q * (values.size() - 1);
    size_t lower = static_cast<size_t>(std::floor(pos));
    size_t upper = static_cast<size_t>(std::ceil(pos));
    double weight = pos - lower;
    
    if (lower == upper) {
        return values[lower];
    } else {
        return values[lower] * (1.0 - weight) + values[upper] * weight;
    }
}

std::vector<MembranePotential> MathUtils::percentile(std::vector<MembranePotential> values, const std::vector<double>& quantiles) {
    std::vector<MembranePotential> result;
    for (double q : quantiles) {
        result.push_back(quantile(values, q));
    }
    return result;
}

double MathUtils::correlation(const std::vector<MembranePotential>& x, const std::vector<MembranePotential>& y) {
    if (x.size() != y.size() || x.empty()) return 0.0;
    
    double xMean = mean(x);
    double yMean = mean(y);
    double numerator = 0.0;
    double xVariance = 0.0;
    double yVariance = 0.0;
    
    for (size_t i = 0; i < x.size(); i++) {
        double dx = x[i] - xMean;
        double dy = y[i] - yMean;
        numerator += dx * dy;
        xVariance += dx * dx;
        yVariance += dy * dy;
    }
    
    return numerator / std::sqrt(xVariance * yVariance);
}

double MathUtils::covariance(const std::vector<MembranePotential>& x, const std::vector<MembranePotential>& y) {
    if (x.size() != y.size() || x.empty()) return 0.0;
    
    double xMean = mean(x);
    double yMean = mean(y);
    double cov = 0.0;
    
    for (size_t i = 0; i < x.size(); i++) {
        double dx = x[i] - xMean;
        double dy = y[i] - yMean;
        cov += dx * dy;
    }
    
    return cov / (x.size() - 1);
}

double MathUtils::pdfNormal(MembranePotential x, MembranePotential mean, MembranePotential stddev) {
    double variance = stddev * stddev;
    return std::exp(-(x - mean) * (x - mean) / (2 * variance)) / std::sqrt(2 * M_PI * variance);
}

double MathUtils::cdfNormal(MembranePotential x, MembranePotential mean, MembranePotential stddev) {
    return 0.5 * (1.0 + std::erf((x - mean) / (stddev * std::sqrt(2.0))));
}

double MathUtils::pdfExponential(MembranePotential x, MembranePotential lambda) {
    return lambda * std::exp(-lambda * x);
}

double MathUtils::cdfExponential(MembranePotential x, MembranePotential lambda) {
    return 1.0 - std::exp(-lambda * x);
}

double MathUtils::pdfGamma(MembranePotential x, MembranePotential shape, MembranePotential scale) {
    return std::pow(x / scale, shape - 1) * std::exp(-x / scale) / (scale * std::tgamma(shape));
}

double MathUtils::cdfGamma(MembranePotential x, MembranePotential shape, MembranePotential scale) {
    return std::incomplete_gamma(shape, x / scale);
}

double MathUtils::pdfBeta(MembranePotential x, MembranePotential alpha, MembranePotential beta) {
    return std::pow(x, alpha - 1) * std::pow(1 - x, beta - 1) / beta(alpha, beta);
}

double MathUtils::cdfBeta(MembranePotential x, MembranePotential alpha, MembranePotential beta) {
    return std::incomplete_beta(x, alpha, beta);
}

double MathUtils::pdfChiSquare(MembranePotential x, MembranePotential degreesOfFreedom) {
    return std::pow(x, degreesOfFreedom/2 - 1) * std::exp(-x/2) / std::pow(2, degreesOfFreedom/2) / std::tgamma(degreesOfFreedom/2);
}

double MathUtils::cdfChiSquare(MembranePotential x, MembranePotential degreesOfFreedom) {
    return std::gamma_cdf(x/2, degreesOfFreedom/2);
}

double MathUtils::pdfStudentT(MembranePotential x, MembranePotential degreesOfFreedom) {
    double coeff = std::tgamma((degreesOfFreedom + 1) / 2) / std::sqrt(degreesOfFreedom * M_PI) / std::tgamma(degreesOfFreedom / 2);
    return coeff * std::pow(1 + x * x / degreesOfFreedom, -(degreesOfFreedom + 1) / 2);
}

double MathUtils::cdfStudentT(MembranePotential x, MembranePotential degreesOfFreedom) {
    return 0.5 + std::signbit(x) * 0.5 * (1 - std::beta_inc(degreesOfFreedom / (degreesOfFreedom + x * x), 0.5, degreesOfFreedom / 2));
}

double MathUtils::pdfF(MembranePotential x, MembranePotential df1, MembranePotential df2) {
    double num = std::pow(x, df1/2 - 1);
    double denom = std::pow(1 + x, (df1 + df2)/2 - 1);
    double beta = std::tgamma(df1/2) * std::tgamma(df2/2) / std::tgamma((df1 + df2)/2);
    return num / denom / beta;
}

double MathUtils::cdfF(MembranePotential x, MembranePotential df1, MembranePotential df2) {
    return std::beta_inc(df1 * x / (df1 * x + df2), df1/2, df2/2);
}

double MathUtils::factorial(int n) {
    if (n < 0) throw std::invalid_argument("Factorial of negative number");
    if (n == 0 || n == 1) return 1.0;
    double result = 1.0;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

double MathUtils::gamma(MembranePotential x) {
    return std::tgamma(x);
}

double MathUtils::lnGamma(MembranePotential x) {
    return std::lgamma(x);
}

double MathUtils::digamma(MembranePotential x) {
    return std::digamma(x);
}

double MathUtils::trigamma(MembranePotential x) {
    return std::trigamma(x);
}

double MathUtils::beta(MembranePotential x, MembranePotential y) {
    return std::beta(x, y);
}

double MathUtils::lnBeta(MembranePotential x, MembranePotential y) {
    return std::lbeta(x, y);
}

std::vector<MembranePotential> MathUtils::add(const std::vector<MembranePotential>& a, const std::vector<MembranePotential>& b) {
    if (a.size() != b.size()) throw std::invalid_argument("Vector sizes must match for addition");
    std::vector<MembranePotential> result;
    result.reserve(a.size());
    for (size_t i = 0; i < a.size(); i++) {
        result.push_back(a[i] + b[i]);
    }
    return result;
}

std::vector<MembranePotential> MathUtils::subtract(const std::vector<MembranePotential>& a, const std::vector<MembranePotential>& b) {
    if (a.size() != b.size()) throw std::invalid_argument("Vector sizes must match for subtraction");
    std::vector<MembranePotential> result;
    result.reserve(a.size());
    for (size_t i = 0; i < a.size(); i++) {
        result.push_back(a[i] - b[i]);
    }
    return result;
}

std::vector<MembranePotential> MathUtils::multiply(const std::vector<MembranePotential>& a, const std::vector<MembranePotential>& b) {
    if (a.size() != b.size()) throw std::invalid_argument("Vector sizes must match for multiplication");
    std::vector<MembranePotential> result;
    result.reserve(a.size());
    for (size_t i = 0; i < a.size(); i++) {
        result.push_back(a[i] * b[i]);
    }
    return result;
}

std::vector<MembranePotential> MathUtils::divide(const std::vector<MembranePotential>& a, const std::vector<MembranePotential>& b) {
    if (a.size() != b.size()) throw std::invalid_argument("Vector sizes must match for division");
    std::vector<MembranePotential> result;
    result.reserve(a.size());
    for (size_t i = 0; i < a.size(); i++) {
        if (b[i] == 0) throw std::invalid_argument("Division by zero");
        result.push_back(a[i] / b[i]);
    }
    return result;
}

MembranePotential MathUtils::dotProduct(const std::vector<MembranePotential>& a, const std::vector<MembranePotential>& b) {
    if (a.size() != b.size()) throw std::invalid_argument("Vector sizes must match for dot product");
    MembranePotential result = 0.0;
    for (size_t i = 0; i < a.size(); i++) {
        result += a[i] * b[i];
    }
    return result;
}

std::vector<MembranePotential> MathUtils::crossProduct(const std::vector<MembranePotential>& a, const std::vector<MembranePotential>& b) {
    if (a.size() != 3 || b.size() != 3) throw std::invalid_argument("Cross product requires 3D vectors");
    std::vector<MembranePotential> result(3);
    result[0] = a[1] * b[2] - a[2] * b[1];
    result[1] = a[2] * b[0] - a[0] * b[2];
    result[2] = a[0] * b[1] - a[1] * b[0];
    return result;
}

MembranePotential MathUtils::norm(const std::vector<MembranePotential>& v) {
    MembranePotential result = 0.0;
    for (MembranePotential val : v) {
        result += val * val;
    }
    return std::sqrt(result);
}

MembranePotential MathUtils::distance(const std::vector<MembranePotential>& a, const std::vector<MembranePotential>& b) {
    if (a.size() != b.size()) throw std::invalid_argument("Vector sizes must match for distance calculation");
    MembranePotential result = 0.0;
    for (size_t i = 0; i < a.size(); i++) {
        result += (a[i] - b[i]) * (a[i] - b[i]);
    }
    return std::sqrt(result);
}

std::vector<std::vector<MembranePotential>> MathUtils::add(const std::vector<std::vector<MembranePotential>>& a, const std::vector<std::vector<MembranePotential>>& b) {
    if (a.size() != b.size() || a.empty()) throw std::invalid_argument("Matrix sizes must match for addition");
    size_t cols = a[0].size();
    std::vector<std::vector<MembranePotential>> result(a.size(), std::vector<MembranePotential>(cols));
    for (size_t i = 0; i < a.size(); i++) {
        if (a[i].size() != b[i].size()) throw std::invalid_argument("Matrix row sizes must match");
        for (size_t j = 0; j < a[i].size(); j++) {
            result[i][j] = a[i][j] + b[i][j];
        }
    }
    return result;
}

std::vector<std::vector<MembranePotential>> MathUtils::subtract(const std::vector<std::vector<MembranePotential>>& a, const std::vector<std::vector<MembranePotential>>& b) {
    if (a.size() != b.size() || a.empty()) throw std::invalid_argument("Matrix sizes must match for subtraction");
    size_t cols = a[0].size();
    std::vector<std::vector<MembranePotential>> result(a.size(), std::vector<MembranePotential>(cols));
    for (size_t i = 0; i < a.size(); i++) {
        if (a[i].size() != b[i].size()) throw std::invalid_argument("Matrix row sizes must match");
        for (size_t j = 0; j < a[i].size(); j++) {
            result[i][j] = a[i][j] - b[i][j];
        }
    }
    return result;
}

std::vector<std::vector<MembranePotential>> MathUtils::multiply(const std::vector<std::vector<MembranePotential>>& a, const std::vector<std::vector<MembranePotential>>& b) {
    if (a.empty() || b.empty()) throw std::invalid_argument("Matrices cannot be empty");
    size_t aRows = a.size();
    size_t aCols = a[0].size();
    size_t bRows = b.size();
    size_t bCols = b[0].size();
    
    if (aCols != bRows) throw std::invalid_argument("Matrix dimensions must match for multiplication");
    
    std::vector<std::vector<MembranePotential>> result(aRows, std::vector<MembranePotential>(bCols, 0.0));
    
    for (size_t i = 0; i < aRows; i++) {
        for (size_t k = 0; k < aCols; k++) {
            for (size_t j = 0; j < bCols; j++) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    
    return result;
}

std::vector<std::vector<MembranePotential>> MathUtils::transpose(const std::vector<std::vector<MembranePotential>>& matrix) {
    if (matrix.empty()) return {};
    size_t rows = matrix.size();
    size_t cols = matrix[0].size();
    std::vector<std::vector<MembranePotential>> result(cols, std::vector<MembranePotential>(rows));
    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            result[j][i] = matrix[i][j];
        }
    }
    return result;
}

MembranePotential MathUtils::matrixDeterminant(const std::vector<std::vector<MembranePotential>>& matrix) {
    size_t n = matrix.size();
    if (matrix.empty() || matrix[0].empty()) throw std::invalid_argument("Matrix cannot be empty");
    if (n != matrix[0].size()) throw std::invalid_argument("Matrix must be square for determinant");
    
    if (n == 1) return matrix[0][0];
    if (n == 2) return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
    
    MembranePotential det = 0.0;
    for (size_t col = 0; col < n; col++) {
        std::vector<std::vector<MembranePotential>> submatrix(n - 1, std::vector<MembranePotential>(n - 1));
        for (size_t i = 1; i < n; i++) {
            size_t subCol = 0;
            for (size_t j = 0; j < n; j++) {
                if (j != col) {
                    submatrix[i - 1][subCol++] = matrix[i][j];
                }
            }
        }
        det += std::pow(-1.0, col) * matrix[0][col] * matrixDeterminant(submatrix);
    }
    
    return det;
}

std::vector<std::vector<MembranePotential>> MathUtils::matrixInverse(const std::vector<std::vector<MembranePotential>>& matrix) {
    size_t n = matrix.size();
    if (n != matrix[0].size()) throw std::invalid_argument("Matrix must be square for inverse");
    
    std::vector<std::vector<MembranePotential>> result(n, std::vector<MembranePotential>(n));
    
    if (n == 1) {
        result[0][0] = 1.0 / matrix[0][0];
        return result;
    }
    
    // Compute adjugate matrix
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            // Compute cofactor
            std::vector<std::vector<MembranePotential>> submatrix(n - 1, std::vector<MembranePotential>(n - 1));
            size_t subRow = 0;
            for (size_t r = 0; r < n; r++) {
                if (r == i) continue;
                size_t subCol = 0;
                for (size_t c = 0; c < n; c++) {
                    if (c == j) continue;
                    submatrix[subRow][subCol++] = matrix[r][c];
                }
                subRow++;
            }
            double cofactor = std::pow(-1.0, i + j) * matrixInverse(submatrix)[0][0];
            result[j][i] = cofactor; // Transpose
        }
    }
    
    // Divide by determinant
    double det = matrixDeterminant(matrix);
    if (std::abs(det) < 1e-10) {
        throw std::invalid_argument("Matrix is singular, cannot compute inverse");
    }
    
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < n; j++) {
            result[i][j] /= det;
        }
    }
    
    return result;
}

std::vector<std::vector<MembranePotential>> MathUtils::matrixMultiply(const std::vector<std::vector<MembranePotential>>& a, const std::vector<std::vector<MembranePotential>>& b) {
    return multiply(a, b);
}

MembranePotential MathUtils::trace(const std::vector<std::vector<MembranePotential>>& matrix) {
    if (matrix.size() != matrix[0].size()) throw std::invalid_argument("Matrix must be square for trace");
    
    MembranePotential trace = 0.0;
    for (size_t i = 0; i < matrix.size(); i++) {
        trace += matrix[i][i];
    }
    return trace;
}

std::vector<std::vector<MembranePotential>> MathUtils::matrixEigenvalues(const std::vector<std::vector<MembranePotential>>& matrix) {
    // For now, return the matrix itself as a placeholder
    // Real eigenvalue computation would require solving characteristic polynomial
    return matrix;
}

std::vector<std::vector<MembranePotential>> MathUtils::matrixEigenvectors(const std::vector<std::vector<MembranePotential>>& matrix) {
    // For now, return identity matrix as placeholder
    size_t n = matrix.size();
    std::vector<std::vector<MembranePotential>> result(n, std::vector<MembranePotential>(n, 0.0));
    for (size_t i = 0; i < n; i++) {
        result[i][i] = 1.0;
    }
    return result;
}

std::vector<std::vector<MembranePotential>> MathUtils::qrDecomposition(const std::vector<std::vector<MembranePotential>>& matrix) {
    // For now, return identity matrices as placeholder
    size_t n = matrix.size();
    std::vector<std::vector<MembranePotential>> Q(n, std::vector<MembranePotential>(n, 0.0));
    std::vector<std::vector<MembranePotential>> R(n, std::vector<MembranePotential>(n, 0.0));
    for (size_t i = 0; i < n; i++) {
        Q[i][i] = 1.0;
        R[i][i] = matrix[i][i];
    }
    return {Q, R};
}

std::vector<std::vector<MembranePotential>> MathUtils::luDecomposition(const std::vector<std::vector<MembranePotential>>& matrix) {
    // For now, return identity matrices as placeholder
    size_t n = matrix.size();
    std::vector<std::vector<MembranePotential>> L(n, std::vector<MembranePotential>(n, 0.0));
    std::vector<std::vector<MembranePotential>> U(n, std::vector<MembranePotential>(n, 0.0));
    for (size_t i = 0; i < n; i++) {
        L[i][i] = 1.0;
        U[i][i] = matrix[i][i];
    }
    return {L, U};
}

std::vector<std::vector<MembranePotential>> MathUtils::choleskyDecomposition(const std::vector<std::vector<MembranePotential>>& matrix) {
    // For now, return identity matrix as placeholder
    size_t n = matrix.size();
    std::vector<std::vector<MembranePotential>> result(n, std::vector<MembranePotential>(n, 0.0));
    for (size_t i = 0; i < n; i++) {
        result[i][i] = 1.0;
    }
    return result;
}

std::vector<std::vector<MembranePotential>> MathUtils::svd(const std::vector<std::vector<MembranePotential>>& matrix) {
    // For now, return identity matrices as placeholder
    size_t n = matrix.size();
    std::vector<std::vector<MembranePotential>> U(n, std::vector<MembranePotential>(n, 0.0));
    std::vector<std::vector<MembranePotential>> S(n, std::vector<MembranePotential>(n, 0.0));
    std::vector<std::vector<MembranePotential>> V(n, std::vector<MembranePotential>(n, 0.0));
    for (size_t i = 0; i < n; i++) {
        U[i][i] = 1.0;
        S[i][i] = 1.0;
        V[i][i] = 1.0;
    }
    return {U, S, V};
}

MembranePotential MathUtils::forwardEuler(MembranePotential y, MembranePotential dydt, MembranePotential dt) {
    return y + dydt * dt;
}

MembranePotential MathUtils::backwardEuler(MembranePotential y, MembranePotential dydt, MembranePotential dt) {
    return y / (1.0 - dydt * dt);
}

MembranePotential MathUtils::rungeKutta4(MembranePotential y, std::function<MembranePotential(MembranePotential)> f, MembranePotential dt) {
    MembranePotential k1 = f(y);
    MembranePotential k2 = f(y + dt * k1 / 2.0);
    MembranePotential k3 = f(y + dt * k2 / 2.0);
    MembranePotential k4 = f(y + dt * k3);
    return y + dt * (k1 + 2 * k2 + 2 * k3 + k4) / 6.0;
}

MembranePotential MathUtils::rungeKutta4(MembranePotential y, MembranePotential dydt, std::function<MembranePotential(MembranePotential, MembranePotential)> f, MembranePotential dt) {
    MembranePotential k1 = f(y, dydt);
    MembranePotential k2 = f(y + dt * k1 / 2.0, dydt);
    MembranePotential k3 = f(y + dt * k2 / 2.0, dydt);
    MembranePotential k4 = f(y + dt * k3, dydt);
    return y + dt * (k1 + 2 * k2 + 2 * k3 + k4) / 6.0;
}

std::pair<std::vector<MembranePotential>, MembranePotential> MathUtils::gradientDescent(const std::vector<MembranePotential>& x, const std::function<MembranePotential(const std::vector<MembranePotential>&)>& f, double learningRate, int maxIter, double tolerance) {
    std::vector<MembranePotential> current = x;
    double currentValue = f(current);
    
    for (int iter = 0; iter < maxIter; iter++) {
        // Compute gradient (simplified - would need proper gradient computation)
        std::vector<MembranePotential> gradient(current.size(), 0.0);
        for (size_t i = 0; i < current.size(); i++) {
            double epsilon = 1e-6;
            std::vector<MembranePotential> xPlus = current;
            xPlus[i] += epsilon;
            double fPlus = f(xPlus);
            std::vector<MembranePotential> xMinus = current;
            xMinus[i] -= epsilon;
            double fMinus = f(xMinus);
            gradient[i] = (fPlus - fMinus) / (2 * epsilon);
        }
        
        // Update
        for (size_t i = 0; i < current.size(); i++) {
            current[i] -= learningRate * gradient[i];
        }
        
        double newValue = f(current);
        if (std::abs(newValue - currentValue) < tolerance) {
            return {current, newValue};
        }
        
        currentValue = newValue;
    }
    
    return {current, currentValue};
}

std::pair<std::vector<MembranePotential>, MembranePotential> MathUtils::stochasticGradientDescent(const std::vector<MembranePotential>& x, const std::function<MembranePotential(const std::vector<MembranePotential>&)>& f, double learningRate, int maxIter, double tolerance) {
    // Simplified SGD - would need mini-batch implementation
    return gradientDescent(x, f, learningRate, maxIter, tolerance);
}

std::pair<std::vector<MembranePotential>, MembranePotential> MathUtils::momentumOptimization(const std::vector<MembranePotential>& x, const std::function<MembranePotential(const std::vector<MembranePotential>&)>& f, double learningRate, double momentum, int maxIter, double tolerance) {
    // Simplified momentum optimization
    std::vector<MembranePotential> current = x;
    std::vector<MembranePotential> velocity(current.size(), 0.0);
    double currentValue = f(current);
    
    for (int iter = 0; iter < maxIter; iter++) {
        // Compute gradient
        std::vector<MembranePotential> gradient(current.size(), 0.0);
        for (size_t i = 0; i < current.size(); i++) {
            double epsilon = 1e-6;
            std::vector<MembranePotential> xPlus = current;
            xPlus[i] += epsilon;
            double fPlus = f(xPlus);
            std::vector<MembranePotential> xMinus = current;
            xMinus[i] -= epsilon;
            double fMinus = f(xMinus);
            gradient[i] = (fPlus - fMinus) / (2 * epsilon);
        }
        
        // Update velocity
        for (size_t i = 0; i < current.size(); i++) {
            velocity[i] = momentum * velocity[i] - learningRate * gradient[i];
        }
        
        // Update position
        for (size_t i = 0; i < current.size(); i++) {
            current[i] += velocity[i];
        }
        
        double newValue = f(current);
        if (std::abs(newValue - currentValue) < tolerance) {
            return {current, newValue};
        }
        
        currentValue = newValue;
    }
    
    return {current, currentValue};
}

std::vector<MembranePotential> MathUtils::randomSample(const std::vector<MembranePotential>& population, int sampleSize, bool withReplacement) {
    if (sampleSize > population.size()) {
        throw std::invalid_argument("Sample size cannot exceed population size");
    }
    
    std::vector<MembranePotential> result;
    result.reserve(sampleSize);
    
    if (withReplacement) {
        for (int i = 0; i < sampleSize; i++) {
            result.push_back(population[randomUniformInt(0, population.size() - 1)]);
        }
    } else {
        // Without replacement - use Fisher-Yates shuffle
        std::vector<MembranePotential> copy = population;
        for (int i = sampleSize - 1; i >= 0; i--) {
            int j = randomUniformInt(0, i);
            result.push_back(copy[j]);
            copy[j] = copy[i];
        }
    }
    
    return result;
}

std::vector<MembranePotential> MathUtils::bootstrap(const std::vector<MembranePotential>& data, int sampleSize) {
    if (sampleSize < 0) sampleSize = data.size();
    if (sampleSize > data.size()) sampleSize = data.size();
    return randomSample(data, sampleSize, true);
}

std::vector<MembranePotential> MathUtils::jackknife(const std::vector<MembranePotential>& data, int leaveOneOut) {
    std::vector<MembranePotential> result;
    result.reserve(leaveOneOut);
    
    for (int i = 0; i < leaveOneOut; i++) {
        double sum = 0.0;
        for (size_t j = 0; j < data.size(); j++) {
            if (j != static_cast<size_t>(i)) {
                sum += data[j];
            }
        }
        result.push_back(sum / (data.size() - 1));
    }
    
    return result;
}

std::vector<MembranePotential> MathUtils::crossValidation(const std::vector<MembranePotential>& data, int folds) {
    if (folds <= 1 || folds > data.size()) {
        throw std::invalid_argument("Invalid number of folds");
    }
    
    int foldSize = data.size() / folds;
    std::vector<MembranePotential> result;
    
    for (int i = 0; i < folds; i++) {
        double sum = 0.0;
        int count = 0;
        
        for (size_t j = 0; j < data.size(); j++) {
            if ((j >= i * foldSize && j < (i + 1) * foldSize) || (i == folds - 1 && j == data.size() - 1)) {
                // Skip this fold
                continue;
            }
            
            sum += data[j];
            count++;
        }
        
        if (count > 0) {
            result.push_back(sum / count);
        } else {
            result.push_back(0.0);
        }
    }
    
    return result;
}

double MathUtils::tTest(const std::vector<MembranePotential>& a, const std::vector<MembranePotential>& b) {
    if (a.size() != b.size() || a.size() < 2) {
        throw std::invalid_argument("Samples must have same size and at least 2 elements");
    }
    
    double meanA = mean(a);
    double meanB = mean(b);
    double varA = variance(a);
    double varB = variance(b);
    
    double pooledVar = ((a.size() - 1) * varA + (b.size() - 1) * varB) / (a.size() + b.size() - 2);
    double stdErr = std::sqrt(pooledVar * (1.0 / a.size() + 1.0 / b.size()));
    
    return (meanA - meanB) / stdErr;
}

double MathUtils::chiSquareTest(const std::vector<MembranePotential>& observed, const std::vector<MembranePotential>& expected) {
    if (observed.size() != expected.size()) {
        throw std::invalid_argument("Observed and expected vectors must have the same size");
    }
    
    double chiSquare = 0.0;
    for (size_t i = 0; i < observed.size(); i++) {
        if (expected[i] == 0) {
            throw std::invalid_argument("Expected value cannot be zero");
        }
        chiSquare += (observed[i] - expected[i]) * (observed[i] - expected[i]) / expected[i];
    }
    
    return chiSquare;
}

double MathUtils::fTest(const std::vector<MembranePotential>& a, const std::vector<MembranePotential>& b) {
    if (a.size() != b.size() || a.size() < 2) {
        throw std::invalid_argument("Samples must have same size and at least 2 elements");
    }
    
    double varA = variance(a);
    double varB = variance(b);
    
    if (varB == 0) {
        throw std::invalid_argument("Variance of second sample cannot be zero");
    }
    
    return varA / varB;
}

double MathUtils::anova(const std::vector<std::vector<MembranePotential>>& groups) {
    if (groups.empty()) {
        throw std::invalid_argument("At least one group required");
    }
    
    int totalN = 0;
    double grandMean = 0.0;
    
    for (const auto& group : groups) {
        totalN += group.size();
        for (double value : group) {
            grandMean += value;
        }
    }
    
    grandMean /= totalN;
    
    double betweenSS = 0.0;
    double withinSS = 0.0;
    
    for (const auto& group : groups) {
        double groupMean = mean(group);
        betweenSS += group.size() * (groupMean - grandMean) * (groupMean - grandMean);
        for (double value : group) {
            withinSS += (value - groupMean) * (value - groupMean);
        }
    }
    
    int k = groups.size();
    int nPerGroup = totalN / k;
    
    double dfBetween = k - 1;
    double dfWithin = totalN - k;
    
    double msBetween = betweenSS / dfBetween;
    double msWithin = withinSS / dfWithin;
    
    if (msWithin == 0) {
        return 0.0;
    }
    
    return msBetween / msWithin;
}

std::string MathUtils::toString(MembranePotential value, int precision) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}

std::vector<MembranePotential> MathUtils::toVector(const std::string& str, char delimiter) {
    std::vector<MembranePotential> result;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        result.push_back(std::stod(token));
    }
    return result;
}

std::string MathUtils::fromVector(const std::vector<MembranePotential>& values, char delimiter) {
    std::stringstream ss;
    for (size_t i = 0; i < values.size(); i++) {
        if (i > 0) ss << delimiter;
        ss << values[i];
    }
    return ss.str();
}

MembranePotential MathUtils::normalize(const std::vector<MembranePotential>& values, MembranePotential min, MembranePotential max) {
    if (values.empty()) return 0.0;
    
    MembranePotential minVal = *std::min_element(values.begin(), values.end());
    MembranePotential maxVal = *std::max_element(values.begin(), values.end());
    
    if (minVal == maxVal) return 0.5;
    
    MembranePotential scale = (max - min) / (maxVal - minVal);
    return scale;
}

MembranePotential MathUtils::scale(const std::vector<MembranePotential>& values, MembranePotential mean, MembranePotential stddev) {
    if (values.empty()) return 0.0;
    
    MembranePotential m = MathUtils::mean(values);
    MembranePotential s = std::sqrt(variance(values));
    
    if (s == 0) return 0.0;
    
    return (m - mean) / s;
}

MembranePotential MathUtils::standardize(const std::vector<MembranePotential>& values) {
    return scale(values, 0.0, 1.0);
}

std::string MathUtils::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

std::vector<std::string> MathUtils::split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        result.push_back(trim(token));
    }
    return result;
}

std::string MathUtils::join(const std::vector<std::string>& strings, char delimiter) {
    std::stringstream ss;
    for (size_t i = 0; i < strings.size(); i++) {
        if (i > 0) ss << delimiter;
        ss << strings[i];
    }
    return ss.str();
}

bool MathUtils::startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

bool MathUtils::endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string MathUtils::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string MathUtils::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}

std::string MathUtils::formatTime(double seconds) {
    int hours = static_cast<int>(seconds / 3600);
    int minutes = static_cast<int>((seconds - hours * 3600) / 60);
    double secs = seconds - hours * 3600 - minutes * 60;
    
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << hours << ":"
       << std::setw(2) << minutes << ":"
       << std::fixed << std::setprecision(2) << secs;
    return ss.str();
}

std::string MathUtils::formatDate(const std::tm& time) {
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << time.tm_year + 1900 << "-"
       << std::setw(2) << time.tm_mon + 1 << "-"
       << std::setw(2) << time.tm_mday;
    return ss.str();
}

std::tm MathUtils::parseDate(const std::string& dateStr) {
    std::tm time = {};
    std::stringstream ss(dateStr);
    char dash;
    ss >> time.tm_year >> dash >> time.tm_mon >> dash >> time.tm_mday;
    time.tm_year -= 1900;
    time.tm_mon--;
    return time;
}

double MathUtils::currentTimeMs() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

double MathUtils::currentTimeUs() {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
}

std::string MathUtils::toHexColor(MembranePotential r, MembranePotential g, MembranePotential b, MembranePotential a) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(r * 255)
       << std::setw(2) << static_cast<int>(g * 255)
       << std::setw(2) << static_cast<int>(b * 255)
       << std::setw(2) << static_cast<int>(a * 255);
    return "#" + ss.str();
}

std::string MathUtils::toRgbColor(MembranePotential r, MembranePotential g, MembranePotential b) {
    std::stringstream ss;
    ss << "rgb(" << static_cast<int>(r * 255) << ","
       << static_cast<int>(g * 255) << ","
       << static_cast<int>(b * 255) << ")";
    return ss.str();
}

void MathUtils::fromHexColor(const std::string& hex, MembranePotential& r, MembranePotential& g, MembranePotential& b, MembranePotential& a) {
    if (hex[0] == '#') {
        hex.erase(0, 1);
    }
    
    std::istringstream ss(hex);
    int hexValue;
    ss >> std::hex >> hexValue;
    r = hexValue / (255.0 * 256 * 256);
    hexValue = (hexValue % (255 * 256 * 256)) / (255 * 256);
    g = hexValue / 255.0;
    hexValue = hexValue % 255;
    b = hexValue / 255.0;
    
    if (hex.size() == 8) {
        int alphaValue = std::stoi(hex.substr(6, 2), nullptr, 16);
        a = alphaValue / 255.0;
    } else {
        a = 1.0;
    }
}

std::string MathUtils::leftPad(const std::string& str, size_t width, char pad) {
    if (str.size() >= width) return str;
    return std::string(width - str.size(), pad) + str;
}

std::string MathUtils::rightPad(const std::string& str, size_t width, char pad) {
    if (str.size() >= width) return str;
    return str + std::string(width - str.size(), pad);
}

std::string MathUtils::center(const std::string& str, size_t width, char pad) {
    if (str.size() >= width) return str;
    size_t leftPad = (width - str.size()) / 2;
    size_t rightPad = width - str.size() - leftPad;
    return std::string(leftPad, pad) + str + std::string(rightPad, pad);
}

std::string MathUtils::ellipsis(const std::string& str, size_t maxLength) {
    if (str.size() <= maxLength) return str;
    return str.substr(0, maxLength - 3) + "...";
}

bool MathUtils::fileExists(const std::string& path) {
    std::ifstream file(path);
    return file.good();
}

bool MathUtils::directoryExists(const std::string& path) {
    DIR* dir = opendir(path.c_str());
    if (dir) {
        closedir(dir);
        return true;
    }
    return false;
}

std::vector<std::string> MathUtils::listFiles(const std::string& directory, const std::string& pattern) {
    std::vector<std::string> result;
    DIR* dir = opendir(directory.c_str());
    if (!dir) return result;
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string filename = entry->d_name;
        if (fnmatch(pattern.c_str(), filename.c_str(), FNM_PATHNAME) == 0) {
            result.push_back(filename);
        }
    }
    
    closedir(dir);
    return result;
}

std::vector<std::string> MathUtils::listDirectories(const std::string& directory) {
    std::vector<std::string> result;
    DIR* dir = opendir(directory.c_str());
    if (!dir) return result;
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_DIR) {
            std::string dirname = entry->d_name;
            if (dirname != "." && dirname != "..") {
                result.push_back(dirname);
            }
        }
    }
    
    closedir(dir);
    return result;
}

bool MathUtils::createDirectory(const std::string& path) {
    return mkdir(path.c_str(), 0777) == 0;
}

bool MathUtils::deleteFile(const std::string& path) {
    return std::remove(path.c_str()) == 0;
}

bool MathUtils::deleteDirectory(const std::string& path) {
    DIR* dir = opendir(path.c_str());
    if (!dir) return false;
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string filename = entry->d_name;
        if (filename == "." || filename == "..") continue;
        
        std::string fullPath = path + "/" + filename;
        if (entry->d_type == DT_DIR) {
            deleteDirectory(fullPath);
        } else {
            deleteFile(fullPath);
        }
    }
    
    closedir(dir);
    return rmdir(path.c_str()) == 0;
}

std::string MathUtils::getFileName(const std::string& path) {
    size_t pos = path.find_last_of("/\\");
    if (pos == std::string::npos) return path;
    return path.substr(pos + 1);
}

std::string MathUtils::getFileExtension(const std::string& path) {
    size_t pos = path.find_last_of(".");
    if (pos == std::string::npos || pos == 0) return "";
    return path.substr(pos + 1);
}

std::string MathUtils::removeFileExtension(const std::string& path) {
    size_t pos = path.find_last_of(".");
    if (pos == std::string::npos || pos == 0) return path;
    return path.substr(0, pos);
}

std::string MathUtils::getDirectoryName(const std::string& path) {
    size_t pos = path.find_last_of("/\\");
    if (pos == std::string::npos) return ".";
    return path.substr(0, pos);
}

std::string MathUtils::absolutePath(const std::string& path) {
    char buffer[1024];
    return realpath(path.c_str(), buffer) ? std::string(buffer) : path;
}

std::string MathUtils::relativePath(const std::string& path, const std::string& base) {
    // Simplified implementation - would need actual relative path calculation
    return path;
}

bool MathUtils::ping(const std::string& host) {
    // Simplified implementation - would need actual ping
    return false;
}

std::string MathUtils::getHostName() {
    char hostname[256];
    hostname[0] = '\0';
    gethostname(hostname, sizeof(hostname));
    return std::string(hostname);
}

std::string MathUtils::getIPAddress(const std::string& hostName) {
    // Simplified implementation - would need actual IP resolution
    return "";
}

std::vector<std::string> MathUtils::resolveDNS(const std::string& hostName) {
    // Simplified implementation - would need actual DNS resolution
    return {};
}

std::vector<std::string> MathUtils::listProcesses() {
    // Simplified implementation - would need actual process listing
    return {};
}

std::string MathUtils::getProcessName(int pid) {
    // Simplified implementation - would need actual process name retrieval
    return "";
}

bool MathUtils::killProcess(int pid) {
    // Simplified implementation - would need actual process termination
    return false;
}

bool MathUtils::isProcessRunning(int pid) {
    // Simplified implementation - would need actual process checking
    return false;
}

std::string MathUtils::getProcessCommandLine(int pid) {
    // Simplified implementation - would need actual command line retrieval
    return "";
}

std::string MathUtils::getEnvironmentVariable(const std::string& name) {
    const char* value = std::getenv(name.c_str());
    return value ? std::string(value) : "";
}

bool MathUtils::setEnvironmentVariable(const std::string& name, const std::string& value) {
    return setenv(name.c_str(), value.c_str(), 1) == 0;
}

bool MathUtils::removeEnvironmentVariable(const std::string& name) {
    return unsetenv(name.c_str()) == 0;
}

std::vector<std::string> MathUtils::listEnvironmentVariables() {
    std::vector<std::string> result;
    extern char** environ;
    for (char** env = environ; *env != nullptr; env++) {
        result.push_back(std::string(*env));
    }
    return result;
}

std::string MathUtils::getOperatingSystem() {
    // Simplified implementation
    return "Unknown";
}

std::string MathUtils::getArchitecture() {
    // Simplified implementation
    return "Unknown";
}

std::string MathUtils::getProcessorName() {
    // Simplified implementation
    return "Unknown";
}

double MathUtils::getCpuUsage() {
    // Simplified implementation
    return 0.0;
}

double MathUtils::getMemoryUsage() {
    // Simplified implementation
    return 0.0;
}

double MathUtils::getDiskUsage(const std::string& path) {
    // Simplified implementation
    return 0.0;
}

std::vector<std::string> MathUtils::getRunningThreads() {
    // Simplified implementation
    return {};
}

double MathUtils::randomUniform(double min, double max) {
    return min + (max - min) * (static_cast<double>(std::rand()) / RAND_MAX);
}

double MathUtils::randomNormal(double mean, double stddev) {
    double u1 = randomUniform(), u2 = randomUniform();
    double randStdNormal = std::sqrt(-2.0 * std::log(u1)) *
        std::cos(2.0 * M_PI * u2);
    return mean + stddev * randStdNormal;
}

double MathUtils::randomExponential(double lambda) {
    double u = randomUniform();
    return -std::log(1.0 - u) / lambda;
}

double MathUtils::randomGamma(double shape, double scale) {
    if (shape <= 0) {
        throw std::invalid_argument("Shape parameter must be positive");
    }
    
    double sum = 0.0;
    for (int i = 0; i < static_cast<int>(shape); i++) {
        sum += randomExponential(1.0);
    }
    
    return sum * scale;
}

double MathUtils::randomBeta(double alpha, double beta) {
    double x = randomGamma(alpha, 1.0);
    double y = randomGamma(beta, 1.0);
    return x / (x + y);
}

double MathUtils::randomChiSquare(double degreesOfFreedom) {
    double sum = 0.0;
    for (int i = 0; i < static_cast<int>(degreesOfFreedom); i++) {
        sum += randomNormal() * randomNormal();
    }
    return sum;
}

double MathUtils::randomStudentT(double degreesOfFreedom) {
    double z = randomNormal();
    double chi2 = randomChiSquare(degreesOfFreedom);
    return z / std::sqrt(chi2 / degreesOfFreedom);
}

double MathUtils::randomF(double df1, double df2) {
    double chi21 = randomChiSquare(df1);
    double chi22 = randomChiSquare(df2);
    return (chi21 / df1) / (chi22 / df2);
}

double MathUtils::randomCauchy(double location, double scale) {
    double u = randomUniform();
    return location + scale * std::tan(M_PI * (u - 0.5));
}

double MathUtils::randomLaplace(double mu, double b) {
    double u = randomUniform();
    if (u < 0.5) {
        return mu + b * std::log(2 * u);
    } else {
        return mu - b * std::log(2 * (1 - u));
    }
}

double MathUtils::randomLogistic(double mu, double s) {
    double u = randomUniform();
    return mu + s * std::log(u / (1.0 - u));
}

double MathUtils::randomWeibull(double shape, double scale) {
    double u = randomUniform();
    return scale * std::pow(-std::log(1.0 - u), 1.0 / shape);
}

double MathUtils::randomUniformInt(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    std::mt19937 gen(std::random_device{}());
    return dist(gen);
}

double MathUtils::randomBinomial(int n, double p) {
    std::binomial_distribution<int> dist(n, p);
    std::mt19937 gen(std::random_device{}());
    return dist(gen);
}

double MathUtils::randomPoisson(double lambda) {
    std::poisson_distribution<int> dist(lambda);
    std::mt19937 gen(std::random_device{}());
    return dist(gen);
}

double MathUtils::randomHypergeometric(int N, int K, int n) {
    std::hypergeometric_distribution<int> dist(N, K, n);
    std::mt19937 gen(std::random_device{}());
    return dist(gen);
}

double MathUtils::randomGeometric(double p) {
    std::geometric_distribution<int> dist(p);
    std::mt19937 gen(std::random_device{}());
    return dist(gen);
}

double MathUtils::randomPascal(int r, double p) {
    std::negative_binomial_distribution<int> dist(r, p);
    std::mt19937 gen(std::random_device{}());
    return dist(gen);
}

double MathUtils::randomNegativeBinomial(double p, int r) {
    std::negative_binomial_distribution<int> dist(r, p);
    std::mt19937 gen(std::random_device{}());
    return dist(gen);
}

double MathUtils::randomPoissonBinomial(const std::vector<double>& probabilities) {
    int result = 0;
    for (double p : probabilities) {
        if (p > 0 && p <= 1) {
            std::bernoulli_distribution dist(p);
            std::mt19937 gen(std::random_device{}());
            if (dist(gen)) {
                result++;
            }
        }
    }
    return result;
}

double MathUtils::randomWrightFisher(int N, double p) {
    int count = 0;
    for (int i = 0; i < N; i++) {
        if (randomUniform() < p) {
            count++;
        }
    }
    return count;
}

double MathUtils::randomMoran(int N, double p) {
    bool old = randomUniform() < p;
    int newCount = 0;
    for (int i = 0; i < N - 1; i++) {
        if (randomUniform() < p) {
            newCount++;
        }
    }
    if (old) {
        newCount++;
    } else {
        newCount--;
    }
    return newCount;
}

double MathUtils::randomEwens(double theta, int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        double p = theta / (theta + i);
        if (randomUniform() < p) {
            sum++;
        }
    }
    return sum;
}

double MathUtils::randomBetaBinomial(int n, double alpha, double beta) {
    double p = randomBeta(alpha, beta);
    return randomBinomial(n, p);
}

double MathUtils::randomBetaNegativeBinomial(int r, double alpha, double beta) {
    double p = randomBeta(alpha, beta);
    return randomNegativeBinomial(p, r);
}

double MathUtils::randomGammaBinomial(int n, double alpha, double beta) {
    double p = randomGamma(alpha, beta);
    p = std::min(1.0, p / (1.0 + p));
    return randomBinomial(n, p);
}

double MathUtils::randomBinomialBeta(double p, double alpha, double beta) {
    double alphaPrime = randomBeta(alpha, beta);
    return randomBinomial(p, alphaPrime);
}

double MathUtils::randomMixture(const std::vector<double>& weights,
                               const std::vector<std::function<double()>>& generators,
                               const std::vector<std::vector<double>>& params) {
    if (weights.size() != generators.size()) {
        throw std::invalid_argument("Weights and generators must have the same size");
    }
    if (params.size() != generators.size()) {
        throw std::invalid_argument("Params must have the same size as generators");
    }
    
    double u = randomUniform();
    double cumulative = 0.0;
    size_t selected = 0;
    
    for (size_t i = 0; i < weights.size(); i++) {
        cumulative += weights[i];
        if (u <= cumulative) {
            selected = i;
            break;
        }
    }
    
    return generators[selected]();
}

double MathUtils::randomMixtureOfNormals(const std::vector<double>& weights,
                                         const std::vector<double>& means,
                                         const std::vector<double>& stddevs) {
    return randomMixture(weights,
        { [means](double m) { return randomNormal(m, 1.0); } },
        { {means[0], stddevs[0]} });
}

double MathUtils::randomMixtureOfExponentials(const std::vector<double>& weights,
                                             const std::vector<double>& lambdas) {
    return randomMixture(weights,
        { [lambdas](double l) { return randomExponential(l); } },
        { {lambdas[0]} });
}

double MathUtils::randomMixtureOfGammas(const std::vector<double>& weights,
                                        const std::vector<double>& shapes,
                                        const std::vector<double>& scales) {
    return randomMixture(weights,
        { [shapes, scales](double s, double sc) { return randomGamma(s, sc); } },
        { {shapes[0], scales[0]} });
}

double MathUtils::randomMixtureOfBetas(const std::vector<double>& weights,
                                       const std::vector<double>& alphas,
                                       const std::vector<double>& betas) {
    return randomMixture(weights,
        { [alphas, betas](double a, double b) { return randomBeta(a, b); } },
        { {alphas[0], betas[0]} });
}

double MathUtils::randomMixtureOfBinomials(int n,
                                           const std::vector<double>& weights,
                                           const std::vector<double>& ps) {
    return randomMixture(weights,
        { [n, ps](double p) { return randomBinomial(n, p); } },
        { {ps[0]} });
}

double MathUtils::randomMixtureOfPoissons(const std::vector<double>& weights,
                                          const std::vector<double>& lambdas) {
    return randomMixture(weights,
        { [lambdas](double l) { return randomPoisson(l); } },
        { {lambdas[0]} });
}

double MathUtils::randomMixtureOfChiSquares(const std::vector<double>& weights,
                                           const std::vector<double>& dfs) {
    return randomMixture(weights,
        { [dfs](double df) { return randomChiSquare(df); } },
        { {dfs[0]} });
}

double MathUtils::randomMixtureOfFs(const std::vector<double>& weights,
                                    const std::vector<double>& df1s,
                                    const std::vector<double>& df2s) {
    return randomMixture(weights,
        { [df1s, df2s](double d1, double d2) { return randomF(d1, d2); } },
        { {df1s[0], df2s[0]} });
}

double MathUtils::randomMixtureOfStudentTs(const std::vector<double>& weights,
                                           const std::vector<double>& dfs) {
    return randomMixture(weights,
        { [dfs](double df) { return randomStudentT(df); } },
        { {dfs[0]} });
}

double MathUtils::randomMixtureOfBetaPrimes(const std::vector<double>& weights,
                                             const std::vector<double>& alphas,
                                             const std::vector<double>& betas) {
    return randomMixture(weights,
        { [alphas, betas](double a, double b) { return randomBetaPrime(a, b); } },
        { {alphas[0], betas[0]} });
}

double MathUtils::randomMixtureOfLaplaces(const std::vector<double>& weights,
                                          const std::vector<double>& mus,
                                          const std::vector<double>& bs) {
    return randomMixture(weights,
        { [mus, bs](double m, double b) { return randomLaplace(m, b); } },
        { {mus[0], bs[0]} });
}

double MathUtils::randomMixtureOfLogistics(const std::vector<double>& weights,
                                           const std::vector<double>& mus,
                                           const std::vector<double>& ss) {
    return randomMixture(weights,
        { [mus, ss](double m, double s) { return randomLogistic(m, s); } },
        { {mus[0], ss[0]} });
}

double MathUtils::randomMixtureOfWeibulls(const std::vector<double>& weights,
                                          const std::vector<double>& shapes,
                                          const std::vector<double>& scales) {
    return randomMixture(weights,
        { [shapes, scales](double s, double sc) { return randomWeibull(s, sc); } },
        { {shapes[0], scales[0]} });
}

double MathUtils::randomMixtureOfCauchys(const std::vector<double>& weights,
                                         const std::vector<double>& locations,
                                         const std::vector<double>& scales) {
    return randomMixture(weights,
        { [locations, scales](double l, double sc) { return randomCauchy(l, sc); } },
        { {locations[0], scales[0]} });
}

double MathUtils::randomMixtureOfDoubleExponentials(const std::vector<double>& weights,
                                                     const std::vector<double>& lambda1s,
                                                     const std::vector<double>& lambda2s) {
    return randomMixture(weights,
        { [lambda1s, lambda2s](double l1, double l2) { return randomDoubleExponential(l1, l2); } },
        { {lambda1s[0], lambda2s[0]} });
}

bool MathUtils::isEven(int n) {
    return n % 2 == 0;
}

bool MathUtils::isOdd(int n) {
    return n % 2 != 0;
}

bool MathUtils::isPrime(int n) {
    if (n <= 1) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    
    for (int i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return false;
    }
    
    return true;
}

int MathUtils::nextPrime(int n) {
    while (!isPrime(++n));
    return n;
}

int MathUtils::previousPrime(int n) {
    while (!isPrime(--n)) {
        if (n <= 1) return 2;
    }
    return n;
}

std::vector<int> MathUtils::primeFactors(int n) {
    std::vector<int> factors;
    int divisor = 2;
    
    while (n > 1) {
        while (n % divisor == 0) {
            factors.push_back(divisor);
            n /= divisor;
        }
        divisor++;
    }
    
    return factors;
}

int MathUtils::gcd(int a, int b) {
    return b == 0 ? a : gcd(b, a % b);
}

int MathUtils::lcm(int a, int b) {
    return a * b / gcd(a, b);
}

std::vector<int> MathUtils::factors(int n) {
    std::vector<int> factors;
    for (int i = 1; i <= std::sqrt(n); i++) {
        if (n % i == 0) {
            factors.push_back(i);
            if (i != n / i) {
                factors.push_back(n / i);
            }
        }
    }
    std::sort(factors.begin(), factors.end());
    return factors;
}

std::vector<int> MathUtils::divisors(int n) {
    return factors(n);
}

int MathUtils::digitSum(int n) {
    int sum = 0;
    while (n > 0) {
        sum += n % 10;
        n /= 10;
    }
    return sum;
}

int MathUtils::digitCount(int n) {
    if (n == 0) return 1;
    int count = 0;
    while (n > 0) {
        count++;
        n /= 10;
    }
    return count;
}

int MathUtils::reverseDigits(int n) {
    int reversed = 0;
    while (n > 0) {
        reversed = reversed * 10 + n % 10;
        n /= 10;
    }
    return reversed;
}

bool MathUtils::isPalindrome(int n) {
    return n == reverseDigits(n);
}

int MathUtils::factorial(int n) {
    if (n < 0) throw std::invalid_argument("Factorial of negative number");
    if (n == 0 || n == 1) return 1;
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

int MathUtils::nChooseK(int n, int k) {
    if (k < 0 || k > n) throw std::invalid_argument("Invalid combination");
    return factorial(n) / (factorial(k) * factorial(n - k));
}

double MathUtils::permutations(int n, int k) {
    return factorial(n) / factorial(n - k);
}

double MathUtils::combinations(int n, int k) {
    return nChooseK(n, k);
}

double MathUtils::permutationsWithRepetitions(int n, const std::vector<int>& repetitions) {
    double result = factorial(n);
    for (int r : repetitions) {
        result /= factorial(r);
    }
    return result;
}

double MathUtils::combinationsWithRepetitions(int n, int k) {
    return nChooseK(n + k - 1, k);
}

double MathUtils::stirlingNumbers(int n, int k) {
    if (k == 1 || k == n) return 1.0;
    if (k == 0 || k > n) return 0.0;
    return k * stirlingNumbers(n - 1, k) + stirlingNumbers(n - 1, k - 1);
}

double MathUtils::bernoulliNumber(int n) {
    if (n == 0) return 1.0;
    if (n == 1) return -1.0 / 2.0;
    return 0.0;
}

double MathUtils::eulerNumber(int n) {
    // Placeholder - would need actual implementation
    return 0.0;
}

double MathUtils::harmonicNumber(int n) {
    double sum = 0.0;
    for (int i = 1; i <= n; i++) {
        sum += 1.0 / i;
    }
    return sum;
}

double MathUtils::zeta(int n, double x) {
    // Placeholder - would need actual implementation
    return 0.0;
}

double MathUtils::erf(double x) {
    // Approximation of error function
    double t = 1.0 / (1.0 + 0.5 * std::abs(x));
    double y = t * std::exp(-x * x - 1.26551223 + t * (1.00002368 +
         t * (0.37409196 + t * (0.09678418 + t * (-0.18628806 +
         t * (0.27886807 - t * (1.00002267 - t * (1.98733191 *
         t * (-0.16092547)))))))));
    return x >= 0 ? y : -y;
}

double MathUtils::erfc(double x) {
    return 1.0 - erf(x);
}

double MathUtils::invErf(double x) {
    // Approximation of inverse error function
    double sign = x >= 0 ? 1.0 : -1.0;
    x = std::abs(x);
    double ln = std::log(1.0 - x * x);
    double y = std::sqrt(std::sqrt(std::pow(4.0 / M_PI + 0.5 * ln, 2.0) - ln));
    return sign * y;
}

double MathUtils::invErfc(double x) {
    return invErf(1.0 - x);
}

double MathUtils::gammaFunction(double x) {
    return std::tgamma(x);
}

double MathUtils::lnGammaFunction(double x) {
    return std::lgamma(x);
}

double MathUtils::digammaFunction(double x) {
    return std::digamma(x);
}

double MathUtils::trigammaFunction(double x) {
    return std::trigamma(x);
}

double MathUtils::betaFunction(double x, double y) {
    return std::beta(x, y);
}

double MathUtils::lnBetaFunction(double x, double y) {
    return std::lbeta(x, y);
}

double MathUtils::incompleteBeta(double x, double a, double b) {
    return std::beta_inc(x, a, b);
}

double MathUtils::incompleteGamma(double x, double a) {
    return std::gamma_inc(a, x);
}

double MathUtils::regularizedIncompleteBeta(double x, double a, double b) {
    return std::beta_inc(x, a, b);
}

double MathUtils::regularizedIncompleteGamma(double x, double a) {
    return std::gamma_inc(a, x);
}

double MathUtils::betainc(double x, double a, double b) {
    return std::beta_inc(x, a, b);
}

double MathUtils::betaincinv(double y, double a, double b) {
    // Placeholder - would need actual implementation
    return 0.0;
}

double MathUtils::gammaln(double x) {
    return std::lgamma(x);
}

double MathUtils::gammainc(double x, double a, double b) {
    return std::gamma_inc(a, x * b);
}

double MathUtils::gammaincinv(double p, double a, double b) {
    // Placeholder - would need actual implementation
    return 0.0;
}

double MathUtils::rbinom(double n, double p) {
    return randomBinomial(static_cast<int>(n), p);
}

double MathUtils::rnorm(double mean, double sigma) {
    return randomNormal(mean, sigma);
}

double MathUtils::rexp(double lambda) {
    return randomExponential(lambda);
}

double MathUtils::rgamma(double shape, double scale) {
    return randomGamma(shape, scale);
}

double MathUtils::rbeta(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rchisq(double df) {
    return randomChiSquare(df);
}

double MathUtils::rf(double df1, double df2) {
    return randomF(df1, df2);
}

double MathUtils::rt(double df) {
    return randomStudentT(df);
}

double MathUtils::rdist(double df1, double df2, double ncp) {
    // Simplified implementation - would need actual non-central F distribution
    return randomF(df1, df2);
}

double MathUtils::rhyper(double m, double n, double k) {
    return randomHypergeometric(static_cast<int>(m), static_cast<int>(n), static_cast<int>(k));
}

double MathUtils::rgeom(double p) {
    return randomGeometric(p);
}

double MathUtils::rpois(double lambda) {
    return randomPoisson(lambda);
}

double MathUtils::rlnorm(double mu, double sigma) {
    double u = randomUniform();
    return std::exp(mu + sigma * std::sqrt(-2.0 * std::log(u)) *
        std::cos(2.0 * M_PI * randomUniform()));
}

double MathUtils::rlogis(double location, double scale) {
    double u = randomUniform();
    return location - scale * std::log(u / (1.0 - u));
}

double MathUtils::rlogis2(double location, double scale) {
    double u = randomUniform();
    return location - scale * std::log((1.0 - u) / u);
}

double MathUtils::rtukey(double df1, double df2, double ncp) {
    // Simplified implementation - would need actual Tukey lambda distribution
    return randomStudentT(df1);
}

double MathUtils::rbeta1(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta2(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta3(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta4(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta5(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta6(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta7(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta8(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta9(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta10(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta11(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta12(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta13(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta14(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta15(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta16(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta17(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta18(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta19(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta20(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta21(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta22(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta23(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta24(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta25(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta26(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta27(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta28(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta29(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta30(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta31(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta32(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta33(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta34(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta35(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta36(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta37(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta38(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta39(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta40(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta41(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta42(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta43(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta44(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta45(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta46(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta47(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta48(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta49(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta50(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta51(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta52(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta53(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta54(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta55(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta56(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta57(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta58(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta59(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta60(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta61(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta62(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta63(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta64(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta65(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta66(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta67(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta68(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta69(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta70(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta71(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta72(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta73(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta74(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta75(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta76(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta77(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta78(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta79(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta80(double alpha, double beta) {
    return randomBeta(alpha, beta);
}

double MathUtils::rbeta81(double alpha, double beta) {
    return randomBeta(alpha, beta);
}
double MathUtils::rbeta82(double alpha, double beta) {
    return randomBeta(alpha, beta);
}
double MathUtils::rbeta83(double alpha, double beta) {
    return randomBeta(alpha, beta);
}
double MathUtils::rbeta84(double alpha, double beta) {
    return randomBeta(alpha, beta);
}
double MathUtils::rbeta85(double alpha, double beta) {
    return randomBeta(alpha, beta);
}
double MathUtils::rbeta86(double alpha, double beta) {
    return randomBeta(alpha, beta);
}
double MathUtils::rbeta87(double alpha, double beta) {
    return randomBeta(alpha, beta);
}
double MathUtils::rbeta88(double alpha, double beta) {
    return randomBeta(alpha, beta);
}
double MathUtils::rbeta89(double alpha, double beta) {
    return randomBeta(alpha, beta);
}
double MathUtils::rbeta90(double alpha, double beta) {
    return randomBeta(alpha, beta);
}
double MathUtils::rbeta91(double alpha, double beta) {
    return randomBeta(alpha, beta);
}
double MathUtils::rbeta92(double alpha, double beta) {
    return randomBeta(alpha, beta);
}
double MathUtils::rbeta93(double alpha, double beta) {
    return randomBeta(alpha, beta);
}
double MathUtils::rbeta94(double alpha, double beta) {
    return randomBeta(alpha, beta);
}
double MathUtils::rbeta95(double alpha, double beta) {
    return randomBeta(alpha, beta);
}
double MathUtils::rbeta96(double alpha, double beta) {
    return randomBeta(alpha, beta);
}
double MathUtils::rbeta97(double alpha, double beta) {
    return randomBeta(alpha, beta);
}
double MathUtils::rbeta98(double alpha, double beta) {
    return randomBeta(alpha, beta);
}
double MathUtils::rbeta99(double alpha, double beta) {
    return randomBeta(alpha, beta);
}
double MathUtils::rbeta100(double alpha, double beta) {
    return randomBeta(alpha, beta);
}
}