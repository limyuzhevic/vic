// Implementation of LogBuffer

bool LogBuffer::push(const LogEntry& entry) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (size_ == capacity_) {
        return false; // Buffer full
    }
    buffer_[tail_] = entry;
    tail_ = (tail_ + 1) % capacity_;
    size_++;
    notEmpty_.notify_one();
    return true;
}

bool LogBuffer::pop(LogEntry& entry) {
    std::unique_lock<std::mutex> lock(mutex_);
    notEmpty_.wait(lock, [this]() { return !buffer_.empty() || closed_; });
    if (buffer_.empty() && closed_) {
        return false; // Buffer closed and empty
    }
    entry = buffer_[head_];
    head_ = (head_ + 1) % capacity_;
    size_--;
    notFull_.notify_one();
    return true;
}

void LogBuffer::close() {
    std::lock_guard<std::mutex> lock(mutex_);
    closed_ = true;
    notEmpty_.notify_all();
}

// Implementation of AsyncLogProcessor

AsyncLogProcessor::AsyncLogProcessor(std::shared_ptr<IAdvancedLogger> logger, 
                                     size_t queueSize, 
                                     size_t numThreads)
    : logger_(logger), buffer_(queueSize), running_(false), flushing_(false),
      flushInterval_(1000), maxBatchSize_(100), batchCount_(0) {
}

AsyncLogProcessor::~AsyncLogProcessor() {
    stop();
}

void AsyncLogProcessor::start() {
    if (running_) return;
    
    running_ = true;
    workers_.clear();
    
    for (size_t i = 0; i < numThreads; ++i) {
        workers_.emplace_back(&AsyncLogProcessor::workerThread, this);
    }
}

void AsyncLogProcessor::stop() {
    if (!running_) return;
    
    running_ = false;
    buffer_.close();
    
    for (auto& worker : workers_) {
        worker.join();
    }
    
    workers_.clear();
    waitForFlush();
}

void AsyncLogProcessor::push(const LogEntry& entry) {
    if (buffer_.push(entry)) {
        cv_.notify_one();
    }
}

void AsyncLogProcessor::waitForFlush() {
    std::unique_lock<std::mutex> lock(mutex_);
    flushing_ = true;
    cv_.notify_all();
    
    // Wait for all batches to be processed
    while (batchCount_ > 0) {
        cv_.wait_for(lock, std::chrono::milliseconds(100));
    }
    
    flushing_ = false;
}

void AsyncLogProcessor::workerThread() {
    std::vector<LogEntry> batch;
    batch.reserve(maxBatchSize_);
    
    while (running_) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait_for(lock, std::chrono::milliseconds(flushInterval_), 
                        [this]() { return !buffer_.isEmpty() || flushing_; });
            
            // Collect batch
            batch.clear();
            while (!buffer_.isEmpty() && batch.size() < maxBatchSize_) {
                LogEntry entry;
                if (buffer_.pop(entry)) {
                    batch.push_back(std::move(entry));
                }
            }
        }
        
        if (!batch.empty()) {
            // Process batch
            for (const auto& entry : batch) {
                logger_->log(entry);
            }
            
            std::lock_guard<std::mutex> lock(mutex_);
            batchCount_--;
            cv_.notify_all();
        }
        
        // Small sleep to prevent busy waiting
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void AsyncLogProcessor::flushBatch() {
    std::unique_lock<std::mutex> lock(mutex_);
    batchCount_++;
}

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {}

ConsoleLogger::ConsoleLogger(LogLevel level) : pImpl(std::make_unique<Impl>()) {
    pImpl->level = level;
}

ConsoleLogger::~ConsoleLogger() = default;

// Implementation of LogFormatter

LogFormatter::LogFormatter(const std::string& format) 
    : format_(format), timestampFormat_(TimestampFormat::ISO8601),
      includeFileInfo_(false), includeFunctionInfo_(false),
      includeThreadId_(false), includeSequence_(false) {
}

