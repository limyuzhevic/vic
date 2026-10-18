// Core Profiling Framework Implementation
//
// This file implements the core profiling system for NLM.

#include "ProfilingFramework.hpp"

namespace nlm {

ProfilingFramework::ProfilingFramework() : initialized_(false) {}

ProfilingFramework::~ProfilingFramework() {
    shutdown();
}

bool ProfilingFramework::initialize(const std::string& configFile) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        if (configFile.empty()) {
            // Initialize with default configuration
            config_.set("profiling.enabled", true, ConfigSource::Default);
            config_.set("profiling.default_backend", "default", ConfigSource::Default);
            config_.set("profiling.sampling_rate", 1000.0, ConfigSource::Default);
            config_.set("profiling.max_memory_mb", 1024.0, ConfigSource::Default);
        } else {
            // Load configuration from file
            // This would require a file loading implementation
            config_.set("profiling.enabled", true, ConfigSource::Default);
            config_.set("profiling.default_backend", "default", ConfigSource::Default);
            config_.set("profiling.sampling_rate", 1000.0, ConfigSource::Default);
            config_.set("profiling.max_memory_mb", 1024.0, ConfigSource::Default);
        }
        
        // Initialize default backend
        auto defaultBackend = createDefaultBackend();
        if (defaultBackend && defaultBackend->initialize(config_)) {
            registerBackend("default", std::move(defaultBackend));
        }
        
        initialized_ = true;
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

void ProfilingFramework::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    activeSessions_.clear();
    
    for (auto& pair : backends_) {
        pair.second->shutdown();
    }
    
    backends_.clear();
    initialized_ = false;
}

bool ProfilingFramework::registerBackend(const std::string& name, 
                                        std::unique_ptr<ProfilingBackend> backend) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!backend) return false;
    
    backends_[name] = std::move(backend);
    return true;
}

std::vector<std::string> ProfilingFramework::getAvailableBackends() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> backendNames;
    backendNames.reserve(backends_.size());
    
    for (const auto& pair : backends_) {
        backendNames.push_back(pair.first);
    }
    
    return backendNames;
}

bool ProfilingFramework::startProfiling(const std::string& sessionId, 
                                       const std::string& backendName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!initialized_) {
        return false;
    }
    
    auto it = backends_.find(backendName);
    if (it == backends_.end()) {
        return false;
    }
    
    auto session = std::make_shared<ProfilingSession>(sessionId);
    activeSessions_[sessionId] = session;
    
    it->second->startProfiling(sessionId);
    return true;
}

bool ProfilingFramework::stopProfiling(const std::string& sessionId) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = activeSessions_.find(sessionId);
    if (it == activeSessions_.end()) {
        return false;
    }
    
    auto backendIt = backends_.find("default");
    if (backendIt == backends_.end()) {
        return false;
    }
    
    backendIt->second->stopProfiling(sessionId);
    it->second->stop();
    
    activeSessions_.erase(it);
    return true;
}

void ProfilingFramework::recordMetric(const std::string& sessionId,
                                      const std::string& metricName,
                                      double value,
                                      const std::string& unit,
                                      const std::string& backendName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = activeSessions_.find(sessionId);
    if (it == activeSessions_.end()) {
        return;
    }
    
    auto backendIt = backends_.find(backendName);
    if (backendIt == backends_.end()) {
        return;
    }
    
    backendIt->second->recordMetric(sessionId, metricName, value, unit);
}

std::vector<std::shared_ptr<ProfileData>> ProfilingFramework::getProfileData(const std::string& sessionId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = activeSessions_.find(sessionId);
    if (it == activeSessions_.end()) {
        return {};
    }
    
    auto backendIt = backends_.find("default");
    if (backendIt == backends_.end()) {
        return {};
    }
    
    return backendIt->second->getProfileData(sessionId);
}

std::vector<std::string> ProfilingFramework::getActiveSessions() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> sessionIds;
    sessionIds.reserve(activeSessions_.size());
    
    for (const auto& pair : activeSessions_) {
        sessionIds.push_back(pair.first);
    }
    
    return sessionIds;
}

