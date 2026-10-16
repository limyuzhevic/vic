// Forward declarations for ProfilerManager
struct ProfilerManager::Impl {
    ProfilerConfig globalConfig;
    std::map<std::string, std::shared_ptr<Profiler>> profilers;
    std::shared_ptr<Profiler> defaultProfiler;
    std::shared_ptr<Logger> logger;
    std::shared_ptr<StatisticsCollector> statisticsCollector;
    std::shared_ptr<RealTimeMonitor> realTimeMonitor;
};

// Implementation of ProfilerManager

ProfilerManager::ProfilerManager() : pImpl(std::make_unique<Impl>()) {
}

ProfilerManager::~ProfilerManager() {
    // Clean up profilers
    for (auto& pair : pImpl->profilers) {
        if (pair.second) {
            pair.second->stop();
        }
    }
}

std::shared_ptr<Profiler> ProfilerManager::createProfiler(const std::string& name, 
                                                          const ProfilerConfig& config) {
    auto profiler = std::make_shared<Profiler>(name, config);
    
    // Integrate with existing systems
    if (logger_) {
        profiler->integrateWithLogger(logger_);
    }
    if (statisticsCollector_) {
        profiler->integrateWithStatisticsCollector(statisticsCollector_);
    }
    if (realTimeMonitor_) {
        profiler->integrateWithRealTimeMonitor(realTimeMonitor_);
    }
    
    pImpl->profilers[name] = profiler;
    
    return profiler;
}

bool ProfilerManager::removeProfiler(const std::string& name) {
    auto it = pImpl->profilers.find(name);
    if (it == pImpl->profilers.end()) {
        return false;
    }
    
    // Stop the profiler
    if (it->second) {
        it->second->stop();
    }
    
    pImpl->profilers.erase(it);
    
    // Update default profiler if needed
    if (pImpl->defaultProfiler && pImpl->defaultProfiler->name_ == name) {
        pImpl->defaultProfiler = nullptr;
    }
    
    return true;
}

std::shared_ptr<Profiler> ProfilerManager::getProfiler(const std::string& name) {
    auto it = pImpl->profilers.find(name);
    if (it != pImpl->profilers.end()) {
        return it->second;
    }
    return nullptr;
}

std::shared_ptr<Profiler> ProfilerManager::getDefaultProfiler() {
    return pImpl->defaultProfiler;
}

void ProfilerManager::setDefaultProfiler(const std::string& name) {
    auto profiler = getProfiler(name);
    if (profiler) {
        pImpl->defaultProfiler = profiler;
    }
}

void ProfilerManager::startAllProfilers() {
    for (auto& pair : pImpl->profilers) {
        if (pair.second) {
            pair.second->start();
        }
    }
}

void ProfilerManager::stopAllProfilers() {
    for (auto& pair : pImpl->profilers) {
        if (pair.second) {
            pair.second->stop();
        }
    }
}

bool ProfilerManager::areAllProfilersRunning() {
    for (const auto& pair : pImpl->profilers) {
        if (pair.second && pair.second->isRunning()) {
            return true;
        }
    }
    return false;
}

void ProfilerManager::setGlobalConfig(const ProfilerConfig& config) {
    pImpl->globalConfig = config;
    
    // Apply to existing profilers
    for (auto& pair : pImpl->profilers) {
        if (pair.second) {
            pair.second->setConfig(config);
        }
    }
}

void ProfilerManager::integrateWithLogger(std::shared_ptr<Logger> logger) {
    pImpl->logger = logger;
    
    // Integrate with existing profilers
    for (auto& pair : pImpl->profilers) {
        if (pair.second) {
            pair.second->integrateWithLogger(logger);
        }
    }
}

void ProfilerManager::integrateWithStatisticsCollector(std::shared_ptr<StatisticsCollector> collector) {
    pImpl->statisticsCollector = collector;
    
    // Integrate with existing profilers
    for (auto& pair : pImpl->profilers) {
        if (pair.second) {
            pair.second->integrateWithStatisticsCollector(collector);
        }
    }
}

void ProfilerManager::integrateWithRealTimeMonitor(std::shared_ptr<RealTimeMonitor> monitor) {
    pImpl->realTimeMonitor = monitor;
    
    // Integrate with existing profilers
    for (auto& pair : pImpl->profilers) {
        if (pair.second) {
            pair.second->integrateWithRealTimeMonitor(monitor);
        }
    }
}

void ProfilerManager::saveAllProfiles(const std::string& directory) {
    for (auto& pair : pImpl->profilers) {
        if (pair.second) {
            pair.second->saveProfile(directory + "/" + pair.first + ".json");
        }
    }
}

void ProfilerManager::loadAllProfiles(const std::string& directory) {
    // This would load profiles from files
    // For now, just call saveAllProfiles to clear existing data
    saveAllProfiles(directory);
}

std::string ProfilerManager::generateGlobalAnalysis() const {
    std::ostringstream oss;
    oss << "Global Performance Analysis\n";
    oss << "==========================\n\n";
    
    oss << "Active Profilers: " << pImpl->profilers.size() << "\n";
    
    for (const auto& pair : pImpl->profilers) {
        oss << "\nProfiler: " << pair.first << "\n";
        if (pair.second) {
            ProfileResult result = pair.second->generateProfileResult();
            oss << result.analysisReport;
        }
    }
    
    return oss.str();
}

std::string ProfilerManager::identifyGlobalBottlenecks() const {
    std::ostringstream oss;
    oss << "Global Bottleneck Analysis\n";
    oss << "=========================\n\n";
    
    // Collect bottlenecks from all profilers
    std::vector<std::pair<std::string, double>> allBottlenecks;
    
    for (const auto& pair : pImpl->profilers) {
        if (pair.second) {
            ProfileResult result = pair.second->generateProfileResult();
            for (const auto& bottleneck : result.bottleneckAnalysis) {
                allBottlenecks.push_back({pair.first + ": " + bottleneck.first, bottleneck.second});
            }
        }
    }
    
    // Sort by severity
    std::sort(allBottlenecks.begin(), allBottlenecks.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    oss << "Top Global Bottlenecks:\n";
    for (size_t i = 0; i < std::min<size_t>(10, allBottlenecks.size()); ++i) {
        oss << "  " << (i + 1) << ". " << allBottlenecks[i].first
            << " (" << allBottlenecks[i].second << " s avg)\n";
    }
    
    return oss.str();
}

void ProfilerManager::setupMainCppHooks() {
    // Setup hooks for main.cpp integration
    // This would integrate with the existing Logger, StatisticsCollector, etc.
}

void ProfilerManager::setupPerformanceMonitoringIntegration() {
    // Setup integration for performance monitoring
    // This would coordinate all performance monitoring components
}

} // namespace nlm
