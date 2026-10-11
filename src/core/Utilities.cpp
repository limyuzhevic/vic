#include "Utilities.hpp"
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <openssl/md5.h>
#include <openssl/sha.h>

namespace nlm {

// FileUtils implementation
bool FileUtils::copyFile(const std::string& source, const std::string& destination,
                       std::shared_ptr<IProgressReporter> reporter,
                       const std::string& description) {
    try {
        std::ifstream src(source, std::ios::binary);
        if (!src) {
            throw FileIOError(source, FileIOError::ErrorType::FileNotFound, "Source file not found");
        }
        
        std::ofstream dest(destination, std::ios::binary);
        if (!dest) {
            throw FileIOError(source, FileIOError::ErrorType::PermissionDenied, "Cannot create destination file");
        }
        
        src.seekg(0, std::ios::end);
        size_t fileSize = src.tellg();
        src.seekg(0, std::ios::beg);
        
        std::vector<char> buffer(8192);
        size_t totalCopied = 0;
        
        while (totalCopied < fileSize) {
            size_t chunkSize = std::min(sizeof(buffer), fileSize - totalCopied);
            src.read(buffer.data(), chunkSize);
            dest.write(buffer.data(), chunkSize);
            totalCopied += chunkSize;
            
            if (reporter) {
                double progress = static_cast<double>(totalCopied) / fileSize;
                reporter->onProgress(progress, description);
                if (reporter->shouldCancel()) {
                    return false;
                }
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        GlobalExceptionHandler::handle(e);
        return false;
    }
}

bool FileUtils::backupFile(const std::string& filepath,
                          std::shared_ptr<IProgressReporter> reporter) {
    try {
        if (!std::filesystem::exists(filepath)) {
            throw FileIOError(filepath, FileIOError::ErrorType::FileNotFound, "File does not exist");
        }
        
        std::string backupPath = getFileBackupPath(filepath);
        
        if (copyFile(filepath, backupPath, reporter, "Creating backup")) {
            return true;
        }
        
        return false;
        
    } catch (const std::exception& e) {
        GlobalExceptionHandler::handle(e);
        return false;
    }
}

bool FileUtils::validateFile(const std::string& filepath,
                            const std::vector<std::string>& allowedExtensions) {
    try {
        std::filesystem::path path(filepath);
        
        if (!std::filesystem::exists(path)) {
            return false;
        }
        
        if (!std::filesystem::is_regular_file(path)) {
            return false;
        }
        
        if (!allowedExtensions.empty()) {
            std::string extension = path.extension().string();
            if (extension.empty()) {
                return false;
            }
            
            // Remove leading dot
            if (extension[0] == '.') {
                extension = extension.substr(1);
            }
            
            bool found = false;
            for (const auto& allowed : allowedExtensions) {
                if (extension == allowed) {
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                return false;
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        GlobalExceptionHandler::handle(e);
        return false;
    }
}

bool FileUtils::isFileLocked(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        return !file.good();
    } catch (...) {
        return true;  // Assume locked if there's an error
    }
}

std::string FileUtils::getFileBackupPath(const std::string& filepath) {
    std::filesystem::path path(filepath);
    std::string stem = path.stem().string();
    std::string extension = path.extension().string();
    
    // Add timestamp to filename
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
    
    return path.parent_path().string() + "/" + stem + "_backup_" + ss.str() + extension;
}

// SerializationUtils implementation
std::string SerializationUtils::encodeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (c < 0x20 || c == 0x7F) {
                    result += "\\u" + StringUtils::formatHex(static_cast<unsigned char>(c));
                } else {
                    result += c;
                }
                break;
        }
    }
    return result;
}

std::string SerializationUtils::decodeString(const std::string& encoded) {
    std::string result;
    size_t i = 0;
    
    while (i < encoded.length()) {
        if (encoded[i] == '\\\\' && i + 1 < encoded.length()) {
            char next = encoded[i + 1];
            switch (next) {
                case '\\\\': result += '\\\\'; i += 2; break;
                case '\"': result += '\"'; i += 2; break;
                case 'b': result += '\b'; i += 2; break;
                case 'f': result += '\f'; i += 2; break;
                case 'n': result += '\n'; i += 2; break;
                case 'r': result += '\r'; i += 2; break;
                case 't': result += '\t'; i += 2; break;
                case 'u': {
                    if (i + 5 < encoded.length()) {
                        std::string hex = encoded.substr(i + 2, 4);
                        try {
                            char c = static_cast<char>(std::stoul(hex, nullptr, 16));
                            result += c;
                            i += 6;
                            continue;
                        } catch (...) {}
                    }
                    result += '\\\\'; i += 2; // Fallback
                } break;
                default: result += '\\\\'; i += 2; break;
            }
        } else {
            result += encoded[i];
            i++;
        }
    }
    
    return result;
}

std::string SerializationUtils::detectFileEncoding(const std::string& filepath) {
    try {
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            return "unknown";
        }
        
        std::vector<char> buffer(1024);
        file.read(buffer.data(), buffer.size());
        
        // Check for UTF-8 BOM
        if (buffer.size() >= 3 && buffer[0] == '\xEF' && buffer[1] == '\xBB' && buffer[2] == '\xBF') {
            return "utf-8";
        }
        
        // Check for UTF-16 LE BOM
        if (buffer.size() >= 2 && buffer[0] == '\xFF' && buffer[1] == '\xFE') {
            return "utf-16le";
        }
        
        // Check for UTF-16 BE BOM
        if (buffer.size() >= 2 && buffer[0] == '\xFE' && buffer[1] == '\xFF') {
            return "utf-16be";
        }
        
        // Check for ASCII or UTF-8 by trying to decode
        try {
            std::string text(buffer.begin(), buffer.begin() + buffer.size());
            // Simple check for valid UTF-8
            bool isValidUTF8 = true;
            size_t i = 0;
            while (i < text.size()) {
                unsigned char byte = static_cast<unsigned char>(text[i]);
                if (byte <= 0x7F) {
                    i++;
                } else if (byte >= 0xC0 && byte <= 0xDF) {
                    if (i + 1 < text.size()) {
                        unsigned char next = static_cast<unsigned char>(text[i + 1]);
                        if ((next & 0xC0) != 0x80) {
                            isValidUTF8 = false;
                            break;
                        }
                        i += 2;
                    } else {
                        isValidUTF8 = false;
                        break;
                    }
                } else if (byte >= 0xE0 && byte <= 0xEF) {
                    if (i + 2 < text.size()) {
                        unsigned char next1 = static_cast<unsigned char>(text[i + 1]);
                        unsigned char next2 = static_cast<unsigned char>(text[i + 2]);
                        if (((next1 & 0xC0) != 0x80) || ((next2 & 0xC0) != 0x80)) {
                            isValidUTF8 = false;
                            break;
                        }
                        i += 3;
                    } else {
                        isValidUTF8 = false;
                        break;
                    }
                } else {
                    isValidUTF8 = false;
                    break;
                }
            }
            
            return isValidUTF8 ? "utf-8" : "binary";
        } catch (...) {
            return "binary";
        }
        
    } catch (...) {
        return "unknown";
    }
}

bool SerializationUtils::isTextFile(const std::string& filepath) {
    std::string encoding = detectFileEncoding(filepath);
    return encoding == "utf-8" || encoding == "utf-16le" || encoding == "utf-16be" || encoding == "ascii";
}

bool SerializationUtils::isBinaryFile(const std::string& filepath) {
    return !isTextFile(filepath);
}

bool SerializationUtils::writeAtomic(const std::string& filepath, const std::string& content,
                                    std::shared_ptr<IProgressReporter> reporter) {
    try {
        std::filesystem::path path(filepath);
        std::string tempPath = path.parent_path().string() + "/" + ".tmp_" + path.filename().string();
        
        // Write to temporary file
        std::ofstream tempFile(tempPath, std::ios::binary);
        if (!tempFile) {
            throw FileIOError(tempPath, FileIOError::ErrorType::WriteError, "Cannot create temporary file");
        }
        
        tempFile.write(content.c_str(), content.size());
        if (!tempFile.good()) {
            std::filesystem::remove(tempPath);
            throw FileIOError(tempPath, FileIOError::ErrorType::WriteError, "Failed to write to temporary file");
        }
        
        tempFile.close();
        
        // Verify file size
        auto tempSize = std::filesystem::file_size(tempPath);
        if (tempSize != content.size()) {
            std::filesystem::remove(tempPath);
            throw FileIOError(tempPath, FileIOError::ErrorType::CorruptFile, "Temporary file size mismatch");
        }
        
        if (reporter) {
            reporter->onProgress(0.5, "Verifying temporary file");
        }
        
        // Atomic replacement
        if (std::filesystem::exists(filepath)) {
            std::filesystem::rename(filepath, filepath + ".bak");
        }
        
        std::filesystem::rename(tempPath, filepath);
        
        if (reporter) {
            reporter->onProgress(1.0, "File saved successfully");
        }
        
        // Clean up backup if requested
        if (std::filesystem::exists(filepath + ".bak")) {
            std::filesystem::remove(filepath + ".bak");
        }
        
        return true;
        
    } catch (const std::exception& e) {
        GlobalExceptionHandler::handle(e);
        
        // Clean up temporary file if it exists
        std::filesystem::path path(filepath);
        std::string tempPath = path.parent_path().string() + "/" + ".tmp_" + path.filename().string();
        if (std::filesystem::exists(tempPath)) {
            std::filesystem::remove(tempPath);
        }
        
        return false;
    }
}

std::string SerializationUtils::calculateMD5(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        return "";
    }
    
