#include "ErrorReporter.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <regex>

namespace nlm {

// ErrorStatistics implementation
namespace {
    const size_t ERROR_SEVERITY_COUNT = 5;
    const size_t ERROR_TYPE_COUNT = 15;
    const size_t ERROR_CATEGORY_COUNT = 8;
}

double ErrorStatistics::getErrorsPerSecond() const {
    if (firstErrorTime.time_since_epoch().count() == 0) return 0.0;
    auto duration = std::chrono::system_clock::now() - firstErrorTime;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    return seconds > 0 ? static_cast<double>(totalErrors) / seconds : 0.0;
}

double ErrorStatistics::getErrorRate(ErrorSeverity severity) const {
    return static_cast<double>(errorsBySeverity[static_cast<size_t>(severity)]) / totalErrors;
}

std::string ErrorStatistics::getTopErrors(size_t count) const {
    std::ostringstream oss;
    oss << "Top errors:" << std::endl;
    
    // Implementation would collect and sort errors
    // For now, return a placeholder
    return oss.str();
}

std::string ErrorStatistics::getErrorDistributionReport() const {
    std::ostringstream oss;
    oss << "Error Distribution:" << std::endl;
    oss << "  Total errors: " << totalErrors << std::endl;
    oss << "  By severity:" << std::endl;
    oss << "    Debug: " << errorsBySeverity[static_cast<size_t>(ErrorSeverity::Debug)] << std::endl;
    oss << "    Info: " << errorsBySeverity[static_cast<size_t>(ErrorSeverity::Info)] << std::endl;
    oss << "    Warning: " << errorsBySeverity[static_cast<size_t>(ErrorSeverity::Warning)] << std::endl;
    oss << "    Error: " << errorsBySeverity[static_cast<size_t>(ErrorSeverity::Error)] << std::endl;
    oss << "    Critical: " << errorsBySeverity[static_cast<size_t>(ErrorSeverity::Critical)] << std::endl;
    
    return oss.str();
}

void ErrorStatistics::reset() {
    totalErrors = 0;
    for (size_t i = 0; i < ERROR_SEVERITY_COUNT; ++i) {
        errorsBySeverity[i] = 0;
    }
    for (size_t i = 0; i < ERROR_TYPE_COUNT; ++i) {
        errorsByType[i] = 0;
    }
    for (size_t i = 0; i < ERROR_CATEGORY_COUNT; ++i) {
        errorsByCategory[i] = 0;
    }
    errorsByContext.clear();
    errorsBySource.clear();
    firstErrorTime = std::chrono::system_clock::time_point();
    lastErrorTime = std::chrono::system_clock::time_point();
}

// ErrorContext implementation
std::string ErrorContext::toString() const {
    std::ostringstream oss;
    oss << "Context{" << std::endl;
    oss << "  operation: \"" << operation << "\"" << std::endl;
    oss << "  component: \"" << component << "\"" << std::endl;
    oss << "  subsystem: \"" << subsystem << "\"" << std::endl;
    oss << "  module: \"" << module << "\"" << std::endl;
    if (!metadata.empty()) {
        oss << "  metadata:" << std::endl;
        for (const auto& pair : metadata) {
            oss << "    \"" << pair.first << "\": \"" << pair.second << "\"" << std::endl;
        }
    }
    oss << "  tags:" << std::endl;
    for (const auto& pair : tags) {
        oss << "    \"" << pair.first << "\": \"" << pair.second << "\"" << std::endl;
    }
    oss << "}";
    return oss.str();
}

bool ErrorContext::hasTag(const std::string& tag) const {
    return tags.find(tag) != tags.end();
}

std::string ErrorContext::getTag(const std::string& tag, const std::string& defaultValue) const {
    auto it = tags.find(tag);
    return (it != tags.end()) ? it->second : defaultValue;
}

// EnhancedError implementation
std::string EnhancedError::formatWithContext() const {
    std::ostringstream oss;
    oss << "[" << static_cast<int>(getSeverity()) << "] "
        << (getType() == ErrorType::Unknown ? "UNKNOWN" : getErrorTypeString(getType()))
        << ": " << getMessage();
    
    if (!getContext().operation.empty()) {
        oss << " [context:" << getContext().operation << "]";
    }
    
    if (!getFile().empty()) {
        oss << " (" << getFile();
        if (getLine() > 0) oss << ":" << getLine();
        oss << ")";
    }
    
    if (!getFunction().empty()) {
        oss << " in " << getFunction();
    }
    
    return oss.str();
}

std::string EnhancedError::formatForJSON() const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"severity\": \"" << static_cast<int>(getSeverity()) << "\",";
    oss << "\"type\": \"" << (getType() == ErrorType::Unknown ? "UNKNOWN" : getErrorTypeString(getType())) << "\",";
    oss << "\"message\": \"" << escapeJSON(getMessage()) << "\",";
    oss << "\"file\": \"" << escapeJSON(getFile()) << "\",";
    oss << "\"line\": " << getLine() << ",";
    oss << "\"function\": \"" << escapeJSON(getFunction()) << "\",";
    oss << "\"context\": " << escapeJSON(getContext().toString()) << "\"};
    
