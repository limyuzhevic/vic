#include "Phase3Experiment.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

namespace nlm {

struct ExperimentLogger::Impl {
    std::ofstream file;
    bool isOpen;
    
    Impl(const std::string& filename) : isOpen(false) {
        file.open(filename);
        if (file.is_open()) {
            isOpen = true;
        }
    }
    
    ~Impl() {
        if (file.is_open()) {
            file.close();
        }
    }
};

ExperimentLogger::ExperimentLogger(const std::string& filename) 
    : pImpl(new Impl(filename)) {}

ExperimentLogger::~ExperimentLogger() {
    delete pImpl;
}

void ExperimentLogger::log(const std::string& message) {
    if (pImpl->isOpen) {
        pImpl->file << message << std::endl;
    }
    std::cout << message << std::endl;
}

void ExperimentLogger::logPhase3Header() {
    std::string header = R"(
========================================
NLM PHASE 3 - WORLD INTERACTION DEMO
========================================
)";
    log(header);
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

void ExperimentLogger::logStep(int step, float reward, float neuromod, float curiosity, 
                               float energy, const std::string& action) {
    std::ostringstream oss;
    oss << "Step " << step << ": reward=" << reward 
        << ", neuromod=" << neuromod << ", curiosity=" << curiosity
        << ", energy=" << energy << ", action=" << action;
    log(oss.str());
}

void ExperimentLogger::logDevelopment(float age, const std::string& stage) {
    log("Development: age=" + std::to_string(age) + ", stage=" + stage);
}

void ExperimentLogger::logSynapticStats(size_t totalSynapses, float avgWeight, float maxWeight) {
    std::ostringstream oss;
    oss << "Synapses: " << totalSynapses << ", avgWeight=" << avgWeight 
        << ", maxWeight=" << maxWeight;
    log(oss.str());
}

void ExperimentLogger::flush() {
    if (pImpl->isOpen) {
        pImpl->file.flush();
    }
}

ExperimentMetrics::ExperimentMetrics() {
    reset();
}

void ExperimentMetrics::reset() {
    episode = 0;
    steps = 0;
    totalReward = 0.0f;
    finalEnergy = 0.0f;
    finalNovelty = 0.0f;
    finalCuriosity = 0.0f;
    predictionError = 0.0f;
    developmentalAge = 0.0f;
    developmentalStage = "Initial";
    totalSynapses = 0;
    averageWeight = 0.0f;
    maxWeight = 0.0f;
    totalSpikes = 0;
    averageFiringRate = 0.0f;
    forwardCount = 0;
    backwardCount = 0;
    turnLeftCount = 0;
    turnRightCount = 0;
    interactCount = 0;
    waitCount = 0;
}

std::string ExperimentMetrics::toCSV() const {
    std::ostringstream oss;
    oss << episode << ","
        << steps << ","
        << totalReward << ","
        << finalEnergy << ","
        << finalNovelty << ","
        << finalCuriosity << ","
        << predictionError << ","
        << developmentalAge << ","
        << developmentalStage << ","
        << totalSynapses << ","
        << averageWeight << ","
        << maxWeight << ","
        << totalSpikes << ","
        << averageFiringRate << ","
        << forwardCount << ","
        << backwardCount << ","
        << turnLeftCount << ","
        << turnRightCount << ","
        << interactCount << ","
        << waitCount;
    return oss.str();
}

} // namespace nlm
