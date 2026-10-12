#ifndef TEST_COMPARATORS_HPP
#define TEST_COMPARATORS_HPP

#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cassert>

/**
 * Value comparators and comparison utilities for NLM testing
 */

namespace TestComparators {
    
    // Floating point comparison utilities
    class FloatingPointComparator {
    public:
        static constexpr double DEFAULT_EPSILON = 1e-9;
        static constexpr double RELATIVE_EPSILON = 1e-9;
        
        static bool areEqual(double a, double b, double epsilon = DEFAULT_EPSILON) {
            return std::abs(a - b) <= epsilon;
        }
        
        static bool areClose(double a, double b, double epsilon = DEFAULT_EPSILON) {
            return std::abs(a - b) <= epsilon;
        }
        
        static bool areEqualRelative(double a, double b, double epsilon = RELATIVE_EPSILON) {
            if (a == b) return true;
            if (a == 0.0 || b == 0.0) return std::abs(a - b) <= epsilon;
            return std::abs((a - b) / std::min(std::abs(a), std::abs(b))) <= epsilon;
        }
        
        static bool areNotEqual(double a, double b, double epsilon = DEFAULT_EPSILON) {
            return !areEqual(a, b, epsilon);
        }
        
        static bool isGreater(double a, double b) {
            return a > b;
        }
        
        static bool isLess(double a, double b) {
            return a < b;
        }
        
        static bool isGreaterOrEqual(double a, double b, double epsilon = DEFAULT_EPSILON) {
            return a > b || areEqual(a, b, epsilon);
        }
        
        static bool isLessOrEqual(double a, double b, double epsilon = DEFAULT_EPSILON) {
            return a < b || areEqual(a, b, epsilon);
        }
        
        static bool isInRange(double value, double min, double max, double epsilon = DEFAULT_EPSILON) {
            return isGreaterOrEqual(value, min, epsilon) && isLessOrEqual(value, max, epsilon);
        }
        
        static double normalize(double value, double min, double max) {
            if (std::abs(max - min) < epsilon) return 0.5;
            return (value - min) / (max - min);
        }
        
        static double denormalize(double normalized, double min, double max) {
            return min + normalized * (max - min);
        }
        
        static double clamp(double value, double min, double max) {
            return std::max(min, std::min(value, max));
        }
        
        static double sigmoid(double x) {
            return 1.0 / (1.0 + std::exp(-x));
        }
        
        static double relu(double x) {
            return x > 0 ? x : 0.0;
        }
        
        static double tanh(double x) {
            return std::tanh(x);
        }
        
        static double softplus(double x) {
            return std::log(1.0 + std::exp(x));
        }
        
        static double absolute(double x) {
            return std::abs(x);
        }
        
        static double square(double x) {
            return x * x;
        }
        
        static double cube(double x) {
            return x * x * x;
        }
        
        static double sqrt(double x) {
            return std::sqrt(x);
        }
        
        static double cbrt(double x) {
            return std::cbrt(x);
        }
    };
    
    // Integer comparison utilities
    class IntegerComparator {
    public:
        static bool areEqual(int a, int b) {
            return a == b;
        }
        
        static bool areNotEqual(int a, int b) {
            return a != b;
        }
        
        static bool isGreater(int a, int b) {
            return a > b;
        }
        
        static bool isLess(int a, int b) {
            return a < b;
        }
        
        static bool isGreaterOrEqual(int a, int b) {
            return a >= b;
        }
        
        static bool isLessOrEqual(int a, int b) {
            return a <= b;
        }
        
        static bool isInRange(int value, int min, int max) {
            return value >= min && value <= max;
        }
        
        static int clamp(int value, int min, int max) {
            return std::max(min, std::min(value, max));
        }
        
        static int modulo(int a, int b) {
            return ((a % b) + b) % b;
        }
        
        static int sign(int x) {
            return (x > 0) - (x < 0);
        }
        