    return oss.str();
}

std::string EnhancedError::formatForXML() const {
    std::ostringstream oss;
    oss << "<error>" << std::endl;
    oss << "  <severity>" << static_cast<int>(getSeverity()) << "</severity>" << std::endl;
    oss << "  <type>" << (getType() == ErrorType::Unknown ? "UNKNOWN" : getErrorTypeString(getType())) << "</type>" << std::endl;
    oss << "  <message>" << escapeXML(getMessage()) << "</message>" << std::endl;
    oss << "  <file>" << escapeXML(getFile()) << "</file>" << std::endl;
    oss << "  <line>" << getLine() << "</line>" << std::endl;
    oss << "  <function>" << escapeXML(getFunction()) << "</function>" << std::endl;
    oss << "  <context>" << escapeXML(getContext().toString()) << "</context>" << std::endl;
    oss << "</error>";
    
    return oss.str();
}

std::string EnhancedError::formatForCSV() const {
    std::ostringstream oss;
    oss << static_cast<int>(getSeverity()) << ","
        << (getType() == ErrorType::Unknown ? "UNKNOWN" : getErrorTypeString(getType())) << ","
        << escapeCSV(getMessage()) << ","
        << escapeCSV(getFile()) << ","
        << getLine() << ","
        << escapeCSV(getFunction()) << ","
        << escapeCSV(getContext().toString());
    
    return oss.str();
}

bool EnhancedError::hasContext(const std::string& key) const {
    return getContextValue(key) != "";
}

std::string EnhancedError::getContextValue(const std::string& key) const {
    auto it = getContext().metadata.find(key);
    return (it != getContext().metadata.end()) ? it->second : "";
}

std::vector<std::string> EnhancedError::getTagList() const {
    std::vector<std::string> tags;
    for (const auto& pair : getContext().tags) {
        tags.push_back(pair.first + "=" + pair.second);
    }
    return tags;
}

// ErrorFormatter implementations
namespace {
    std::string getErrorTypeString(ErrorType type) {
        static const std::map<ErrorType, std::string> typeStrings = {
            {ErrorType::SystemError, "SystemError"},
            {ErrorType::ConfigurationError, "ConfigurationError"},
            {ErrorType::MemoryError, "MemoryError"},
            {ErrorType::NeuralError, "NeuralError"},
            {ErrorType::FileError, "FileError"},
            {ErrorType::NetworkError, "NetworkError"},
            {ErrorType::ValidationError, "ValidationError"},
            {ErrorType::PlasticityError, "PlasticityError"},
            {ErrorType::DevelopmentError, "DevelopmentError"},
            {ErrorType::PredictionError, "PredictionError"},
            {ErrorType::NeuromodulationError, "NeuromodulationError"},
            {ErrorType::MemoryErrorCore, "MemoryError"},
            {ErrorType::AttentionError, "AttentionError"},
            {ErrorType::MotorError, "MotorError"},
            {ErrorType::CognitiveError, "CognitiveError"},
            {ErrorType::DevelopmentalError, "DevelopmentalError"},
            {ErrorType::Unknown, "Unknown"}
        };
        
        auto it = typeStrings.find(type);
        return (it != typeStrings.end()) ? it->second : "Unknown";
    }
    
    std::string escapeJSON(const std::string& str) {
        std::ostringstream oss;
        for (char c : str) {
            switch (c) {
                case '\"': oss << "\\\""; break;
                case '\\': oss << "\\\\"; break;
                case '\b': oss << "\\b"; break;
                case '\f': oss << "\\f"; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;
                default:
                    if (c < ' ') {
                        oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
                    } else {
                        oss << c;
                    }
            }
        }
        return oss.str();
    }
    
