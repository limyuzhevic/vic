// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Performance monitoring information
struct PerformanceMetrics {
    uint64_t totalLogs = 0;
    uint64_t filteredLogs = 0;
    double averageProcessingTime = 0.0;
    uint64_t criticalEvents = 0;
    std::chrono::system_clock::time_point lastLogTime;
    std::chrono::system_clock::duration totalProcessingTime;
    
    PerformanceMetrics() : lastLogTime(std::chrono::system_clock::now()), totalProcessingTime(std::chrono::system_clock::duration(0)) {}
};

// Logger configuration structure
struct LogConfig {
    LogLevel level;
    bool useColors;
    std::string filePath;
    bool logToFile;
    bool enablePerformanceMonitoring;
    bool enableStructuredLogging;
    std::unordered_map<std::string, std::string> filters;
    
    LogConfig()
        : level(LogLevel::Info), useColors(true), filePath(""), logToFile(false),
          enablePerformanceMonitoring(false), enableStructuredLogging(false) {}
};

// Log entry structure
struct LogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    // Structured logging fields
    std::unordered_map<std::string, std::string> fields;
    std::unordered_map<std::string, std::variant<int, double, std::string, bool>> structuredData;
    
    LogEntry() : level(LogLevel::Info), line(0), timestamp(std::chrono::system_clock::now()) {}
    LogEntry(LogLevel l, const std::string& msg, const std::string& f = "", int ln = 0, const std::string& func = "")
        : level(l), message(msg), file(f), line(ln), function(func), timestamp(std::chrono::system_clock::now()) {}
};

// Logger interface
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(const LogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    // Structured logging support
    virtual void logStructured(const std::string& message, const std::unordered_map<std::string, std::string>& fields) = 0;
    virtual void logStructured(const std::string& message, const std::unordered_map<std::string, std::variant<int, double, std::string, bool>>& data) = 0;
    // Performance monitoring
    virtual double getLastProcessingTime() const = 0;
    virtual uint64_t getTotalLogCount() const = 0;
    virtual std::string getPerformanceSummary() const = 0;
};

// Console logger implementation
class ConsoleLogger : public ILogger {
public:
    ConsoleLogger();
    explicit ConsoleLogger(LogLevel level);
    explicit ConsoleLogger(const LogConfig& config);
    ~ConsoleLogger() override;
    
    void log(const LogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Enable/disable colors
    void setUseColors(bool useColors);
    bool getUseColors() const;
    
    // Set output file (in addition to console)
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Configuration support
    void configureFromConfig(const LogConfig& config);
    bool loadConfigFromFile(const std::string& filepath);
    
    // Structured logging support
    void logStructured(const std::string& message, const std::unordered_map<std::string, std::string>& fields) override;
    void logStructured(const std::string& message, const std::unordered_map<std::string, std::variant<int, double, std::string, bool>>& data) override;
    
    // Performance monitoring
    double getLastProcessingTime() const override;
    uint64_t getTotalLogCount() const override;
    uint64_t getFilteredCount(const std::string& filter) const;
    void resetPerformanceMetrics();
    std::string getPerformanceSummary() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    
    bool shouldLog(const LogEntry& entry) const;
    bool matchesFilter(const LogEntry& entry, const std::string& filter) const;
    void updatePerformanceMetrics(double processingTime, bool filtered);
    
    // Optimization: pre-allocate string buffer
    std::string getLogMessage(const LogEntry& entry, std::vector<char>& buffer) const;
};

// Main Logger class
class Logger {
public:
    Logger();
    ~Logger();
    
    // Disable copying
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    // Add logger sink
    void addLogger(std::shared_ptr<ILogger> logger);
    
    // Remove all loggers
    void clearLoggers();
    
    // Set minimum level for all loggers
    void setLevel(LogLevel level);
    
    // Global logging functions
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static Logger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<Logger> logger);
    
    // Configuration support
    void loadConfigFromFile(const std::string& filepath);
    void configureFromGlobalConfig(const nlm::Config& config);
    
    // Performance monitoring
    void resetPerformanceMetrics();
    std::string getPerformanceSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macro for file/line/function info
#define NLM_LOG_DEBUG(msg) nlm::Logger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::Logger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::Logger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::Logger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::Logger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Stream-style logging helper
class LogStream {
public:
    LogStream(LogLevel level, const std::string& file, int line, const std::string& function);
    ~LogStream();
    
    template<typename T>
    LogStream& operator<<(const T& value) {
        oss_ << value;
        return *this;
    }
    
    // Support for structured logging
    LogStream& operator<<(const std::pair<std::string, std::string>& field);
    
private:
    LogLevel level_;
    std::ostringstream oss_;
    std::string file_;
    int line_;
    std::string function_;
};

#define NLM_LOG_STREAM(level) nlm::LogStream(level, __FILE__, __LINE__, __func__)

} // namespace nlm