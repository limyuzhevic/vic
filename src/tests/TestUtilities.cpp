// Test utilities implementation for NLM
// Provides helper functions for testing neural network operations

#include "TestUtilities.hpp"
#include <fstream>
#include <stdexcept>

namespace nlm {

// Vector utilities implementation
class VectorUtilsImpl {
public:
    // Calculate Euclidean distance between two vectors
    static float euclideanDistance(const std::vector<float>& a, const std::vector<float>& b) {
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must have the same size");
        }
        
        float sum = 0.0f;
        for (size_t i = 0; i < a.size(); ++i) {
            float diff = a[i] - b[i];
            sum += diff * diff;
        }
        return std::sqrt(sum);
    }
    
    // Calculate cosine similarity between two vectors
    static float cosineSimilarity(const std::vector<float>& a, const std::vector<float>& b) {
        if (a.size() != b.size()) {
            throw std::invalid_argument("Vectors must have the same size");
        }
        
        float dotProduct = 0.0f;
        float normA = 0.0f;
        float normB = 0.0f;
        
        for (size_t i = 0; i < a.size(); ++i) {
            dotProduct += a[i] * b[i];
            normA += a[i] * a[i];
            normB += b[i] * b[i];
        }
        
        float normProduct = std::sqrt(normA) * std::sqrt(normB);
        if (normProduct == 0.0f) {
            return 0.0f;
        }
        
        return dotProduct / normProduct;
    }
    
    // Normalize vector to unit length
    static std::vector<float> normalize(const std::vector<float>& vec) {
        float norm = 0.0f;
        for (float val : vec) {
            norm += val * val;
        }
        norm = std::sqrt(norm);
        
        if (norm == 0.0f) {
            return vec;
        }
        
        std::vector<float> result(vec.size());
        for (size_t i = 0; i < vec.size(); ++i) {
            result[i] = vec[i] / norm;
        }
        
        return result;
    }
    
    // Clamp vector values to [min, max] range
    static std::vector<float> clamp(const std::vector<float>& vec, float min, float max) {
        std::vector<float> result(vec.size());
        for (size_t i = 0; i < vec.size(); ++i) {
            result[i] = std::clamp(vec[i], min, max);
        }
        return result;
    }
    
    // Calculate mean of vector values
    static float mean(const std::vector<float>& vec) {
        if (vec.empty()) return 0.0f;
        
        float sum = 0.0f;
        for (float val : vec) {
            sum += val;
        }
        return sum / vec.size();
    }
    
    // Calculate standard deviation of vector values
    static float standardDeviation(const std::vector<float>& vec, float mean = 0.0f) {
        if (vec.empty()) return 0.0f;
        
        if (mean == 0.0f) {
            mean = VectorUtilsImpl::mean(vec);
        }
        
        float sum = 0.0f;
        for (float val : vec) {
            float diff = val - mean;
            sum += diff * diff;
        }
        return std::sqrt(sum / vec.size());
    }
    
    // Check if two vectors are approximately equal
    static bool isEqual(const std::vector<float>& a, const std::vector<float>& b, 
                       float tolerance = 1e-6f) {
        if (a.size() != b.size()) return false;
        
        for (size_t i = 0; i < a.size(); ++i) {
            if (std::abs(a[i] - b[i]) > tolerance) {
                return false;
            }
        }
        return true;
    }
};

// Matrix utilities implementation
class MatrixUtilsImpl {
public:
    // Multiply two matrices
    static std::vector<std::vector<float>> multiply(
        const std::vector<std::vector<float>>& a,
        const std::vector<std::vector<float>>& b) {
        
        if (a[0].size() != b.size()) {
            throw std::invalid_argument("Matrix dimensions don't match for multiplication");
        }
        
        size_t rowsA = a.size();
        size_t colsA = a[0].size();
        size_t colsB = b[0].size();
        
        std::vector<std::vector<float>> result(rowsA, std::vector<float>(colsB, 0.0f));
        
        for (size_t i = 0; i < rowsA; ++i) {
            for (size_t j = 0; j < colsB; ++j) {
                for (size_t k = 0; k < colsA; ++k) {
                    result[i][j] += a[i][k] * b[k][j];
                }
            }
        }
        
        return result;
    }
    