    MD5_CTX md5;
    MD5_Init(&md5);
    
    char buffer[8192];
    while (file.read(buffer, sizeof(buffer))) {
        MD5_Update(&md5, buffer, file.gcount());
    }
    
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5_Final(digest, &md5);
    
    std::stringstream ss;
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];
    }
    
    return ss.str();
}

std::string SerializationUtils::calculateSHA256(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        return "";
    }
    
    SHA256_CTX sha;
    SHA256_Init(&sha);
    
    char buffer[8192];
    while (file.read(buffer, sizeof(buffer))) {
        SHA256_Update(&sha, buffer, file.gcount());
    }
    
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256_Final(digest, &sha);
    
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];
    }
    
    return ss.str();
}

uint64_t SerializationUtils::getFileSize(const std::string& filepath) {
    std::filesystem::path path(filepath);
    if (std::filesystem::exists(path) && std::filesystem::is_regular_file(path)) {
        return std::filesystem::file_size(path);
    }
    return 0;
}

bool SerializationUtils::hasDiskSpace(const std::string& filepath, uint64_t requiredBytes) {
    std::filesystem::path path(filepath);
    auto space = std::filesystem::disk_space(path.root_path());
    return space.available >= requiredBytes;
}

std::string SerializationUtils::sanitizePath(const std::string& path) {
    std::filesystem::path sanitized(path);
    
    // Remove path traversal attempts
    std::string normalized = sanitized.normalize().string();
    
    // Replace backslashes with forward slashes
    std::replace(normalized.begin(), normalized.end(), '\\', '/');
    
    return normalized;
}