        static bool isEven(int x) {
            return x % 2 == 0;
        }
        
        static bool isOdd(int x) {
            return x % 2 != 0;
        }
        
        static int round(double x) {
            return static_cast<int>(std::round(x));
        }
        
        static int floor(double x) {
            return static_cast<int>(std::floor(x));
        }
        
        static int ceil(double x) {
            return static_cast<int>(std::ceil(x));
        }
    };
    
    // Vector comparison utilities
    class VectorComparator {
    public:
        static constexpr double DEFAULT_EPSILON = 1e-9;
        
        template<typename T>
        static bool areEqual(const std::vector<T>& a, const std::vector<T>& b, double epsilon = DEFAULT_EPSILON) {
            if (a.size() != b.size()) return false;
            for (size_t i = 0; i < a.size(); ++i) {
                if constexpr (std::is_floating_point_v<T>) {
                    if (std::abs(a[i] - b[i]) > epsilon) return false;
                } else {
                    if (a[i] != b[i]) return false;
                }
            }
            return true;
        }
        
        template<typename T>
        static bool areNotEqual(const std::vector<T>& a, const std::vector<T>& b, double epsilon = DEFAULT_EPSILON) {
            return !areEqual(a, b, epsilon);
        }
        
        template<typename T>
        static bool isSubset(const std::vector<T>& subset, const std::vector<T>& superset) {
            for (const auto& element : subset) {
                if (std::find(superset.begin(), superset.end(), element) == superset.end()) {
                    return false;
                }
            }
            return true;
        }
        
        template<typename T>
        static bool isSuperset(const std::vector<T>& superset, const std::vector<T>& subset) {
            return isSubset(subset, superset);
        }
        
        template<typename T>
        static bool isPermutation(const std::vector<T>& a, const std::vector<T>& b) {
            if (a.size() != b.size()) return false;
            std::vector<T> sorted_a = a;
            std::vector<T> sorted_b = b;
            std::sort(sorted_a.begin(), sorted_a.end());
            std::sort(sorted_b.begin(), sorted_b.end());
            return sorted_a == sorted_b;
        }
        
        template<typename T>
        static bool contains(const std::vector<T>& container, const T& element) {
            return std::find(container.begin(), container.end(), element) != container.end();
        }
        
        template<typename T>
        static size_t countOccurrences(const std::vector<T>& container, const T& element) {
            return std::count(container.begin(), container.end(), element);
        }
        
        template<typename T>
        static size_t findFirst(const std::vector<T>& container, const T& element) {
            auto it = std::find(container.begin(), container.end(), element);
            return it != container.end() ? std::distance(container.begin(), it) : static_cast<size_t>(-1);
        }
        
        template<typename T>
        static std::vector<size_t> findAll(const std::vector<T>& container, const T& element) {
            std::vector<size_t> indices;
            for (size_t i = 0; i < container.size(); ++i) {
                if (container[i] == element) {
                    indices.push_back(i);
                }
            }
            return indices;
        }
        
        template<typename T>
        static std::vector<T> filter(const std::vector<T>& container, std::function<bool(const T&)> predicate) {
            std::vector<T> result;
            for (const auto& element : container) {
                if (predicate(element)) {
                    result.push_back(element);
                }
            }
            return result;
        }
        
        template<typename T>
        static T sum(const std::vector<T>& container) {
            T result = T();
            for (const auto& element : container) {
                result += element;
            }
            return result;
        }
        
        template<typename T>
        static T product(const std::vector<T>& container) {
            T result = T(1);
            for (const auto& element : container) {
                result *= element;
            }
            return result;
        }
        
        template<typename T>
        static double mean(const std::vector<T>& container) {
            if (container.empty()) return 0.0;
            return static_cast<double>(sum(container)) / container.size();
        }
        