std::string LogFormatter::format(const LogEntry& entry) const {
    std::ostringstream oss;
    std::string result = format_;
    
    // Replace placeholders
    size_t pos = 0;
    while ((pos = result.find('%', pos)) != std::string::npos) {
        char placeholder = result[pos + 1];
        std::string replacement;
        
        switch (placeholder) {
            case 't': // timestamp
                replacement = timestampToString(entry.timestamp);
                break;
            case 'T': // timestamp with milliseconds
                replacement = timestampToString(entry.timestamp) + '.' + 
                             std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                                 entry.timestamp.time_since_epoch()).count() % 1000);
                break;
            case 'l': // level
                replacement = levelToString(entry.level);
                break;
            case 'm': // message
                replacement = entry.message;
                break;
            case 'f': // file
                if (includeFileInfo_) replacement = entry.file;
                break;
            case 'n': // line
                if (includeFileInfo_) replacement = std::to_string(entry.line);
                break;
            case 'F': // function
                if (includeFunctionInfo_) replacement = entry.function;
                break;
            case 'p': // process id
                replacement = std::to_string(getpid());
                break;
            case 'h': // host name
                {
                    char hostname[256];
                    hostname[255] = '\0';
                    if (gethostname(hostname, 255) == 0) {
                        replacement = hostname;
                    } else {
                        replacement = "localhost";
                    }
                }
                break;
            case 'w': // thread id
                if (includeThreadId_) replacement = std::to_string(entry.threadId);
                break;
            case 's': // sequence number
                if (includeSequence_) replacement = std::to_string(entry.sequenceNumber);
                break;
            case 'M': // module
                replacement = entry.module;
                break;
            case 'c': // category
                replacement = entry.category;
                break;
            case 'g': // context id
                replacement = entry.contextId;
                break;
            case 'S': // logger name
                replacement = entry.loggerName;
                break;
            case '{': {
                // Find matching }
                size_t endPos = result.find('}', pos);
                if (endPos != std::string::npos) {
                    std::string fieldName = result.substr(pos + 2, endPos - pos - 2);
                    auto it = entry.fields.find(fieldName);
                    if (it != entry.fields.end()) {
                        replacement = it->second;
                    }
                    pos = endPos;
                }
                break;
            }
        }
        
        // Replace the placeholder
        if (pos < result.size()) {
            result.replace(pos, 2, replacement);
        }
        pos += replacement.size();
    }
    
    return result;
}

std::string LogFormatter::timestampToString(const std::chrono::system_clock::time_point& tp) const {
    auto time = std::chrono::system_clock::to_time_t(tp);
    std::tm tm_time = *std::localtime(&time);
    
    switch (timestampFormat_) {
        case TimestampFormat::ISO8601: {
            char buffer[32];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", &tm_time);
            return std::string(buffer);
        }
        case TimestampFormat::ISO8601Milli: {
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                tp.time_since_epoch()) % 1000;
            char buffer[32];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", &tm_time);
            return std::string(buffer) + '.' + std::to_string(ms.count());
        }
        case TimestampFormat::ISO8601Nano: {
            auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                tp.time_since_epoch()) % 1000000000;
            char buffer[32];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", &tm_time);
            return std::string(buffer) + '.' + std::to_string(ns.count());
        }
        case TimestampFormat::HumanReadable: {
            char buffer[32];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_time);
            return std::string(buffer);
        }
        case TimestampFormat::HumanReadableMS: {
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                tp.time_since_epoch()) % 1000;
            char buffer[32];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_time);
            return std::string(buffer) + '.' + std::to_string(ms.count());
        }
        case TimestampFormat::HumanReadablePrecise: {
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                tp.time_since_epoch()) % 1000;
            auto us = std::chrono::duration_cast<std::chrono::microseconds>(
                tp.time_since_epoch()) % 1000;
            char buffer[32];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_time);
            return std::string(buffer) + '.' + std::to_string(ms.count()) + 
                   '.' + std::to_string(us.count());
        }
        case TimestampFormat::Epoch: {
            auto epoch = tp.time_since_epoch();
            auto seconds = std::chrono::duration_cast<std::chrono::seconds>(epoch);
            return std::to_string(seconds.count());
        }
        case TimestampFormat::EpochMS: {
            auto epoch = tp.time_since_epoch();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(epoch);
            return std::to_string(ms.count());
        }
        case TimestampFormat::EpochNS: {
            auto epoch = tp.time_since_epoch();
            auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(epoch);
            return std::to_string(ns.count());
        }
        default: {
            char buffer[32];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", &tm_time);
            return std::string(buffer);
        }
    }
}

std::string LogFormatter::formatToString(const std::variant<std::string, int, double, bool>& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        oss << arg;
    }, value);
    return oss.str();
}