    // Transpose matrix
    static std::vector<std::vector<float>> transpose(const std::vector<std::vector<float>>& matrix) {
        size_t rows = matrix.size();
        size_t cols = matrix[0].size();
        
        std::vector<std::vector<float>> result(cols, std::vector<float>(rows));
        
        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                result[j][i] = matrix[i][j];
            }
        }
        
        return result;
    }
    
    // Calculate matrix determinant (for 2x2 and 3x3 matrices)
    static float determinant(const std::vector<std::vector<float>>& matrix) {
        size_t rows = matrix.size();
        if (rows == 2 && matrix[0].size() == 2) {
            return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
        } else if (rows == 3 && matrix[0].size() == 3) {
            float a = matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]);
            float b = matrix[0][1] * (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0]);
            float c = matrix[0][2] * (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]);
            return a - b + c;
        } else {
            throw std::invalid_argument("Determinant only implemented for 2x2 and 3x3 matrices");
        }
    }
    
    // Check if matrix is square
    static bool isSquare(const std::vector<std::vector<float>>& matrix) {
        return !matrix.empty() && matrix.size() == matrix[0].size();
    }
    
    // Check if matrices have compatible dimensions for addition
    static bool areCompatibleForAddition(const std::vector<std::vector<float>>& a,
                                         const std::vector<std::vector<float>>& b) {
        return a.size() == b.size() && a[0].size() == b[0].size();
    }
};

// Statistics utilities implementation
class StatsUtilsImpl {
public:
    // Calculate Pearson correlation coefficient
    static float pearsonCorrelation(const std::vector<float>& x, const std::vector<float>& y) {
        if (x.size() != y.size() || x.size() < 2) {
            throw std::invalid_argument("Vectors must have same size and at least 2 elements");
        }
        
        float sumX = 0.0f, sumY = 0.0f, sumXY = 0.0f, sumX2 = 0.0f, sumY2 = 0.0f;
        
        for (size_t i = 0; i < x.size(); ++i) {
            sumX += x[i];
            sumY += y[i];
            sumXY += x[i] * y[i];
            sumX2 += x[i] * x[i];
            sumY2 += y[i] * y[i];
        }
        
        float n = static_cast<float>(x.size());
        float numerator = n * sumXY - sumX * sumY;
        float denominator = std::sqrt((n * sumX2 - sumX * sumX) * (n * sumY2 - sumY * sumY));
        
        if (denominator == 0.0f) {
            return 0.0f;
        }
        
        return numerator / denominator;
    }
    
    // Calculate coefficient of variation
    static float coefficientOfVariation(const std::vector<float>& data) {
        float mean = VectorUtilsImpl::mean(data);
        float stddev = VectorUtilsImpl::standardDeviation(data, mean);
        
        if (mean == 0.0f) {
            return 0.0f;
        }
        
        return stddev / mean;
    }
    
    // Calculate median of vector
    static float median(std::vector<float> sorted) {
        if (sorted.empty()) return 0.0f;
        
        std::sort(sorted.begin(), sorted.end());
        size_t middle = sorted.size() / 2;
        
        if (sorted.size() % 2 == 0) {
            return (sorted[middle - 1] + sorted[middle]) / 2.0f;
        } else {
            return sorted[middle];
        }
    }
    
    // Calculate percentiles
    static float percentile(std::vector<float> data, float percentile) {
        if (data.empty()) return 0.0f;
        
        std::sort(data.begin(), data.end());
        float index = percentile / 100.0f * (data.size() - 1);
        
        if (std::floor(index) == std::ceil(index)) {
            return data[static_cast<size_t>(index)];
        } else {
            size_t lower = static_cast<size_t>(index);
            size_t upper = lower + 1;
            float weight = index - lower;
            return data[lower] * (1.0f - weight) + data[upper] * weight;
        }
    }
    
    // Remove outliers using IQR method
    static std::vector<float> removeOutliersIQR(const std::vector<float>& data, float IQRMultiplier) {
        if (data.empty()) return data;
        
        std::vector<float> sorted = data;
        std::sort(sorted.begin(), sorted.end());
        
        size_t n = sorted.size();
        size_t q1Index = n / 4;
        size_t q3Index = (3 * n) / 4;
        
        float q1 = sorted[q1Index];
        float q3 = sorted[q3Index];
        
        float IQR = q3 - q1;
        float lowerBound = q1 - IQRMultiplier * IQR;
        float upperBound = q3 + IQRMultiplier * IQR;
        
        std::vector<float> result;
        for (float val : data) {
            if (val >= lowerBound && val <= upperBound) {
                result.push_back(val);
            }
        }
        
        return result;
    }
};