        template<typename T>
        static double variance(const std::vector<T>& container, double mean = -1.0) {
            if (container.empty()) return 0.0;
            if (mean < 0.0) mean = mean(container);
            
            double sum_sq = 0.0;
            for (const auto& element : container) {
                sum_sq += static_cast<double>(element) - mean;
            }
            return sum_sq / container.size();
        }
        
        template<typename T>
        static double standardDeviation(const std::vector<T>& container, double variance = -1.0) {
            if (variance < 0.0) variance = variance(container);
            return std::sqrt(variance);
        }
        
        template<typename T>
        static double median(std::vector<T> container) {
            if (container.empty()) return 0.0;
            std::sort(container.begin(), container.end());
            size_t n = container.size();
            if (n % 2 == 0) {
                return (container[n/2 - 1] + container[n/2]) / 2.0;
            } else {
                return container[n/2];
            }
        }
        
        template<typename T>
        static double percentile(std::vector<T> container, double percentile) {
            if (container.empty()) return 0.0;
            std::sort(container.begin(), container.end());
            size_t n = container.size();
            double index = (percentile / 100.0) * (n - 1);
            size_t lower = static_cast<size_t>(std::floor(index));
            size_t upper = static_cast<size_t>(std::ceil(index));
            
            if (lower == upper) {
                return static_cast<double>(container[lower]);
            }
            
            double weight = index - lower;
            return (1.0 - weight) * static_cast<double>(container[lower]) + 
                   weight * static_cast<double>(container[upper]);
        }
        
        template<typename T>
        static bool isSorted(const std::vector<T>& container, bool ascending = true) {
            if (container.size() <= 1) return true;
            
            for (size_t i = 1; i < container.size(); ++i) {
                if (ascending) {
                    if (container[i-1] > container[i]) return false;
                } else {
                    if (container[i-1] < container[i]) return false;
                }
            }
            return true;
        }
        
        template<typename T>
        static std::vector<T> sortedCopy(const std::vector<T>& container, bool ascending = true) {
            std::vector<T> result = container;
            std::sort(result.begin(), result.end(), 
                     ascending ? std::less<T>() : std::greater<T>());
            return result;
        }
        
        template<typename T>
        static std::vector<T> uniqueCopy(const std::vector<T>& container) {
            std::vector<T> result;
            for (const auto& element : container) {
                if (std::find(result.begin(), result.end(), element) == result.end()) {
                    result.push_back(element);
                }
            }
            return result;
        }
        
        template<typename T>
        static bool hasDuplicates(const std::vector<T>& container) {
            std::unordered_set<T> seen;
            for (const auto& element : container) {
                if (seen.find(element) != seen.end()) {
                    return true;
                }
                seen.insert(element);
            }
            return false;
        }
        
        template<typename T>
        static size_t countDistinct(const std::vector<T>& container) {
            std::unordered_set<T> unique_set;
            for (const auto& element : container) {
                unique_set.insert(element);
            }
            return unique_set.size();
        }
    };
    
    // String comparison utilities
    class StringComparator {
    public:
        static bool areEqual(const std::string& a, const std::string& b) {
            return a == b;
        }
        
        static bool areNotEqual(const std::string& a, const std::string& b) {
            return a != b;
        }
        
        static bool isPrefix(const std::string& haystack, const std::string& needle) {
            return haystack.size() >= needle.size() && 
                   std::equal(needle.begin(), needle.end(), haystack.begin());
        }
        
        static bool isSuffix(const std::string& haystack, const std::string& needle) {
            return haystack.size() >= needle.size() && 
                   std::equal(needle.rbegin(), needle.rend(), haystack.rbegin());
        }
        
        static bool contains(const std::string& haystack, const std::string& needle) {
            return haystack.find(needle) != std::string::npos;
        }
        
        static bool startsWith(const std::string& haystack, const std::string& needle) {
            return isPrefix(haystack, needle);
        }
        