// ConsoleLogger levelToColor implementation
std::string ConsoleLogger::levelToColor(LogLevel level) const {
    switch (level) {
        case LogLevel::Trace: return "\033[90m";     // Gray
        case LogLevel::Verbose: return "\033[36m";   // Cyan
        case LogLevel::Debug: return "\033[94m";     // Blue
        case LogLevel::Info: return "\033[32m";      // Green
        case LogLevel::Notice: return "\033[34m";    // Blue
        case LogLevel::Warning: return "\033[33m";   // Yellow
        case LogLevel::Error: return "\033[31m";     // Red
        case LogLevel::Critical: return "\033[91m";  // Bright Red
        case LogLevel::Alert: return "\033[95m";     // Magenta
        case LogLevel::Emergency: return "\033[95m"; // Magenta
        case LogLevel::Fatal: return "\033[1m\033[91m"; // Bold Red
        case LogLevel::None: return "";               // No color
        default: return "";                          // No color
    }
}

// Implementation of ConsoleLogger

struct ConsoleLogger::Impl {
    LogLevel level = LogLevel::Info;
    bool useColors = true;
    std::ofstream fileOut;
    std::mutex mutex;
    LogFormatter formatter;
    std::string outputFormat;
    TimestampFormat timestampFormat = TimestampFormat::ISO8601;
    
    // File rotation settings
    uint64_t maxFileSize = 10 * 1024 * 1024; // 10MB
    std::string timeBasedRotation = "daily";
    uint64_t currentFileSize = 0;
    uint64_t rotationCounter = 0;
    std::chrono::system_clock::time_point lastRotationTime;
    
    // Statistics
    uint64_t logCount = 0;
    std::chrono::system_clock::time_point firstLogTime;
    std::chrono::system_clock::time_point lastLogTime;
    
    // Async logging
    std::unique_ptr<AsyncLogProcessor> asyncProcessor;
    bool useAsync = false;
    
    // Filter
    LogFilter filter;
    
    // Log storage for query
    std::vector<LogEntry> logHistory;
    size_t maxHistorySize = 10000;
};

ConsoleLogger::ConsoleLogger() : pImpl(std::make_unique<Impl>()) {
    pImpl->firstLogTime = std::chrono::system_clock::now();
}

ConsoleLogger::ConsoleLogger(const LogConfig& config) : pImpl(std::make_unique<Impl>()) {
    pImpl->firstLogTime = std::chrono::system_clock::now();
    configure(config);
}

ConsoleLogger::~ConsoleLogger() {
    waitForFlush();
}

void ConsoleLogger::log(const LogEntry& entry) {
    if (!shouldLog(entry)) {
        return;
    }
    
    // Check if we should use async logging
    if (pImpl->useAsync && pImpl->asyncProcessor) {
        pImpl->asyncProcessor->push(entry);
        return;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Format the log entry
    std::string formattedMessage = pImpl->formatter.format(entry);
    
    // Add color if enabled
    std::string output;
    if (pImpl->useColors) {
        output = levelToColor(entry.level) + formattedMessage + "\033[0m";
    } else {
        output = formattedMessage;
    }
    
    // Console output
    std::cout << output << "\n";
    
    // File output with rotation
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut << output << "\n";
        pImpl->fileOut.flush();
        pImpl->currentFileSize += output.size() + 1; // +1 for newline
        
        // Check for rotation
        rotateFileIfNeeded();
    }
    
    // Update statistics
    pImpl->logCount++;
    pImpl->lastLogTime = std::chrono::system_clock::now();
    
    // Store in history for queries
    if (pImpl->logHistory.size() < pImpl->maxHistorySize) {
        pImpl->logHistory.push_back(entry);
    }
}

void ConsoleLogger::setLevel(LogLevel level) {
    pImpl->level = level;
}

LogLevel ConsoleLogger::getLevel() const {
    return pImpl->level;
}

void ConsoleLogger::configure(const LogConfig& config) {
    pImpl->level = config.level;
    pImpl->useColors = config.useColors;
    pImpl->formatter = LogFormatter(); // Default formatter
    pImpl->outputFormat = "%timestamp %level[%module] %message";
    pImpl->timestampFormat = config.timestampFormat;
    pImpl->maxFileSize = config.maxLogFileSize;
    pImpl->timeBasedRotation = config.timeBasedRotation;
    pImpl->maxHistorySize = config.maxLogBufferSize;
    
    // Apply timestamp format string if provided
    if (!config.timestampFormatString.empty()) {
        pImpl->formatter.setTimestampFormat(config.timestampFormatString);
    }
}

