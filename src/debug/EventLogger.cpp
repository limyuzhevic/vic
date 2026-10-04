#include "EventLogger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace nlm {

SimulationEvent::SimulationEvent()
    : type_(EventType::Custom),
      source_(""),
      message_(""),
      timestamp_(std::chrono::system_clock::now()),
      simulationStep_(0),
      simulationTime_(0.0),
      priority_(EventPriority::Normal),
      location_("")
{
}

SimulationEvent::SimulationEvent(EventType type, const std::string& source, const std::string& message)
    : type_(type),
      source_(source),
      message_(message),
      timestamp_(std::chrono::system_clock::now()),
      simulationStep_(0),
      simulationTime_(0.0),
      priority_(EventPriority::Normal),
      location_("")
{
}

template<typename T>
void SimulationEvent::setData(const std::string& key, const T& value) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    std::ostringstream oss;
    oss << value;
    dataMap_[key] = oss.str();
}

template<typename T>
T SimulationEvent::getData(const std::string& key) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    auto it = dataMap_.find(key);
    if (it == dataMap_.end()) {
        throw std::runtime_error("SimulationEvent data key not found: " + key);
    }
    std::istringstream iss(it->second);
    T result;
    iss >> result;
    return result;
}

bool SimulationEvent::hasData(const std::string& key) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return dataMap_.find(key) != dataMap_.end();
}