std::string SerializationUtils::normalizePath(const std::string& path) {
    std::filesystem::path sanitized(path);
    return sanitized.normalize().string();
}

bool SerializationUtils::isPathSafe(const std::string& path) {
    try {
        std::filesystem::path sanitizedPath = sanitizePath(path);
        
        // Check for path traversal
        if (sanitizedPath.is_relative()) {
            return false;
        }
        
        // Check for null bytes
        if (sanitizedPath.string().find('\0') != std::string::npos) {
            return false;
        }
        
        return true;
        
    } catch (...) {
        return false;
    }
}

// ConfigUtils implementation
bool ConfigUtils::validateConfigKey(const std::string& key) {
    // Check for empty key
    if (key.empty()) {
        return false;
    }
    
    // Check for invalid characters
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    // Check for reserved keywords
    const std::vector<std::string> reserved = {
        "NULL", "TRUE", "FALSE", "NONE", "DEFAULT", "INVALID"
    };
    
    for (const auto& reservedKey : reserved) {
        if (toLower(key) == toLower(reservedKey)) {
            return false;
        }
    }
    
    return true;
}

bool ConfigUtils::validateConfigValue(const std::string& key, const std::string& value) {
    // Basic validation - can be extended based on key type
    if (value.empty()) {
        return true;  // Empty values might be valid
    }
    
    // Check for null bytes
    if (value.find('\0') != std::string::npos) {
        return false;
    }
    
    // Check for control characters (except whitespace)
    for (char c : value) {
        if (c < 32 && c != '\t' && c != '\n' && c != '\r') {
            return false;
        }
    }
    
    return true;
}

std::string ConfigUtils::suggestSimilarKey(const std::string& key,
                                          const std::vector<std::string>& availableKeys) {
    std::string bestMatch;
    size_t bestDistance = std::string::npos;
    
    for (const auto& availableKey : availableKeys) {
        size_t distance = levenshteinDistance(key, availableKey);
        if (distance < bestDistance || bestDistance == std::string::npos) {
            bestDistance = distance;
            bestMatch = availableKey;
        }
    }
    
    // Only suggest if distance is small enough
    if (bestDistance <= 3) {
        return bestMatch;
    }
    
    return "";
}

bool ConfigUtils::mergeConfigs(const std::string& baseFile, const std::string& overrideFile,
                               const std::string& outputFile, bool backupOriginal) {
    try {
        if (backupOriginal) {
            SerializationUtils::writeAtomic(baseFile + ".backup",
                                           std::ifstream(baseFile).rdbuf(), std::ofstream(baseFile + ".backup"));
        }
        
        // Implement merge logic
        // For now, just copy override file
        SerializationUtils::writeAtomic(outputFile, std::ifstream(overrideFile).rdbuf());
        
        return true;
        
    } catch (const std::exception& e) {
        GlobalExceptionHandler::handle(e);
        return false;
    }
}

bool ConfigUtils::configsEqual(const std::string& file1, const std::string& file2) {
    std::ifstream f1(file1, std::ios::binary);
    std::ifstream f2(file2, std::ios::binary);
    
    if (!f1 || !f2) {
        return false;
    }
    
    std::istreambuf_iterator<char> begin1(f1), end1;
    std::istreambuf_iterator<char> begin2(f2), end2;
    
    return std::equal(begin1, end1, begin2, end2);
}

std::string ConfigUtils::diffConfigs(const std::string& file1, const std::string& file2) {
    // Implement diff logic
    // For now, return empty string
    return "";
}

std::string ConfigUtils::optimizeConfig(const std::string& filepath,
                                        const std::map<std::string, std::string>& optimizations) {
    // Implement optimization logic
    // For now, return original filepath
    return filepath;
}

bool ConfigUtils::isConfigSecure(const std::string& filepath) {
    try {
        std::filesystem::path path(filepath);
        
        // Check file permissions
        auto perms = std::filesystem::status(path).permissions();
        
        // Ideally, config files should not be executable
        if (perms & std::filesystem::perms::owner_exec) {
            return false;
        }
        if (perms & std::filesystem::perms::group_exec) {
            return false;
        }
        if (perms & std::filesystem::perms::others_exec) {
            return false;
        }
        
        return true;
        
    } catch (...) {
        return false;
    }
}