LogConfig ConsoleLogger::getConfig() const {
    LogConfig config;
    config.name = "ConsoleLogger";
    config.level = pImpl->level;
    config.useColors = pImpl->useColors;
    config.timestampFormat = pImpl->timestampFormat;
    config.maxLogFileSize = pImpl->maxFileSize;
    config.timeBasedRotation = pImpl->timeBasedRotation;
    config.maxLogBufferSize = pImpl->maxHistorySize;
    config.enableStructuredLogging = false;
    return config;
}

void ConsoleLogger::setFilter(const LogFilter& filter) {
    pImpl->filter = filter;
}

LogFilter ConsoleLogger::getFilter() const {
    return pImpl->filter;
}

bool ConsoleLogger::shouldLog(const LogEntry& entry) const {
    // Check level filtering
    if (pImpl->level > entry.level) {
        return false;
    }
    
    // Check filter criteria (simplified implementation)
    if (pImpl->filter.negate) {
        // If negated, check if entry matches filter
        bool matches = true;
        
        if (pImpl->filter.minLevel && entry.level < *pImpl->filter.minLevel) {
            matches = false;
        }
        if (pImpl->filter.maxLevel && entry.level > *pImpl->filter.maxLevel) {
            matches = false;
        }
        
        return !matches;
    } else {
        // Normal filtering
        bool matches = true;
        
        if (pImpl->filter.minLevel && entry.level < *pImpl->filter.minLevel) {
            matches = false;
        }
        if (pImpl->filter.maxLevel && entry.level > *pImpl->filter.maxLevel) {
            matches = false;
        }
        if (!pImpl->filter.modules.empty() && 
            std::find(pImpl->filter.modules.begin(), pImpl->filter.modules.end(), 
                     entry.module) == pImpl->filter.modules.end()) {
            matches = false;
        }
        if (!pImpl->filter.categories.empty() && 
            std::find(pImpl->filter.categories.begin(), pImpl->filter.categories.end(), 
                     entry.category) == pImpl->filter.categories.end()) {
            matches = false;
        }
        if (!pImpl->filter.tags.empty() && 
            entry.tags.find(" " + *pImpl->filter.tags.begin() + " ") == std::string::npos) {
            matches = false;
        }
        
        return matches;
    }
}

uint64_t ConsoleLogger::getLogCount() const {
    return pImpl->logCount;
}

std::chrono::system_clock::time_point ConsoleLogger::getLastLogTime() const {
    return pImpl->lastLogTime;
}

double ConsoleLogger::getAverageLogRate() const {
    if (pImpl->logCount == 0 || pImpl->firstLogTime == pImpl->lastLogTime) {
        return 0.0;
    }
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(
        pImpl->lastLogTime - pImpl->firstLogTime).count();
    if (duration == 0) duration = 1;
    return static_cast<double>(pImpl->logCount) / duration;
}

void ConsoleLogger::resetStats() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->logCount = 0;
    pImpl->firstLogTime = std::chrono::system_clock::now();
    pImpl->lastLogTime = pImpl->firstLogTime;
}

void ConsoleLogger::flush() {
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.flush();
    }
}

bool ConsoleLogger::isBufferFull() const {
    return pImpl->logHistory.size() >= pImpl->maxHistorySize;
}

// Implement the rest of ConsoleLogger methods

void ConsoleLogger::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

bool ConsoleLogger::getUseColors() const {
    return pImpl->useColors;
}

void ConsoleLogger::setFormatter(const LogFormatter& formatter) {
    pImpl->formatter = formatter;
}

void ConsoleLogger::setOutputFormat(const std::string& format) {
    pImpl->outputFormat = format;
    pImpl->formatter = LogFormatter(format);
}

void ConsoleLogger::setOutputFile(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->fileOut.close();
    pImpl->fileOut.open(filepath, std::ios::app);
    pImpl->currentFileSize = 0;
}

void ConsoleLogger::closeOutputFile() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (pImpl->fileOut.is_open()) {
        pImpl->fileOut.close();
    }
}

void ConsoleLogger::enableFileRotation(uint64_t maxSize, const std::string& timePattern) {
    pImpl->maxFileSize = maxSize;
    pImpl->timeBasedRotation = timePattern;
}

void ConsoleLogger::enableAsyncLogging(size_t queueSize, size_t numThreads) {
    pImpl->useAsync = true;
    pImpl->asyncProcessor = std::make_unique<AsyncLogProcessor>(this, queueSize, numThreads);
    pImpl->asyncProcessor->start();
}

void ConsoleLogger::setFlushInterval(uint32_t interval) {
    if (pImpl->asyncProcessor) {
        pImpl->asyncProcessor->setFlushInterval(interval);
    }
}