        static bool endsWith(const std::string& haystack, const std::string& needle) {
            return isSuffix(haystack, needle);
        }
        
        static bool isEmpty(const std::string& s) {
            return s.empty();
        }
        
        static bool isNotEmpty(const std::string& s) {
            return !s.empty();
        }
        
        static size_t length(const std::string& s) {
            return s.length();
        }
        
        static std::string toUpper(const std::string& s) {
            std::string result = s;
            std::transform(result.begin(), result.end(), result.begin(), 
                          [](unsigned char c) { return std::toupper(c); });
            return result;
        }
        
        static std::string toLower(const std::string& s) {
            std::string result = s;
            std::transform(result.begin(), result.end(), result.begin(), 
                          [](unsigned char c) { return std::tolower(c); });
            return result;
        }
        
        static std::string trim(const std::string& s) {
            size_t start = s.find_first_not_of(" \t\n\r");
            size_t end = s.find_last_not_of(" \t\n\r");
            if (start == std::string::npos) return "";
            return s.substr(start, end - start + 1);
        }
        
        static std::string ltrim(const std::string& s) {
            size_t start = s.find_first_not_of(" \t\n\r");
            if (start == std::string::npos) return "";
            return s.substr(start);
        }
        
        static std::string rtrim(const std::string& s) {
            size_t end = s.find_last_not_of(" \t\n\r");
            if (end == std::string::npos) return "";
            return s.substr(0, end + 1);
        }
        
        static bool isWhitespace(const std::string& s) {
            return std::all_of(s.begin(), s.end(), [](char c) { 
                return std::isspace(static_cast<unsigned char>(c)); 
            });
        }
        
        static size_t countSubstrings(const std::string& haystack, const std::string& needle) {
            size_t count = 0;
            size_t pos = 0;
            while ((pos = haystack.find(needle, pos)) != std::string::npos) {
                count++;
                pos += needle.length();
            }
            return count;
        }
        
        static std::vector<std::string> split(const std::string& s, char delimiter) {
            std::vector<std::string> tokens;
            std::string token;
            std::istringstream tokenStream(s);
            while (std::getline(tokenStream, token, delimiter)) {
                tokens.push_back(token);
            }
            return tokens;
        }
        
        static std::string join(const std::vector<std::string>& tokens, char delimiter) {
            std::ostringstream oss;
            for (size_t i = 0; i < tokens.size(); ++i) {
                if (i > 0) oss << delimiter;
                oss << tokens[i];
            }
            return oss.str();
        }
        
        static bool isPalindrome(const std::string& s) {
            std::string reversed = s;
            std::reverse(reversed.begin(), reversed.end());
            return s == reversed;
        }
        
        static size_t levenshteinDistance(const std::string& a, const std::string& b) {
            // Standard Levenshtein distance implementation
            if (a.size() < b.size()) return levenshteinDistance(b, a);
            
            std::vector<size_t> previous(b.size() + 1, 0);
            std::vector<size_t> current(b.size() + 1, 0);
            
            for (size_t i = 0; i <= a.size(); ++i) {
                current[0] = i;
                for (size_t j = 0; j <= b.size(); ++j) {
                    size_t cost = (i > 0 && j > 0 && a[i-1] != b[j-1]) ? 1 : 0;
                    current[j] = std::min({current[j-1] + 1, previous[j] + 1, previous[j-1] + cost});
                }
                previous = current;
            }
            
            return previous[b.size()];
        }
        
        static bool isAnagram(const std::string& a, const std::string& b) {
            if (a.size() != b.size()) return false;
            std::string sorted_a = a;
            std::string sorted_b = b;
            std::sort(sorted_a.begin(), sorted_a.end());
            std::sort(sorted_b.begin(), sorted_b.end());
            return sorted_a == sorted_b;
        }
    };
    
    // Matrix comparison utilities
    class MatrixComparator {
    public:
        static constexpr double DEFAULT_EPSILON = 1e-9;
        
