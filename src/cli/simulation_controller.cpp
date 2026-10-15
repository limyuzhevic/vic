// Simulation controller for CLI
// Implementation of high-level simulation management

#include "simulation_controller.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <algorithm>

using namespace nlm;

SimulationController::SimulationController(std::shared_ptr<Config> config) 
    : config_(config), verboseMode_(false), simulationRunning_(false) {
}

SimulationController::~SimulationController() {
    if (simulationRunning_ && simulationThread_.joinable()) {
        simulationRunning_ = false;
        simulationThread_.join();
    }
}

CommandResult SimulationController::runSteps(size_t steps, bool withAgent,
                                          const std::string& brainConfig,
                                          const std::string& worldConfig) {
    try {
        // Initialize simulation if not already initialized
        if (!brain_) {
            if (!initializeBrain(brainConfig)) {
                return CommandResult(1, "Failed to initialize brain", false, "Brain initialization failed");
            }
        }
        
        if (withAgent && !agentBrain_) {
            if (!initializeAgentBrain(brainConfig)) {
                return CommandResult(1, "Failed to initialize agent brain", false, "Agent brain initialization failed");
            }
        }
        
        if (withAgent && !world_) {
            if (!initializeWorld(worldConfig)) {
                return CommandResult(1, "Failed to initialize world", false, "World initialization failed");
            }
        }
        
        // Reset simulation statistics
        stats_.totalSteps = steps;
        stats_.totalSpikes = 0;
        stats_.averageFiringRate = 0.0f;
        stats_.activeNeurons = 0;
        stats_.firingNeurons = 0;
        stats_.simulationTime = 0.0;
        stats_.state = SimulationState::Running;
        
        startTime_ = std::chrono::steady_clock::now();
        
        // Run simulation loop
        CommandResult result = runSimulationLoop(steps, withAgent);
        
        // Calculate final statistics
        if (stats_.totalSteps > 0) {
            stats_.averageFiringRate = static_cast<float>(stats_.totalSpikes) / stats_.totalSteps;
        }
        
        stats_.state = SimulationState::Completed;
        
        if (verboseMode_) {
            printStatus();
        }
        
        return result;
        
    } catch (const std::exception& e) {
        stats_.state = SimulationState::Error;
        return CommandResult(1, std::string("Error during simulation: ") + e.what(), false, e.what());
    }
}

CommandResult SimulationController::step(size_t steps) {
    if (!brain_) {
        return CommandResult(1, "Brain not initialized", false, "Please initialize brain first");
    }
    
    // Run specified number of steps
    startTime_ = std::chrono::steady_clock::now();
    
    for (size_t step = 0; step < steps; ++step) {
        if (interruptRequested) {
            stats_.state = SimulationState::Paused;
            return CommandResult(0, "Simulation paused by user request");
        }
        
        brain_->step(static_cast<SimulationStep>(step), static_cast<Timestamp>(step * 0.001));
        
        // Update statistics
        stats_.totalSpikes += brain_->getTotalSpikeCount();
        stats_.activeNeurons = brain_->getTotalNeuronCount();
        stats_.firingNeurons = brain_->getFiringNeuronCount();
        
        if (verboseMode_) {
            std::cout << "Step " << step << ": " << brain_->getFiringNeuronCount() 
                      << " firing neurons" << std::endl;
        }
    }
    
    auto endTime = std::chrono::steady_clock::now();
    stats_.simulationTime = std::chrono::duration<double>(endTime - startTime_).count();
    
    return CommandResult(0, "Completed " + std::to_string(steps) + " steps");
}

CommandResult SimulationController::runEpisode(size_t steps) {
    if (!agentBrain_ || !world_) {
        return CommandResult(1, "Agent or world not initialized", false, "Please initialize agent brain and world first");
    }
    
    std::stringstream output;
    output << "Starting agent episode with " << steps << " steps..." << std::endl;
    
    // Reset episode state
    agentBrain_->reset();
    world_->reset();
    
    // Run episode
    for (size_t step = 0; step < steps; ++step) {
        if (interruptRequested) {
            output << "Episode interrupted by user request" << std::endl;
            break;
        }
        
        // Process sensory input
        const SensoryPercept& percept = world_->getSensoryPercept();
        agentBrain_->processSensoryInput(percept);
        
        // Apply motor command
        MotorCommand motorCmd = agentBrain_->decodeMotorCommand();
        ActionResult result = world_->applyMotorCommand(motorCmd, world_->getSimulationTime());
        
        // Update world
        world_->update(0.001);
        
        // Log progress
        if (verboseMode_ || step % 10 == 0) {
            output << "Episode " << step << ": ";
            output << "Spikes: " << agentBrain_->getBrain()->getTotalSpikeCount();
            output << ", Dopamine: " << agentBrain_->getNeuromodulationLevel();
            output << ", Curiosity: " << agentBrain_->getCuriosityLevel();
            output << std::endl;
        }
        
        // Update statistics
        stats_.totalSteps += step;
        stats_.totalSpikes += agentBrain_->getBrain()->getTotalSpikeCount();
    }
    
    output << "Episode completed successfully" << std::endl;
    return CommandResult(0, output.str());
}