bool ConfigUtils::sanitizeConfigFile(const std::string& filepath) {
    try {
        std::ifstream input(filepath);
        std::string content((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
        
        // Basic sanitization - remove potentially dangerous content
        // This is a placeholder implementation
        content = StringUtils::replaceAll(content, "../../", "");
        content = StringUtils::replaceAll(content, "../", "");
        
        std::ofstream output(filepath + ".sanitized");
        output << content;
        
        return true;
        
    } catch (...) {
        return false;
    }
}

// MathUtils implementation

double MathUtils::clamp(double value, double min, double max) {
    return std::max(min, std::min(value, max));
}

double MathUtils::lerp(double a, double b, double t) {
    return a + (b - a) * t;
}

double MathUtils::normalize(double value, double min, double max) {
    return (value - min) / (max - min);
}

double MathUtils::denormalize(double normalized, double min, double max) {
    return min + normalized * (max - min);
}

double MathUtils::mean(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double MathUtils::variance(const std::vector<double>& values) {
    if (values.size() <= 1) return 0.0;
    double m = mean(values);
    double sum = 0.0;
    for (double v : values) {
        sum += (v - m) * (v - m);
    }
    return sum / (values.size() - 1);
}

double MathUtils::standardDeviation(const std::vector<double>& values) {
    return std::sqrt(variance(values));
}

double MathUtils::median(std::vector<double> values) {
    if (values.empty()) return 0.0;
    std::sort(values.begin(), values.end());
    size_t n = values.size();
    if (n % 2 == 0) {
        return (values[n / 2 - 1] + values[n / 2]) / 2.0;
    } else {
        return values[n / 2];
    }
}

double MathUtils::dotProduct(const std::vector<double>& a, const std::vector<double>& b) {
    if (a.size() != b.size()) return 0.0;
    double result = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        result += a[i] * b[i];
    }
    return result;
}

double MathUtils::magnitude(const std::vector<double>& v) {
    return std::sqrt(dotProduct(v, v));
}

double MathUtils::normalizeVector(std::vector<double>& v) {
    double mag = magnitude(v);
    if (mag > 0.0) {
        for (double& val : v) {
            val /= mag;
        }
    }
    return mag;
}

double MathUtils::angleBetween(const std::vector<double>& a, const std::vector<double>& b) {
    double dot = dotProduct(a, b);
    double magA = magnitude(a);
    double magB = magnitude(b);
    if (magA * magB == 0.0) return 0.0;
    double cosAngle = dot / (magA * magB);
    cosAngle = clamp(cosAngle, -1.0, 1.0);
    return std::acos(cosAngle);
}

std::vector<std::vector<double>> MathUtils::matrixMultiply(const std::vector<std::vector<double>>& A,
                                                           const std::vector<std::vector<double>>& B) {
    size_t rowsA = A.size();
    size_t colsA = A[0].size();
    size_t rowsB = B.size();
    size_t colsB = B[0].size();
    
    if (colsA != rowsB) return {};
    
    std::vector<std::vector<double>> result(rowsA, std::vector<double>(colsB, 0.0));
    
    for (size_t i = 0; i < rowsA; ++i) {
        for (size_t j = 0; j < colsB; ++j) {
            for (size_t k = 0; k < colsA; ++k) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    
    return result;
}

std::vector<std::vector<double>> MathUtils::transpose(const std::vector<std::vector<double>>& matrix) {
    size_t rows = matrix.size();
    if (rows == 0) return {};
    size_t cols = matrix[0].size();
    
    std::vector<std::vector<double>> result(cols, std::vector<double>(rows));
    
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            result[j][i] = matrix[i][j];
        }
    }
    
    return result;
}

std::vector<std::vector<double>> MathUtils::identityMatrix(size_t size) {
    std::vector<std::vector<double>> result(size, std::vector<double>(size, 0.0));
    for (size_t i = 0; i < size; ++i) {
        result[i][i] = 1.0;
    }
    return result;
}

bool MathUtils::isNear(double a, double b, double epsilon) {
    return std::abs(a - b) < epsilon;
}

bool MathUtils::isPositive(double value, double epsilon) {
    return value > epsilon;
}

bool MathUtils::isNegative(double value, double epsilon) {
    return value < -epsilon;
}

double MathUtils::roundToPrecision(double value, int precision) {
    double factor = std::pow(10.0, precision);
    return std::round(value * factor) / factor;
}

double MathUtils::floorToPrecision(double value, int precision) {
    double factor = std::pow(10.0, precision);
    return std::floor(value * factor) / factor;
}

double MathUtils::ceilToPrecision(double value, int precision) {
    double factor = std::pow(10.0, precision);
    return std::ceil(value * factor) / factor;
}

double MathUtils::safeDivide(double numerator, double denominator, double defaultValue) {
    if (std::abs(denominator) < 1e-10) {
        return defaultValue;
    }
    return numerator / denominator;
}

double MathUtils::safeLog(double value, double defaultValue) {
    if (value <= 0.0) {
        return defaultValue;
    }
    return std::log(value);
}

double MathUtils::safeSqrt(double value, double defaultValue) {
    if (value < 0.0) {
        return defaultValue;
    }
    return std::sqrt(value);
}

double MathUtils::safeExp(double value, double defaultValue) {
    if (value > 700.0) {  // Prevent overflow
        return defaultValue;
    }
    return std::exp(value);
}

double MathUtils::erf(double x) {
    // Approximation of error function
    double a = 0.3480242 * x + (-0.0958798) * x * x;
    double b = 1.0 - exp(-x * x) * (a + 1.0);
    return sign(x) * std::sqrt(1.0 - b);
}

double MathUtils::erfc(double x) {
    return 1.0 - erf(x);
}

double MathUtils::phi(double x) {
    return (1.0 / std::sqrt(2.0 * M_PI)) * std::exp(-x * x / 2.0);
}

double MathUtils::Phi(double x) {
    return 0.5 * (1.0 + erf(x / std::sqrt(2.0)));
}

double MathUtils::relu(double x) {
    return std::max(0.0, x);
}

double MathUtils::leakyRelu(double x, double alpha) {
    return (x > 0.0) ? x : alpha * x;
}

double MathUtils::sigmoid(double x) {
    return 1.0 / (1.0 + std::exp(-x));
}

double MathUtils::tanh(double x) {
    return std::tanh(x);
}

double MathUtils::softmax(double x) {
    return std::exp(x) / (1.0 + std::exp(x));
}

double MathUtils::mse(double prediction, double target) {
    return (prediction - target) * (prediction - target) / 2.0;
}

double MathUtils::crossEntropy(double prediction, double target) {
    double p = clamp(prediction, 1e-15, 1.0 - 1e-15);
    double t = clamp(target, 1e-15, 1.0 - 1e-15);
    return -t * std::log(p) - (1.0 - t) * std::log(1.0 - p);
}

double MathUtils::hingeLoss(double prediction, double target) {
    double margin = 1.0 - target * prediction;
    return std::max(0.0, margin);
}

// StringUtils implementation

std::string StringUtils::format(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    std::string result = vformat(format, args);
    va_end(args);
    return result;
}

std::string StringUtils::vformat(const std::string& format, va_list args) {
    // Implementation using snprintf
    char buffer[4096];
    vsnprintf(buffer, sizeof(buffer), format.c_str(), args);
    return std::string(buffer);
}

std::string StringUtils::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::string StringUtils::ltrim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    return str.substr(start);
}

std::string StringUtils::rtrim(const std::string& str) {
    size_t end = str.find_last_not_of(" \t\n\r");
    if (end == std::string::npos) return "";
    return str.substr(0, end + 1);
}

std::vector<std::string> StringUtils::split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        result.push_back(token);
    }
    
    return result;
}

