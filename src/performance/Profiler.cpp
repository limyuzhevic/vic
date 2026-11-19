#include "Profiler.hpp"

namespace nlm {

Profiler& Profiler::getInstance() {
    static Profiler instance;
    return instance;
}

void Profiler::start(const std::string& name) {
    timers[name] = Timer();
}

void Profiler::stop(const std::string& name) {
    if (timers.find(name) != timers.end()) {
        double elapsed = timers[name].elapsed();
        timers.erase(name);
        profiles[name].addSample(elapsed);
    }
}

const Profiler::ProfileData& Profiler::getProfile(const std::string& name) const {
    static ProfileData empty(".");
    auto it = profiles.find(name);
    return it != profiles.end() ? it->second : empty;
}

std::vector<Profiler::ProfileData> Profiler::getAllProfiles() const {
    std::vector<ProfileData> result;
    result.reserve(profiles.size());
    for (const auto& pair : profiles) {
        result.push_back(pair.second);
    }
    return result;
}

void Profiler::clear() {
    profiles.clear();
    timers.clear();
}

void Profiler::printSummary() const {
    std::cout << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                          NLM Performance Profiler                            ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ " << std::left << std::setw(30) << "Operation" 
              << " ║ " << std::right << std::setw(10) << "Calls" 
              << " ║ " << std::right << std::setw(12) << "Total Time" 
              << " ║ " << std::right << std::setw(12) << "Avg Time" 
              << " ║ " << std::right << std::setw(12) << "Min Time" 
              << " ║ " << std::right << std::setw(12) << "Max Time" 
              << " ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    
    for (const auto& pair : profiles) {
        const auto& data = pair.second;
        std::cout << "║ " << std::left << std::setw(30) << data.name 
                  << " ║ " << std::right << std::setw(10) << data.callCount
                  << " ║ " << std::right << std::setw(12) << std::fixed << std::setprecision(4) << data.totalTime
                  << " ║ " << std::right << std::setw(12) << std::fixed << std::setprecision(4) << data.avgTime
                  << " ║ " << std::right << std::setw(12) << std::fixed << std::setprecision(4) << data.minTime
                  << " ║ " << std::right << std::setw(12) << std::fixed << std::setprecision(4) << data.maxTime
                  << " ║" << std::endl;
    }
    
    std::cout << "╠════════════════════════════════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ Total Time: " << std::fixed << std::setprecision(4) << getTotalTime() 
              << " s | " << profiles.size() << " operations profiled              ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
}

std::string Profiler::getSummaryString() const {
    std::ostringstream oss;
    oss << "Performance Summary (Total Time: " << std::fixed << std::setprecision(4) << getTotalTime() 
        << " s, " << profiles.size() << " operations):" << std::endl;
    
    for (const auto& pair : profiles) {
        oss << "  " << pair.second.toString() << std::endl;
    }
    
    return oss.str();
}

} // namespace nlm
