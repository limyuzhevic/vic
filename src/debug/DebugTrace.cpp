#include "DebugTrace.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <algorithm>

namespace nlm {

DebugEvent::DebugEvent()
    : type_(DebugEventType::Custom),
      source_(""),
      message_(""),
      timestamp_(std::chrono::system_clock::now()),
      threadId_(std::hash<std::thread::id>{}(std::this_thread::get_id())),
      executionTime_(0.0),
      lineNumber_(0),
      fileName_(""),
      functionName_("")
{
}

DebugEvent::DebugEvent(DebugEventType type, const std::string& source, const std::string& message)
    : type_(type),
      source_(source),
      message_(message),
      timestamp_(std::chrono::system_clock::now()),
      threadId_(std::hash<std::thread::id>{}(std::this_thread::get_id())),
      executionTime_(0.0),
      lineNumber_(0),
      fileName_(""),
      functionName_("")
{
}

template<typename T>
void DebugEvent::setData(const std::string& key, const T& value) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    std::ostringstream oss;
    oss << value;
    dataMap_[key] = oss.str();
}

template<typename T>
T DebugEvent::getData(const std::string& key) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    auto it = dataMap_.find(key);
    if (it == dataMap_.end()) {
        throw std::runtime_error("DebugEvent data key not found: " + key);
    }
    std::istringstream iss(it->second);
    T result;
    iss >> result;
    return result;
}

bool DebugEvent::hasData(const std::string& key) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return dataMap_.find(key) != dataMap_.end();
}

std::string DebugEvent::serialize() const {
    std::ostringstream oss;
    
    // Serialize event type
    oss << static_cast<int>(type_) << ";";
    
    // Serialize string data with escape
    auto serializeString = [&oss](const std::string& str) {
        std::string escaped = str;
        size_t pos = 0;
        while ((pos = escaped.find(';', pos)) != std::string::npos) {
            escaped.insert(pos, ";");
            pos += 2;
        }
        oss << escaped << ";";
    };
    
    serializeString(source_);
    serializeString(message_);
    serializeString(fileName_);
    serializeString(functionName_);
    
    // Serialize numeric data
    oss << executionTime_ << ";";
    oss << lineNumber_ << ";";
    oss << threadId_ << ";";
    
    // Serialize timestamp
    auto timePoint = std::chrono::system_clock::to_time_t(timestamp_);
    oss << std::put_time(std::gmtime(&timePoint), "%Y-%m-%d %H:%M:%S") << ";";
    
    // Serialize data map
    oss << dataMap_.size() << ";";
    for (const auto& pair : dataMap_) {
        serializeString(pair.first);
        serializeString(pair.second);
    }
    
    return oss.str();
}

