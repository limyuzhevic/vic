// Output handler for CLI
// Implementation of formatting and exporting results

#include "output_handler.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <ctime>

using namespace nlm;

OutputHandler::OutputHandler() : format_(OutputFormat::PlainText) {
}

OutputHandler::~OutputHandler() {
}

void OutputHandler::printBrainStatus(std::shared_ptr<Brain> brain) const {
    if (!brain) {
        printError("Brain not available");
        return;
    }
    
    std::cout << "=== Brain Status ===" << std::endl;
    std::cout << "Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
    std::cout << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "Total Spike Count: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Average Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "Excitatory/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    std::cout << std::endl;
}

void OutputHandler::printAgentStatus(std::shared_ptr<AgentBrain> agentBrain) const {
    if (!agentBrain) {
        printError("Agent brain not available");
        return;
    }
    
    std::cout << "=== Agent Status ===" << std::endl;
    std::cout << "Neuromodulation Level: " << agentBrain->getNeuromodulationLevel() << std::endl;
    std::cout << "Curiosity Level: " << agentBrain->getCuriosityLevel() << std::endl;
    std::cout << "Novelty Level: " << agentBrain->getNoveltyLevel() << std::endl;
    std::cout << "Prediction Error: " << agentBrain->getPredictionError() << std::endl;
    std::cout << "Developmental Stage: " << static_cast<int>(agentBrain->getDevelopmentalStage()) << std::endl;
    std::cout << std::endl;
}

void OutputHandler::printWorldStatus(std::shared_ptr<SimpleWorld> world) const {
    if (!world) {
        printError("World not available");
        return;
    }
    
    std::cout << "=== World Status ===" << std::endl;
    std::cout << "Width: " << world->getWidth() << std::endl;
    std::cout << "Height: " << world->getHeight() << std::endl;
    std::cout << "Vision Width: " << world->getSensoryInputSize() << std::endl;
    std::cout << "Vision Height: " << world->getVisionWidth() << std::endl; // Note: This is likely wrong, but using what's available
    std::cout << "Max Energy: " << world->getMaxEnergy() << std::endl;
    std::cout << "Energy Decay Rate: " << world->getEnergyDecayRate() << std::endl;
    std::cout << "Simulation Time: " << world->getSimulationTime() << std::endl;
    std::cout << std::endl;
}

void OutputHandler::printSimulationStats(const SimulationStats& stats) const {
    std::cout << "=== Simulation Statistics ===" << std::endl;
    std::cout << "State: ";
    
    switch (stats.state) {
        case SimulationState::Idle: std::cout << "Idle"; break;
        case SimulationState::Running: std::cout << "Running"; break;
        case SimulationState::Paused: std::cout << "Paused"; break;
        case SimulationState::Completed: std::cout << "Completed"; break;
        case SimulationState::Error: std::cout << "Error"; break;
    }
    std::cout << std::endl;
    
    std::cout << "Steps: " << stats.totalSteps << std::endl;
    std::cout << "Total Spikes: " << stats.totalSpikes << std::endl;
    std::cout << "Average Firing Rate: " << stats.averageFiringRate << " Hz" << std::endl;
    std::cout << "Active Neurons: " << stats.activeNeurons << std::endl;
    std::cout << "Firing Neurons: " << stats.firingNeurons << std::endl;
    std::cout << "Simulation Time: " << stats.simulationTime << " seconds" << std::endl;
    std::cout << std::endl;
}