CommandResult SimulationController::train(size_t episodes) {
    if (!agentBrain_ || !world_) {
        return CommandResult(1, "Agent or world not initialized", false, "Please initialize agent brain and world first");
    }
    
    std::stringstream output;
    output << "Starting agent training for " << episodes << " episodes..." << std::endl;
    
    for (size_t episode = 0; episode < episodes; ++episode) {
        if (interruptRequested) {
            output << "Training interrupted by user request" << std::endl;
            break;
        }
        
        output << "Episode " << episode << ": ";
        
        // Run episode
        CommandResult episodeResult = runEpisode(100); // Default 100 steps per episode
        
        if (!episodeResult.success) {
            output << "Failed: " << episodeResult.errorMessage << std::endl;
            return episodeResult;
        }
        
        output << "Completed" << std::endl;
    }
    
    output << "Training completed successfully" << std::endl;
    return CommandResult(0, output.str());
}

CommandResult SimulationController::reset() {
    if (brain_) {
        brain_->reset();
        if (brain_->initialize()) {
            return CommandResult(0, "Brain reset and re-initialized successfully");
        } else {
            return CommandResult(1, "Failed to re-initialize brain after reset", false, "Brain initialization failed");
        }
    }
    return CommandResult(0, "No brain to reset");
}

CommandResult SimulationController::saveBrain(const std::string& filepath) const {
    if (!brain_) {
        return CommandResult(1, "Brain not initialized", false, "Cannot save uninitialized brain");
    }
    
    if (brain_->save(filepath)) {
        return CommandResult(0, "Brain saved successfully to: " + filepath);
    } else {
        return CommandResult(1, "Failed to save brain to file", false, "File save operation failed");
    }
}

CommandResult SimulationController::loadBrain(const std::string& filepath) {
    if (!brain_) {
        return CommandResult(1, "Brain not initialized", false, "Cannot load into uninitialized brain");
    }
    
    if (brain_->load(filepath)) {
        return CommandResult(0, "Brain loaded successfully from: " + filepath);
    } else {
        return CommandResult(1, "Failed to load brain from file", false, "File load operation failed");
    }
}

CommandResult SimulationController::exportStats(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return CommandResult(1, "Failed to open file for writing", false, "Cannot create output file: " + filepath);
    }
    
    file << "NLM Simulation Statistics" << std::endl;
    file << "=========================" << std::endl;
    file << std::endl;
    
    file << "Simulation Time: " << stats_.simulationTime << " seconds" << std::endl;
    file << "Total Steps: " << stats_.totalSteps << std::endl;
    file << "Total Spikes: " << stats_.totalSpikes << std::endl;
    file << "Average Firing Rate: " << stats_.averageFiringRate << " Hz" << std::endl;
    file << "Active Neurons: " << stats_.activeNeurons << std::endl;
    file << "Firing Neurons: " << stats_.firingNeurons << std::endl;
    file << "Simulation State: ";
    
    switch (stats_.state) {
        case SimulationState::Idle: file << "Idle"; break;
        case SimulationState::Running: file << "Running"; break;
        case SimulationState::Paused: file << "Paused"; break;
        case SimulationState::Completed: file << "Completed"; break;
        case SimulationState::Error: file << "Error"; break;
    }
    file << std::endl;
    
    file << std::endl;
    file << "Brain Statistics:" << std::endl;
    if (brain_) {
        file << "  Total Neurons: " << brain_->getTotalNeuronCount() << std::endl;
        file << "  Total Synapses: " << brain_->getTotalSynapseCount() << std::endl;
        file << "  Active Neurons: " << brain_->getActiveNeuronCount() << std::endl;
        file << "  Firing Neurons: " << brain_->getFiringNeuronCount() << std::endl;
        file << "  Excitation/Inhibition Ratio: " << brain_->getExcitationInhibitionRatio() << std::endl;
    } else {
        file << "  Brain not available" << std::endl;
    }
    
    file.close();
    return CommandResult(0, "Statistics exported to: " + filepath);
}