bool DebugEvent::deserialize(const std::string& data) {
    try {
        std::istringstream iss(data);
        std::string token;
        
        // Deserialize event type
        if (!(std::getline(iss, token, ';') && !token.empty())) return false;
        type_ = static_cast<DebugEventType>(std::stoi(token));
        
        // Deserialize string data
        auto deserializeString = [&iss]() {
            std::string token;
            if (!(std::getline(iss, token, ';') && !token.empty())) {
                return std::string();
            }
            // Reverse escape
            std::string result;
            for (size_t i = 0; i < token.length(); ++i) {
                if (i + 1 < token.length() && token[i] == ';' && token[i + 1] == ';') {
                    result += ';';
                    i++;
                } else {
                    result += token[i];
                }
            }
            return result;
        };
        
        source_ = deserializeString();
        message_ = deserializeString();
        fileName_ = deserializeString();
        functionName_ = deserializeString();
        
        // Deserialize numeric data
        if (!(std::getline(iss, token, ';') && !token.empty())) return false;
        executionTime_ = std::stod(token);
        
        if (!(std::getline(iss, token, ';') && !token.empty())) return false;
        lineNumber_ = std::stoi(token);
        
        if (!(std::getline(iss, token, ';') && !token.empty())) return false;
        threadId_ = std::stoull(token);
        
        // Deserialize timestamp
        std::string timeStr;
        if (!(std::getline(iss, timeStr, ';') && !timeStr.empty())) return false;
        std::tm tm = {};
        std::istringstream timeStream(timeStr);
        timeStream >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        timestamp_ = std::chrono::system_clock::from_time_t(std::mktime(&tm));
        
        // Deserialize data map
        size_t dataMapSize;
        if (!(std::getline(iss, token, ';') && !token.empty())) return false;
        dataMapSize = std::stoul(token);
        
        dataMap_.clear();
        for (size_t i = 0; i < dataMapSize; ++i) {
            std::string key = deserializeString();
            std::string value = deserializeString();
            dataMap_[key] = value;
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

bool DebugEvent::operator==(const DebugEvent& other) const {
    return type_ == other.type_ && source_ == other.source_ && 
           message_ == other.message_ && timestamp_ == other.timestamp_;
}

DebugTrace::DebugTrace()
    : minSeverity_(0),
      maxEvents_(0),
      timeWindowStart_(0.0),
      timeWindowEnd_(0.0),
      timeWindowEnabled_(false)
{
}

DebugTrace::~DebugTrace() = default;

void DebugTrace::addEvent(const DebugEvent& event) {
    if (!shouldLogEvent(event)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(eventsMutex_);
    
    events_.push_back(event);
    
    // Limit event count if specified
    if (maxEvents_ > 0 && events_.size() > maxEvents_) {
        events_.erase(events_.begin(), events_.begin() + (events_.size() - maxEvents_));
    }
    
    // Notify callbacks
    for (const auto& callback : callbacks_) {
        try {
            callback(event);
        } catch (...) {
            // Ignore callback exceptions
        }
    }
}

void DebugTrace::addEvent(DebugEventType type, const std::string& source, const std::string& message) {
    DebugEvent event(type, source, message);
    addEvent(event);
}

void DebugTrace::addEventFilter(DebugEventType type, bool include) {
    eventFilters_[type] = include;
}

void DebugTrace::clearEventFilters() {
    eventFilters_.clear();
}

bool DebugTrace::shouldLogEvent(const DebugEvent& event) const {
    // Check event type filter
    if (!eventFilters_.empty()) {
        auto it = eventFilters_.find(event.getType());
        if (it == eventFilters_.end() || !it->second) {
            return false;
        }
    }
    
    // Check time window
    if (timeWindowEnabled_) {
        if (!isInTimeWindow(event)) {
            return false;
        }
    }
    
    return true;
}

void DebugTrace::addEventCallback(std::function<void(const DebugEvent&)> callback) {
    std::lock_guard<std::mutex> lock(eventsMutex_);
    callbacks_.push_back(std::move(callback));
}

void DebugTrace::removeEventCallback(std::function<void(const DebugEvent&)> callback) {
    std::lock_guard<std::mutex> lock(eventsMutex_);
    callbacks_.erase(std::remove(callbacks_.begin(), callbacks_.end(), callback), callbacks_.end());
}

void DebugTrace::clearEventCallbacks() {
    std::lock_guard<std::mutex> lock(eventsMutex_);
    callbacks_.clear();
}

void DebugTrace::recordExecutionTime(const std::string& operation, double time) {
    executionTimes_[operation].push_back(time);
}

double DebugTrace::getAverageExecutionTime(const std::string& operation) const {
    auto it = executionTimes_.find(operation);
    if (it == executionTimes_.end() || it->second.empty()) {
        return 0.0;
    }
    
    double sum = 0.0;
    for (double time : it->second) {
        sum += time;
    }
    return sum / it->second.size();
}

double DebugTrace::getMaxExecutionTime(const std::string& operation) const {
    auto it = executionTimes_.find(operation);
    if (it == executionTimes_.end() || it->second.empty()) {
        return 0.0;
    }
    
    return *std::max_element(it->second.begin(), it->second.end());
}

void DebugTrace::recordState(const std::string& stateName, const std::string& stateData) {
    stateData_[stateName] = stateData;
}

std::string DebugTrace::getState(const std::string& stateName) const {
    auto it = stateData_.find(stateName);
    if (it == stateData_.end()) {
        return "";
    }
    return it->second;
}

bool DebugTrace::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << serialize();
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

bool DebugTrace::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        return deserialize(content);
    } catch (...) {
        return false;
    }
}

std::string DebugTrace::serialize() const {
    std::ostringstream oss;
    
    // Serialize event count
    oss << events_.size() << ";";
    
    // Serialize event filters
    oss << eventFilters_.size() << ";";
    for (const auto& pair : eventFilters_) {
        oss << static_cast<int>(pair.first) << ";" << (pair.second ? "1" : "0") << ";";
    }
    
    // Serialize min severity and max events
    oss << minSeverity_ << ";";
    oss << maxEvents_ << ";";
    
    // Serialize time window
    oss << timeWindowEnabled_ << ";";
    oss << timeWindowStart_ << ";";
    oss << timeWindowEnd_ << ";";
    
    // Serialize execution times
    oss << executionTimes_.size() << ";";
    for (const auto& pair : executionTimes_) {
        oss << pair.first << ";" << pair.second.size() << ";";
        for (double time : pair.second) {
            oss << time << ";";
        }
    }
    
    // Serialize state data
    oss << stateData_.size() << ";";
    for (const auto& pair : stateData_) {
        oss << pair.first << ";" << pair.second << ";";
    }
    
    // Serialize resource snapshots
    oss << resourceSnapshots_.size() << ";";
    for (const auto& snapshot : resourceSnapshots_) {
        auto timePoint = std::chrono::system_clock::to_time_t(snapshot.timestamp);
        oss << std::put_time(std::gmtime(&timePoint), "%Y-%m-%d %H:%M:%S") << ";";
        oss << snapshot.cpuUsage << ";";
        oss << snapshot.memoryUsage << ";";
        oss << snapshot.activeMemoryAllocations << ";";
        oss << snapshot.threadCount << ";";
    }
    
    // Serialize events
    oss << ";"; // Event separator
    for (size_t i = 0; i < events_.size(); ++i) {
        oss << events_[i].serialize();
        if (i < events_.size() - 1) {
            oss << "|";
        }
    }
    
    return oss.str();
}

bool DebugTrace::deserialize(const std::string& data) {
    try {
        std::istringstream iss(data);
        std::string token;
        
        // Clear existing data
        events_.clear();
        callbacks_.clear();
        eventFilters_.clear();
        executionTimes_.clear();
        stateData_.clear();
        resourceSnapshots_.clear();
        
        // Deserialize event count
        if (!(std::getline(iss, token, ';') && !token.empty())) return false;
        size_t eventCount = std::stoul(token);
        
        // Deserialize event filters
        size_t filterCount;
        if (!(std::getline(iss, token, ';') && !token.empty())) return false;
        filterCount = std::stoul(token);
        
        for (size_t i = 0; i < filterCount; ++i) {
            int type;
            bool include;
            if (!(std::getline(iss, token, ';') && !token.empty())) return false;
            type = std::stoi(token);
            
            if (!(std::getline(iss, token, ';') && !token.empty())) return false;
            include = (token == "1");
            
            eventFilters_[static_cast<DebugEventType>(type)] = include;
        }
        
        // Deserialize min severity and max events
        if (!(std::getline(iss, token, ';') && !token.empty())) return false;
        minSeverity_ = std::stoi(token);
        
        if (!(std::getline(iss, token, ';') && !token.empty())) return false;
        maxEvents_ = std::stoul(token);
        
        // Deserialize time window
        if (!(std::getline(iss, token, ';') && !token.empty())) return false;
        timeWindowEnabled_ = (token == "1");
        
        if (!(std::getline(iss, token, ';') && !token.empty())) return false;
        timeWindowStart_ = std::stod(token);
        
        if (!(std::getline(iss, token, ';') && !token.empty())) return false;
        timeWindowEnd_ = std::stod(token);
        
        // Deserialize execution times
        size_t execTimeCount;
        if (!(std::getline(iss, token, ';') && !token.empty())) return false;
        execTimeCount = std::stoul(token);
        
        for (size_t i = 0; i < execTimeCount; ++i) {
            std::string operation;
            size_t timeCount;
            
            if (!(std::getline(iss, token, ';') && !token.empty())) return false;
            operation = token;
            
            if (!(std::getline(iss, token, ';') && !token.empty())) return false;
            timeCount = std::stoul(token);
            
            std::vector<double> times;
            for (size_t j = 0; j < timeCount; ++j) {
                if (!(std::getline(iss, token, ';') && !token.empty())) return false;
                times.push_back(std::stod(token));
            }
            
            executionTimes_[operation] = times;
        }
        
        // Deserialize state data
        size_t stateDataCount;
        if (!(std::getline(iss, token, ';') && !token.empty())) return false;
        stateDataCount = std::stoul(token);
        
        for (size_t i = 0; i < stateDataCount; ++i) {
            std::string key;
            std::string value;
            
            if (!(std::getline(iss, token, ';') && !token.empty())) return false;
            key = token;
            
            if (!(std::getline(iss, token, ';') && !token.empty())) return false;
            value = token;
            
            stateData_[key] = value;
        }
        
        // Deserialize resource snapshots
        size_t snapshotCount;
        if (!(std::getline(iss, token, ';') && !token.empty())) return false;
        snapshotCount = std::stoul(token);
        
        resourceSnapshots_.resize(snapshotCount);
        for (size_t i = 0; i < snapshotCount; ++i) {
            std::string timeStr;
            double cpuUsage, memoryUsage;
            size_t memoryAllocations, threadCount;
            
            if (!(std::getline(iss, timeStr, ';') && !timeStr.empty())) return false;
            std::tm tm = {};
            std::istringstream timeStream(timeStr);
            timeStream >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
            resourceSnapshots_[i].timestamp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
            
            if (!(std::getline(iss, token, ';') && !token.empty())) return false;
            cpuUsage = std::stod(token);
            
            if (!(std::getline(iss, token, ';') && !token.empty())) return false;
            memoryUsage = std::stod(token);
            
            if (!(std::getline(iss, token, ';') && !token.empty())) return false;
            memoryAllocations = std::stoul(token);
            
            if (!(std::getline(iss, token, ';') && !token.empty())) return false;
            threadCount = std::stoul(token);
            
            resourceSnapshots_[i] = {resourceSnapshots_[i].timestamp, cpuUsage, memoryUsage, memoryAllocations, threadCount};
        }
        
        // Skip separator
        if (!(std::getline(iss, token, ';') && !token.empty())) return false;
        if (token != "|") return false;
        
        // Deserialize events
        std::string eventData;
        std::getline(iss, eventData);
        
        std::istringstream eventStream(eventData);
        std::string eventToken;
        
        for (size_t i = 0; i < eventCount; ++i) {
            DebugEvent event;
            if (!(std::getline(eventStream, eventToken, '|') && !eventToken.empty())) return false;
            
            if (!event.deserialize(eventToken)) return false;
            events_.push_back(std::move(event));
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

void DebugTrace::setTimeWindow(double startTime, double endTime) {
    timeWindowStart_ = startTime;
    timeWindowEnd_ = endTime;
    timeWindowEnabled_ = true;
}

bool DebugTrace::isInTimeWindow(const DebugEvent& event) const {
    if (!timeWindowEnabled_) return true;
    
    auto eventTime = std::chrono::system_clock::to_time_t(event.getTimestamp());
    double eventTimeSeconds = std::difftime(eventTime, std::time(nullptr));
    
    return (eventTimeSeconds >= timeWindowStart_ && eventTimeSeconds <= timeWindowEnd_);
}

void DebugTrace::recordResourceUsage(double cpuUsage, double memoryUsage, 
                                    size_t memoryAllocations, size_t threadCount) {
    ResourceSnapshot snapshot;
    snapshot.timestamp = std::chrono::system_clock::now();
    snapshot.cpuUsage = cpuUsage;
    snapshot.memoryUsage = memoryUsage;
    snapshot.activeMemoryAllocations = memoryAllocations;
    snapshot.threadCount = threadCount;
    
    resourceSnapshots_.push_back(snapshot);
}

} // namespace nlm
