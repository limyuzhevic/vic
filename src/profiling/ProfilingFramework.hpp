// Core Profiling Framework
//
// This file implements the core profiling system for NLM, providing
// performance measurement, profiling, and analysis capabilities.

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>

#include "core/Config/Config.hpp"

namespace nlm {

class ProfileData {
public:
    std::string name;
    std::string description;
    double timestamp;
    std::map<std::string, double> metrics;
    std::map<std::string, std::string> metadata;
    std::vector<std::string> tags;
    
    ProfileData(const std::string& name, const std::string& description)
        : name(name), description(description), timestamp(getCurrentTimestamp()) {}
    
    virtual ~ProfileData() = default;
    
    virtual void serialize(std::ostream& output) const {
        output << "{\n";
        output << "  \"name\": \"" << name << "\",\n";
        output << "  \"description\": \"" << description << "\",\n";
        output << "  \"timestamp\": " << timestamp << ",\n";
        output << "  \"metrics\": {\n";
        for (size_t i = 0; i < metrics.size(); ++i) {
            auto it = std::next(metrics.begin(), i);
            output << "    \"" << it->first << "\": " << it->second;
            if (i < metrics.size() - 1) output << ",\n";
            else output << "\n";
        }
        output << "  },\n";
        output << "  \"metadata\": {\n";
        for (size_t i = 0; i < metadata.size(); ++i) {
            auto it = std::next(metadata.begin(), i);
            output << "    \"" << it->first << "\": \"" << it->second << "\"";
            if (i < metadata.size() - 1) output << ",\n";
            else output << "\n";
        }
        output << "  }\n";
        output << "}\n";
    }
    
    virtual std::string toJSON() const {
        std::ostringstream json;
        serialize(json);
        return json.str();
    }
    
    virtual std::string toText() const {
        std::ostringstream text;
        text << "Profile: " << name << "\n";
        text << "Description: " << description << "\n";
        text << "Timestamp: " << timestamp << "\n";
        text << "Metrics:\n";
        for (const auto& metric : metrics) {
            text << "  " << metric.first << ": " << metric.second << "\n";
        }
        text << "Tags: ";
        for (size_t i = 0; i < tags.size(); ++i) {
            text << tags[i];
            if (i < tags.size() - 1) text << ", ";
        }
        text << "\n";
        return text.str();
    }
    
    void addMetric(const std::string& name, double value) {
        metrics[name] = value;
    }
    
    void addMetadata(const std::string& name, const std::string& value) {
        metadata[name] = value;
    }
    
    void addTag(const std::string& tag) {
        tags.push_back(tag);
    }
    
    double getMetric(const std::string& name, double defaultValue = 0.0) const {
        auto it = metrics.find(name);
        return (it != metrics.end()) ? it->second : defaultValue;
    }
    
    static double getCurrentTimestamp() {
        return std::chrono::duration<double>(
            std::chrono::steady_clock::now().time_since_epoch()
        ).count();
    }
};

class ProfilingSession {
public:
    std::string sessionId;
    double startTime;
    double endTime;
    bool isActive;
    std::vector<std::shared_ptr<ProfileData>> profileData;
    std::map<std::string, std::shared_ptr<ProfilingSession>> childSessions;
    
    ProfilingSession(const std::string& id)
        : sessionId(id), startTime(ProfileData::getCurrentTimestamp()), 
          isActive(true) {}
    
    virtual ~ProfilingSession() = default;
    
    void start() {
        startTime = ProfileData::getCurrentTimestamp();
        isActive = true;
    }
    
    void stop() {
        endTime = ProfileData::getCurrentTimestamp();
        isActive = false;
    }
    
    std::shared_ptr<ProfileData> recordProfile(const std::string& name, 
                                              const std::string& description) {
        auto data = std::make_shared<ProfileData>(name, description);
        profileData.push_back(data);
        return data;
    }
    
    std::shared_ptr<ProfilingSession> createChildSession(const std::string& id) {
        auto childSession = std::make_shared<ProfilingSession>(id);
        childSessions[id] = childSession;
        return childSession;
    }
    
    double getDuration() const {
        if (isActive) {
            return ProfileData::getCurrentTimestamp() - startTime;
        }
        return endTime - startTime;
    }
    
    void serialize(std::ostream& output) const {
        output << "{\n";
        output << "  \"sessionId\": \"" << sessionId << "\",\n";
        output << "  \"startTime\": " << startTime << ",\n";
        output << "  \"endTime\": " << endTime << ",\n";
        output << "  \"isActive\": " << (isActive ? "true" : "false") << ",\n";
        output << "  \"profileData\": [\n";
        for (size_t i = 0; i < profileData.size(); ++i) {
            profileData[i]->serialize(output);
            if (i < profileData.size() - 1) output << ",\n";
        }
        output << "\n  ],\n";
        output << "  \"childSessions\": {\n";
        for (size_t i = 0; i < childSessions.size(); ++i) {
            auto it = std::next(childSessions.begin(), i);
            output << "    \"" << it->first << "\": ";
            it->second->serialize(output);
            if (i < childSessions.size() - 1) output << ",\n";
        }
        output << "\n  }\n";
        output << "}\n";
    }
};

class ProfilingBackend {
public:
    virtual ~ProfilingBackend() = default;
    
    virtual bool initialize(const Config& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isAvailable() const = 0;
    virtual std::string getName() const = 0;
    
    virtual void startProfiling(const std::string& sessionId) = 0;
    virtual void stopProfiling(const std::string& sessionId) = 0;
    virtual void recordMetric(const std::string& sessionId, 
                            const std::string& metricName,
                            double value,
                            const std::string& unit = "") = 0;
    
    virtual std::vector<std::shared_ptr<ProfileData>> getProfileData(const std::string& sessionId) = 0;
    virtual std::vector<std::string> getAvailableSessions() const = 0;
};

class ProfilingFramework {
public:
    ProfilingFramework();
    ~ProfilingFramework();
    
    static ProfilingFramework& getInstance() {
        static ProfilingFramework instance;
        return instance;
    }
    
    bool initialize(const std::string& configFile = "");
    void shutdown();
    
    bool registerBackend(const std::string& name, std::unique_ptr<ProfilingBackend> backend);
    std::vector<std::string> getAvailableBackends() const;
    
    bool startProfiling(const std::string& sessionId, const std::string& backendName = "default");
    bool stopProfiling(const std::string& sessionId);
    
    void recordMetric(const std::string& sessionId,
                     const std::string& metricName,
                     double value,
                     const std::string& unit = "",
                     const std::string& backendName = "default");
    
    std::vector<std::shared_ptr<ProfileData>> getProfileData(const std::string& sessionId) const;
    std::vector<std::string> getActiveSessions() const;
    
    void saveProfile(const std::string& sessionId, const std::string& filename) const;
    std::string generateReport(const std::string& sessionId, 
                              const std::string& format = "text") const;
    
    bool isProfiling(const std::string& sessionId) const;
    
    void setConfig(const Config& config);
    
private:
    std::map<std::string, std::unique_ptr<ProfilingBackend>> backends_;
    std::map<std::string, std::shared_ptr<ProfilingSession>> activeSessions_;
    mutable std::mutex mutex_;
    Config config_;
    bool initialized_;
    
    void cleanupInactiveSessions();
};

} // namespace nlm