void ProfilingFramework::saveProfile(const std::string& sessionId, const std::string& filename) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = activeSessions_.find(sessionId);
    if (it == activeSessions_.end()) {
        return;
    }
    
    auto backendIt = backends_.find("default");
    if (backendIt == backends_.end()) {
        return;
    }
    
    std::ofstream outputFile(filename);
    if (!outputFile.is_open()) {
        return;
    }
    
    auto profileData = backendIt->second->getProfileData(sessionId);
    
    outputFile << "[\n";
    for (size_t i = 0; i < profileData.size(); ++i) {
        outputFile << profileData[i]->toJSON();
        if (i < profileData.size() - 1) outputFile << ",\n";
    }
    outputFile << "\n]\n";
}

std::string ProfilingFramework::generateReport(const std::string& sessionId, 
                                               const std::string& format) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = activeSessions_.find(sessionId);
    if (it == activeSessions_.end()) {
        return "Session not found: " + sessionId;
    }
    
    auto backendIt = backends_.find("default");
    if (backendIt == backends_.end()) {
        return "Default backend not available";
    }
    
    auto profileData = backendIt->second->getProfileData(sessionId);
    
    if (format == "text") {
        std::ostringstream report;
        report << "Profiling Report for Session: " << sessionId << "\n";
        report << "========================================\n\n";
        
        for (const auto& data : profileData) {
            report << data->toText() << "\n\n";
        }
        
        return report.str();
    } else if (format == "json") {
        std::ostringstream report;
        report << "{\n";
        report << "  \"sessionId\": \"" << sessionId << "\",\n";
        report << "  \"profileCount\": " << profileData.size() << ",\n";
        report << "  \"profiles\": [\n";
        
        for (size_t i = 0; i < profileData.size(); ++i) {
            report << "    " << profileData[i]->toJSON();
            if (i < profileData.size() - 1) report << ",\n";
        }
        report << "\n  ]\n";
        report << "}\n";
        
        return report.str();
    }
    
    return "Unsupported report format: " + format;
}

bool ProfilingFramework::isProfiling(const std::string& sessionId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = activeSessions_.find(sessionId);
    return (it != activeSessions_.end()) && it->second->isActive;
}

void ProfilingFramework::setConfig(const Config& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    config_ = config;
}

void ProfilingFramework::cleanupInactiveSessions() {
    std::vector<std::string> toRemove;
    
    for (const auto& pair : activeSessions_) {
        if (!pair.second->isActive) {
            toRemove.push_back(pair.first);
        }
    }
    
    for (const auto& sessionId : toRemove) {
        activeSessions_.erase(sessionId);
    }
}

std::unique_ptr<ProfilingBackend> ProfilingFramework::createDefaultBackend() {
    // Create a simple default backend for demonstration
    class DefaultProfilingBackend : public ProfilingBackend {
    public:
        bool initialize(const Config& config) override {
            available_ = true;
            return true;
        }
        
        void shutdown() override {
            available_ = false;
        }
        
        bool isAvailable() const override {
            return available_;
        }
        
        std::string getName() const override {
            return "default";
        }
        
        void startProfiling(const std::string& sessionId) override {
            // Implementation would start actual profiling
        }
        
        void stopProfiling(const std::string& sessionId) override {
            // Implementation would stop actual profiling
        }
        
        void recordMetric(const std::string& sessionId,
                         const std::string& metricName,
                         double value,
                         const std::string& unit = "") override {
            // Implementation would record metrics to backend
        }
        
        std::vector<std::shared_ptr<ProfileData>> getProfileData(const std::string& sessionId) override {
            return {};
        }
        
        std::vector<std::string> getAvailableSessions() const override {
            return {};
        }
    
    private:
        bool available_ = false;
    };
    
    return std::make_unique<DefaultProfilingBackend>();
}

} // namespace nlm