void ConsoleLogger::waitForFlush() {
    if (pImpl->asyncProcessor) {
        pImpl->asyncProcessor->waitForFlush();
    }
}

void ConsoleLogger::rotateFileIfNeeded() {
    // Check if we need to rotate based on size
    if (pImpl->currentFileSize >= pImpl->maxFileSize && pImpl->enableRotation) {
        std::lock_guard<std::mutex> lock(pImpl->mutex);
        
        // Close current file
        if (pImpl->fileOut.is_open()) {
            pImpl->fileOut.close();
        }
        
        // Generate new filename
        std::string newFilename = getRotatedFilename(pImpl->outputFormat, pImpl->rotationCounter++);
        
        // Reopen with new filename
        pImpl->fileOut.open(newFilename, std::ios::app);
        pImpl->currentFileSize = 0;
        pImpl->lastRotationTime = std::chrono::system_clock::now();
    }
    
    // Check if we need to rotate based on time
    auto now = std::chrono::system_clock::now();
    if (pImpl->timeBasedRotation != "none") {
        auto duration = std::chrono::duration_cast<std::chrono::hours>(now - pImpl->lastRotationTime);
        
        if (pImpl->timeBasedRotation == "hourly" && duration.count() >= 1) {
            rotateFileIfNeeded();
        } else if (pImpl->timeBasedRotation == "daily" && duration.count() >= 24) {
            rotateFileIfNeeded();
        } else if (pImpl->timeBasedRotation == "weekly" && duration.count() >= 168) {
            rotateFileIfNeeded();
        }
    }
}

std::string ConsoleLogger::getRotatedFilename(const std::string& basePath, uint64_t counter) const {
    // Simple implementation - in real code, we'd use proper path manipulation
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_time = *std::localtime(&time);
    
    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", &tm_time);
    
    std::string timestamp(buffer);
    
    if (counter > 0) {
        return basePath + "." + timestamp + "." + std::to_string(counter) + ".log";
    }
    return basePath + "." + timestamp + ".log";
}

// Implement other Logger methods

void ConsoleLogger::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->logHistory.clear();
}

uint64_t ConsoleLogger::getTotalLogCount() const {
    return pImpl->logCount;
}

void ConsoleLogger::compact() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    // Simple implementation - in real code, we'd implement proper compaction
    if (pImpl->logHistory.size() > pImpl->maxHistorySize / 2) {
        // Remove oldest entries
        size_t newSize = pImpl->maxHistorySize / 2;
        pImpl->logHistory.erase(pImpl->logHistory.begin(), 
                               pImpl->logHistory.begin() + 
                               (pImpl->logHistory.size() - newSize));
    }
}

void ConsoleLogger::rotate() {
    rotateFileIfNeeded();
}

// Implement query methods for ConsoleLogger

std::vector<LogEntry> ConsoleLogger::query(const LogFilter& filter) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<LogEntry> result;
    
    for (const auto& entry : pImpl->logHistory) {
        if (shouldLog(entry)) {
            result.push_back(entry);
        }
    }
    
    return result;
}

std::vector<LogEntry> ConsoleLogger::search(const std::string& pattern, 
                                           const LogFilter& filter) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<LogEntry> result;
    
    for (const auto& entry : pImpl->logHistory) {
        if (!shouldLog(entry)) continue;
        
        // Simple string search in message
        if (entry.message.find(pattern) != std::string::npos) {
            result.push_back(entry);
        }
    }
    
    return result;
}

void ConsoleLogger::exportLogs(const std::string& filepath, 
                              const LogFilter& filter,
                              const std::string& format) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ofstream outFile(filepath);
    if (!outFile.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filepath);
    }
    
    auto logs = query(filter);
    
    if (format == "json") {
        nlohmann::json j = logs;
        outFile << j.dump(2);
    } else if (format == "csv") {
        // Write CSV header
        outFile << "timestamp,level,message,file,line,function,module\n";
        for (const auto& entry : logs) {
            outFile << timestampToString(entry.timestamp) << ","
                    << levelToString(entry.level) << ","
                    << escapeCsvField(entry.message) << ","
                    << escapeCsvField(entry.file) << ","
                    << entry.line << ","
                    << escapeCsvField(entry.function) << ","
                    << escapeCsvField(entry.module) << "\n";
        }
    }
    
    outFile.close();
}

