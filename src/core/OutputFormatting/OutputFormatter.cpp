# Output formatting implementation
#include "OutputFormatter.hpp"
#include <sstream>
#include <iomanip>
#include <cmath>

namespace nlm {

std::string OutputFormatter::formatDuration(double seconds) {
    std::stringstream ss;
    if (seconds < 60.0) {
        ss << std::fixed << std::setprecision(2) << seconds << "s";
    } else if (seconds < 3600.0) {
        int minutes = static_cast<int>(seconds / 60.0);
        double secs = seconds - minutes * 60.0;
        ss << minutes << "m " << std::fixed << std::setprecision(1) << secs << "s";
    } else {
        int hours = static_cast<int>(seconds / 3600.0);
        double remainder = seconds - hours * 3600.0;
        int minutes = static_cast<int>(remainder / 60.0);
        double secs = remainder - minutes * 60.0;
        ss << hours << "h " << minutes << "m " << std::fixed << std::setprecision(1) << secs << "s";
    }
    return ss.str();
}

std::string OutputFormatter::formatMemory(size_t bytes) {
    std::stringstream ss;
    if (bytes < 1024) {
        ss << bytes << " B";
    } else if (bytes < 1024 * 1024) {
        ss << std::fixed << std::setprecision(1) << (bytes / 1024.0) << " KB";
    } else if (bytes < 1024 * 1024 * 1024) {
        ss << std::fixed << std::setprecision(1) << (bytes / (1024.0 * 1024.0)) << " MB";
    } else {
        ss << std::fixed << std::setprecision(1) << (bytes / (1024.0 * 1024.0 * 1024.0)) << " GB";
    }
    return ss.str();
}

std::string OutputFormatter::formatPercentage(double value) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << value * 100 << "%";
    return ss.str();
}

} // namespace nlm