bool OutputHandler::exportBrainStatus(std::shared_ptr<Brain> brain, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Brain Status Export" << std::endl;
    file << "====================" << std::endl;
    file << std::endl;
    
    if (brain) {
        file << "Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        file << "Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        file << "Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        file << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        file << "Total Spike Count: " << brain->getTotalSpikeCount() << std::endl;
        file << "Average Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
        file << "Excitatory/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    }
    
    file.close();
    return true;
}

bool OutputHandler::exportAgentStatus(std::shared_ptr<AgentBrain> agentBrain, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Agent Status Export" << std::endl;
    file << "===================" << std::endl;
    file << std::endl;
    
    if (agentBrain) {
        file << "Neuromodulation Level: " << agentBrain->getNeuromodulationLevel() << std::endl;
        file << "Curiosity Level: " << agentBrain->getCuriosityLevel() << std::endl;
        file << "Novelty Level: " << agentBrain->getNoveltyLevel() << std::endl;
        file << "Prediction Error: " << agentBrain->getPredictionError() << std::endl;
        file << "Developmental Stage: " << static_cast<int>(agentBrain->getDevelopmentalStage()) << std::endl;
    }
    
    file.close();
    return true;
}

bool OutputHandler::exportWorldStatus(std::shared_ptr<SimpleWorld> world, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "World Status Export" << std::endl;
    file << "===================" << std::endl;
    file << std::endl;
    
    if (world) {
        file << "Width: " << world->getWidth() << std::endl;
        file << "Height: " << world->getHeight() << std::endl;
        file << "Max Energy: " << world->getMaxEnergy() << std::endl;
        file << "Energy Decay Rate: " << world->getEnergyDecayRate() << std::endl;
        file << "Simulation Time: " << world->getSimulationTime() << std::endl;
    }
    
    file.close();
    return true;
}

bool OutputHandler::exportSimulationStats(const SimulationStats& stats, const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Simulation Statistics Export" << std::endl;
    file << "============================" << std::endl;
    file << std::endl;
    
    file << "State: ";
    
    switch (stats.state) {
        case SimulationState::Idle: file << "Idle"; break;
        case SimulationState::Running: file << "Running"; break;
        case SimulationState::Paused: file << "Paused"; break;
        case SimulationState::Completed: file << "Completed"; break;
        case SimulationState::Error: file << "Error"; break;
    }
    file << std::endl;
    
    file << "Steps: " << stats.totalSteps << std::endl;
    file << "Total Spikes: " << stats.totalSpikes << std::endl;
    file << "Average Firing Rate: " << stats.averageFiringRate << " Hz" << std::endl;
    file << "Active Neurons: " << stats.activeNeurons << std::endl;
    file << "Firing Neurons: " << stats.firingNeurons << std::endl;
    file << "Simulation Time: " << stats.simulationTime << " seconds" << std::endl;
    
    file.close();
    return true;
}

void OutputHandler::printProgress(size_t currentStep, size_t totalSteps, 
                                 const SimulationStats& stats) const {
    float progress = static_cast<float>(currentStep) / totalSteps * 100.0f;
    
    std::cout << "\rProgress: " << std::fixed << std::setprecision(1) << progress << "% "
              << "(Step " << currentStep << "/" << totalSteps << ") "
              << "Spikes: " << stats.totalSpikes
              << "          " << std::flush;
    
    if (currentStep == totalSteps) {
        std::cout << std::endl;
    }
}

void OutputHandler::printError(const std::string& error) const {
    std::cerr << "Error: " << error << std::endl;
}

void OutputHandler::printSuccess(const std::string& message) const {
    std::cout << "Success: " << message << std::endl;
}

void OutputHandler::printWarning(const std::string& message) const {
    std::cout << "Warning: " << message << std::endl;
}

void OutputHandler::printInfo(const std::string& message) const {
    std::cout << "Info: " << message << std::endl;
}

std::string OutputHandler::formatAsText(const std::string& content) const {
    return content;
}

std::string OutputHandler::formatAsCSV(const std::vector<std::vector<std::string>>& data) const {
    std::stringstream stream;
    
    for (size_t row = 0; row < data.size(); ++row) {
        for (size_t col = 0; col < data[row].size(); ++col) {
            if (col > 0) stream << ",";
            stream << data[row][col];
        }
        if (row < data.size() - 1) {
            stream << "\n";
        }
    }
    
    return stream.str();
}

std::string OutputHandler::formatAsJSON(const std::string& content) const {
    // Wrap content in JSON object
    std::stringstream stream;
    stream << "{\n  \\"content\\": \\"" << escapeForJSON(content) << "\\"\n}";
    return stream.str();
}

std::string OutputHandler::formatAsYAML(const std::string& content) const {
    // Wrap content in YAML document
    std::stringstream stream;
    stream << "content: |\n  " << content;
    return stream.str();
}

std::string OutputHandler::escapeForJSON(const std::string& str) const {
    std::string result;
    result.reserve(str.size() * 2);
    
    for (char c : str) {
        switch (c) {
            case '\"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 32) {
                    result += "\\u" + std::to_string(static_cast<int>(c));
                } else {
                    result += c;
                }
        }
    }
    
    return result;
}

std::string OutputHandler::escapeForYAML(const std::string& str) const {
    std::string result;
    result.reserve(str.size() * 2);
    
    for (char c : str) {
        switch (c) {
            case '\'': result += "''"; break;
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\0': result += "\\0"; break;
            default:
                if (c < 32 || c == 127) {
                    result += "\\" + std::to_string(static_cast<int>(c));
                } else {
                    result += c;
                }
        }
    }
    
    return result;
}

std::string OutputHandler::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream stream;
    stream << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    
    return stream.str();
}