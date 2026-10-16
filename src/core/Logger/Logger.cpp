// NLM Logger Implementation
// Enhanced logging with formatting and progress indicators

#include "core/Logger/Logger.hpp"
#include <sstream>
#include <iomanip>
#include <cstdarg>
#include <algorithm>
#include <chrono>
#include <ctime>

namespace nlm {

namespace LoggerImpl {
    const char* LOG_LEVEL_NAMES[] = {"QUIET", "ERROR", "WARNING", "INFO", "DEBUG", "VERBOSE"};
    const char* COLOR_CODES[] = {"", "\033[31m", "\033[33m", "\033[32m", "\033[36m", "\033[35m"};
    const char* RESET_CODE = "\033[0m";
}

void OutputFormatter::printBanner(const std::string& title, const std::string& subtitle) {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                                                               ║" << std::endl;
    std::cout << "║                            " << std::string(title.length() / 2, ' ') << title << std::string((80 - title.length()) / 2 - title.length() / 2, ' ') << "                            ║" << std::endl;
    if (!subtitle.empty()) {
        std::cout << "║                       " << std::string((80 - subtitle.length()) / 2, ' ') << subtitle << std::string((80 - subtitle.length()) / 2 - subtitle.length() / 2, ' ') << "                        ║" << std::endl;
    }
    std::cout << "║                                                                               ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "\n";
}

void OutputFormatter::printSection(const std::string& title) {
    std::cout << "\n";
    std::cout << "─" << std::string(80, '─') << "─" << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << "─" << std::string(80, '─') << "─" << std::endl;
    std::cout << "\n";
}

void OutputFormatter::printSubsection(const std::string& title) {
    std::cout << "\n";
    std::cout << "  " << title << ":" << std::endl;
    std::cout << "  " << std::string(title.length(), '-') << std::endl;
}

void OutputFormatter::printStatus(const std::string& label, const std::string& value) {
    std::cout << "    " << label << ": " << value << std::endl;
}

void OutputFormatter::printSuccess(const std::string& message) {
    std::cout << "✓ " << message << std::endl;
}

void OutputFormatter::printError(const std::string& message) {
    std::cout << "✗ " << message << std::endl;
}

void OutputFormatter::printWarning(const std::string& message) {
    std::cout << "⚠ " << message << std::endl;
}

void OutputFormatter::printProgress(const std::string& message) {
    std::cout << "⟳ " << message << "..." << std::endl;
}

void OutputFormatter::printDivider() {
    std::cout << "\n";
    std::cout << std::string(80, '─') << std::endl;
    std::cout << "\n";
}

void OutputFormatter::printTimestamped(const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::cout << "[" << std::put_time(std::localtime(&time), "%H:%M:%S") << "] " << message << std::endl;
}

void OutputFormatter::printFormatted(const std::string& format, ...) {
    va_list args;
    va_start(args, format);
    
    std::stringstream ss;
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format.c_str(), args);
    ss << buffer;
    
    va_end(args);
    std::cout << ss.str() << std::endl;
}

void OutputFormatter::printProgressBar(int current, int total, const std::string& label) {
    if (total <= 0) return;
    
    float percentage = static_cast<float>(current) / static_cast<float>(total);
    int barWidth = 50;
    int filledWidth = static_cast<int>(percentage * barWidth);
    
    std::cout << "\r";
    if (!label.empty()) {
        std::cout << "  " << label << ": ";
    }
    std::cout << "[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < filledWidth) {
            std::cout << "=";
        } else {
            std::cout << " ";
        }
    }
    std::cout << "] " << static_cast<int>(percentage * 100) << "%";
    
    if (current >= total) {
        std::cout << "\n";
    }
}

// ProgressIndicator implementation
ProgressIndicator::ProgressIndicator(const std::string& label, int total)
    : label(label), total(total), current(0), active(true) {
    startTime = std::chrono::steady_clock::now();
    drawProgressBar();
}

ProgressIndicator::~ProgressIndicator() {
    if (active) {
        complete();
    }
}

void ProgressIndicator::update(int current, const std::string& message) {
    if (current >= 0) {
        this->current = current;
    }
    drawProgressBar();
    if (!message.empty()) {
        std::cout << "  " << message << std::endl;
    }
}

void ProgressIndicator::increment() {
    current++;
    drawProgressBar();
}

void ProgressIndicator::setLabel(const std::string& label) {
    this->label = label;
    drawProgressBar();
}

void ProgressIndicator::complete() {
    if (!active) return;
    
    update(total);
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "\n";
    std::cout << "  Completed in " << duration.count() << " ms" << std::endl;
    active = false;
}

void ProgressIndicator::drawProgressBar() {
    OutputFormatter::printProgressBar(current, total, label);
}

// ConsoleLogger implementation
ConsoleLogger::ConsoleLogger(LogLevel level) : level(level) {}

ConsoleLogger::~ConsoleLogger() = default;