std::string StringUtils::join(const std::vector<std::string>& parts, const std::string& delimiter) {
    std::ostringstream oss;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) oss << delimiter;
        oss << parts[i];
    }
    return oss.str();
}

bool StringUtils::matchesPattern(const std::string& str, const std::string& pattern) {
    // Simple wildcard matching
    bool inWild = false;
    for (size_t i = 0; i < str.size(); ++i) {
        if (inWild) {
            if (pattern[i] == '*') {
                continue;
            } else if (pattern[i] == '?') {
                // Match any single character
                i++;
            } else {
                // Unexpected pattern character
                return false;
            }
        } else {
            if (str[i] == pattern[i] || pattern[i] == '*') {
                if (pattern[i] == '*') {
                    inWild = true;
                }
            } else {
                return false;
            }
        }
    }
    return true;
}

std::string StringUtils::replaceAll(const std::string& str, const std::string& from, const std::string& to) {
    std::string result = str;
    size_t pos = 0;
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    return result;
}

std::string StringUtils::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string StringUtils::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string StringUtils::sha256(const std::string& str) {
    SHA256_CTX sha;
    SHA256_Init(&sha);
    SHA256_Update(&sha, str.c_str(), str.size());
    
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256_Final(digest, &sha);
    
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];
    }
    
    return ss.str();
}

std::string StringUtils::md5(const std::string& str) {
    MD5_CTX md5;
    MD5_Init(&md5);
    MD5_Update(&md5, str.c_str(), str.size());
    
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5_Final(digest, &md5);
    
    std::stringstream ss;
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];
    }
    
    return ss.str();
}

std::string StringUtils::base64Encode(const std::string& str) {
    static const std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    std::stringstream result;
    int val = 0;
    int bits = 0;
    
    for (unsigned char c : str) {
        val = (val << 8) | c;
        bits += 8;
        
        while (bits >= 6) {
            bits -= 6;
            result << alphabet[(val >> bits) & 0x3F];
        }
    }
    
    if (bits > 0) {
        result << alphabet[(val << (6 - bits)) & 0x3F];
    }
    
    // Add padding
    int padCount = (3 - (str.size() % 3)) % 3;
    for (int i = 0; i < padCount; ++i) {
        result << '=';
    }
    
    return result.str();
}

std::string StringUtils::base64Decode(const std::string& str) {
    static const std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    static const std::string reverseAlphabet(256, '\0');
    
    // Build reverse lookup table
    for (int i = 0; i < alphabet.size(); ++i) {
        reverseAlphabet[static_cast<unsigned char>(alphabet[i])] = i;
    }
    
    std::string result;
    int val = 0;
    int bits = 0;
    
    for (char c : str) {
        if (c == '=') continue;  // Skip padding
        
        int charIndex = static_cast<unsigned char>(c);
        if (charIndex >= reverseAlphabet.size() || reverseAlphabet[charIndex] == '\0') {
            continue;  // Skip invalid characters
        }
        
        int charValue = reverseAlphabet[charIndex];
        val = (val << 6) | charValue;
        bits += 6;
        
        if (bits >= 8) {
            bits -= 8;
            result += static_cast<char>((val >> bits) & 0xFF);
        }
    }
    
    return result;
}

std::string StringUtils::generateUUID() {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    
    // Generate random bits
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned int> dis(0, UINT32_MAX);
    
    // UUID version 4 (random)
    ss << dis(gen) << "-";
    ss << dis(gen) << "-";
    ss << (0x4000 | (dis(gen) & 0x0FFF)) << "-";
    ss << (0x8000 | (dis(gen) & 0x3FFF)) << "-";
    ss << dis(gen) << dis(gen) << dis(gen);
    
    return ss.str();
}

