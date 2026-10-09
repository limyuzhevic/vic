#pragma once

#include <string>
#include <regex>
#include <chrono>
#include <set>
#include <memory>
#include <functional>
#include <unordered_map>
#include <mutex>
#include "Enhanced/LogFormatter.hpp"

namespace nlm {

// Base filter class
class LogFilter {
public:
    virtual ~LogFilter() = default;
    virtual bool shouldLog(const EnhancedLogEntry& entry) const = 0;
    virtual std::string getName() const = 0;
    virtual void reset() {}
};

// Level-based filter
class LevelFilter : public LogFilter {
public:
    explicit LevelFilter(LogLevel minLevel) : minLevel_(minLevel) {}
    
    bool shouldLog(const EnhancedLogEntry& entry) const override {
        return entry.level >= minLevel_;
    }
    
    std::string getName() const override {
        return "LevelFilter(minLevel=" + std::to_string(static_cast<int>(minLevel_)) + ")";
    }
    
    void setLevel(LogLevel level) { minLevel_ = level; }
    
private:
    LogLevel minLevel_;
};

// Exclusion filter
class ExclusionFilter : public LogFilter {
public:
    explicit ExclusionFilter(const std::set<LogLevel>& excludedLevels) : excludedLevels_(excludedLevels) {}
    
    bool shouldLog(const EnhancedLogEntry& entry) const override {
        return excludedLevels_.find(entry.level) == excludedLevels_.end();
    }
    
    std::string getName() const override {
        return "ExclusionFilter(excluded=" + levelsToString(excludedLevels_) + ")";
    }
    
private:
    std::set<LogLevel> excludedLevels_;
    
    std::string levelsToString(const std::set<LogLevel>& levels) const {
        std::string result;
        bool first = true;
        for (const auto& level : levels) {
            if (!first) result += ",";
            first = false;
            result += levelToString(level);
        }
        return result;
    }
};

// Tag-based filter
class TagFilter : public LogFilter {
public:
    explicit TagFilter(const std::set<std::string>& requiredTags) : requiredTags_(requiredTags) {}
    
    bool shouldLog(const EnhancedLogEntry& entry) const override {
        for (const auto& tag : requiredTags_) {
            if (entry.tags.find(tag) == entry.tags.end()) {
                return false;
            }
        }
        return true;
    }
    
    std::string getName() const override {
        return "TagFilter(tags=" + tagsToString(requiredTags_) + ")";
    }
    
private:
    std::set<std::string> requiredTags_;
    
    std::string tagsToString(const std::set<std::string>& tags) const {
        std::string result;
        bool first = true;
        for (const auto& tag : tags) {
            if (!first) result += ",";
            first = false;
            result += tag;
        }
        return result;
    }
};

// Regex-based filter
class RegexFilter : public LogFilter {
public:
    explicit RegexFilter(const std::string& pattern, bool matchMessage = true, 
                         bool matchFile = false, bool matchFunction = false) 
        : regex_(pattern), matchMessage_(matchMessage), matchFile_(matchFile), matchFunction_(matchFunction) {}
    
    bool shouldLog(const EnhancedLogEntry& entry) const override {
        std::string content;
        if (matchMessage_ && matchFile_ && matchFunction_) {
            content = entry.message + " " + entry.file + " " + entry.function;
        } else if (matchMessage_) {
            content = entry.message;
        } else if (matchFile_) {
            content = entry.file;
        } else if (matchFunction_) {
            content = entry.function;
        }
        
        return std::regex_search(content, regex_);
    }
    
    std::string getName() const override {
        return "RegexFilter(pattern=" + regex_.str() + ")";
    }
    
private:
    std::regex regex_;
    bool matchMessage_;
    bool matchFile_;
    bool matchFunction_;
};

// Time-based filter (log entries within a time window)
class TimeWindowFilter : public LogFilter {
public:
    TimeWindowFilter(
        const std::chrono::system_clock::time_point& startTime,
        const std::chrono::system_clock::time_point& endTime)
        : startTime_(startTime), endTime_(endTime) {}
    
    bool shouldLog(const EnhancedLogEntry& entry) const override {
        return entry.timestamp >= startTime_ && entry.timestamp <= endTime_;
    }
    