// String utilities implementation
class StringUtilsImpl {
public:
    // Format string with printf-style formatting
    static std::string format(const std::string& format, ...) {
        va_list args;
        va_start(args, format);
        
        // Determine required buffer size
        char buffer[1024];
        int size = std::vsnprintf(buffer, sizeof(buffer), format.c_str(), args);
        va_end(args);
        
        if (size < 0) {
            return "";
        }
        
        return std::string(buffer, static_cast<size_t>(size));
    }
    
    // Split string by delimiter
    static std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> result;
        std::stringstream ss(str);
        std::string token;
        
        while (std::getline(ss, token, delimiter)) {
            result.push_back(token);
        }
        
        return result;
    }
    
    // Trim whitespace from both ends
    static std::string trim(const std::string& str) {
        size_t start = 0;
        size_t end = str.size();
        
        // Trim leading whitespace
        while (start < end && std::isspace(str[start])) {
            ++start;
        }
        
        // Trim trailing whitespace
        while (end > start && std::isspace(str[end - 1])) {
            --end;
        }
        
        return str.substr(start, end - start);
    }
    
    // Convert string to lowercase
    static std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return result;
    }
    
    // Convert string to uppercase
    static std::string toUpper(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c) { return std::toupper(c); });
        return result;
    }
    
    // Check if string contains substring
    static bool contains(const std::string& str, const std::string& substring) {
        return str.find(substring) != std::string::npos;
    }
    
    // Replace all occurrences of substring
    static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to) {
        std::string result = str;
        size_t pos = 0;
        while ((pos = result.find(from, pos)) != std::string::npos) {
            result.replace(pos, from.length(), to);
            pos += to.length();
        }
        return result;
    }
};

// Random number generation utilities implementation
class RandomUtilsImpl {
public:
    // Generate random float in [min, max] range
    static float randomFloat(float min, float max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dist(min, max);
        return dist(gen);
    }
    
    // Generate random integer in [min, max] range
    static int randomInt(int min, int max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(min, max);
        return dist(gen);
    }
    
    // Generate random boolean
    static bool randomBool(float probability) {
        return randomFloat() < probability;
    }
    
    // Generate random permutation of indices
    static std::vector<size_t> randomPermutation(size_t n) {
        std::vector<size_t> indices(n);
        for (size_t i = 0; i < n; ++i) {
            indices[i] = i;
        }
        
        for (size_t i = n - 1; i > 0; --i) {
            size_t j = static_cast<size_t>(randomFloat() * (i + 1));
            std::swap(indices[i], indices[j]);
        }
        
        return indices;
    }
    
    // Shuffle vector
    template<typename T>
    static void shuffle(std::vector<T>& vec) {
        size_t n = vec.size();
        for (size_t i = n - 1; i > 0; --i) {
            size_t j = static_cast<size_t>(randomFloat() * (i + 1));
            std::swap(vec[i], vec[j]);
        }
    }
};

// File I/O utilities implementation
class FileUtilsImpl {
public:
    // Read file content as string
    static std::string readFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filepath);
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
    
    // Write string to file
    static bool writeFile(const std::string& filepath, const std::string& content) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << content;
        return true;
    }
    
    // Check if file exists
    static bool exists(const std::string& filepath) {
        std::ifstream file(filepath);
        return file.good();
    }
    
    // Get file extension
    static std::string getExtension(const std::string& filepath) {
        size_t pos = filepath.find_last_of('.');
        if (pos == std::string::npos) {
            return "";
        }
        return filepath.substr(pos + 1);
    }
    
    // Get file size
    static size_t getSize(const std::string& filepath) {
        std::ifstream file(filepath, std::ifstream::ate | std::ifstream::binary);
        return file.tellg();
    }
    
    // List files in directory
    static std::vector<std::string> listFiles(const std::string& directory) {
        std::vector<std::string> files;
        WIN32_FIND_DATA findData;
        HANDLE hFind = FindFirstFile((directory + "/*").c_str(), &findData);
        
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                    files.push_back(findData.cFileName);
                }
            } while (FindNextFile(hFind, &findData));
            FindClose(hFind);
        }
        
        return files;
    }
};

// Override the namespace implementations
namespace VectorUtils {
    float euclideanDistance(const std::vector<float>& a, const std::vector<float>& b) {
        return VectorUtilsImpl::euclideanDistance(a, b);
    }
    
    float cosineSimilarity(const std::vector<float>& a, const std::vector<float>& b) {
        return VectorUtilsImpl::cosineSimilarity(a, b);
    }
    
    std::vector<float> normalize(const std::vector<float>& vec) {
        return VectorUtilsImpl::normalize(vec);
    }
    