void ConsoleLogger::importLogs(const std::string& filepath) {
    std::ifstream inFile(filepath);
    if (!inFile.is_open()) {
        throw std::runtime_error("Cannot open file for reading: " + filepath);
    }
    
    // Simple implementation - in real code, we'd implement proper parsing
    // For now, just clear and log a message
    clear();
    
    inFile.close();
}

nlohmann::json ConsoleLogger::aggregate(const LogFilter& filter, 
                                       const std::string& interval) const {
    // Simple implementation - in real code, we'd implement proper aggregation
    nlohmann::json result;
    result["type"] = "aggregation";
    result["interval"] = interval;
    result["count"] = static_cast<uint64_t>(pImpl->logCount);
    
    return result;
}

std::map<LogLevel, uint64_t> ConsoleLogger::getLevelCounts(const LogFilter& filter) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::map<LogLevel, uint64_t> counts;
    
    for (const auto& entry : pImpl->logHistory) {
        if (shouldLog(entry)) {
            counts[entry.level]++;
        }
    }
    
    return counts;
}

std::map<std::string, uint64_t> ConsoleLogger::getSourceCounts(const LogFilter& filter) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::map<std::string, uint64_t> counts;
    
    for (const auto& entry : pImpl->logHistory) {
        if (shouldLog(entry)) {
            std::string source = entry.file + ":" + std::to_string(entry.line);
            counts[source]++;
        }
    }
    
    return counts;
}

std::map<std::string, uint64_t> ConsoleLogger::getModuleCounts(const LogFilter& filter) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::map<std::string, uint64_t> counts;
    
    for (const auto& entry : pImpl->logHistory) {
        if (shouldLog(entry)) {
            counts[entry.module]++;
        }
    }
    
    return counts;
}

// Logger class implementation
struct Logger::Impl {
    std::vector<std::shared_ptr<ILogger>> loggers;
    std::mutex mutex;
};

Logger::Logger() : pImpl(std::make_unique<Impl>()) {}

Logger::~Logger() = default;

void Logger::addLogger(std::shared_ptr<ILogger> logger) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.push_back(logger);
}

void Logger::clearLoggers() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->loggers.clear();
}

void Logger::setLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        logger->setLevel(level);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function,
                const std::map<std::string, std::string>& fields) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    LogEntry entry;
    entry.level = level;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    entry.timestamp = std::chrono::system_clock::now();
    entry.fields = fields;
    
    for (auto& logger : pImpl->loggers) {
        logger->log(entry);
    }
}

void Logger::log(LogLevel level, const std::string& message,
                const std::string& file, int line, const std::string& function) {
    log(level, message, file, line, function, {});
}

void Logger::debug(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Debug, message, file, line, function);
}

void Logger::info(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Info, message, file, line, function);
}

void Logger::warning(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Warning, message, file, line, function);
}

void Logger::error(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Error, message, file, line, function);
}

void Logger::critical(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Critical, message, file, line, function);
}

void Logger::verbose(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Verbose, message, file, line, function);
}

void Logger::trace(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Trace, message, file, line, function);
}

void Logger::notice(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Notice, message, file, line, function);
}

void Logger::alert(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Alert, message, file, line, function);
}

void Logger::emergency(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Emergency, message, file, line, function);
}

void Logger::fatal(const std::string& message, const std::string& file, int line, const std::string& function) {
    log(LogLevel::Fatal, message, file, line, function);
}

bool Logger::isEnabled(LogLevel level) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& logger : pImpl->loggers) {
        if (logger->getLevel() <= level) {
            return true;
        }
    }
    return false;
}

Logger& Logger::getGlobal() {
    static Logger globalLogger;
    return globalLogger;
}

void Logger::setGlobal(std::shared_ptr<Logger> logger) {
    // This is a placeholder for advanced global logger management
    // In practice, we just use getGlobal() which is already static
}

// LogStream implementation
LogStream::LogStream(LogLevel level, const std::string& file, int line, 
                   const std::string& function,
                   const std::map<std::string, std::string>& fields)
    : level_(level), file_(file), line_(line), function_(function),
      fields_(fields) {}

LogStream::LogStream(LogLevel level, const std::string& file, int line, const std::string& function)
    : level_(level), file_(file), line_(line), function_(function) {}

LogStream::~LogStream() {
    Logger::getGlobal().log(level_, oss_.str(), file_, line_, function_, fields_);
}

LogStream& LogStream::setField(const std::string& key, const std::string& value) {
    fields_[key] = value;
    return *this;
}

} // namespace nlm