    std::string getName() const override {
        auto start = std::chrono::system_clock::to_time_t(startTime_);
        auto end = std::chrono::system_clock::to_time_t(endTime_);
        char startBuf[100], endBuf[100];
        strftime(startBuf, sizeof(startBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&start));
        strftime(endBuf, sizeof(endBuf), "%Y-%m-%d %H:%M:%S", std::localtime(&end));
        return std::string("TimeWindowFilter(start=") + startBuf + ", end=" + endBuf + ")";
    }
    
    void setTimeWindow(const std::chrono::system_clock::time_point& start, const std::chrono::system_clock::time_point& end) {
        startTime_ = start;
        endTime_ = end;
    }
    
private:
    std::chrono::system_clock::time_point startTime_;
    std::chrono::system_clock::time_point endTime_;
};

// Composite filter (AND/OR combinations)
class CompositeFilter : public LogFilter {
public:
    enum class Operation { AND, OR };
    
    CompositeFilter(Operation op) : operation_(op) {}
    
    void addFilter(std::unique_ptr<LogFilter> filter) {
        filters_.push_back(std::move(filter));n    }
    
    bool shouldLog(const EnhancedLogEntry& entry) const override {
        if (filters_.empty()) return true;
        
        bool result = operation_ == Operation::AND ? true : false;
        
        for (const auto& filter : filters_) {
            bool filterResult = filter->shouldLog(entry);
            if (operation_ == Operation::AND) {
                result = result && filterResult;
            } else {
                result = result || filterResult;
            }
            
            if (operation_ == Operation::AND && !result) break;
            if (operation_ == Operation::OR && result) break;
        }
        
        return result;
    }
    
    std::string getName() const override {
        std::string result = "CompositeFilter(" + (operation_ == Operation::AND ? "AND" : "OR");
        result += " filters: [";
        for (size_t i = 0; i < filters_.size(); ++i) {
            if (i > 0) result += ", ";
            result += filters_[i]->getName();
        }
        result += "])";
        return result;
    }
    
private:
    Operation operation_;
    std::vector<std::unique_ptr<LogFilter>> filters_;
};

// Filter factory for easy filter creation
class LogFilterFactory {
public:
    static std::unique_ptr<LogFilter> createFilter(const std::string& type, const std::string& config = "") {
        if (type == "Level") {
            // Parse config for level (e.g., "Info", "Warning", etc.)
            LogLevel level;
            if (config == "Debug") level = LogLevel::Debug;
            else if (config == "Info") level = LogLevel::Info;
            else if (config == "Warning") level = LogLevel::Warning;
            else if (config == "Error") level = LogLevel::Error;
            else if (config == "Critical") level = LogLevel::Critical;
            else level = LogLevel::Info;
            return std::make_unique<LevelFilter>(level);
        } else if (type == "Exclusion") {
            std::set<LogLevel> excludedLevels;
            // Simple parsing for demo - could be more sophisticated
            if (config.find("Debug") != std::string::npos) excludedLevels.insert(LogLevel::Debug);
            if (config.find("Info") != std::string::npos) excludedLevels.insert(LogLevel::Info);
            return std::make_unique<ExclusionFilter>(excludedLevels);
        } else if (type == "Tag") {
            std::set<std::string> tags;
            // Simple parsing for demo - could be more sophisticated
            size_t pos = 0;
            while ((pos = config.find(',', pos)) != std::string::npos) {
                std::string tag = config.substr(0, pos);
                tags.insert(tag);
                config.erase(0, pos + 1);
            }
            tags.insert(config);
            return std::make_unique<TagFilter>(tags);
        } else if (type == "Regex") {
            bool matchMessage = true;
            bool matchFile = false;
            bool matchFunction = false;
            // Simple parsing for demo
            if (config.find("file") != std::string::npos) matchFile = true;
            if (config.find("function") != std::string::npos) matchFunction = true;
            return std::make_unique<RegexFilter>(config, matchMessage, matchFile, matchFunction);
        } else if (type == "CompositeAND" || type == "CompositeOR") {
            auto composite = std::make_unique<CompositeFilter>(type == "CompositeAND" ? CompositeFilter::Operation::AND : CompositeFilter::Operation::OR);
            // For demo, no additional filters are added in factory
            return composite;
        } else {
            // Default to Level filter
            return std::make_unique<LevelFilter>(LogLevel::Info);
        }
    }
};

} // namespace nlm