    std::vector<float> clamp(const std::vector<float>& vec, float min, float max) {
        return VectorUtilsImpl::clamp(vec, min, max);
    }
    
    float mean(const std::vector<float>& vec) {
        return VectorUtilsImpl::mean(vec);
    }
    
    float standardDeviation(const std::vector<float>& vec, float mean) {
        return VectorUtilsImpl::standardDeviation(vec, mean);
    }
    
    bool isEqual(const std::vector<float>& a, const std::vector<float>& b, float tolerance) {
        return VectorUtilsImpl::isEqual(a, b, tolerance);
    }
}

namespace MatrixUtils {
    std::vector<std::vector<float>> multiply(const std::vector<std::vector<float>>& a, const std::vector<std::vector<float>>& b) {
        return MatrixUtilsImpl::multiply(a, b);
    }
    
    std::vector<std::vector<float>> transpose(const std::vector<std::vector<float>>& matrix) {
        return MatrixUtilsImpl::transpose(matrix);
    }
    
    float determinant(const std::vector<std::vector<float>>& matrix) {
        return MatrixUtilsImpl::determinant(matrix);
    }
    
    bool isSquare(const std::vector<std::vector<float>>& matrix) {
        return MatrixUtilsImpl::isSquare(matrix);
    }
    
    bool areCompatibleForAddition(const std::vector<std::vector<float>>& a, const std::vector<std::vector<float>>& b) {
        return MatrixUtilsImpl::areCompatibleForAddition(a, b);
    }
}

namespace StatsUtils {
    float pearsonCorrelation(const std::vector<float>& x, const std::vector<float>& y) {
        return StatsUtilsImpl::pearsonCorrelation(x, y);
    }
    
    float coefficientOfVariation(const std::vector<float>& data) {
        return StatsUtilsImpl::coefficientOfVariation(data);
    }
    
    float median(std::vector<float> sorted) {
        return StatsUtilsImpl::median(std::move(sorted));
    }
    
    float percentile(std::vector<float> data, float percentile) {
        return StatsUtilsImpl::percentile(std::move(data), percentile);
    }
    
    std::vector<float> removeOutliersIQR(const std::vector<float>& data, float IQRMultiplier) {
        return StatsUtilsImpl::removeOutliersIQR(data, IQRMultiplier);
    }
}

namespace StringUtils {
    std::string format(const std::string& format, ...) {
        return StringUtilsImpl::format(format, args...);
    }
    
    std::vector<std::string> split(const std::string& str, char delimiter) {
        return StringUtilsImpl::split(str, delimiter);
    }
    
    std::string trim(const std::string& str) {
        return StringUtilsImpl::trim(str);
    }
    
    std::string toLower(const std::string& str) {
        return StringUtilsImpl::toLower(str);
    }
    
    std::string toUpper(const std::string& str) {
        return StringUtilsImpl::toUpper(str);
    }
    
    bool contains(const std::string& str, const std::string& substring) {
        return StringUtilsImpl::contains(str, substring);
    }
    
    std::string replaceAll(const std::string& str, const std::string& from, const std::string& to) {
        return StringUtilsImpl::replaceAll(str, from, to);
    }
}

namespace RandomUtils {
    float randomFloat(float min, float max) {
        return RandomUtilsImpl::randomFloat(min, max);
    }
    
    int randomInt(int min, int max) {
        return RandomUtilsImpl::randomInt(min, max);
    }
    
    bool randomBool(float probability) {
        return RandomUtilsImpl::randomBool(probability);
    }
    
    std::vector<size_t> randomPermutation(size_t n) {
        return RandomUtilsImpl::randomPermutation(n);
    }
    
    template<typename T>
    void shuffle(std::vector<T>& vec) {
        RandomUtilsImpl::shuffle(vec);
    }
}

namespace FileUtils {
    std::string readFile(const std::string& filepath) {
        return FileUtilsImpl::readFile(filepath);
    }
    
    bool writeFile(const std::string& filepath, const std::string& content) {
        return FileUtilsImpl::writeFile(filepath, content);
    }
    
    bool exists(const std::string& filepath) {
        return FileUtilsImpl::exists(filepath);
    }
    
    std::string getExtension(const std::string& filepath) {
        return FileUtilsImpl::getExtension(filepath);
    }
    
    size_t getSize(const std::string& filepath) {
        return FileUtilsImpl::getSize(filepath);
    }
    
    std::vector<std::string> listFiles(const std::string& directory) {
        return FileUtilsImpl::listFiles(directory);
    }
}

} // namespace nlm
