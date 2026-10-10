// Logger interface
#ifndef NLM_CORE_LOGGER_HPP
#define NLM_CORE_LOGGER_HPP

#include <string>
#include <memory>
#include <chrono>
#include <ctime>

namespace nlm {
namespace core {

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

class Logger {
public:
    Logger();
    ~Logger();
    
    void log(LogLevel level, const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    void critical(const std::string& message);
    
    static Logger& getInstance();
    
private:
    std::string levelToString(LogLevel level) const;
    std::string getCurrentTimestamp() const;
};

} // namespace core
} // namespace nlm

#endif // NLM_CORE_LOGGER_HPP
