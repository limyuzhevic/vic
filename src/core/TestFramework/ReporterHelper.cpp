#include "Reporter.hpp"
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

namespace nlm {

std::string Reporter::formatTimestamp(const std::chrono::system_clock::time_point& time) const {
    auto time_t = std::chrono::system_clock::to_time_t(time);
    std::tm tm_buf;
    gmtime_r(&time_t, &tm_buf);
    
    std::stringstream ss;
    ss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Reporter::escapeXML(const std::string& text) const {
    std::string escaped = text;
    
    // Replace XML special characters
    size_t pos = 0;
    while ((pos = escaped.find('&', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&amp;");
        pos += 5;
    }
    
    pos = 0;
    while ((pos = escaped.find('<', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&lt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = escaped.find('>', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&gt;");
        pos += 4;
    }
    
    pos = 0;
    while ((pos = escaped.find('"', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&quot;");
        pos += 6;
    }
    
    pos = 0;
    while ((pos = escaped.find('\'', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "&apos;");
        pos += 6;
    }
    
    return escaped;
}

std::string Reporter::escapeJSON(const std::string& text) const {
    std::stringstream ss;
    ss << "\"";
    
    for (char c : text) {
        switch (c) {
            case '\\': ss << "\\\\"; break;
            case '\"': ss << "\\\""; break;
            case '\b': ss << "\\b"; break;
            case '\f': ss << "\\f"; break;
            case '\n': ss << "\\n"; break;
            case '\r': ss << "\\r"; break;
            case '\t': ss << "\\t"; break;
            default:
                if (c < 0x20 || c == 0x7f) {
                    ss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)c;
                } else {
                    ss << c;
                }
                break;
        }
    }
    
    ss << "\"";
    return ss.str();
}

std::string Reporter::formatDuration(std::chrono::milliseconds duration) const {
    auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
    duration -= hours;
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
    duration -= minutes;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    duration -= seconds;
    auto milliseconds = duration;
    
    std::stringstream ss;
    
    if (hours.count() > 0) {
        ss << hours.count() << "h ";
    }
    
    if (minutes.count() > 0 || hours.count() > 0) {
        ss << minutes.count() % 60 << "m ";
    }
    
    if (seconds.count() > 0 || minutes.count() > 0 || hours.count() > 0) {
        ss << seconds.count() % 60 << "s ";
    }
    
    ss << milliseconds.count() << "ms";
    
    return ss.str();
}

} // namespace nlm
