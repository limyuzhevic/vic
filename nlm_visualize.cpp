#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <thread>
#include <chrono>
#include <algorithm>
#include <cmath>

#include "src/core/Config/Config.hpp"
#include "src/core/Logger/Logger.hpp"
#include "src/brain/Brain.hpp"
#include "src/agent/AgentBrain.hpp"
#include "src/world/SimpleWorld.hpp"

// Simple text-based visualization
void visualizeBrain(const nlm::Brain& brain, int maxNeurons) {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                    NLM BRAIN VISUALIZATION                        ║\n";
    std::cout << "╠═══════════════════════════════════════════════════════════════╣\n";
    
    // Get basic stats
    int totalNeurons = brain.getTotalNeuronCount();
    int firingNeurons = brain.getFiringNeuronCount();
    int totalSpikes = brain.getTotalSpikeCount();
    float firingRate = (totalNeurons > 0) ? (static_cast<float>(firingNeurons) / totalNeurons) * 100.0f : 0.0f;
    
    // Display stats
    std::cout << "║ STATISTICS:\n";
    std::cout << "║ Total Neurons:     " << std::setw(6) << totalNeurons << "\n";
    std::cout << "║ Firing Neurons:    " << std::setw(6) << firingNeurons << " (" << std::fixed << std::setprecision(1) << firingRate << "%)\n";
    std::cout << "║ Total Spikes:      " << std::setw(6) << totalSpikes << "\n";
    std::cout << "║ Average Firing Rate: " << std::setw(6) << brain.getAverageFiringRate() << " Hz\n";
    
    // Show neuron activity distribution
    if (totalNeurons > 0) {
        std::cout << "║\n";
        std::cout << "║ NEURON ACTIVITY (sample):\n";
        
        // Get some neurons for display
        auto regions = brain.getRegions();
        size_t displayCount = std::min(static_cast<size_t>(maxNeurons), totalNeurons);
        size_t displayed = 0;
        
        for (const auto& region : regions) {
            auto neurons = region->getAllNeurons();
            for (size_t i = 0; i < neurons.size() && displayed < displayCount; ++i) {
                if (neurons[i]->isActive()) {
                    std::cout << "║   Neuron " << std::setw(4) << displayed << ": ACTIVE";
                } else {
                    std::cout << "║   Neuron " << std::setw(4) << displayed << ": resting";
                }
                std::cout << " (V=" << std::setprecision(2) << neurons[i]->getMembranePotential() << "mV)";
                std::cout << "\n";
                displayed++;
            }
        }
    }
    
    std::cout << "║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════╝\n";
}

void printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " [options]\n";
    std::cout << "\nOptions:\n";
    std::cout << "  --help                    Show this help message\n";
    std::cout << "  --config <file>           Load configuration from file\n";
    std::cout << "  --steps <n>              Number of simulation steps (default: 100)\n";
    std::cout << "  --world                   Run with simple world simulation\n";
    std::cout << "  --auto                    Auto-mode with periodic updates\n";
    std::cout << "  --max-neurons <n>         Maximum neurons to display (default: 20)\n";
    std::cout << "\nExamples:\n";
    std::cout << "  " << programName << " --steps 500          Run 500 steps\n";
    std::cout << "  " << programName << " --world --auto       Run with world and auto-refresh\n";
    std::cout << "  " << programName << " --config myconfig.cfg  Load custom config\n";
}

