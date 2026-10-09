#include "Profiler.hpp"

namespace nlm {

Profiler::Profiler() : pImpl(std::make_unique<Impl>()) {}

void Profiler::startProfile(const std::string& name) {
    ProfileRecord record;
    record.name = name;
    record.startTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9; // nanoseconds to seconds
    pImpl->activeProfiles.push_back(record);
}

void Profiler::stopProfile(const std::string& name) {
    if (pImpl->activeProfiles.empty()) return;
    
    double endTime = std::chrono::high_resolution_clock::now().time_since_epoch().count() / 1e9;
    
    // Find and remove the most recent matching profile
    for (auto it = pImpl->activeProfiles.rbegin(); it != pImpl->activeProfiles.rend(); ++it) {
        if (it->name == name) {
            double elapsed = endTime - it->startTime;
            
            // Update profile data
            auto& data = pImpl->profileData[name];
            data.name = name;
            data.totalTime += elapsed;
            data.minTime = (data.minTime == 0 || elapsed < data.minTime) ? elapsed : data.minTime;
            data.maxTime = (elapsed > data.maxTime) ? elapsed : data.maxTime;
            data.callCount++;
            data.averageTime = data.totalTime / data.callCount;
            
            // Notify callbacks
            for (auto& callback : pImpl->callbacks) {
                callback(data);
            }
            
            // Remove this profile record
            auto revIt = std::make_reverse_iterator(it);
            pImpl->activeProfiles.erase(std::next(revIt).base());
            return;
        }
    }
}

const Profiler::ProfileData* Profiler::getProfile(const std::string& name) const {
    auto it = pImpl->profileData.find(name);
    return it != pImpl->profileData.end() ? &it->second : nullptr;
}

const std::map<std::string, Profiler::ProfileData>& Profiler::getAllProfiles() const {
    return pImpl->profileData;
}

void Profiler::clear() {
    pImpl->activeProfiles.clear();
    pImpl->profileData.clear();
}

double Profiler::getTotalTime() const {
    double total = 0.0;
    for (const auto& pair : pImpl->profileData) {
        total += pair.second.totalTime;
    }
    return total;
}

size_t Profiler::getProfileCount() const {
    return pImpl->profileData.size();
}

std::string Profiler::exportProfileData() const {
    std::ostringstream oss;
    oss << "NLM Performance Profile Data" << std::endl;
    oss << "Generated: " << std::chrono::high_resolution_clock::now().time_since_epoch().count() << std::endl;
    oss << std::endl;
    
    oss << "Profile Statistics:" << std::endl;
    oss << "  Total profiles: " << getProfileCount() << std::endl;
    oss << "  Total execution time: " << getTotalTime() << "s" << std::endl;
    oss << std::endl;
    
    oss << "Detailed Profiles:" << std::endl;
    for (const auto& pair : pImpl->profileData) {
        const auto& data = pair.second;
        oss << "  " << data.name << ":" << std::endl;
        oss << "    Calls: " << data.callCount << std::endl;
        oss << "    Total time: " << data.totalTime << "s" << std::endl;
        oss << "    Average time: " << data.averageTime << "s" << std::endl;
        oss << "    Min time: " << data.minTime << "s" << std::endl;
        oss << "    Max time: " << data.maxTime << "s" << std::endl;
        oss << std::endl;
    }
    
    return oss.str();
}

void Profiler::exportProfileToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << exportProfileData();
        file.close();
    }
}

void Profiler::registerCallback(ProfileCallback callback) {
    pImpl->callbacks.push_back(callback);
}

std::vector<std::string> Profiler::getActiveProfiles() const {
    std::vector<std::string> names;
    for (const auto& record : pImpl->activeProfiles) {
        names.push_back(record.name);
    }
    return names;
}

void Profiler::merge(const Profiler& other) {
    for (const auto& pair : other.pImpl->profileData) {
        auto& thisData = pImpl->profileData[pair.first];
        
        // Merge profile data
        thisData.name = pair.second.name;
        thisData.totalTime += pair.second.totalTime;
        thisData.minTime = std::min(thisData.minTime, pair.second.minTime);
        thisData.maxTime = std::max(thisData.maxTime, pair.second.maxTime);
        thisData.callCount += pair.second.callCount;
        thisData.averageTime = thisData.totalTime / thisData.callCount;
    }
}

void Profiler::resetSession() {
    clear();
}

} // namespace nlm