void ConsoleLogger::log(LogLevel level, const std::string& message) {
    if (level < this->level) return;
    
    std::cout << formatMessage(level, message);
}

LogLevel ConsoleLogger::getLevel() const {
    return level;
}

void ConsoleLogger::setLevel(LogLevel level) {
    this->level = level;
}

std::string ConsoleLogger::formatMessage(LogLevel level, const std::string& message) const {
    std::stringstream ss;
    
    if (level == LogLevel::Error) {
        ss << "\033[31m";  // Red
    } else if (level == LogLevel::Warning) {
        ss << "\033[33m";  // Yellow
    } else if (level == LogLevel::Info) {
        ss << "\033[32m";  // Green
    } else if (level == LogLevel::Debug) {
        ss << "\033[36m";  // Cyan
    } else if (level == LogLevel::Verbose) {
        ss << "\033[35m";  // Magenta
    }
    
    ss << "[" << LoggerImpl::LOG_LEVEL_NAMES[static_cast<int>(level)] << "] ";
    ss << message;
    
    if (level != LogLevel::Quiet) {
        ss << "\033[0m";  // Reset
    }
    
    ss << std::endl;
    return ss.str();
}

// MultiLogger implementation
MultiLogger::MultiLogger() : globalLevel(LogLevel::Info) {}

MultiLogger::~MultiLogger() = default;

void MultiLogger::addLogger(std::shared_ptr<ConsoleLogger> logger) {
    if (logger) {
        loggers.push_back(logger);
    }
}

void MultiLogger::log(LogLevel level, const std::string& message) {
    if (level < globalLevel) return;
    
    for (auto& logger : loggers) {
        logger->log(level, message);
    }
}

void MultiLogger::setGlobalLevel(LogLevel level) {
    globalLevel = level;
    for (auto& logger : loggers) {
        logger->setLevel(level);
    }
}

// PerformanceTimer implementation
PerformanceTimer::PerformanceTimer(const std::string& label) 
    : label(label), running(false) {}

PerformanceTimer::~PerformanceTimer() {
    if (running) {
        stop();
    }
}

void PerformanceTimer::start() {
    if (running) return;
    startTime = std::chrono::steady_clock::now();
    running = true;
}

void PerformanceTimer::stop() {
    if (!running) return;
    endTime = std::chrono::steady_clock::now();
    running = false;
}

double PerformanceTimer::getElapsedSeconds() const {
    if (running) {
        auto end = std::chrono::steady_clock::now();
        return std::chrono::duration<double>(end - startTime).count();
    } else {
        return std::chrono::duration<double>(endTime - startTime).count();
    }
}

double PerformanceTimer::getElapsedMilliseconds() const {
    return getElapsedSeconds() * 1000.0;
}

const std::string& PerformanceTimer::getLabel() const {
    return label;
}

// OperationResult implementation
OperationResult::OperationResult(bool success, const std::string& message)
    : success(success), message(message), elapsedTime(0.0) {}

void OperationResult::printSummary() const {
    if (success) {
        std::cout << "✓ " << message;
    } else {
        std::cout << "✗ " << message;
    }
    std::cout << std::endl;
}

// Metrics implementation
namespace Metrics {
    std::string formatBytes(size_t bytes) {
        static const char* suffixes[] = {"B", "KB", "MB", "GB", "TB"};
        int suffixIndex = 0;
        double size = static_cast<double>(bytes);
        
        while (size >= 1024.0 && suffixIndex < 4) {
            size /= 1024.0;
            suffixIndex++;
        }
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << size << " " << suffixes[suffixIndex];
        return ss.str();
    }
    
    std::string formatDuration(double seconds) {
        if (seconds < 60.0) {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(3) << seconds << "s";
            return ss.str();
        } else if (seconds < 3600.0) {
            double minutes = seconds / 60.0;
            std::stringstream ss;
            ss << std::fixed << std::setprecision(1) << minutes << "m";
            return ss.str();
        } else {
            double hours = seconds / 3600.0;
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << hours << "h";
            return ss.str();
        }
    }
    
    std::string formatTime(double seconds) {
        int hours = static_cast<int>(seconds / 3600.0);
        int minutes = static_cast<int>((seconds - hours * 3600.0) / 60.0);
        double secs = seconds - hours * 3600.0 - minutes * 60.0;
        
        std::stringstream ss;
        if (hours > 0) ss << hours << ":";
        ss << std::setfill('0') << std::setw(2) << minutes << ":";
        ss << std::setfill('0') << std::setw(6) << std::fixed << std::setprecision(3) << secs;
        return ss.str();
    }
    
    std::string capitalize(const std::string& str) {
        if (str.empty()) return str;
        return static_cast<char>(std::toupper(str[0])) + str.substr(1);
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
    
    bool startsWith(const std::string& str, const std::string& prefix) {
        return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
    }
    
    bool endsWith(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }
}

} // namespace nlm