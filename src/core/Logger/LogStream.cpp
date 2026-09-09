// Implementation of LogStream
namespace nlm {

LogStream::LogStream(LogLevel level, const std::string& category,
                     const std::string& file, int line, const std::string& function)
    : level_(level), category_(category), file_(file), line_(line), function_(function),
      threadId_(std::hash<std::thread::id>{}(std::this_thread::get_id())),
      neuronId_(), step_(), regionId_() {}

LogStream::~LogStream() {
    // Create structured log entry
    LogEntry entry;
    entry.level = level_;
    entry.message = oss_.str();
    entry.file = file_;
    entry.line = line_;
    entry.function = function_;
    entry.timestamp = std::chrono::system_clock::now();
    entry.threadId = threadId_;
    entry.category = category_;
    entry.neuronId = neuronId_;
    entry.simulationStep = step_;
    entry.regionId = regionId_;
    
    Logger::getGlobal().log(level_, entry.message, entry.file, entry.line, entry.function,
                           entry.category, entry.kvPairs);
}

LogStream& LogStream::operator<<(const std::pair<std::string, std::string>& kv) {
    // Store key-value pair for structured logging
    // In a real implementation, this would be stored in the LogEntry
    // For now, we'll just append to the stream
    oss_ << kv.first << "=" << kv.second << " ";
    return *this;
}

LogStream& LogStream::withThreadId(uint64_t threadId) {
    threadId_ = threadId;
    return *this;
}

LogStream& LogStream::withNeuronId(const std::string& neuronId) {
    neuronId_ = neuronId;
    return *this;
}

LogStream& LogStream::withStep(const std::string& step) {
    step_ = step;
    return *this;
}

LogStream& LogStream::withRegionId(const std::string& regionId) {
    regionId_ = regionId;
    return *this;
}

} // namespace nlm