    std::string escapeXML(const std::string& str) {
        std::ostringstream oss;
        for (char c : str) {
            switch (c) {
                case '\"': oss << "&quot;"; break;
                case '\&': oss << "&amp;"; break;
                case '\'': oss << "&apos;"; break;
                case '\<': oss << "&lt;"; break;
                case '\>': oss << "&gt;"; break;
                default: oss << c;
            }
        }
        return oss.str();
    }
    
    std::string escapeCSV(const std::string& str) {
        if (str.find(',') == std::string::npos && str.find('\"') == std::string::npos && str.find('\n') == std::string::npos) {
            return str;
        }
        std::ostringstream oss;
        oss << "\"" << escapeCSV(str) << "\"";
        return oss.str();
    }
}

std::string SimpleErrorFormatter::format(const std::shared_ptr<Error>& error) const {
    return error->format();
}

std::string SimpleErrorFormatter::format(const std::vector<std::shared_ptr<Error>>& errors) const {
    std::ostringstream oss;
    for (const auto& error : errors) {
        oss << error->format() << std::endl;
    }
    return oss.str();
}

std::string DetailedErrorFormatter::format(const std::shared_ptr<Error>& error) const {
    auto enhancedError = std::dynamic_pointer_cast<EnhancedError>(error);
    if (enhancedError) {
        return enhancedError->formatWithContext();
    }
    return error->format();
}

std::string DetailedErrorFormatter::format(const std::vector<std::shared_ptr<Error>>& errors) const {
    std::ostringstream oss;
    for (const auto& error : errors) {
        oss << format(error) << std::endl;
    }
    return oss.str();
}

std::string JSONErrorFormatter::format(const std::shared_ptr<Error>& error) const {
    auto enhancedError = std::dynamic_pointer_cast<EnhancedError>(error);
    if (enhancedError) {
        return enhancedError->formatForJSON();
    }
    
    std::ostringstream oss;
    oss << "{\"severity\": \"" << static_cast<int>(error->getSeverity()) << "\",";
    oss << "\"type\": \"Unknown\"";
    oss << "\"message\": \"" << escapeJSON(error->getMessage()) << "\"}";
    
    return oss.str();
}

std::string JSONErrorFormatter::format(const std::vector<std::shared_ptr<Error>>& errors) const {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < errors.size(); ++i) {
        if (i > 0) oss << ",";
        oss << format(errors[i]);
    }
    oss << "]";
    return oss.str();
}

std::string XMLErrorFormatter::format(const std::shared_ptr<Error>& error) const {
    auto enhancedError = std::dynamic_pointer_cast<EnhancedError>(error);
    if (enhancedError) {
        return enhancedError->formatForXML();
    }
    
    std::ostringstream oss;
    oss << "<error>" << std::endl;
    oss << "  <severity>" << static_cast<int>(error->getSeverity()) << "</severity>" << std::endl;
    oss << "  <type>Unknown</type>" << std::endl;
    oss << "  <message>" << escapeXML(error->getMessage()) << "</message>" << std::endl;
    oss << "</error>";
    
    return oss.str();
}

std::string XMLErrorFormatter::format(const std::vector<std::shared_ptr<Error>>& errors) const {
    std::ostringstream oss;
    oss << "<errors>" << std::endl;
    for (const auto& error : errors) {
        oss << format(error) << std::endl;
    }
    oss << "</errors>";
    return oss.str();
}

std::string CSVErrorFormatter::format(const std::shared_ptr<Error>& error) const {
    auto enhancedError = std::dynamic_pointer_cast<EnhancedError>(error);
    if (enhancedError) {
        return enhancedError->formatForCSV();
    }
    
    std::ostringstream oss;
    oss << static_cast<int>(error->getSeverity()) << ","
        << "Unknown" << ","
        << escapeCSV(error->getMessage()) << ","
        << escapeCSV(error->getFile()) << ","
        << error->getLine() << ","
        << escapeCSV(error->getFunction());
    
    return oss.str();
}

std::string CSVErrorFormatter::format(const std::vector<std::shared_ptr<Error>>& errors) const {
    std::ostringstream oss;
    oss << "severity,type,message,file,line,function,context" << std::endl;
    for (const auto& error : errors) {
        oss << format(error) << std::endl;
    }
    return oss.str();
}