        template<typename T>
        static bool areEqual(const std::vector<std::vector<T>>& a, const std::vector<std::vector<T>>& b, 
                           double epsilon = DEFAULT_EPSILON) {
            if (a.size() != b.size()) return false;
            for (size_t i = 0; i < a.size(); ++i) {
                if (a[i].size() != b[i].size()) return false;
                for (size_t j = 0; j < a[i].size(); ++j) {
                    if constexpr (std::is_floating_point_v<T>) {
                        if (std::abs(a[i][j] - b[i][j]) > epsilon) return false;
                    } else {
                        if (a[i][j] != b[i][j]) return false;
                    }
                }
            }
            return true;
        }
        
        template<typename T>
        static bool areNotEqual(const std::vector<std::vector<T>>& a, const std::vector<std::vector<T>>& b, 
                              double epsilon = DEFAULT_EPSILON) {
            return !areEqual(a, b, epsilon);
        }
        
        template<typename T>
        static bool isSquare(const std::vector<std::vector<T>>& matrix) {
            return !matrix.empty() && matrix.size() == matrix[0].size();
        }
        
        template<typename T>
        static bool isSymmetric(const std::vector<std::vector<T>>& matrix, double epsilon = DEFAULT_EPSILON) {
            size_t n = matrix.size();
            for (size_t i = 0; i < n; ++i) {
                for (size_t j = 0; j < n; ++j) {
                    if constexpr (std::is_floating_point_v<T>) {
                        if (std::abs(matrix[i][j] - matrix[j][i]) > epsilon) return false;
                    } else {
                        if (matrix[i][j] != matrix[j][i]) return false;
                    }
                }
            }
            return true;
        }
        
        template<typename T>
        static bool isDiagonal(const std::vector<std::vector<T>>& matrix) {
            size_t n = matrix.size();
            for (size_t i = 0; i < n; ++i) {
                for (size_t j = 0; j < n; ++j) {
                    if (i != j && matrix[i][j] != T(0)) return false;
                }
            }
            return true;
        }
        
        template<typename T>
        static bool isIdentity(const std::vector<std::vector<T>>& matrix, double epsilon = DEFAULT_EPSILON) {
            size_t n = matrix.size();
            if (!isSquare(matrix)) return false;
            
            for (size_t i = 0; i < n; ++i) {
                for (size_t j = 0; j < n; ++j) {
                    if (i == j) {
                        if constexpr (std::is_floating_point_v<T>) {
                            if (std::abs(matrix[i][j] - T(1)) > epsilon) return false;
                        } else {
                            if (matrix[i][j] != T(1)) return false;
                        }
                    } else {
                        if (matrix[i][j] != T(0)) return false;
                    }
                }
            }
            return true;
        }
        
        template<typename T>
        static std::vector<T> getRow(const std::vector<std::vector<T>>& matrix, size_t row) {
            if (row >= matrix.size()) throw std::out_of_range("Row index out of range");
            return matrix[row];
        }
        
        template<typename T>
        static std::vector<T> getColumn(const std::vector<std::vector<T>>& matrix, size_t column) {
            if (matrix.empty() || column >= matrix[0].size()) throw std::out_of_range("Column index out of range");
            std::vector<T> column_vector;
            column_vector.reserve(matrix.size());
            for (size_t i = 0; i < matrix.size(); ++i) {
                column_vector.push_back(matrix[i][column]);
            }
            return column_vector;
        }
        
        template<typename T>
        static T getElement(const std::vector<std::vector<T>>& matrix, size_t row, size_t column) {
            if (row >= matrix.size() || column >= matrix[row].size()) throw std::out_of_range("Index out of range");
            return matrix[row][column];
        }
        
