#include "CLIUtils.hpp"

namespace nlm {
namespace cli {

void CLIUtils::printBanner(const std::string& toolName, const std::string& version) {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "NLM " << toolName << " v" << version << std::endl;
    std::cout << std::string(80, '=') << std::endl << std::endl;
}

void CLIUtils::printError(const std::string& message) {
    std::cerr << "ERROR: " << message << std::endl;
}

void CLIUtils::printWarning(const std::string& message) {
    std::cout << "WARNING: " << message << std::endl;
}

void CLIUtils::printSuccess(const std::string& message) {
    std::cout << "SUCCESS: " << message << std::endl;
}

void CLIUtils::printInfo(const std::string& message) {
    std::cout << "INFO: " << message << std::endl;
}

void CLIUtils::printHeader(const std::string& title) {
    std::cout << "\n" << title << std::endl;
    std::cout << std::string(title.length(), '=') << std::endl;
}

void CLIUtils::printSeparator(char separator, size_t width) {
    std::cout << std::string(width, separator) << std::endl;
}

std::string CLIUtils::currentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string CLIUtils::formatDuration(std::chrono::milliseconds duration) {
    auto totalMs = duration.count();
    auto hours = totalMs / (1000 * 60 * 60);
    auto minutes = (totalMs % (1000 * 60 * 60)) / (1000 * 60);
    auto seconds = (totalMs % (1000 * 60)) / 1000;
    auto ms = totalMs % 1000;
    
    std::stringstream ss;
    if (hours > 0) ss << hours << "h ";
    if (minutes > 0 || hours > 0) ss << minutes << "m ";
    ss << seconds << "." << std::setfill('0') << std::setw(3) << ms << "s";
    return ss.str();
}

std::string CLIUtils::trim(const std::string& str) {
    const size_t first = str.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) return str;
    const size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

bool CLIUtils::endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() &&
           0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

std::string CLIUtils::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string CLIUtils::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

static std::string humanReadableSizeImpl(size_t bytes) {
    const std::string units[] = {"B", "KB", "MB", "GB", "TB"};
    double size = static_cast<double>(bytes);
    int unitIndex = 0;
    
    while (size >= 1024.0 && unitIndex < 4) {
        size /= 1024.0;
        unitIndex++;
    }
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << size << " " << units[unitIndex];
    return ss.str();
}

void CLIUtils::ensureDirectoryExists(const std::string& path) {
    if (!directoryExists(path)) {
        std::error_code ec;
        std::filesystem::create_directories(path, ec);
        if (ec) {
            throw CLIError("Failed to create directory " + path + ": " + ec.message());
        }
    }
}

bool CLIUtils::fileExists(const std::string& path) {
    return std::filesystem::exists(path);
}

bool CLIUtils::directoryExists(const std::string& path) {
    return std::filesystem::is_directory(path);
}

} // namespace cli
} // namespace nlm