// ErrorFilter implementations
bool SeverityFilter::shouldReport(const std::shared_ptr<Error>& error) const {
    return error->getSeverity() >= minSeverity;
}

bool ErrorTypeFilter::shouldReport(const std::shared_ptr<Error>& error) const {
    return errorTypes.find(error->getType()) != errorTypes.end();
}

bool ContextFilter::shouldReport(const std::shared_ptr<Error>& error) const {
    auto enhancedError = std::dynamic_pointer_cast<EnhancedError>(error);
    if (enhancedError) {
        return contexts.find(enhancedError->getContext().operation) != contexts.end();
    }
    return false;
}

// AsynchronousErrorQueue implementation
struct AsynchronousErrorQueue::Impl {
    std::queue<std::shared_ptr<Error>> queue;
    size_t maxQueueSize;
    std::mutex mutex;
    std::condition_variable condVar;
    bool closed;
    std::shared_ptr<ErrorSink> sink;
    size_t workerThreads;
    std::vector<std::thread> workers;
    std::atomic<bool> running;

    Impl(size_t maxQueueSize, size_t workerThreads) 
        : maxQueueSize(maxQueueSize), workerThreads(workerThreads), running(false), closed(false) {}

    void workerFunction() {
        while (running) {
            std::shared_ptr<Error> error;
            {
                std::unique_lock<std::mutex> lock(mutex);
                condVar.wait(lock, [this]() { return !queue.empty() || !running; });
                if (!running || queue.empty()) continue;
                error = queue.front();
                queue.pop();
            }
            
            if (error && sink) {
                sink->consume(error);
            }
        }
    }

    void push(std::shared_ptr<Error> error) {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(error);
        condVar.notify_one();
    }

    bool tryPush(std::shared_ptr<Error> error) {
        std::lock_guard<std::mutex> lock(mutex);
        if (queue.size() >= maxQueueSize) {
            return false;
        }
        queue.push(error);
        return true;
    }

    std::shared_ptr<Error> pop() {
        std::unique_lock<std::mutex> lock(mutex);
        condVar.wait(lock, [this]() { return !queue.empty() || !running; });
        if (queue.empty() && !running) {
            return nullptr;
        }
        std::shared_ptr<Error> error = queue.front();
        queue.pop();
        return error;
    }

    bool tryPop(std::shared_ptr<Error>& error) {
        std::lock_guard<std::mutex> lock(mutex);
        if (queue.empty()) {
            return false;
        }
        error = queue.front();
        queue.pop();
        return true;
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.size();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.empty();
    }

    void setSink(std::shared_ptr<ErrorSink> sink) {
        this->sink = sink;
    }

    void start() {
        running = true;
        workers.clear();
        for (size_t i = 0; i < workerThreads; ++i) {
            workers.emplace_back(&Impl::workerFunction, this);
        }
    }

    void stop() {
        running = false;
        condVar.notify_all();
        for (auto& worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        workers.clear();
    }

    bool isRunning() const {
        return running;
    }
};

AsynchronousErrorQueue::AsynchronousErrorQueue(size_t maxQueueSize, size_t workerThreads)
    : pImpl(std::make_unique<Impl>(maxQueueSize, workerThreads)) {}

AsynchronousErrorQueue::~AsynchronousErrorQueue() {
    stop();
}

void AsynchronousErrorQueue::push(std::shared_ptr<Error> error) {
    pImpl->push(error);
}

bool AsynchronousErrorQueue::tryPush(std::shared_ptr<Error> error) {
    return pImpl->tryPush(error);
}

std::shared_ptr<Error> AsynchronousErrorQueue::pop() {
    return pImpl->pop();
}

bool AsynchronousErrorQueue::tryPop(std::shared_ptr<Error>& error) {
    return pImpl->tryPop(error);
}

size_t AsynchronousErrorQueue::size() const {
    return pImpl->size();
}

bool AsynchronousErrorQueue::empty() const {
    return pImpl->empty();
}

void AsynchronousErrorQueue::setSink(std::shared_ptr<ErrorSink> sink) {
    pImpl->setSink(sink);
}

void AsynchronousErrorQueue::start() {
    pImpl->start();
}

void AsynchronousErrorQueue::stop() {
    pImpl->stop();
}

bool AsynchronousErrorQueue::isRunning() const {
    return pImpl->isRunning();
}

// ErrorReporter implementation
struct ErrorReporter::Impl {
    std::string name;
    bool enabled;
    ErrorSeverity errorLevel;
    ErrorOutputFormat outputFormat;
    size_t maxErrors;
    size_t errorBufferSize;
    ErrorContext defaultContext;
    ErrorStatistics statistics;
    bool autoRecoveryEnabled;
    std::vector<std::shared_ptr<ErrorFilter>> filters;
    std::vector<std::shared_ptr<ErrorFormatter>> formatters;
    std::vector<std::shared_ptr<Error>> errors;
    std::mutex mutex;
    std::shared_ptr<AsynchronousErrorQueue> errorQueue;
    std::shared_ptr<ErrorSink> errorSink;
    std::atomic<size_t> processedErrors;