std::string StringUtils::generateSessionId() {
    return generateUUID() + "_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
}

size_t StringUtils::countOccurrences(const std::string& str, const std::string& substring) {
    if (substring.empty()) return 0;
    
    size_t count = 0;
    size_t pos = 0;
    while ((pos = str.find(substring, pos)) != std::string::npos) {
        count++;
        pos += substring.length();
    }
    
    return count;
}

bool StringUtils::containsWhitespace(const std::string& str) {
    for (char c : str) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            return true;
        }
    }
    return false;
}

bool StringUtils::isPrintable(const std::string& str) {
    for (char c : str) {
        if (!std::isprint(static_cast<unsigned char>(c))) {
            return false;
        }
    }
    return true;
}

bool StringUtils::isNumeric(const std::string& str) {
    if (str.empty()) return false;
    
    bool hasDecimal = false;
    bool hasSign = false;
    
    for (char c : str) {
        if (c == '+' || c == '-') {
            if (hasSign) return false;
            hasSign = true;
        } else if (c == '.') {
            if (hasDecimal) return false;
            hasDecimal = true;
        } else if (!std::isdigit(static_cast<unsigned char>(c))) {
            return false;
        }
    }
    
    return true;
}

std::string StringUtils::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    // Add milliseconds
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

std::string StringUtils::formatTimestamp(const std::chrono::system_clock::time_point& timepoint) {
    auto time = std::chrono::system_clock::to_time_t(timepoint);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    return ss.str();
}

std::chrono::system_clock::time_point StringUtils::parseTimestamp(const std::string& timestamp) {
    std::tm tm = {};
    std::stringstream ss(timestamp);
    
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
        throw ConfigurationException("Invalid timestamp format: " + timestamp);
    }
    
    std::time_t time = std::mktime(&tm);
    return std::chrono::system_clock::from_time_t(time);
}

// ThreadUtils implementation

void ThreadUtils::spinLock(std::atomic_flag& lock) {
    while (std::atomic_flag_test_and_set_explicit(&lock, std::memory_order_acquire)) {}
}

void ThreadUtils::spinUnlock(std::atomic_flag& lock) {
    std::atomic_flag_clear_explicit(&lock, std::memory_order_release);
}

bool ThreadUtils::tryLock(std::atomic_flag& lock) {
    return !std::atomic_flag_test_and_set_explicit(&lock, std::memory_order_acquire);
}

void ThreadUtils::atomicIncrement(std::atomic<size_t>& counter) {
    std::atomic_fetch_add_explicit(&counter, 1, std::memory_order_relaxed);
}

size_t ThreadUtils::atomicDecrement(std::atomic<size_t>& counter) {
    return std::atomic_fetch_sub_explicit(&counter, 1, std::memory_order_relaxed);
}

size_t ThreadUtils::atomicRead(const std::atomic<size_t>& counter) {
    return std::atomic_load_explicit(&counter, std::memory_order_relaxed);
}

void ThreadUtils::atomicWrite(std::atomic<size_t>& counter, size_t value) {
    std::atomic_store_explicit(&counter, value, std::memory_order_relaxed);
}

void ThreadUtils::lockMutex(std::mutex& mutex) {
    mutex.lock();
}

bool ThreadUtils::tryLockMutex(std::mutex& mutex) {
    return mutex.try_lock();
}

void ThreadUtils::unlockMutex(std::mutex& mutex) {
    mutex.unlock();
}

void ThreadUtils::waitCondition(std::condition_variable& cv, std::mutex& mutex,
                               std::function<bool()> predicate) {
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, predicate);
}

void ThreadUtils::notifyCondition(std::condition_variable& cv) {
    cv.notify_one();
}

void ThreadUtils::notifyAllCondition(std::condition_variable& cv) {
    cv.notify_all();
}

void ThreadUtils::setThreadLocal(const std::string& key, const std::string& value) {
    thread_local static std::map<std::string, std::string> threadLocalStorage;
    threadLocalStorage[key] = value;
}

std::string ThreadUtils::getThreadLocal(const std::string& key) {
    thread_local static std::map<std::string, std::string> threadLocalStorage;
    auto it = threadLocalStorage.find(key);
    if (it != threadLocalStorage.end()) {
        return it->second;
    }
    return "";
}

void ThreadUtils::removeThreadLocal(const std::string& key) {
    thread_local static std::map<std::string, std::string> threadLocalStorage;
    threadLocalStorage.erase(key);
}

void ThreadUtils::createThreadPool(size_t numThreads) {
    // Implementation would create and manage thread pool
    // For now, this is a placeholder
}

void ThreadUtils::destroyThreadPool() {
    // Implementation would clean up thread pool
    // For now, this is a placeholder
}

bool ThreadUtils::submitTask(std::function<void()> task) {
    // Implementation would submit task to thread pool
    // For now, this is a placeholder
    return true;
}

std::future<void> ThreadUtils::submitTaskAndGetFuture(std::function<void()> task) {
    // Implementation would submit task to thread pool and return future
    // For now, this is a placeholder
    return std::async(std::launch::deferred, task);
}