void runWithWorld(std::shared_ptr<nlm::Brain> brain, std::shared_ptr<nlm::AgentBrain> agent,
                  std::shared_ptr<nlm::SimpleWorld> world, int steps, bool autoMode, int maxNeurons) {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                NLM WORLD SIMULATION VISUALIZATION                  ║\n";
    std::cout << "╠═══════════════════════════════════════════════════════════════╣\n";
    
    for (int step = 0; step < steps; ++step) {
        // Update world
        world->update(0.1);
        
        // Get percept and process
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        
        // Step brain
        brain->step(step);
        
        // Decode action and apply
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Apply reward modulation
        auto body = world->getAgentBody();
        float reward = 0.0f;
        if (body.energy > 0.0f) reward = body.energy * 0.1f;
        agent->applyRewardModulation(reward, 0.0f);
        
        // Update development
        agent->updateDevelopment(0.1);
        
        // Display stats every 10 steps
        if (step % 10 == 0) {
            system("clear"); // Clear screen (Unix-like systems)
            
            std::cout << "\n╔═══════════════════════════════════════════════════════════════╗\n";
            std::cout << "║                    STEP " << std::setw(3) << step << "/" << steps << "                        ║\n";
            std::cout << "╠═══════════════════════════════════════════════════════════════╣\n";
            
            // World status
            std::cout << "║ WORLD STATUS:\n";
            std::cout << "║ Position:  (" << std::fixed << std::setprecision(1) << body.x << ", " << body.y << ")\n";
            std::cout << "║ Energy:    " << std::setw(6) << body.energy << " / " << world->getMaxEnergy() << "\n";
            std::cout << "║ Age:      " << std::setw(6) << body.age << " steps\n";
            std::cout << "║ Time:     " << std::setw(6) << world->getSimulationTime() << "s\n";
            
            // Agent status
            std::cout << "║ AGENT STATS:\n";
            std::cout << "║ Curiosity:  " << std::setw(6) << agent->getCuriosityLevel() << "\n";
            std::cout << "║ Novelty:    " << std::setw(6) << agent->getNoveltyLevel() << "\n";
            std::cout << "║ Prediction Error: " << std::setw(6) << agent->getPredictionError() << "\n";
            std::cout << "║ Neuromod:  " << std::setw(6) << agent->getNeuromodulationLevel() << "\n";
            std::cout << "║ Dev Stage: " << std::setw(6) << static_cast<int>(agent->getDevelopmentalStage()) << "\n";
            
            // Brain stats
            visualizeBrain(*brain, maxNeurons);
            
            std::cout << "║\n";
            std::cout << "║ Press Ctrl+C to exit early, waiting 0.1s...\n";
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            // Clear input buffer
            std::cin.clear();
        }
        
        if (autoMode) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
}

void runBasicBrainOnly(std::shared_ptr<nlm::Brain> brain, int steps, int maxNeurons) {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                 NLM BRAIN-ONLY SIMULATION VISUALIZATION              ║\n";
    std::cout << "╠═══════════════════════════════════════════════════════════════╣\n";
    
    for (int step = 0; step < steps; ++step) {
        // Step brain with some input
        brain->step(step);
        
        // Inject some sensory input periodically
        if (step % 50 == 0) {
            brain->injectCurrentToNeurons(nlm::NeuronType::Sensory, 20.0f);
        }
        
        // Display every 25 steps
        if (step % 25 == 0) {
            system("clear"); // Clear screen (Unix-like systems)
            
            std::cout << "\n╔═══════════════════════════════════════════════════════════════╗\n";
            std::cout << "║                    STEP " << std::setw(3) << step << "/" << steps << "                        ║\n";
            std::cout << "╠═══════════════════════════════════════════════════════════════╣\n";
            
            // Basic stats
            std::cout << "║ NEURAL ACTIVITY:\n";
            std::cout << "║ Firing Neurons: " << std::setw(6) << brain->getFiringNeuronCount() << " (" 
                      << std::fixed << std::setprecision(1) << (brain->getFiringNeuronCount() / 
                      std::max(1.0, static_cast<double>(brain->getTotalNeuronCount()))) * 100.0f 
                      << "% of total)\n";
            std::cout << "║ Total Spikes:   " << std::setw(6) << brain->getTotalSpikeCount() << "\n";
            std::cout << "║ Firing Rate:    " << std::setw(6) << brain->getAverageFiringRate() << " Hz\n";
            
            // Visualize brain
            visualizeBrain(*brain, maxNeurons);
            
            std::cout << "║\n";
            std::cout << "║ Press Ctrl+C to exit early, waiting 0.5s...\n";
            
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
}

int main(int argc, char** argv) {
    std::string configFile = "configs/default.cfg";
    int steps = 100;
    bool useWorld = false;
    bool autoMode = false;
    int maxNeurons = 20;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg.substr(0, 7) == "--steps") {
            if (arg.find('=') != std::string::npos) {
                steps = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                steps = std::stoi(argv[++i]);
            }
        } else if (arg == "--world") {
            useWorld = true;
        } else if (arg == "--auto") {
            autoMode = true;
        } else if (arg.substr(0, 13) == "--max-neurons") {
            if (arg.find('=') != std::string::npos) {
                maxNeurons = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                maxNeurons = std::stoi(argv[++i]);
            }
        }
    }
    
    // Initialize logger
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    // Initialize random seed
    srand(static_cast<unsigned int>(time(0)));
    
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                      NLM VISUALIZATION TOOL                        ║\n";
    std::cout << "╠═══════════════════════════════════════════════════════════════╣\n";
    std::cout << "║ Starting NLM visualization with " << (useWorld ? "world simulation" : "brain-only simulation") << "...\n";
    std::cout << "║ Config: " << configFile << "\n";
    std::cout << "║ Steps:  " << steps << "\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════╝\n";
    
    try {
        // Load configuration
        auto config = std::make_shared<nlm::Config>();
        if (config->loadFromFile(configFile)) {
            std::cout << "[INFO] Loaded configuration from: " << configFile << std::endl;
        } else {
            std::cout << "[WARNING] Could not load configuration from " << configFile << ", using defaults.\n";
        }
        
        // Set default values if needed
        if (!config->has("random_seed")) {
            config->set("random_seed", static_cast<int64_t>(42));
        }
        if (!config->has("simulation_timestep")) {
            config->set("simulation_timestep", 0.001);
        }
        if (!config->has("neuron_count")) {
            config->set("neuron_count", static_cast<int64_t>(500));
        }
        if (!config->has("region_count")) {
            config->set("region_count", static_cast<int64_t>(1));
        }
        
        // Initialize brain
        auto brain = std::make_shared<nlm::Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "[ERROR] Failed to initialize brain!" << std::endl;
            return 1;
        }
        
        brain->logStatus();
        
        // Create agent if using world
        std::shared_ptr<nlm::AgentBrain> agent;
        std::shared_ptr<nlm::SimpleWorld> world;
        
        if (useWorld) {
            world = std::make_shared<nlm::SimpleWorld>();
            world->configure(20, 20, 8, 8);
            world->reset();
            world->setAgentStart(10.0, 10.0);
            
            agent = std::make_shared<nlm::AgentBrain>(brain);
            agent->initialize(*world);
            
            // Enable some features
            agent->enableRewardModulation(true);
            agent->enableCuriosity(true);
            agent->enableDevelopment(true);
            
            std::cout << "\n[INFO] World simulation initialized.\n";
        }
        
        // Run simulation
        if (useWorld) {
            runWithWorld(brain, agent, world, steps, autoMode, maxNeurons);
        } else {
            runBasicBrainOnly(brain, steps, maxNeurons);
        }
        
        std::cout << "\n";
        std::cout << "╔═══════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                    SIMULATION COMPLETE                           ║\n";
        std::cout << "╠═══════════════════════════════════════════════════════════════╣\n";
        std::cout << "║ Final Statistics:\n";
        std::cout << "║ Total Neurons: " << brain->getTotalNeuronCount() << "\n";
        std::cout << "║ Total Spikes:  " << brain->getTotalSpikeCount() << "\n";
        std::cout << "║ Firing Rate:   " << brain->getAverageFiringRate() << " Hz\n";
        std::cout << "╚═══════════════════════════════════════════════════════════════╝\n";
        
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception occurred: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "[ERROR] Unknown exception occurred!" << std::endl;
        return 1;
    }
    
    return 0;
}