    Impl(const std::string& name) 
        : name(name)
        , enabled(true)
        , errorLevel(ErrorSeverity::Error)
        , outputFormat(ErrorOutputFormat::Detailed)
        , maxErrors(1000)
        , errorBufferSize(100)
        , autoRecoveryEnabled(false)
        , processedErrors(0) {}

    void addErrorToBuffer(std::shared_ptr<Error> error) {
        std::lock_guard<std::mutex> lock(mutex);
        if (errors.size() >= maxErrors) {
            errors.erase(errors.begin());
        }
        errors.push_back(error);
    }

    bool shouldFilter(const std::shared_ptr<Error>& error) const {
        for (const auto& filter : filters) {
            if (!filter->shouldReport(error)) {
                return true;
            }
        }
        return false;
    }

    void formatAndReportError(std::shared_ptr<Error> error) {
        if (shouldFilter(error)) {
            return;
        }
        
        addErrorToBuffer(error);
        statistics.totalErrors++;
        statistics.errorsBySeverity[static_cast<size_t>(error->getSeverity())]++;
        
        if (statistics.firstErrorTime.time_since_epoch().count() == 0) {
            statistics.firstErrorTime = std::chrono::system_clock::now();
        }
        statistics.lastErrorTime = std::chrono::system_clock::now();
        
        // Format and output error
        std::string formatted = formatError(error);
        outputError(formatted);
        
        // Send to asynchronous queue if enabled
        if (errorQueue && errorQueue->isRunning()) {
            errorQueue->push(error);
        }
        
        // Send to error sink if set
        if (errorSink) {
            errorSink->consume(error);
            processedErrors++;
        }
    }

    std::string formatError(const std::shared_ptr<Error>& error) const {
        if (formatters.empty()) {
            return error->format();
        }
        
        auto formatter = formatters[0];
        return formatter->format(error);
    }

    void outputError(const std::string& formatted) {
        switch (outputFormat) {
            case ErrorOutputFormat::Simple:
                std::cout << formatted << std::endl;
                break;
            case ErrorOutputFormat::Detailed:
                std::cout << formatted << std::endl;
                break;
            case ErrorOutputFormat::JSON:
                std::cout << formatted << std::endl;
                break;
            case ErrorOutputFormat::XML:
                std::cout << formatted << std::endl;
                break;
            case ErrorOutputFormat::CSV:
                std::cout << formatted << std::endl;
                break;
        }
    }

    std::string generateStatisticsReport() const {
        std::ostringstream oss;
        oss << "ErrorReporter Statistics for '" << name << "':" << std::endl;
        oss << statistics.getErrorDistributionReport() << std::endl;
        oss << "Errors per second: " << statistics.getErrorsPerSecond() << std::endl;
        oss << "Top errors:" << std::endl;
        oss << statistics.getTopErrors(10) << std::endl;
        return oss.str();
    }

    std::string formatErrors(const std::vector<std::shared_ptr<Error>>& errorsToFormat) const {
        if (formatters.empty()) {
            std::ostringstream oss;
            for (const auto& error : errorsToFormat) {
                oss << error->format() << std::endl;
            }
            return oss.str();
        }
        
        return formatters[0]->format(errorsToFormat);
    }

    bool attemptRecovery(std::shared_ptr<Error> error) {
        // Recovery implementation
        return false;
    }