void SimulationController::printStatus() const {
    std::cout << "=== NLM Simulation Status ===" << std::endl;
    std::cout << "Simulation State: ";
    
    switch (stats_.state) {
        case SimulationState::Idle: std::cout << "Idle"; break;
        case SimulationState::Running: std::cout << "Running"; break;
        case SimulationState::Paused: std::cout << "Paused"; break;
        case SimulationState::Completed: std::cout << "Completed"; break;
        case SimulationState::Error: std::cout << "Error"; break;
    }
    std::cout << std::endl;
    
    std::cout << "Steps Completed: " << stats_.totalSteps << std::endl;
    std::cout << "Total Spikes: " << stats_.totalSpikes << std::endl;
    std::cout << "Average Firing Rate: " << stats_.averageFiringRate << " Hz" << std::endl;
    std::cout << "Active Neurons: " << stats_.activeNeurons << std::endl;
    std::cout << "Firing Neurons: " << stats_.firingNeurons << std::endl;
    std::cout << "Simulation Time: " << stats_.simulationTime << " seconds" << std::endl;
    
    if (brain_) {
        std::cout << std::endl;
        std::cout << "Brain Status:" << std::endl;
        std::cout << "  Total Neurons: " << brain_->getTotalNeuronCount() << std::endl;
        std::cout << "  Total Synapses: " << brain_->getTotalSynapseCount() << std::endl;
        std::cout << "  Developmental Stage: " << static_cast<int>(brain_->getDevelopmentalStage()) << std::endl;
    }
    
    if (agentBrain_) {
        std::cout << std::endl;
        std::cout << "Agent Status:" << std::endl;
        std::cout << "  Neuromodulation Level: " << agentBrain_->getNeuromodulationLevel() << std::endl;
        std::cout << "  Curiosity Level: " << agentBrain_->getCuriosityLevel() << std::endl;
        std::cout << "  Novelty Level: " << agentBrain_->getNoveltyLevel() << std::endl;
        std::cout << "  Prediction Error: " << agentBrain_->getPredictionError() << std::endl;
    }
    
    std::cout << std::endl;
}

bool SimulationController::initializeBrain(const std::string& config) {
    if (!config_.get()) {
        return false;
    }
    
    brain_ = std::make_shared<Brain>(config_);
    if (!brain_->initialize()) {
        return false;
    }
    
    if (verboseMode_) {
        std::cout << "Brain initialized successfully" << std::endl;
    }
    
    return true;
}

bool SimulationController::initializeAgentBrain(const std::string& config) {
    if (!brain_.get()) {
        return false;
    }
    
    agentBrain_ = std::make_shared<AgentBrain>(brain_);
    
    // Initialize with default world if not already set
    if (!world_.get()) {
        world_ = std::make_shared<SimpleWorld>();
        world_->configure(100.0f, 100.0f, 8, 8); // Default world size
    }
    
    agentBrain_->initialize(*world_);
    
    if (verboseMode_) {
        std::cout << "Agent brain initialized successfully" << std::endl;
    }
    
    return true;
}

bool SimulationController::initializeWorld(const std::string& config) {
    if (world_.get()) {
        return true; // Already initialized
    }
    
    world_ = std::make_shared<SimpleWorld>();
    
    // Parse configuration
    // For now, use default configuration
    world_->configure(100.0f, 100.0f, 8, 8);
    
    if (verboseMode_) {
        std::cout << "World initialized successfully" << std::endl;
    }
    
    return true;
}

CommandResult SimulationController::runSimulationLoop(size_t steps, bool withAgent) {
    std::stringstream output;
    
    for (SimulationStep step = 0; step < steps; ++step) {
        if (interruptRequested) {
            output << "Simulation interrupted by user request" << std::endl;
            stats_.state = SimulationState::Paused;
            return CommandResult(0, output.str());
        }
        
        // Execute simulation step
        brain_->step(step, step * 0.001);
        
        // Update statistics
        stats_.totalSpikes += brain_->getTotalSpikeCount();
        stats_.activeNeurons = brain_->getTotalNeuronCount();
        stats_.firingNeurons = brain_->getFiringNeuronCount();
        
        // Log progress if verbose or at key milestones
        if (verboseMode_ || step % 10 == 0 || step == steps - 1) {
            output << "Step " << step << ": "
                   << brain_->getFiringNeuronCount() << " firing neurons, "
                   << "Curiosity: " << (agentBrain_ ? agentBrain_->getCuriosityLevel() : 0.0f)
                   << std::endl;
        }
        
        // Agent interaction if enabled
        if (withAgent && agentBrain_ && world_) {
            const SensoryPercept& percept = world_->getSensoryPercept();
            agentBrain_->processSensoryInput(percept);
            
            MotorCommand motorCmd = agentBrain_->decodeMotorCommand();
            world_->applyMotorCommand(motorCmd, world_->getSimulationTime());
            world_->update(0.001);
        }
    }
    
    output << "Simulation completed successfully!" << std::endl;
    output << "Total spikes: " << stats_.totalSpikes 
           << ", Average firing rate: " << stats_.averageFiringRate << " Hz" << std::endl;
    
    auto endTime = std::chrono::steady_clock::now();
    stats_.simulationTime = std::chrono::duration<double>(endTime - startTime_).count();
    
    return CommandResult(0, output.str());
}