        template<typename T>
        static double determinant(const std::vector<std::vector<T>>& matrix, double epsilon = DEFAULT_EPSILON) {
            if (!isSquare(matrix)) throw std::invalid_argument("Matrix must be square");
            size_t n = matrix.size();
            
            if (n == 1) return static_cast<double>(matrix[0][0]);
            if (n == 2) {
                return static_cast<double>(matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0]);
            }
            
            double det = 0.0;
            for (size_t j = 0; j < n; ++j) {
                std::vector<std::vector<T>> submatrix(n-1, std::vector<T>(n-1));
                for (size_t r = 1; r < n; ++r) {
                    size_t col_index = 0;
                    for (size_t c = 0; c < n; ++c) {
                        if (c != j) {
                            submatrix[r-1][col_index++] = matrix[r][c];
                        }
                    }
                }
                det += (j % 2 == 0 ? 1 : -1) * static_cast<double>(matrix[0][j]) * determinant(submatrix, epsilon);
            }
            return det;
        }
        
        template<typename T>
        static double trace(const std::vector<std::vector<T>>& matrix) {
            if (!isSquare(matrix)) throw std::invalid_argument("Matrix must be square");
            size_t n = matrix.size();
            double trace = 0.0;
            for (size_t i = 0; i < n; ++i) {
                trace += static_cast<double>(matrix[i][i]);
            }
            return trace;
        }
        
        template<typename T>
        static std::vector<std::vector<T>> transpose(const std::vector<std::vector<T>>& matrix) {
            if (matrix.empty()) return {};
            size_t rows = matrix.size();
            size_t cols = matrix[0].size();
            
            std::vector<std::vector<T>> transposed(cols, std::vector<T>(rows));
            for (size_t i = 0; i < rows; ++i) {
                for (size_t j = 0; j < cols; ++j) {
                    transposed[j][i] = matrix[i][j];
                }
            }
            return transposed;
        }
        
        template<typename T>
        static std::vector<std::vector<T>> add(const std::vector<std::vector<T>>& a, const std::vector<std::vector<T>>& b) {
            if (a.size() != b.size() || a[0].size() != b[0].size()) {
                throw std::invalid_argument("Matrices must have same dimensions");
            }
            
            std::vector<std::vector<T>> result(a.size(), std::vector<T>(a[0].size()));
            for (size_t i = 0; i < a.size(); ++i) {
                for (size_t j = 0; j < a[i].size(); ++j) {
                    result[i][j] = a[i][j] + b[i][j];
                }
            }
            return result;
        }
        
        template<typename T>
        static std::vector<std::vector<T>> subtract(const std::vector<std::vector<T>>& a, const std::vector<std::vector<T>>& b) {
            if (a.size() != b.size() || a[0].size() != b[0].size()) {
                throw std::invalid_argument("Matrices must have same dimensions");
            }
            
            std::vector<std::vector<T>> result(a.size(), std::vector<T>(a[0].size()));
            for (size_t i = 0; i < a.size(); ++i) {
                for (size_t j = 0; j < a[i].size(); ++j) {
                    result[i][j] = a[i][j] - b[i][j];
                }
            }
            return result;
        }
        
        template<typename T>
        static std::vector<std::vector<T>> multiply(const std::vector<std::vector<T>>& a, const std::vector<std::vector<T>>& b) {
            size_t a_rows = a.size();
            size_t a_cols = a[0].size();
            size_t b_rows = b.size();
            size_t b_cols = b[0].size();
            
            if (a_cols != b_rows) {
                throw std::invalid_argument("Matrix dimensions don't match for multiplication");
            }
            
            std::vector<std::vector<T>> result(a_rows, std::vector<T>(b_cols, T(0)));
            for (size_t i = 0; i < a_rows; ++i) {
                for (size_t j = 0; j < b_cols; ++j) {
                    for (size_t k = 0; k < a_cols; ++k) {
                        result[i][j] += a[i][k] * b[k][j];
                    }
                }
            }
            return result;
        }
    };
}

#endif // TEST_COMPARATORS_HPP