    void updateStatistics(const std::shared_ptr<Error>& error) {
        statistics.totalErrors++;
        statistics.errorsBySeverity[static_cast<size_t>(error->getSeverity())]++;
        
        if (error->getCategory() == "System") {
            statistics.errorsByCategory[static_cast<size_t>(ErrorCategory::System)]++;
        } else if (error->getCategory() == "Runtime") {
            statistics.errorsByCategory[static_cast<size_t>(ErrorCategory::Runtime)]++;
        } else if (error->getCategory() == "Configuration") {
            statistics.errorsByCategory[static_cast<size_t>(ErrorCategory::Configuration)]++;
        } else if (error->getCategory() == "Validation") {
            statistics.errorsByCategory[static_cast<size_t>(ErrorCategory::Validation)]++;
        } else if (error->getCategory() == "Resource") {
            statistics.errorsByCategory[static_cast<size_t>(ErrorCategory::Resource)]++;
        } else if (error->getCategory() == "Development") {
            statistics.errorsByCategory[static_cast<size_t>(ErrorCategory::Development)]++;
        }
    }
};

ErrorReporter::ErrorReporter() : pImpl(std::make_unique<Impl>("GlobalErrorReporter")) {
    // Add default formatters
    pImpl->formatters.push_back(std::make_shared<DetailedErrorFormatter>());
}

ErrorReporter::ErrorReporter(const std::string& name) : pImpl(std::make_unique<Impl>(name)) {
    // Add default formatters
    pImpl->formatters.push_back(std::make_shared<DetailedErrorFormatter>());
}

ErrorReporter::~ErrorReporter() = default;

void ErrorReporter::addError(std::shared_ptr<Error> error) {
    if (!enabled_ || !error) return;
    
    pImpl->updateStatistics(error);
    pImpl->formatAndReportError(error);
}

void ErrorReporter::addError(const std::string& message, ErrorType type,
                           ErrorSeverity severity,
                           const std::string& file,
                           int line,
                           const std::string& function,
                           const std::string& category,
                           const ErrorContext& context) {
    auto error = std::make_shared<Error>(message, type, severity, file, line, function, category);
    error->setContext(context);
    addError(error);
}

void ErrorReporter::addEnhancedError(std::shared_ptr<EnhancedError> error) {
    if (!enabled_ || !error) return;
    
    pImpl->updateStatistics(error);
    pImpl->formatAndReportError(error);
}

void ErrorReporter::removeError(std::shared_ptr<Error> error) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    auto it = std::find(errors.begin(), errors.end(), error);
    if (it != errors.end()) {
        errors.erase(it);
    }
}

void ErrorReporter::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    errors.clear();
}

size_t ErrorReporter::getErrorCount(ErrorSeverity severity) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    size_t count = 0;
    for (const auto& error : errors) {
        if (error->getSeverity() >= severity) {
            count++;
        }
    }
    return count;
}

std::vector<std::shared_ptr<Error>> ErrorReporter::getErrors(ErrorSeverity severity) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::shared_ptr<Error>> filteredErrors;
    for (const auto& error : errors) {
        if (error->getSeverity() >= severity) {
            filteredErrors.push_back(error);
        }
    }
    return filteredErrors;
}

std::vector<std::shared_ptr<EnhancedError>> ErrorReporter::getEnhancedErrors(ErrorSeverity severity) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::shared_ptr<EnhancedError>> filteredErrors;
    for (const auto& error : errors) {
        auto enhancedError = std::dynamic_pointer_cast<EnhancedError>(error);
        if (enhancedError && enhancedError->getSeverity() >= severity) {
            filteredErrors.push_back(enhancedError);
        }
    }
    return filteredErrors;
}

std::shared_ptr<Error> ErrorReporter::getLastError() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (errors.empty()) return nullptr;
    return errors.back();
}

std::shared_ptr<EnhancedError> ErrorReporter::getLastEnhancedError() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto it = errors.rbegin(); it != errors.rend(); ++it) {
        auto enhancedError = std::dynamic_pointer_cast<EnhancedError>(*it);
        if (enhancedError) return enhancedError;
    }
    return nullptr;
}

void ErrorReporter::addFilter(std::shared_ptr<ErrorFilter> filter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (filter) {
        pImpl->filters.push_back(filter);
    }
}

void ErrorReporter::removeFilter(std::shared_ptr<ErrorFilter> filter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->filters.erase(std::remove(pImpl->filters.begin(), pImpl->filters.end(), filter),
                       pImpl->filters.end());
}

void ErrorReporter::clearFilters() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->filters.clear();
}

bool ErrorReporter::shouldReport(const std::shared_ptr<Error>& error) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return !pImpl->shouldFilter(error);
}

