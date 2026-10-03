// NLM CLI Utils Implementation File
// This file contains the implementation of CLI utility functions

#include "CLIUtils.hpp"
#include <cstring>
#include <codecvt>
#include <locale>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <sys/stat.h> // For file existence checks
#include <unistd.h>   // For getcwd

namespace nlm {
namespace cli {
namespace utils {

// String utilities
std::string trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.size();
    
    while (start < end && std::isspace(str[start])) {
        start++;
    }
    
    while (end > start && std::isspace(str[end - 1])) {
        end--;
    }
    
    return str.substr(start, end - start);
}

std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}

std::string capitalize(const std::string& str) {
    if (str.empty()) {
        return str;
    }
    
    std::string result = str;
    result[0] = std::toupper(result[0]);
    return result;
}

std::string leftPad(const std::string& str, size_t width) {
    if (str.length() >= width) {
        return str;
    }
    
    return std::string(width - str.length(), ' ') + str;
}

std::string centerText(const std::string& text, size_t width) {
    if (text.length() >= width) {
        return text;
    }
    
    size_t padding = (width - text.length()) / 2;
    size_t leftPadding = padding;
    size_t rightPadding = width - text.length() - leftPadding;
    
    return std::string(leftPadding, ' ') + text + std::string(rightPadding, ' ');
}

std::string rightPad(const std::string& str, size_t width) {
    if (str.length() >= width) {
        return str;
    }
    
    return str + std::string(width - str.length(), ' ');
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string join(const std::vector<std::string>& strings, const std::string& delimiter) {
    std::ostringstream result;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i > 0) {
            result << delimiter;
        }
        result << strings[i];
    }
    return result.str();
}

// Color output utilities
bool supportsColors() {
    // Check if output is a terminal
    #ifdef __linux__
    return isatty(fileno(stdout)) != 0;
    #else
    return true;
    #endif
}

void printColored(const std::string& text, const std::string& colorCode) {
    if (supportsColors() && !colorCode.empty()) {
        std::cout << colorCode << text << "\033[0m";
    } else {
        std::cout << text;
    }
}

void printError(const std::string& message) {
    if (supportsColors()) {
        std::cerr << "\033[31m[ERROR]\033[0m " << message << std::endl;
    } else {
        std::cerr << "[ERROR] " << message << std::endl;
    }
}

void printWarning(const std::string& message) {
    if (supportsColors()) {
        std::cout << "\033[33m[WARNING]\033[0m " << message << std::endl;
    } else {
        std::cout << "[WARNING] " << message << std::endl;
    }
}

void printInfo(const std::string& message) {
    std::cout << "[INFO] " << message << std::endl;
}

void printSuccess(const std::string& message) {
    if (supportsColors()) {
        std::cout << "\033[32m[SUCCESS]\033[0m " << message << std::endl;
    } else {
        std::cout << "[SUCCESS] " << message << std::endl;
    }
}

void printDebug(const std::string& message) {
    if (supportsColors()) {
        std::cout << "\033[36m[DEBUG]\033[0m " << message << std::endl;
    } else {
        std::cout << "[DEBUG] " << message << std::endl;
    }
}

// Progress and timing utilities
std::string formatDuration(std::chrono::duration<double> duration) {
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    duration -= seconds;
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    
    std::ostringstream result;
    result << seconds.count() << "." << std::setfill('0') << std::setw(3)
           << (milliseconds.count() % 1000) << "s";
    return result.str();
}

std::string formatMemorySize(size_t bytes) {
    static const char* suffixes[] = {"B", "KB", "MB", "GB", "TB"};
    int suffixIndex = 0;
    double size = static_cast<double>(bytes);
    
    while (size >= 1024.0 && suffixIndex < 4) {
        size /= 1024.0;
        suffixIndex++;
    }
    
    std::ostringstream result;
    result << std::fixed << std::setprecision(2) << size << " " << suffixes[suffixIndex];
    return result.str();
}

void printProgress(const ProgressInfo& info) {
    std::cout << "\r"; // Carriage return to overwrite line
    
    // Progress bar
    std::cout << "[";
    size_t barWidth = 50;
    size_t filledWidth = static_cast<size_t>(info.progress * barWidth);
    
    for (size_t i = 0; i < barWidth; ++i) {
        if (i < filledWidth) {
            std::cout << "=";
        } else if (i == filledWidth) {
            std::cout << ">";
        } else {
            std::cout << " ";
        }
    }
    
    std::cout << "] " << std::fixed << std::setprecision(1) << (info.progress * 100) << "%";
    
    if (!info.description.empty()) {
        std::cout << " - " << info.description;
    }
    
    if (info.showETA && info.progress > 0.0) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now() - info.startTime).count();
        auto remaining = static_cast<double>(elapsed) / info.progress - elapsed;
        
        if (remaining > 0) {
            int minutes = static_cast<int>(remaining) / 60;
            int seconds = static_cast<int>(remaining) % 60;
            std::cout << " - ETA: ";
            if (minutes > 0) {
                std::cout << minutes << "m ";
            }
            std::cout << seconds << "s";
        }
    }
    
    std::cout.flush();
}

// Configuration utilities
double parseFloat(const std::string& str) {
    std::istringstream iss(str);
    double value;
    iss >> value;
    return value;
}

int64_t parseInt(const std::string& str) {
    std::istringstream iss(str);
    int64_t value;
    iss >> value;
    return value;
}

bool parseBool(const std::string& str) {
    std::string lowerStr = toLower(str);
    return (lowerStr == "true" || lowerStr == "yes" || lowerStr == "1");
}

std::string currentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto inTimeT = std::chrono::system_clock::to_time_t(now);
    
    std::ostringstream result;
    result << std::put_time(std::localtime(&inTimeT), "%Y-%m-%d %H:%M:%S");
    return result.str();
}

// File utilities
bool fileExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

bool directoryExists(const std::string& path) {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) != 0) {
        return false;
    }
    return S_ISDIR(buffer.st_mode);
}

std::string getFileExtension(const std::string& filename) {
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos) {
        return "";
    }
    return filename.substr(dotPos + 1);
}

std::string getFileName(const std::string& path) {
    size_t slashPos = path.find_last_of('/');
    size_t backslashPos = path.find_last_of('\\');
    size_t lastSep = std::max(slashPos, backslashPos);
    
    if (lastSep == std::string::npos) {
        return path;
    }
    
    return path.substr(lastSep + 1);
}

} // namespace utils

} // namespace cli
} // namespace nlm