// NLM Phase 6: Visualization Demo
// Demonstrates the new visualization system for the integrated artificial brain

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "world/SimpleWorld.hpp"
#include "visualization/VisualizationInterface.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <chrono>
#include <csignal>

// Global flag for graceful shutdown
volatile bool running = true;

void signalHandler(int signal) {
    running = false;
    std::cout << "\nReceived interrupt signal, shutting down gracefully..." << std::endl;
}

class VisualizationDemo {
public:
    VisualizationDemo() : configFile("configs/default.cfg") {
        std::signal(SIGINT, signalHandler);
        std::signal(SIGTERM, signalHandler);
    }
    
    bool run(const std::string& mode = "terminal", int duration = 60, 
             const std::string& outputDir = "visualization_output") {
        
        std::cout << "========================================" << std::endl;
        std::cout << "NLM Phase 6 - Visualization Demo" << std::endl;
        std::cout << "Mode: " << mode << " | Duration: " << duration << "s" << std::endl;
        std::cout << "========================================" << std::endl;
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Load configuration
        auto brainConfig = std::make_shared<Config>();
        
        if (brainConfig->loadFromFile(configFile)) {
            std::cout << "Loaded configuration from: " << configFile << std::endl;
        } else {
            std::cout << "Using default configuration." << std::endl;
        }
        
        // Set configuration values for Phase 6 demo
        brainConfig->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        brainConfig->set("simulation_timestep", 0.001, ConfigSource::Default);
        brainConfig->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        brainConfig->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        brainConfig->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // Initialize simulation clock
        double timestep = brainConfig->getOr<double>("simulation_timestep", 0.001);
        SimulationClock clock(timestep);
        
        // Initialize brain
        std::cout << "Initializing NLM Brain..." << std::endl;
        auto brain = std::make_shared<Brain>(brainConfig);
        
        if (!brain->initialize()) {
            std::cout << "ERROR: Failed to initialize brain!" << std::endl;
            return false;
        }
        
        brain->logStatus();
        
        // Initialize agent components
        std::cout << "Initializing Agent Components..." << std::endl;
        auto agentBody = std::make_shared<AgentBody>();
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        
        // Initialize world
        std::cout << "Initializing World..." << std::endl;
        auto world = std::make_shared<SimpleWorld>();
        agentBrain->initialize(*world);
        
        // Initialize visualization system
        std::cout << "Initializing Visualization System..." << std::endl;
        std::shared_ptr<nlm::VisualizationInterface> visualizer;
        
        VisualizationMode visMode;
        if (mode == "terminal") {
            visMode = VisualizationMode::Terminal;
        } else if (mode == "headless") {
            visMode = VisualizationMode::Headless;
        } else if (mode == "web") {
            visMode = VisualizationMode::Web;
        } else {
            std::cout << "Unknown visualization mode: " << mode << ". Using terminal." << std::endl;
            visMode = VisualizationMode::Terminal;
        }
        
        // Create visualization manager
        auto visManager = std::make_shared<nlm::VisualizationManager>();
        visualizer = visManager->createVisualizer(visMode);
        
        if (!visualizer->initialize(brain, agentBrain, world)) {
            std::cout << "ERROR: Failed to initialize visualization!" << std::endl;
            return false;
        }
        
        std::cout << "Visualization system initialized successfully." << std::endl;
        
        // Set visualization options
        if (mode == "terminal") {
            visualizer->setOption("neuron_details", "true");
            visualizer->setOption("memory_details", "true");
            visualizer->setOption("neuromodulation_details", "true");
            visualizer->setOption("refresh_rate", "10");
        }
        
        // Create visualization data structure
        nlm::VisualizationData currentData;
        currentData.timestamp = 0.0;
        currentData.step = 0;
        
        // Calculate steps based on duration
        int totalSteps = static_cast<int>(duration / timestep);
        int exportInterval = std::max(100, totalSteps / 10);  // Export every 10% of the run
        
        std::cout << "Starting simulation..." << std::endl;
        std::cout << "Total steps: " << totalSteps << " | Export interval: " << exportInterval << std::endl;
        
        // Main simulation loop
        auto startTime = std::chrono::steady_clock::now();
        
        for (int step = 0; step < totalSteps && running; ++step) {
            // Update simulation time
            currentData.timestamp = step * timestep;
            currentData.step = step;
            
            // Update brain state
            brain->step(step, currentData.timestamp);
            
            // Update agent components
            if (agentBrain) {
                // Create simple sensory percept
                nlm::SensoryPercept percept;
                // TODO: Add realistic sensory input
                
                // Process sensory input
                agentBrain->processSensoryInput(percept);
                
                // Get motor command
                nlm::MotorCommand command = agentBrain->decodeMotorCommand();
                
                // Apply reward modulation
                float reward = 0.1f;  // Placeholder reward
                agentBrain->applyRewardModulation(reward, 0.0f);
                
                // Update development
                agentBrain->updateDevelopment(timestep);
            }
            
            // Update visualization
            visManager->updateAll(currentData);
            
            // Export data periodically
            if (step > 0 && step % exportInterval == 0) {
                std::string exportFile = outputDir + "/visualization_step_" + 
                                        std::to_string(step) + ".csv";
                visManager->exportAll(outputDir, "csv");
                std::cout << "Exported data to: " << exportFile << std::endl;
            }
            
            // Render visualization (for terminal mode)
            if (mode == "terminal" && step % 10 == 0) {
                visManager->renderAll();
                std::cout << "\rStep: " << step << "/" << totalSteps 
                         << " | Spikes: " << brain->getTotalSpikeCount() 
                         << " | Time: " << std::fixed << std::setprecision(2) << currentData.timestamp << "s"
                         << std::flush;
            }
            
            // Update visualization data
            currentData.totalSpikeCount = brain->getTotalSpikeCount();
            currentData.firingNeuronCount = brain->getFiringNeuronCount();
            currentData.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
            currentData.simulationTime = currentData.timestamp;
            
            // Update memory system data
            if (brain->getWorkingMemory()) {
                currentData.workingMemoryLoad = 0.5f;  // Placeholder
            }
            if (brain->getEpisodicMemory()) {
                currentData.episodicMemoryCount = brain->getEpisodicMemory()->getEpisodeCount();
            }
            
            // Update neuromodulation data
            if (brain->getDopamine()) {
                currentData.dopamineLevel = brain->getDopamine()->getLevel();
            }
            if (brain->getCuriosity()) {
                currentData.curiosityLevel = brain->getCuriosity()->getLevel();
            }
            if (brain->getNovelty()) {
                currentData.noveltyLevel = brain->getNovelty()->getLevel();
            }
            if (brain->getPredictionErrorSignal()) {
                currentData.predictionError = brain->getPredictionErrorSignal()->getError();
            }
            
            // Update agent state data
            if (agentBrain) {
                currentData.agentEnergy = 1.0f;  // Placeholder
                currentData.agentHealth = 1.0f;  // Placeholder
                currentData.agentPosition = {agentBody->x, agentBody->y};
                currentData.agentOrientation = agentBody->orientation;
            }
            
            // Check for graceful shutdown
            auto currentTime = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration<double>(currentTime - startTime).count();
            if (elapsed > static_cast<double>(duration)) {
                break;
            }
        }
        
        // Final render
        if (mode == "terminal") {
            visManager->renderAll();
        }
        
        // Final export
        std::cout << "\nExporting final visualization data..." << std::endl;
        visManager->exportAll(outputDir, "csv");
        
        // Generate summary report
        std::cout << "\n========================================" << std::endl;
        std::cout << "Simulation Complete!" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Final Statistics:" << std::endl;
        std::cout << "  Total Steps: " << totalSteps << std::endl;
        std::cout << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "  Average Firing Rate: " << std::fixed << std::setprecision(2) 
                  << brain->getAverageFiringRate() << " Hz" << std::endl;
        std::cout << "  E/I Ratio: " << std::fixed << std::setprecision(3) 
                  << brain->getExcitationInhibitionRatio() << std::endl;
        std::cout << "  Regions: " << brain->getRegionCount() << std::endl;
        std::cout << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        
        // Shutdown visualization system
        visManager->shutdownAll();
        
        std::cout << "\nVisualization data exported to: " << outputDir << std::endl;
        std::cout << "Demo completed successfully!" << std::endl;
        
        return true;
    }
    
private:
    std::string configFile;
};

int main(int argc, char** argv) {
    std::cout << "========================================" << std::endl;
    std::cout << "NLM Phase 6 - Visualization Demo" << std::endl;
    std::cout << "========================================" << std::endl;
    
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <mode> [duration] [output_dir]" << std::endl;
        std::cout << "Modes: terminal, headless, web" << std::endl;
        std::cout << "Example: " << argv[0] << " terminal 60 visualization_output" << std::endl;
        return 1;
    }
    
    std::string mode = argv[1];
    int duration = (argc > 2) ? std::stoi(argv[2]) : 60;
    std::string outputDir = (argc > 3) ? argv[3] : "visualization_output";
    
    VisualizationDemo demo;
    bool success = demo.run(mode, duration, outputDir);
    
    return success ? 0 : 1;
}
