// src/cli/SimulationController.cpp
#include "SimulationController.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>

namespace nlm {

struct SimulationController::Impl {
    bool running;
    bool paused;
    double speed;
    double timestep;
    SimulationStep currentStep;
    Timestamp currentTime;
    std::shared_ptr<Brain> brain;
    
    std::vector<std::string> recordedData;
    bool recording;
    std::string recordingFormat;
    std::string outputPath;
    
    std::function<void()> stepCallback;
    std::function<void()> pauseCallback;
    std::function<void()> resumeCallback;
    std::function<void()> completeCallback;
    
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point lastStepTime;
    
    Impl() : running(false)
           , paused(false)
           , speed(1.0)
           , timestep(0.001)
           , currentStep(0)
           , currentTime(0.0)
           , recording(false)
           , stepCallback(nullptr)
           , pauseCallback(nullptr)
           , resumeCallback(nullptr)
           , completeCallback(nullptr) {
    }
};

SimulationController::SimulationController() : pImpl(std::make_unique<Impl>()) {
}

SimulationController::~SimulationController() = default;

bool SimulationController::startSimulation(double timestep, std::shared_ptr<Brain> brain) {
    if (running) {
        NLM_LOG_WARNING("Simulation is already running");
        return false;
    }
    
    if (!brain) {
        NLM_LOG_ERROR("Brain is null");
        return false;
    }
    
    pImpl->timestep = timestep;
    pImpl->brain = brain;
    pImpl->currentStep = 0;
    pImpl->currentTime = 0.0;
    pImpl->paused = false;
    pImpl->running = true;
    pImpl->startTime = std::chrono::high_resolution_clock::now();
    pImpl->lastStepTime = pImpl->startTime;
    pImpl->recordedData.clear();
    pImpl->recording = false;
    
    NLM_LOG_INFO("Simulation started with timestep: " + std::to_string(timestep));
    return true;
}

bool SimulationController::pauseSimulation() {
    if (!running) {
        NLM_LOG_WARNING("Simulation is not running");
        return false;
    }
    
    if (paused) {
        NLM_LOG_WARNING("Simulation is already paused");
        return false;
    }
    
    pImpl->paused = true;
    NLM_LOG_INFO("Simulation paused");
    
    if (pImpl->pauseCallback) {
        pImpl->pauseCallback();
    }
    
    return true;
}

bool SimulationController::resumeSimulation() {
    if (!running) {
        NLM_LOG_WARNING("Simulation is not running");
        return false;
    }
    
    if (!paused) {
        NLM_LOG_WARNING("Simulation is not paused");
        return false;
    }
    
    pImpl->paused = false;
    NLM_LOG_INFO("Simulation resumed");
    
    if (pImpl->resumeCallback) {
        pImpl->resumeCallback();
    }
    
    return true;
}

bool SimulationController::stepSimulation() {
    if (!running) {
        NLM_LOG_WARNING("Simulation is not running");
        return false;
    }
    
    if (paused) {
        // Execute one step when paused
        return executeStep();
    }
    
    // Check if it's time to execute a step based on speed
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration<double>(currentTime - pImpl->lastStepTime).count();
    
    if (elapsed >= (pImpl->timestep * pImpl->speed)) {
        return executeStep();n    }
    
    return true;
}

bool SimulationController::setSimulationSpeed(double speed) {
    if (speed <= 0.0) {
        NLM_LOG_ERROR("Simulation speed must be positive");
        return false;
    }
    
    pImpl->speed = speed;
    NLM_LOG_INFO("Simulation speed set to: " + std::to_string(speed) + "x");
    return true;
}

double SimulationController::getSimulationSpeed() const {
    return pImpl->speed;
}

bool SimulationController::isSimulationRunning() const {
    return pImpl->running;
}

bool SimulationController::isSimulationPaused() const {
    return pImpl->paused;
}

SimulationStep SimulationController::getCurrentStep() const {
    return pImpl->currentStep;
}

Timestamp SimulationController::getCurrentTime() const {
    return pImpl->currentTime;
}

bool SimulationController::startRecording(const std::string& outputPath, const std::string& format) {
    if (!running) {
        NLM_LOG_ERROR("Cannot start recording: simulation is not running");
        return false;
    }
    
    pImpl->outputPath = outputPath;
    pImpl->recordingFormat = format;
    pImpl->recording = true;
    pImpl->recordedData.clear();
    
    NLM_LOG_INFO(std::string("Recording started to: ") + outputPath + 
                std::string(" in format: ") + format);
    
    return true;
}

bool SimulationController::stopRecording() {
    if (!pImpl->recording) {
        NLM_LOG_WARNING("Recording is not active");
        return false;
    }
    
    pImpl->recording = false;
    
    // Write recorded data to file
    if (!pImpl->outputPath.empty()) {
        std::ofstream file(pImpl->outputPath);
        if (file.is_open()) {
            for (const auto& line : pImpl->recordedData) {
                file << line << std::endl;
            }
            file.close();
            NLM_LOG_INFO(std::string("Recording saved to: ") + pImpl->outputPath);
        } else {
            NLM_LOG_ERROR(std::string("Failed to save recording to: ") + pImpl->outputPath);
        }
    }
    
    return true;
}

std::vector<std::string> SimulationController::getRecordedData() const {
    return pImpl->recordedData;
}

void SimulationController::clearRecordedData() {
    pImpl->recordedData.clear();
    NLM_LOG_INFO("Recorded data cleared");
}

void SimulationController::setEventCallback(const std::string& eventType, 
                                           const std::function<void()>& callback) {
    if (eventType == "step") {
        pImpl->stepCallback = callback;
    } else if (eventType == "pause") {
        pImpl->pauseCallback = callback;
    } else if (eventType == "resume") {
        pImpl->resumeCallback = callback;
    } else if (eventType == "complete") {
        pImpl->completeCallback = callback;
    } else {
        NLM_LOG_WARNING(std::string("Unknown event type: ") + eventType);
    }
}

std::string SimulationController::getSimulationStatistics() const {
    std::stringstream stats;
    stats << "=== Simulation Statistics ===" << std::endl;
    stats << "Status: " << (running ? (paused ? "PAUSED" : "RUNNING") : "STOPPED") << std::endl;
    stats << "Current Step: " << pImpl->currentStep << std::endl;
    stats << "Current Time: " << pImpl->currentTime << " seconds" << std::endl;
    stats << "Speed: " << pImpl->speed << "x" << std::endl;
    stats << "Timestep: " << pImpl->timestep << std::endl;
    stats << "Recorded Data Points: " << pImpl->recordedData.size() << std::endl;
    
    if (pImpl->brain) {
        stats << "Brain Neurons: " << pImpl->brain->getTotalNeuronCount() << std::endl;
        stats << "Brain Synapses: " << pImpl->brain->getTotalSynapseCount() << std::endl;
        stats << "Average Firing Rate: " << pImpl->brain->getAverageFiringRate() << std::endl;
    }
    
    return stats.str();
}

void SimulationController::resetSimulation() {
    if (running) {
        NLM_LOG_INFO("Resetting simulation...");
        pImpl->currentStep = 0;
        pImpl->currentTime = 0.0;
        pImpl->paused = false;
        pImpl->recordedData.clear();
        
        if (pImpl->brain) {
            pImpl->brain->reset();
        }
    }
}

bool SimulationController::executeStep() {
    if (!pImpl->brain) {
        return false;
    }
    
    auto stepStart = std::chrono::high_resolution_clock::now();
    
    // Execute one simulation step
    pImpl->brain->step(pImpl->currentStep, pImpl->currentTime);
    
    pImpl->currentStep++;
    pImpl->currentTime += pImpl->timestep;
    pImpl->lastStepTime = std::chrono::high_resolution_clock::now();
    
    // Record step data if recording is enabled
    if (pImpl->recording) {
        std::stringstream stepData;
        stepData << "Step: " << pImpl->currentStep - 1 << " "
                << "Time: " << pImpl->currentTime << " "
                << "Firing: " << pImpl->brain->getFiringNeuronCount() << " "
                << "Spikes: " << pImpl->brain->getTotalSpikeCount();
        
        pImpl->recordedData.push_back(stepData.str());
        
        if (pImpl->recordingFormat == "csv" && !pImpl->outputPath.empty()) {
            // Write CSV data
            std::ofstream csvFile(pImpl->outputPath, std::ios::app);
            if (csvFile.is_open()) {
                csvFile << pImpl->currentStep - 1 << ","
                       << pImpl->currentTime << ","
                       << pImpl->brain->getFiringNeuronCount() << ","
                       << pImpl->brain->getTotalSpikeCount() << std::endl;
                csvFile.close();
            }
        }
    }
    
    // Execute callbacks
    if (pImpl->stepCallback) {
        pImpl->stepCallback();
    }
    
    // Check if simulation is complete (basic completion condition)
    if (pImpl->currentStep >= 10000) { // Default max steps
        pImpl->running = false;
        if (pImpl->completeCallback) {
            pImpl->completeCallback();
        }
        NLM_LOG_INFO("Simulation completed");
    }
    
    auto stepEnd = std::chrono::high_resolution_clock::now();
    auto stepDuration = std::chrono::duration<double>(stepEnd - stepStart).count();
    
    if (stepDuration > 0.001) { // Log slow steps
        NLM_LOG_INFO(std::string("Step " + std::to_string(pImpl->currentStep - 1) + 
                            " completed in " + std::to_string(stepDuration) + " seconds"));
    }
    
    return true;
}

} // namespace nlm