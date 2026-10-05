// Base Experiment Framework Implementation

#include "BaseExperimentFramework.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// ===== EXPERIMENT LOGGER IMPLEMENTATION =====

ExperimentLogger::ExperimentLogger(const std::string& filename) : isOpen(false) {
    logFile.open(filename);
    if (logFile.is_open()) {
        isOpen = true;
    }
}

void ExperimentLogger::log(const std::string& message) {
    if (isOpen) {
        logFile << message << std::endl;
    }
    std::cout << message << std::endl;
}

void ExperimentLogger::logEpisodeStart(int episode) {
    log("--- Episode " + std::to_string(episode) + " started ---");
}

void ExperimentLogger::logEpisodeEnd(int episode, float totalReward, int steps, float finalNovelty) {
    std::ostringstream oss;
    oss << "--- Episode " << episode << " ended ---\n"
        << "  Steps: " << steps << "\n"
        << "  Total Reward: " << totalReward << "\n"
        << "  Final Novelty: " << finalNovelty << "\n"
        << "-------------------------------------";
    log(oss.str());
}

void ExperimentLogger::flush() {
    if (isOpen) {
        logFile.flush();
    }
}

} // namespace nlm
