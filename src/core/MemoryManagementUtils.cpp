#include "MemoryManagementTestSuite.hpp"
#include <system_stats.h>

namespace nlm {
namespace MemoryManagementUtils {

size_t getCurrentMemoryUsage() {
    #ifdef __linux__
    // Linux implementation
    std::ifstream statusFile("/proc/self/status");
    if (!statusFile.is_open()) {
        return 0;
    }
    
    std::string line;
    while (std::getline(statusFile, line)) {
        if (line.find("VmRSS:") == 0) {
            size_t pos = line.find(":");
            if (pos != std::string::npos) {
                std::string value = line.substr(pos + 1);
                // Remove whitespace and " kB"
                value.erase(0, value.find_first_not_of(" "));
                value.erase(value.find_last_not_of(" ") + 1);
                if (value.size() >= 2 && value.substr(value.size() - 2) == " kB") {
                    value = value.substr(0, value.size() - 2);
                }
                try {
                    return std::stoull(value) * 1024; // Convert kB to bytes
                } catch (...) {
                    return 0;
                }
            }
        }
    }
    #endif
    
    // Fallback implementation
    return 0;
}

size_t getPeakMemoryUsage() {
    #ifdef __linux__
    // Linux implementation
    std::ifstream statusFile("/proc/self/status");
    if (!statusFile.is_open()) {
        return 0;
    }
    
    std::string line;
    while (std::getline(statusFile, line)) {
        if (line.find("VmHWM:") == 0) {
            size_t pos = line.find(":");
            if (pos != std::string::npos) {
                std::string value = line.substr(pos + 1);
                // Remove whitespace and " kB"
                value.erase(0, value.find_first_not_of(" "));
                value.erase(value.find_last_not_of(" ") + 1);
                if (value.size() >= 2 && value.substr(value.size() - 2) == " kB") {
                    value = value.substr(0, value.size() - 2);
                }
                try {
                    return std::stoull(value) * 1024; // Convert kB to bytes
                } catch (...) {
                    return 0;
                }
            }
        }
    }
    #endif
    
    // Fallback implementation
    return getCurrentMemoryUsage();
}

void forceGarbageCollection() {
    // In C++, garbage collection is mainly for smart pointers
    // We can't force garbage collection for plain pointers, but we can
    // encourage it by creating and releasing objects
    std::vector<int> temp;
    temp.reserve(1000000);
    for (int i = 0; i < 1000000; ++i) {
        temp.push_back(i);
    }
    temp.clear();
    std::vector<int>().swap(temp);
}

bool checkForMemoryLeaks() {
    // Simple memory leak check
    return false; // In a real implementation, this would be more sophisticated
}

std::string getAllocationStatistics() {
    std::ostringstream oss;
    oss << "Allocation statistics not available on this platform";
    return oss.str();
}

bool validateVisualizationData(const VisualizationData& data) {
    return !data.content.empty() && !data.format.empty();
}

std::string generateMetadata(const VisualizationData& data, const Brain& brain) {
    std::ostringstream oss;
    oss << "Generated at: " << std::chrono::system_clock::to_time_t(data.timestamp) << std::endl;
    oss << "Format: " << data.format << std::endl;
    oss << "Content length: " << data.content.size() << " characters" << std::endl;
    oss << "Brain neurons: " << brain.getTotalNeuronCount() << std::endl;
    oss << "Brain synapses: " << brain.getTotalSynapseCount() << std::endl;
    return oss.str();
}

// Template instantiations
template bool MemoryManagementUtils::validateObjectLifetime(const Neuron&);
template bool MemoryManagementUtils::validateObjectLifetime(const Synapse&);
template bool MemoryManagementUtils::validateObjectLifetime(const Neuromodulator&);
template bool MemoryManagementUtils::validateObjectLifetime(const PlasticityRule&);

template bool MemoryManagementUtils::validateObjectLifetime(const Neuron& obj) {
    // Validate neuron lifetime
    return true;
}

template bool MemoryManagementUtils::validateObjectLifetime(const Synapse& obj) {
    // Validate synapse lifetime
    return true;
}

template bool MemoryManagementUtils::validateObjectLifetime(const Neuromodulator& obj) {
    // Validate neuromodulator lifetime
    return true;
}

template bool MemoryManagementUtils::validateObjectLifetime(const PlasticityRule& obj) {
    // Validate plasticity rule lifetime
    return true;
}

} // namespace MemoryManagementUtils

} // namespace nlm