void ErrorReporter::addFormatter(std::shared_ptr<ErrorFormatter> formatter) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (formatter) {
        pImpl->formatters.push_back(formatter);
    }
}

void ErrorReporter::removeFormatter(const std::string& formatName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->formatters.erase(
        std::remove_if(pImpl->formatters.begin(), pImpl->formatters.end(),
            [&formatName](const std::shared_ptr<ErrorFormatter>& formatter) {
                return formatter->getFormatName() == formatName;
            }),
        pImpl->formatters.end()
    );
}

void ErrorReporter::clearFormatters() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->formatters.clear();
}

std::string ErrorReporter::formatErrors(const std::vector<std::shared_ptr<Error>>& errors) const {
    if (pImpl->formatters.empty()) {
        std::ostringstream oss;
        for (const auto& error : errors) {
            oss << error->format() << std::endl;
        }
        return oss.str();
    }
    
    return pImpl->formatErrors(errors);
}

std::string ErrorReporter::formatErrors(ErrorSeverity severity) const {
    return formatErrors(getErrors(severity));
}

std::string ErrorReporter::formatAllErrors() const {
    return formatErrors(ErrorSeverity::Critical);
}

void ErrorReporter::report(std::shared_ptr<Error> error) {
    if (!enabled_ || !error) return;
    
    pImpl->updateStatistics(error);
    pImpl->formatAndReportError(error);
}

void ErrorReporter::report(const std::string& message, ErrorType type,
                          ErrorSeverity severity,
                          const std::string& file,
                          int line,
                          const std::string& function,
                          const std::string& category,
                          const ErrorContext& context) {
    auto error = std::make_shared<EnhancedError>(message, type, severity, file, line, function, category, context);
    report(error);
}

void ErrorReporter::logDebug(const std::string& message, const std::string& file,
                           int line, const std::string& function,
                           const std::string& context) {
    ErrorContextBuilder builder;
    if (!context.empty()) {
        builder.setOperation(context);
    }
    report(message, ErrorType::Unknown, ErrorSeverity::Debug, file, line, function, "", builder.build());
}

void ErrorReporter::logInfo(const std::string& message, const std::string& file,
                          int line, const std::string& function,
                          const std::string& context) {
    ErrorContextBuilder builder;
    if (!context.empty()) {
        builder.setOperation(context);
    }
    report(message, ErrorType::Unknown, ErrorSeverity::Info, file, line, function, "", builder.build());
}

void ErrorReporter::logWarning(const std::string& message, const std::string& file,
                              int line, const std::string& function,
                              const std::string& context) {
    ErrorContextBuilder builder;
    if (!context.empty()) {
        builder.setOperation(context);
    }
    report(message, ErrorType::Unknown, ErrorSeverity::Warning, file, line, function, "", builder.build());
}

void ErrorReporter::logError(const std::string& message, const std::string& file,
                            int line, const std::string& function,
                            const std::string& context) {
    ErrorContextBuilder builder;
    if (!context.empty()) {
        builder.setOperation(context);
    }
    report(message, ErrorType::Unknown, ErrorSeverity::Error, file, line, function, "", builder.build());
}

void ErrorReporter::logCritical(const std::string& message, const std::string& file,
                               int line, const std::string& function,
                               const std::string& context) {
    ErrorContextBuilder builder;
    if (!context.empty()) {
        builder.setOperation(context);
    }
    report(message, ErrorType::Unknown, ErrorSeverity::Critical, file, line, function, "", builder.build());
}

std::string ErrorReporter::getStatisticsReport() const {
    return pImpl->generateStatisticsReport();
}

std::string ErrorReporter::getErrorsByTypeReport() const {
    std::ostringstream oss;
    oss << "Errors by type:" << std::endl;
    for (const auto& pair : pImpl->statistics.errorsByContext) {
        oss << "  " << pair.first << ": " << pair.second << std::endl;
    }
    return oss.str();
}

std::string ErrorReporter::getErrorsBySeverityReport() const {
    std::ostringstream oss;
    oss << "Errors by severity:" << std::endl;
    oss << "  Debug: " << pImpl->statistics.errorsBySeverity[static_cast<size_t>(ErrorSeverity::Debug)] << std::endl;
    oss << "  Info: " << pImpl->statistics.errorsBySeverity[static_cast<size_t>(ErrorSeverity::Info)] << std::endl;
    oss << "  Warning: " << pImpl->statistics.errorsBySeverity[static_cast<size_t>(ErrorSeverity::Warning)] << std::endl;
    oss << "  Error: " << pImpl->statistics.errorsBySeverity[static_cast<size_t>(ErrorSeverity::Error)] << std::endl;
    oss << "  Critical: " << pImpl->statistics.errorsBySeverity[static_cast<size_t>(ErrorSeverity::Critical)] << std::endl;
    return oss.str();
}