std::vector<double> ThreadUtils::parallelForEach(const std::vector<double>& input,
                                                 std::function<double(double)> func,
                                                 size_t numThreads) {
    size_t numCPU = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = numCPU;
    
    std::vector<double> result(input.size());
    
    if (numThreads <= 1 || input.size() <= 1000) {
        // Sequential execution
        for (size_t i = 0; i < input.size(); ++i) {
            result[i] = func(input[i]);
        }
    } else {
        // Parallel execution
        size_t chunkSize = input.size() / numThreads;
        std::vector<std::future<double>> futures;
        
        for (size_t i = 0; i < numThreads; ++i) {
            size_t start = i * chunkSize;
            size_t end = (i == numThreads - 1) ? input.size() : start + chunkSize;
            
            futures.push_back(std::async(std::launch::async, [func, &input, start, end]() {
                double localSum = 0.0;
                for (size_t j = start; j < end; ++j) {
                    localSum += func(input[j]);
                }
                return localSum;
            }));
        }
        
        double totalSum = 0.0;
        for (auto& future : futures) {
            totalSum += future.get();
        }
        
        // For now, just return sequential result (parallel implementation would be complex)
        for (size_t i = 0; i < input.size(); ++i) {
            result[i] = func(input[i]);
        }
    }
    
    return result;
}

std::vector<double> ThreadUtils::parallelReduce(const std::vector<double>& input,
                                                 std::function<double(double, double)> func,
                                                 double initialValue,
                                                 size_t numThreads) {
    size_t numCPU = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = numCPU;
    
    std::vector<double> results(numThreads, initialValue);
    
    if (numThreads <= 1 || input.size() <= 1000) {
        // Sequential execution
        for (double val : input) {
            results[0] = func(results[0], val);
        }
    } else {
        // Parallel execution
        size_t chunkSize = input.size() / numThreads;
        std::vector<std::future<double>> futures;
        
        for (size_t i = 0; i < numThreads; ++i) {
            size_t start = i * chunkSize;
            size_t end = (i == numThreads - 1) ? input.size() : start + chunkSize;
            
            futures.push_back(std::async(std::launch::async, [func, &input, start, end, initialValue]() {
                double localResult = initialValue;
                for (size_t j = start; j < end; ++j) {
                    localResult = func(localResult, input[j]);
                }
                return localResult;
            }));
        }
        
        // Combine results
        for (auto& future : futures) {
            results[0] = func(results[0], future.get());
        }
    }
    
    return {results[0]};
}

double ThreadUtils::getThreadCPUTime(size_t threadId) {
    // Implementation would get CPU time for specific thread
    // For now, return 0.0
    return 0.0;
}

double ThreadUtils::getThreadWallTime(size_t threadId) {
    // Implementation would get wall time for specific thread
    // For now, return 0.0
    return 0.0;
}

std::vector<double> ThreadUtils::getAllThreadCPUTimes() {
    return {};
}

std::vector<double> ThreadUtils::getAllThreadWallTimes() {
    return {};
}

bool ThreadUtils::isThreadSafe() {
    // Implementation would check thread safety
    // For now, return true
    return true;
}

bool ThreadUtils::checkThreadSafety(std::function<bool()> test) {
    return test();
}

std::vector<std::string> ThreadUtils::getThreadSafetyViolations() {
    return {};
}

// ValidationUtils implementation

bool ValidationUtils::isValidInteger(const std::string& str) {
    if (str.empty()) return false;
    
    size_t pos;
    try {
        std::stoi(str, pos);
        return pos == str.size();
    } catch (...) {
        return false;
    }
}

bool ValidationUtils::isValidDouble(const std::string& str) {
    if (str.empty()) return false;
    
    size_t pos;
    try {
        std::stod(str, pos);
        return pos == str.size();
    } catch (...) {
        return false;
    }
}

bool ValidationUtils::isValidBoolean(const std::string& str) {
    if (str.empty()) return false;
    
    return str == "true" || str == "false";
}

bool ValidationUtils::isValidString(const std::string& str) {
    // Check for control characters (except whitespace)
    for (char c : str) {
        if (c < 32 && c != '\t' && c != '\n' && c != '\r') {
            return false;
        }
    }
    
    // Check for null bytes
    if (str.find('\0') != std::string::npos) {
        return false;
    }
    
    return true;
}

bool ValidationUtils::isValidIPv4(const std::string& str) {
    // Simple IPv4 validation
    std::vector<std::string> parts = StringUtils::split(str, '.');
    if (parts.size() != 4) return false;
    
    for (const auto& part : parts) {
        if (!isValidInteger(part)) return false;
        int value = std::stoi(part);
        if (value < 0 || value > 255) return false;
    }
    
    return true;
}

bool ValidationUtils::isValidIPv6(const std::string& str) {
    // Simple IPv6 validation (placeholder)
    return str.find(':') != std::string::npos;
}

bool ValidationUtils::isValidMAC(const std::string& str) {
    // Simple MAC address validation
    std::vector<std::string> parts = StringUtils::split(str, ':');
    if (parts.size() != 6) return false;
    
    for (const auto& part : parts) {
        if (part.size() != 2) return false;
        if (!isValidInteger("0x" + part)) return false;
    }
    
    return true;
}

bool ValidationUtils::isValidUUID(const std::string& str) {
    // UUID validation (version 4)
    std::regex uuidRegex("^[0-9a-f]{8}-[0-9a-f]{4}-4[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$", std::regex::icase);
    return std::regex_match(str, uuidRegex);
}

bool ValidationUtils::isInRange(double value, double min, double max) {
    return value >= min && value <= max;
}

