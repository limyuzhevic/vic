#include "PerformanceTools.hpp"

namespace nlm {

struct Profiler::Impl {
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> startTimes;
    std::unordered_map<std::string, std::chrono::duration<double>> durations;
    std::unordered_map<std::string, std::unordered_map<std::string, double>> profileData;
    bool profilingActive;
    
    Impl() : profilingActive(false) {}
};

Profiler::Profiler() : pImpl(std::make_unique<Impl>()) {}

Profiler::~Profiler() = default;

Profiler::Profiler(Profiler&&) noexcept = default;
Profiler& Profiler::operator=(Profiler&&) noexcept = default;

std::string Profiler::startProfile(const std::string& functionName) {
    if (!pImpl->profilingActive) {
        return ""; // Not profiling
    }
    
    std::string profileId = functionName + "_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    pImpl->startTimes[profileId] = std::chrono::steady_clock::now();
    return profileId;
}

void Profiler::stopProfile(const std::string& profileId) {
    auto it = pImpl->startTimes.find(profileId);
    if (it == pImpl->startTimes.end()) {
        return;
    }
    
    auto endTime = std::chrono::steady_clock::now();
    pImpl->durations[profileId] = endTime - it->second;
    
    // Store profile data
    double duration = pImpl->durations[profileId].count();
    pImpl->profileData[profileId]["duration"] = duration;
    pImpl->profileData[profileId]["cpu_time"] = duration;
    
    pImpl->startTimes.erase(it);
}

std::unique_ptr<Profiler> Profiler::startScopedProfile(const std::string& scopeName) {
    if (!pImpl->profilingActive) {
        return nullptr;
    }
    
    std::string profileId = startProfile(scopeName);
    if (profileId.empty()) {
        return nullptr;
    }
    
    auto scopedProfiler = std::make_unique<Profiler>();
    scopedProfiler->pImpl = std::make_unique<Impl>();
    
    // Transfer the profile ID
    scopedProfiler->pImpl->startTimes[profileId] = pImpl->startTimes[profileId];
    
    return scopedProfiler;
}

std::unordered_map<std::string, double> Profiler::getProfileData(const std::string& profileId) const {
    auto it = pImpl->profileData.find(profileId);
    if (it != pImpl->profileData.end()) {
        return it->second;
    }
    return {};
}

std::unordered_map<std::string, std::unordered_map<std::string, double>> Profiler::getAllProfileData() const {
    return pImpl->profileData;
}

void Profiler::clearProfiles() {
    pImpl->startTimes.clear();
    pImpl->durations.clear();
    pImpl->profileData.clear();
}

bool Profiler::exportProfiles(const std::string& format, const std::string& filepath) {
    // Implementation would export profile data in specified format
    return false;
}

std::unordered_map<std::string, double> Profiler::getProfilingStats() const {
    std::unordered_map<std::string, double> stats;
    
    stats["total_profiles"] = static_cast<double>(pImpl->profileData.size());
    stats["profiling_active"] = pImpl->profilingActive ? 1.0 : 0.0;
    
    if (!pImpl->profileData.empty()) {
        double totalDuration = 0.0;
        for (const auto& [profileId, data] : pImpl->profileData) {
            auto it = data.find("duration");
            if (it != data.end()) {
                totalDuration += it->second;
            }
        }
        stats["total_duration"] = totalDuration;
        stats["average_duration"] = totalDuration / pImpl->profileData.size();
    }
    
    return stats;
}

bool Profiler::isProfiling() const {
    return pImpl->profilingActive;
}

} // namespace nlm