std::string ErrorReporter::getErrorsByContextReport() const {
    std::ostringstream oss;
    oss << "Errors by context:" << std::endl;
    for (const auto& pair : pImpl->statistics.errorsByContext) {
        oss << "  " << pair.first << ": " << pair.second << std::endl;
    }
    return oss.str();
}

bool ErrorReporter::attemptRecovery(std::shared_ptr<Error> error) {
    if (!autoRecoveryEnabled_) return false;
    return pImpl->attemptRecovery(error);
}

bool ErrorReporter::autoRecover() {
    // Auto recovery implementation
    return false;
}

void ErrorReporter::enableAsynchronousProcessing(bool enabled) {
    if (enabled) {
        pImpl->errorQueue = std::make_shared<AsynchronousErrorQueue>();
        pImpl->errorQueue->start();
    } else if (pImpl->errorQueue) {
        pImpl->errorQueue->stop();
        pImpl->errorQueue.reset();
    }
}

bool ErrorReporter::isAsynchronousProcessingEnabled() const {
    return pImpl->errorQueue && pImpl->errorQueue->isRunning();
}

void ErrorReporter::setErrorQueue(std::shared_ptr<AsynchronousErrorQueue> queue) {
    pImpl->errorQueue = queue;
}

void ErrorReporter::setErrorSink(std::shared_ptr<ErrorSink> sink) {
    pImpl->errorSink = sink;
    if (errorQueue_) {
        errorQueue_->setSink(sink);
    }
}

bool ErrorReporter::exportErrors(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << formatAllErrors();
    return true;
}

bool ErrorReporter::importErrors(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Parse error line and add to error reporter
        // Simplified implementation
    }
    
    return true;
}

bool ErrorReporter::exportStatistics(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << getStatisticsReport();
    return true;
}

// Global error reporter management
namespace {
    ErrorReporter* globalErrorReporter = nullptr;
    std::mutex globalErrorReporterMutex;
}

ErrorReporter& ErrorReporter::getGlobal() {
    std::lock_guard<std::mutex> lock(globalErrorReporterMutex);
    if (!globalErrorReporter) {
        globalErrorReporter = new ErrorReporter("GlobalErrorReporter");
    }
    return *globalErrorReporter;
}

void ErrorReporter::setGlobal(ErrorReporter& reporter) {
    std::lock_guard<std::mutex> lock(globalErrorReporterMutex);
    globalErrorReporter = &reporter;
}

void ErrorReporter::resetGlobal() {
    std::lock_guard<std::mutex> lock(globalErrorReporterMutex);
    if (globalErrorReporter) {
        delete globalErrorReporter;
        globalErrorReporter = nullptr;
    }
}

// ErrorContextBuilder implementation
ErrorContextBuilder& ErrorContextBuilder::setOperation(const std::string& operation) {
    context_.operation = operation;
    return *this;
}

ErrorContextBuilder& ErrorContextBuilder::setComponent(const std::string& component) {
    context_.component = component;
    return *this;
}

ErrorContextBuilder& ErrorContextBuilder::setSubsystem(const std::string& subsystem) {
    context_.subsystem = subsystem;
    return *this;
}

ErrorContextBuilder& ErrorContextBuilder::setModule(const std::string& module) {
    context_.module = module;
    return *this;
}

ErrorContextBuilder& ErrorContextBuilder::addMetadata(const std::string& key, const std::string& value) {
    context_.metadata[key] = value;
    return *this;
}

ErrorContextBuilder& ErrorContextBuilder::setUserId(const std::string& userId) {
    context_.userId = userId;
    return *this;
}

ErrorContextBuilder& ErrorContextBuilder::setSessionId(const std::string& sessionId) {
    context_.sessionId = sessionId;
    return *this;
}

ErrorContextBuilder& ErrorContextBuilder::addTag(const std::string& tag, const std::string& value) {
    context_.tags[tag] = value;
    return *this;
}

ErrorContext ErrorContextBuilder::build() const {
    return context_;
}

} // namespace nlm