bool ValidationUtils::isInRange(int value, int min, int max) {
    return value >= min && value <= max;
}

bool ValidationUtils::isInRange(const std::string& str, size_t min, size_t max) {
    try {
        size_t value = std::stoull(str);
        return value >= min && value <= max;
    } catch (...) {
        return false;
    }
}

bool ValidationUtils::isValidEmail(const std::string& email) {
    // Simple email validation
    size_t atPos = email.find('@');
    if (atPos == std::string::npos || atPos == 0 || atPos == email.size() - 1) {
        return false;
    }
    
    std::string domain = email.substr(atPos + 1);
    size_t dotPos = domain.find('.');
    return dotPos != std::string::npos && dotPos < domain.size() - 1;
}

bool ValidationUtils::isValidURL(const std::string& url) {
    // Simple URL validation
    return url.find("://") != std::string::npos;
}

bool ValidationUtils::isValidPath(const std::string& path) {
    try {
        std::filesystem::path p(path);
        return !p.empty();
    } catch (...) {
        return false;
    }
}

bool ValidationUtils::isValidFilename(const std::string& filename) {
    // Check for invalid characters in filename
    for (char c : filename) {
        if (c == '/' || c == '\\' || c == ':' || c == '*' || c == '?' || c == '"' || c == '<' || c == '>' || c == '|') {
            return false;
        }
    }
    
    return true;
}

bool ValidationUtils::containsOnlyAllowedCharacters(const std::string& str,
                                                    const std::string& allowedChars) {
    for (char c : str) {
        if (allowedChars.find(c) == std::string::npos) {
            return false;
        }
    }
    return true;
}

bool ValidationUtils::isASCII(const std::string& str) {
    for (char c : str) {
        if (static_cast<unsigned char>(c) > 127) {
            return false;
        }
    }
    return true;
}

bool ValidationUtils::isPrintable(const std::string& str) {
    for (char c : str) {
        if (!std::isprint(static_cast<unsigned char>(c))) {
            return false;
        }
    }
    return true;
}

bool ValidationUtils::validateNeuralConfig(const std::map<std::string, double>& config) {
    // Validate neural network configuration parameters
    const std::vector<std::string> validKeys = {
        "learning_rate", "hidden_size", "num_layers", "dropout_rate",
        "weight_init_std", "batch_size", "momentum"
    };
    
    for (const auto& pair : config) {
        if (std::find(validKeys.begin(), validKeys.end(), pair.first) == validKeys.end()) {
            return false;
        }
        
        // Check value ranges
        double value = pair.second;
        if (value < 0.0) {
            return false;
        }
    }
    
    return true;
}

bool ValidationUtils::validatePlasticityConfig(const std::map<std::string, double>& config) {
    // Validate plasticity configuration parameters
    const std::vector<std::string> validKeys = {
        "stdp_lr", "hebbian_lr", "synaptogenesis_rate", "pruning_rate",
        "weight_decay", "max_weight"
    };
    
    for (const auto& pair : config) {
        if (std::find(validKeys.begin(), validKeys.end(), pair.first) == validKeys.end()) {
            return false;
        }
        
        // Check value ranges
        double value = pair.second;
        if (value < 0.0 || value > 1.0) {
            return false;
        }
    }
    
    return true;
}

bool ValidationUtils::validateMemoryConfig(const std::map<std::string, size_t>& config) {
    // Validate memory configuration parameters
    const std::vector<std::string> validKeys = {
        "max_memory", "initial_memory", "growth_rate", "max_capacity"
    };
    
    for (const auto& pair : config) {
        if (std::find(validKeys.begin(), validKeys.end(), pair.first) == validKeys.end()) {
            return false;
        }
        
        // Check value ranges
        size_t value = pair.second;
        if (value == 0) {
            return false;
        }
    }
    
    return true;
}

bool ValidationUtils::isPerformanceAcceptable(double value, double threshold) {
    return value <= threshold;
}

bool ValidationUtils::isMemoryUsageAcceptable(size_t memoryBytes, size_t limitBytes) {
    return memoryBytes <= limitBytes;
}

bool ValidationUtils::isCPULoadAcceptable(double cpuLoad, double threshold) {
    return cpuLoad <= threshold;
}

bool ValidationUtils::isRecoverableError(const NLMException& error) {
    // Determine if error is recoverable based on error type
    std::string message = error.getMessage();
    
    // Check for recoverable error patterns
    if (message.find("not found") != std::string::npos) {
        return true;  // Missing file can be recovered
    }
    if (message.find("timeout") != std::string::npos) {
        return true;  // Timeout can be recovered with retry
    }
    if (message.find("locked") != std::string::npos) {
        return true;  // Lock can be recovered
    }
    
    return false;
}

std::string ValidationUtils::getErrorSeverity(const NLMException& error) {
    std::string message = error.getMessage();
    
    if (message.find("Critical") != std::string::npos || 
        message.find("Fatal") != std::string::npos) {
        return "Critical";
    } else if (message.find("Error") != std::string::npos) {
        return "Error";
    } else if (message.find("Warning") != std::string::npos) {
        return "Warning";
    }
    
    return "Info";
}

bool ValidationUtils::isCriticalError(const NLMException& error) {
    std::string severity = getErrorSeverity(error);
    return severity == "Critical";
}

// Global exception handler initialization
GlobalExceptionHandler::ExceptionHandler GlobalExceptionHandler::handler_;

} // namespace nlm
