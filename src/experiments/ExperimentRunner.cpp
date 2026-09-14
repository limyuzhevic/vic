// Implement the missing saveResults functionality in Experiment.cpp
#include "Experiment.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

bool ExperimentRunner::saveResults(const std::string& filepath) const {
    std::ofstream file(filepath);
    
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for writing: " + filepath);
        return false;
    }
    
    file << "NLM Experiment Results" << std::endl;
    file << "=======================" << std::endl;
    file << std::endl;
    
    file << "Global Statistics:" << std::endl;
    file << "  Total Experiments: " << pImpl->experiments.size() << std::endl;
    file << "  Global Seed: " << pImpl->globalSeed << std::endl;
    file << std::endl;
    
    file << "Individual Experiments:" << std::endl;
    for (size_t i = 0; i < pImpl->experiments.size(); ++i) {
        const auto& exp = pImpl->experiments[i];
        
        file << "  Experiment " << i << ": " << exp->getName() << std::endl;
        file << "    Seed: " << exp->getSeed() << std::endl;
        file << "    Start Step: " << exp->getStartStep() << std::endl;
        file << "    End Step: " << exp->getEndStep() << std::endl;
        file << "    Step Count: " << exp->getStepData().size() << std::endl;
        file << "    Metrics Count: " << exp->getMetrics().size() << std::endl;
        
        // Write metrics
        if (!exp->getMetrics().empty()) {
            file << "    Metrics:" << std::endl;
            for (const auto& metric : exp->getMetrics()) {
                file << "      " << metric.first << ": " << metric.second << std::endl;
            }
        }
        
        file << "    Notes: " << exp->getNotes().size() << std::endl;
        for (size_t j = 0; j < exp->getNotes().size() && j < 3; ++j) {
            file << "      - " << exp->getNotes()[j] << std::endl;
        }
        if (exp->getNotes().size() > 3) {
            file << "      ... and " << (exp->getNotes().size() - 3) << " more notes" << std::endl;
        }
        
        file << "    Step Data Sample:" << std::endl;
        const auto& stepData = exp->getStepData();
        if (!stepData.empty()) {
            // Show first and last few steps
            size_t sampleSize = std::min<size_t>(3, stepData.size());
            
            for (size_t j = 0; j < sampleSize; ++j) {
                const auto& step = stepData[j];
                file << "      Step " << step.step 
                     << ": neurons=" << step.neuronCount
                     << ", synapses=" << step.synapseCount
                     << ", firing=" << step.firingCount
                     << ", rate=" << std::fixed << std::setprecision(2) << step.avgFiringRate << std::endl;
            }
            
            if (stepData.size() > sampleSize * 2) {
                file << "      ... " << (stepData.size() - sampleSize * 2) << " more steps" << std::endl;
            }
            
            if (stepData.size() > 0) {
                const auto& lastStep = stepData.back();
                file << "      Last Step " << lastStep.step 
                     << ": neurons=" << lastStep.neuronCount
                     << ", synapses=" << lastStep.synapseCount
                     << ", firing=" << lastStep.firingCount
                     << ", rate=" << std::fixed << std::setprecision(2) << lastStep.avgFiringRate << std::endl;
            }
        }
        
        file << "" << std::endl;
    }
    
    // Add summary statistics
    file << "Summary:" << std::endl;
    file << "  Total Spikes Across All Experiments: " << calculateTotalSpikes() << std::endl;
    file << "  Average Neurons per Experiment: " << calculateAverageNeuronCount() << std::endl;
    file << "  Average Synapses per Experiment: " << calculateAverageSynapseCount() << std::endl;
    file << "  Experiments with Learning: " << countExperimentsWithLearning() << std::endl;
    file << std::endl;
    
    // Add timestamps
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    file << "Generated: " << std::ctime(&time);
    
    NLM_LOG_INFO("Experiment results saved to: " + filepath);
    return true;
}

// Helper functions for saveResults
uint64_t ExperimentRunner::calculateTotalSpikes() const {
    uint64_t total = 0;
    for (const auto& exp : pImpl->experiments) {
        for (const auto& step : exp->getStepData()) {
            total += step.firingCount;
        }
    }
    return total;
}

double ExperimentRunner::calculateAverageNeuronCount() const {
    if (pImpl->experiments.empty()) return 0.0;
    
    double total = 0;
    for (const auto& exp : pImpl->experiments) {
        for (const auto& step : exp->getStepData()) {
            total += step.neuronCount;
        }
    }
    
    return total / pImpl->experiments.size() / pImpl->experiments[0]->getStepData().size();
}

double ExperimentRunner::calculateAverageSynapseCount() const {
    if (pImpl->experiments.empty()) return 0.0;
    
    double total = 0;
    for (const auto& exp : pImpl->experiments) {
        for (const auto& step : exp->getStepData()) {
            total += step.synapseCount;
        }
    }
    
    return total / pImpl->experiments.size() / pImpl->experiments[0]->getStepData().size();
}

int ExperimentRunner::countExperimentsWithLearning() const {
    int count = 0;
    for (const auto& exp : pImpl->experiments) {
        // Simple heuristic: if there's significant weight change or spike activity
        bool hasLearning = false;
        for (const auto& metric : exp->getMetrics()) {
            if (metric.first.find("weight_change") != std::string::npos && metric.second > 0.001) {
                hasLearning = true;
                break;
            }
            if (metric.first.find("spike") != std::string::npos && metric.second > 100) {
                hasLearning = true;
                break;
            }
        }
        
        if (hasLearning) count++;
    }
    return count;
}

} // namespace nlm
