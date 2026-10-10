// ConfigHelp.cpp implementation
#include "ConfigHelp.hpp"
#include <algorithm>

namespace nlm {

// ConfigHelp implementation details
namespace {
    // Helper function to get profile name string
    const char* profileToString(ConfigProfile profile) {
        switch (profile) {
            case ConfigProfile::Research: return "Research";
            case ConfigProfile::Simulation: return "Simulation";
            case ConfigProfile::Production: return "Production";
            case ConfigProfile::Development: return "Development";
            case ConfigProfile::Benchmark: return "Benchmark";
            default: return "None";
        }
    }
    
    // Helper function to get category description
    std::string getCategoryDescription(const std::string& category) {
        if (category == "General") {
            return "General configuration parameters";
        } else if (category == "Neural") {
            return "Neural network parameters";
        } else if (category == "Plasticity") {
            return "Learning and adaptation settings";
        } else if (category == "Development") {
            return "Growth and maturation settings";
        } else if (category == "Neuromodulation") {
            return "Reward and motivation settings";
        } else if (category == "Environment") {
            return "World interaction settings";
        } else if (category == "Logging") {
            return "Output and debugging settings";
        } else if (category == "Visualization") {
            return "Display and rendering settings";
        } else {
            return "Custom configuration parameters";
        }
    }
}

} // namespace nlm