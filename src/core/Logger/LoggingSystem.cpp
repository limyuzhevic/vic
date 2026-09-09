// Global thread pool for async logging
namespace nlm::async {

static std::vector<std::thread> loggingThreads;
static std::queue<std::function<void()>> taskQueue;
static std::mutex taskQueueMutex;
static std::condition_variable taskCondition;
static bool stopThreads = false;
static size_t activeThreads = 0;
static size_t maxThreads = std::thread::hardware_concurrency();

void startLoggingThreadPool(size_t threadCount) {
    std::unique_lock<std::mutex> lock(taskQueueMutex);
    
    if (!loggingThreads.empty()) {
        return; // Already started
    }
    
    maxThreads = std::min(threadCount, maxThreads);
    activeThreads = maxThreads;
    
    for (size_t i = 0; i < maxThreads; ++i) {
        loggingThreads.emplace_back([]() {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(taskQueueMutex);
                    taskCondition.wait(lock, []() { return !taskQueue.empty() || stopThreads; });
                    
                    if (stopThreads && taskQueue.empty()) {
                        return;
                    }
                    
                    task = std::move(taskQueue.front());
                    taskQueue.pop();
                }
                
                if (task) {
                    task();
                }
            }
        });
    }
}

void stopLoggingThreadPool() {
    {
        std::unique_lock<std::mutex> lock(taskQueueMutex);
        stopThreads = true;
    }
    
    taskCondition.notify_all();
    
    for (auto& thread : loggingThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    loggingThreads.clear();
    taskQueue = std::queue<std::function<void()>>();
}

bool isLoggingThreadPoolRunning() {
    std::unique_lock<std::mutex> lock(taskQueueMutex);
    return !loggingThreads.empty() && !stopThreads;
}

} // namespace nlm::async

// Global logging system initialization
void nlm::initializeLogging(const LoggingConfig& config) {
    static bool initialized = false;
    if (initialized) return;
    
    initialized = true;
    
    // Initialize thread pool
    async::startLoggingThreadPool(config.asyncThreadCount);
    
    // Configure global logger
    Logger& globalLogger = Logger::getGlobal();
    globalLogger.clearLoggers();
    
    if (config.enableFileLogging && !config.logFile.empty()) {
        auto fileLogger = std::make_shared<FileLogger>(config);
        globalLogger.addLogger(fileLogger);
        globalLogger.addAsyncLogger(fileLogger);
    }
    
    if (config.enableAsyncLogging) {
        auto consoleLogger = std::make_shared<ConsoleLogger>(config);
        globalLogger.addLogger(consoleLogger);
        globalLogger.addAsyncLogger(consoleLogger);
    }
    
    // Set global log level
    LogLevel level;
    if (config.defaultLevel == LogLevel::Trace) {
        level = LogLevel::Trace;
    } else if (config.defaultLevel == LogLevel::Debug) {
        level = LogLevel::Debug;
    } else if (config.defaultLevel == LogLevel::Info) {
        level = LogLevel::Info;
    } else if (config.defaultLevel == LogLevel::Warning) {
        level = LogLevel::Warning;
    } else if (config.defaultLevel == LogLevel::Error) {
        level = LogLevel::Error;
    } else {
        level = LogLevel::Critical;
    }
    
    globalLogger.setLevel(level);
    
    // Set category filters
    for (const auto& category : config.enabledCategories) {
        globalLogger.setCategoryFilter(category, true);
    }
    
    for (const auto& category : config.disabledCategories) {
        globalLogger.setCategoryFilter(category, false);
    }
}

void nlm::shutdownLogging() {
    async::stopLoggingThreadPool();
}