std::string SimulationEvent::serialize() const {
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
    serializeString(location_);
    
    // Serialize numeric data
    oss << simulationStep_ << ";";
    oss << simulationTime_ << ";";
    oss << static_cast<int>(priority_) << ";";
    
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

bool SimulationEvent::deserialize(const std::string& data) {
    try {
        std::istringstream iss(data);
        std::string token;
        
        // Deserialize event type
        if (!(std::getline(iss, token, ';') && !token.empty())) return false;
        type_ = static_cast<EventType>(std::stoi(token));
        
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
        location_ = deserializeString();
        
        // Deserialize numeric data
        if (!(std::getline(iss, token, ';') && !token.empty())) return false;
        simulationStep_ = std::stoull(token);
        
        if (!(std::getline(iss, token, ';') && !token.empty())) return false;
        simulationTime_ = std::stod(token);
        
        if (!(std::getline(iss, token, ';') && !token.empty())) return false;
        priority_ = static_cast<EventPriority>(std::stoi(token));
        
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

bool SimulationEvent::operator==(const SimulationEvent& other) const {
    return type_ == other.type_ && source_ == other.source_ && 
           message_ == other.message_ && timestamp_ == other.timestamp_;
}

EventLogger::EventLogger()
    : minPriority_(EventPriority::Low),
      minStep_(0),
      maxStep_(UINT64_MAX),
      timeWindowStart_(0.0),
      timeWindowEnd_(0.0),
      timeWindowEnabled_(false)
{
}

EventLogger::EventLogger(std::shared_ptr<DebuggingSystem> debuggingSystem)
    : minPriority_(EventPriority::Low),
      minStep_(0),
      maxStep_(UINT64_MAX),
      timeWindowStart_(0.0),
      timeWindowEnd_(0.0),
      timeWindowEnabled_(false),
      debuggingSystem_(debuggingSystem)
{
}

EventLogger::~EventLogger() = default;

void EventLogger::setEventFilter(EventType type, bool include) {
    eventFilters_[type] = include;
}

void EventLogger::clearEventFilters() {
    eventFilters_.clear();
}

void EventLogger::setEventPriorityFilter(EventPriority minPriority) {
    minPriority_ = minPriority;
}

void EventLogger::setSimulationStepFilter(uint64_t minStep, uint64_t maxStep) {
    minStep_ = minStep;
    maxStep_ = maxStep;
}

void EventLogger::setTimeWindow(double startTime, double endTime) {
    timeWindowStart_ = startTime;
    timeWindowEnd_ = endTime;
    timeWindowEnabled_ = true;
}

void EventLogger::logEvent(EventType type, const std::string& source, const std::string& message) {
    SimulationEvent event(type, source, message);
    logEvent(std::move(event));
}

void EventLogger::logEvent(SimulationEvent event) {
    if (!shouldLogEvent(event)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(eventsMutex_);
    events_.push_back(std::move(event));
    
    notifyDebuggingSystem(events_.back());
}

void EventLogger::logEventWithData(EventType type, const std::string& source, const std::string& message,
                                 const std::unordered_map<std::string, std::string>& data) {
    SimulationEvent event(type, source, message);
    
    for (const auto& pair : data) {
        event.setData(pair.first, pair.second);
    }
    
    logEvent(std::move(event));
}

std::vector<SimulationEvent> EventLogger::getEvents(EventType type) const {
    std::vector<SimulationEvent> filteredEvents;
    {
        std::lock_guard<std::mutex> lock(eventsMutex_);
        for (const auto& event : events_) {
            if (event.getType() == type) {
                filteredEvents.push_back(event);
            }
        }
    }
    return filteredEvents;
}

std::vector<SimulationEvent> EventLogger::getEventsInTimeRange(
    double startTime, double endTime) const {
    std::vector<SimulationEvent> filteredEvents;
    {
        std::lock_guard<std::mutex> lock(eventsMutex_);
        for (const auto& event : events_) {
            auto eventTime = std::chrono::system_clock::to_time_t(event.getTimestamp());
            double eventTimeSeconds = std::difftime(eventTime, std::time(nullptr));
            
            if (eventTimeSeconds >= startTime && eventTimeSeconds <= endTime) {
                filteredEvents.push_back(event);
            }
        }
    }
    return filteredEvents;
}

bool EventLogger::shouldLogEvent(const SimulationEvent& event) const {
    // Check event type filter
    if (!eventFilters_.empty()) {
        auto it = eventFilters_.find(event.getType());
        if (it == eventFilters_.end() || !it->second) {
            return false;
        }
    }
    
    // Check priority filter
    if (event.getPriority() < minPriority_) {
        return false;
    }
    
    // Check step filter
    if (event.getSimulationStep() < minStep_ || event.getSimulationStep() > maxStep_) {
        return false;
    }
    
    // Check time window
    if (timeWindowEnabled_) {
        auto eventTime = std::chrono::system_clock::to_time_t(event.getTimestamp());
        double eventTimeSeconds = std::difftime(eventTime, std::time(nullptr));
        
        if (eventTimeSeconds < timeWindowStart_ || eventTimeSeconds > timeWindowEnd_) {
            return false;
        }
    }
    
    return true;
}

size_t EventLogger::getEventCount(EventType type) const {
    size_t count = 0;
    {
        std::lock_guard<std::mutex> lock(eventsMutex_);
        for (const auto& event : events_) {
            if (event.getType() == type) {
                count++;
            }
        }
    }
    return count;
}

double EventLogger::getAverageTimeBetweenEvents() const {
    if (events_.size() < 2) return 0.0;
    
    std::lock_guard<std::mutex> lock(eventsMutex_);
    
    std::vector<double> intervals;
    for (size_t i = 1; i < events_.size(); ++i) {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            events_[i].getTimestamp() - events_[i-1].getTimestamp()).count();
        intervals.push_back(duration / 1000000.0); // Convert to seconds
    }
    
    if (intervals.empty()) return 0.0;
    
    double sum = std::accumulate(intervals.begin(), intervals.end(), 0.0);
    return sum / intervals.size();
}

std::chrono::system_clock::time_point EventLogger::getFirstEventTime() const {
    std::lock_guard<std::mutex> lock(eventsMutex_);
    if (events_.empty()) return std::chrono::system_clock::time_point();
    
    return events_.front().getTimestamp();
}

std::chrono::system_clock::time_point EventLogger::getLastEventTime() const {
    std::lock_guard<std::mutex> lock(eventsMutex_);
    if (events_.empty()) return std::chrono::system_clock::time_point();
    
    return events_.back().getTimestamp();
}

std::string EventLogger::analyzeEventFrequency() const {
    std::ostringstream oss;
    oss << "Event Frequency Analysis:\n";
    oss << "=========================\n\n";
    
    std::unordered_map<EventType, size_t> eventCounts;
    {
        std::lock_guard<std::mutex> lock(eventsMutex_);
        for (const auto& event : events_) {
            eventCounts[event.getType()]++;
        }
    }
    
    for (const auto& pair : eventCounts) {
        oss << eventTypeToString(pair.first) << ": " << pair.second << " events\n";
    }
    
    return oss.str();
}

std::string EventLogger::analyzeEventSequences() const {
    std::ostringstream oss;
    oss << "Event Sequence Analysis:\n";
    oss << "=========================\n\n";
    
    std::lock_guard<std::mutex> lock(eventsMutex_);
    
    if (events_.size() < 2) {
        oss << "Insufficient events for sequence analysis.\n";
        return oss.str();
    }
    
    // Find most common event pairs
    std::unordered_map<std::string, std::unordered_map<std::string, size_t>> sequenceCounts;
    
    for (size_t i = 0; i < events_.size() - 1; ++i) {
        std::string event1 = eventTypeToString(events_[i].getType());
        std::string event2 = eventTypeToString(events_[i+1].getType());
        sequenceCounts[event1][event2]++;
    }
    
    // Find most common sequence
    std::string mostCommonSeq;
    size_t maxCount = 0;
    
    for (const auto& pair1 : sequenceCounts) {
        for (const auto& pair2 : pair1.second) {
            if (pair2.second > maxCount) {
                maxCount = pair2.second;
                mostCommonSeq = pair1.first + " -> " + pair2.first;
            }
        }
    }
    
    if (!mostCommonSeq.empty()) {
        oss << "Most common sequence: " << mostCommonSeq << " (" << maxCount << " times)\n";
    } else {
        oss << "No significant sequences found.\n";
    }
    
    return oss.str();
}

std::string EventLogger::analyzeCorrelation(const std::string& event1, const std::string& event2) const {
    std::ostringstream oss;
    oss << "Correlation Analysis: " << event1 << " vs " << event2 << "\n";
    oss << "========================\n\n";
    
    // Simple correlation based on temporal proximity
    size_t event1Count = 0, event2Count = 0, simultaneousCount = 0;
    {
        std::lock_guard<std::mutex> lock(eventsMutex_);
        for (const auto& e : events_) {
            std::string typeStr = eventTypeToString(e.getType());
            if (typeStr == event1) event1Count++;
            if (typeStr == event2) event2Count++;
        }
        
        // Check for simultaneous events (within 1 second)
        for (size_t i = 0; i < events_.size(); ++i) {
            auto time1 = std::chrono::system_clock::to_time_t(events_[i].getTimestamp());
            
            for (size_t j = i + 1; j < events_.size(); ++j) {
                auto time2 = std::chrono::system_clock::to_time_t(events_[j].getTimestamp());
                if (std::abs(std::difftime(time1, time2)) < 1.0) {
                    std::string type1 = eventTypeToString(events_[i].getType());
                    std::string type2 = eventTypeToString(events_[j].getType());
                    if ((type1 == event1 && type2 == event2) || 
                        (type1 == event2 && type2 == event1)) {
                        simultaneousCount++;
                    }
                }
            }
        }
    }
    
    oss << "Event counts:\n";
    oss << "  " << event1 << ": " << event1Count << "\n";
    oss << "  " << event2 << ": " << event2Count << "\n";
    oss << "  Simultaneous pairs: " << simultaneousCount << "\n";
    
    if (event1Count > 0 && event2Count > 0) {
        double correlation = static_cast<double>(simultaneousCount) / 
                            std::min(event1Count, event2Count);
        oss << "Correlation: " << correlation << "\n";
        
        if (correlation > 0.5) {
            oss << "Strong temporal correlation detected!\n";
        } else if (correlation > 0.2) {
            oss << "Moderate temporal correlation detected.\n";
        } else {
            oss << "Weak or no temporal correlation.\n";
        }
    }
    
    return oss.str();
}

void EventLogger::notifyDebuggingSystem(const SimulationEvent& event) {
    if (debuggingSystem_) {
        // Convert simulation event to debug event
        DebugEventType debugType;
        
        switch (event.getType()) {
            case EventType::StepCompleted: debugType = DebugEventType::StepEnd; break;
            case EventType::NeuronFired: debugType = DebugEventType::NeuronSpike; break;
            case EventType::SynapseActivated: debugType = DebugEventType::SynapseFire; break;
            case EventType::MemoryAccessed: debugType = DebugEventType::MemoryAccess; break;
            case EventType::MemoryWritten: debugType = DebugEventType::MemoryAllocation; break;
            case EventType::MemoryRead: debugType = DebugEventType::MemoryDeallocation; break;
            case EventType::PlasticityChange: debugType = DebugEventType::PlasticityChange; break;
            case EventType::NeuromodulationApplied: debugType = DebugEventType::Neuromodulation; break;
            case EventType::SensorInputReceived: debugType = DebugEventType::SensorInput; break;
            case EventType::ActionProduced: debugType = DebugEventType::MotorOutput; break;
            case EventType::RewardReceived: debugType = DebugEventType::RewardSignal; break;
            case EventType::PredictionError: debugType = DebugEventType::PredictionError; break;
            case EventType::DevelopmentStageChanged: debugType = DebugEventType::DevelopmentChange; break;
            case EventType::LearningSignal: debugType = DebugEventType::LearningSignal; break;
            case EventType::ConnectionCreated: debugType = DebugEventType::ConnectionCreate; break;
            case EventType::ConnectionRemoved: debugType = DebugEventType::ConnectionRemove; break;
            case EventType::WeightChanged: debugType = DebugEventType::WeightChange; break;
            case EventType::RegionCreated: debugType = DebugEventType::RegionCreate; break;
            case EventType::RegionDestroyed: debugType = DebugEventType::RegionDestroy; break;
            default: debugType = DebugEventType::Custom; break;
        }
        
        debuggingSystem_->recordEvent(debugType, event.getSource(), event.getMessage());
    }
}

std::string EventLogger::eventTypeToString(EventType type) const {
    switch (type) {
        case EventType::StepCompleted: return "StepCompleted";
        case EventType::NeuronFired: return "NeuronFired";
        case EventType::SynapseActivated: return "SynapseActivated";
        case EventType::MemoryAccessed: return "MemoryAccessed";
        case EventType::MemoryWritten: return "MemoryWritten";
        case EventType::MemoryRead: return "MemoryRead";
        case EventType::PlasticityChange: return "PlasticityChange";
        case EventType::NeuromodulationApplied: return "NeuromodulationApplied";
        case EventType::SensorInputReceived: return "SensorInputReceived";
        case EventType::ActionProduced: return "ActionProduced";
        case EventType::RewardReceived: return "RewardReceived";
        case EventType::PredictionError: return "PredictionError";
        case EventType::DevelopmentStageChanged: return "DevelopmentStageChanged";
        case EventType::LearningSignal: return "LearningSignal";
        case EventType::ConnectionCreated: return "ConnectionCreated";
        case EventType::ConnectionRemoved: return "ConnectionRemoved";
        case EventType::WeightChanged: return "WeightChanged";
        case EventType::RegionCreated: return "RegionCreated";
        case EventType::RegionDestroyed: return "RegionDestroyed";
        case EventType::Custom: return "Custom";
        default: return "Unknown";
    }
}

std::string EventLogger::eventPriorityToString(EventPriority priority) const {
    switch (priority) {
        case EventPriority::Low: return "Low";
        case EventPriority::Normal: return "Normal";
        case EventPriority::High: return "High";
        case EventPriority::Critical: return "Critical";
        default: return "Unknown";
    }
}

EventLogger::~EventLogger() = default;

StateTracker::StateTracker() = default;

StateTracker::~StateTracker() = default;

void StateTracker::recordState(const std::string& stateName, const std::string& stateData, 
                             const std::string& context) {
    StateEntry entry;
    entry.timestamp = std::chrono::system_clock::now();
    entry.data = stateData;
    entry.context = context;
    
    std::lock_guard<std::mutex> lock(historyMutex_);
    stateHistory_[stateName].push_back(entry);
    
    // Keep only last 1000 entries per state
    if (stateHistory_[stateName].size() > 1000) {
        stateHistory_[stateName].erase(stateHistory_[stateName].begin());
    }
    
    if (debuggingSystem_) {
        debuggingSystem_->recordState(stateName, stateData);
    }
}

void StateTracker::recordState(const std::string& stateName, double value, const std::string& unit) {
    std::ostringstream oss;
    oss << value << " " << unit;
    recordState(stateName, oss.str(), "numeric");
}

void StateTracker::recordState(const std::string& stateName, size_t count, const std::string& description) {
    std::ostringstream oss;
    oss << count << " (" << description << ")";
    recordState(stateName, oss.str(), "count");
}

std::string StateTracker::getState(const std::string& stateName) const {
    std::lock_guard<std::mutex> lock(historyMutex_);
    auto it = stateHistory_.find(stateName);
    if (it == stateHistory_.end() || it->second.empty()) {
        return "";
    }
    return it->second.back().data;
}

std::string StateTracker::getState(const std::string& stateName, const std::string& defaultValue) const {
    std::string value = getState(stateName);
    return value.empty() ? defaultValue : value;
}

double StateTracker::getStateAsDouble(const std::string& stateName) const {
    std::string value = getState(stateName);
    if (value.empty()) return 0.0;
    
    try {
        return std::stod(value);
    } catch (...) {
        return 0.0;
    }
}

size_t StateTracker::getStateAsSize(const std::string& stateName) const {
    std::string value = getState(stateName);
    if (value.empty()) return 0;
    
    try {
        return static_cast<size_t>(std::stoul(value));
    } catch (...) {
        return 0;
    }
}

std::vector<std::pair<std::chrono::system_clock::time_point, std::string>> 
    StateTracker::getStateHistory(const std::string& stateName) const {
    std::lock_guard<std::mutex> lock(historyMutex_);
    auto it = stateHistory_.find(stateName);
    if (it == stateHistory_.end()) {
        return {};
    }
    
    std::vector<std::pair<std::chrono::system_clock::time_point, std::string>> history;
    for (const auto& entry : it->second) {
        history.emplace_back(entry.timestamp, entry.data);
    }
    return history;
}

std::string StateTracker::analyzeStateChanges(const std::string& stateName) const {
    std::ostringstream oss;
    oss << "State Change Analysis for " << stateName << ":\n";
    oss << "==================================\n\n";
    
    auto history = getStateHistory(stateName);
    if (history.size() < 2) {
        oss << "Insufficient data for analysis.\n";
        return oss.str();
    }
    
    // Calculate changes
    oss << "Total recorded states: " << history.size() << "\n";
    oss << "First value: " << history.front().second << "\n";
    oss << "Last value: " << history.back().second << "\n";
    
    // Count changes
    size_t changeCount = 0;
    std::string current = history.front().second;
    for (size_t i = 1; i < history.size(); ++i) {
        if (history[i].second != current) {
            changeCount++;
            current = history[i].second;
        }
    }
    
    oss << "Total changes: " << changeCount << "\n";
    oss << "Change frequency: " << (static_cast<double>(changeCount) / (history.size() - 1)) << " per event\n";
    
    return oss.str();
}

std::string StateTracker::analyzeCorrelations(const std::string& state1, const std::string& state2) const {
    std::ostringstream oss;
    oss << "Correlation Analysis: " << state1 << " vs " << state2 << "\n";
    oss << "========================\n\n";
    
    auto history1 = getStateHistory(state1);
    auto history2 = getStateHistory(state2);
    
    if (history1.empty() || history2.empty()) {
        oss << "Insufficient data for correlation analysis.\n";
        return oss.str();
    }
    
    // Simple correlation based on temporal proximity
    std::vector<double> values1, values2;
    
    // Match timestamps (simplified)
    for (size_t i = 0; i < std::min(history1.size(), history2.size()); ++i) {
        values1.push_back(std::stod(history1[i].second));
        values2.push_back(std::stod(history2[i].second));
    }
    
    if (values1.size() < 2) {
        oss << "Insufficient paired data for correlation analysis.\n";
        return oss.str();
    }
    
    // Calculate Pearson correlation
    double sumX = std::accumulate(values1.begin(), values1.end(), 0.0);
    double sumY = std::accumulate(values2.begin(), values2.end(), 0.0);
    double sumXY = 0.0, sumX2 = 0.0, sumY2 = 0.0;
    
    for (size_t i = 0; i < values1.size(); ++i) {
        sumXY += values1[i] * values2[i];
        sumX2 += values1[i] * values1[i];
        sumY2 += values2[i] * values2[i];
    }
    
    double n = values1.size();
    double denominator = std::sqrt((n * sumX2 - sumX * sumX) * (n * sumY2 - sumY * sumY));
    
    oss << "Data points: " << values1.size() << "\n";
    oss << "Mean of " << state1 << ": " << (sumX / n) << "\n";
    oss << "Mean of " << state2 << ": " << (sumY / n) << "\n";
    
    if (denominator > 0) {
        double correlation = (n * sumXY - sumX * sumY) / denominator;
        oss << "Pearson Correlation: " << correlation << "\n";
        
        if (std::abs(correlation) > 0.7) {
            oss << "Strong correlation detected!\n";
        } else if (std::abs(correlation) > 0.3) {
            oss << "Moderate correlation detected.\n";
        } else {
            oss << "Weak or no correlation.\n";
        }
    } else {
        oss << "Cannot calculate correlation (insufficient variance).\n";
    }
    
    return oss.str();
}

EventLogger::~EventLogger() = default;

StateTracker::~StateTracker() = default;

AnomalyDetector::AnomalyDetector()
    : threshold_(2.0), window_(60.0), minSampleCount_(5) {
}

AnomalyDetector::AnomalyDetector(std::shared_ptr<EventLogger> eventLogger)
    : eventLogger_(eventLogger), threshold_(2.0), window_(60.0), minSampleCount_(5) {
}

std::vector<SimulationEvent> AnomalyDetector::detectAnomalies() const {
    if (!eventLogger_) return {};
    
    std::vector<SimulationEvent> anomalies;
    
    // Detect anomalies based on event frequency
    auto events = eventLogger_->getEvents();
    
    // Group events by type
    std::unordered_map<EventType, std::vector<SimulationEvent>> eventsByType;
    for (const auto& event : events) {
        eventsByType[event.getType()].push_back(event);
    }
    
    for (const auto& pair : eventsByType) {
        const auto& eventList = pair.second;
        
        if (eventList.size() < minSampleCount_) continue;
        
        // Calculate basic statistics
        size_t count = eventList.size();
        double avgInterval = eventLogger_->getAverageTimeBetweenEvents();
        
        // Check for unusual patterns (simplified)
        if (count > 100) { // More than 100 events of same type
            for (const auto& event : eventList) {
                anomalies.push_back(event);
            }
        }
    }
    
    return anomalies;
}

std::string AnomalyDetector::getAnomalyReport() const {
    std::ostringstream oss;
    oss << "Anomaly Detection Report\n";
    oss << "========================\n\n";
    
    if (!eventLogger_) {
        oss << "No event logger available.\n";
        return oss.str();
    }
    
    auto anomalies = detectAnomalies();
    
    if (anomalies.empty()) {
        oss << "No anomalies detected.\n";
    } else {
        oss << "Detected anomalies: " << anomalies.size() << "\n";
        for (const auto& anomaly : anomalies) {
            oss << "  - " << eventLogger_->eventTypeToString(anomaly.getType()) 
                << " at " << anomaly.getSource() << "\n";
        }
    }
    
    return oss.str();
}

double AnomalyDetector::calculateAnomalyScore(const std::vector<double>& values) const {
    if (values.empty()) return 0.0;
    
    double mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    double variance = 0.0;
    for (double value : values) {
        variance += (value - mean) * (value - mean);
    }
    variance /= values.size();
    
    return std::sqrt(variance); // Score based on standard deviation
}

std::string AnomalyDetector::analyzeAnomalyPattern(const SimulationEvent& anomaly) const {
    return "Anomaly pattern analysis not implemented.\n";
}

bool AnomalyDetector::exportAnomalies(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << exportAnomaliesAsString();
        file.close();
        return true;
    } catch (...) {
        return false;
    }
}

std::string AnomalyDetector::exportAnomaliesAsString() const {
    std::ostringstream oss;
    oss << "Anomaly Report\n";
    oss << "============\n\n";
    
    if (!eventLogger_) {
        oss << "No event logger available.\n";
        return oss.str();
    }
    
    auto anomalies = detectAnomalies();
    
    if (anomalies.empty()) {
        oss << "No anomalies detected.\n";
    } else {
        oss << "Total anomalies: " << anomalies.size() << "\n\n";
        
        std::unordered_map<EventType, size_t> anomalyCounts;
        for (const auto& anomaly : anomalies) {
            anomalyCounts[anomaly.getType()]++;
        }
        
        for (const auto& pair : anomalyCounts) {
            oss << eventLogger_->eventTypeToString(pair.first) 
                << ": " << pair.second << " occurrences\n";
        }
    }
    
    return oss.str();
}

EventLogger::~EventLogger() = default;

StateTracker::~StateTracker() = default;

AnomalyDetector::~AnomalyDetector() = default;

EventLogger::~EventLogger() = default;

} // namespace nlm
