#include <nlm/Brain.hpp>
#include <nlm/AgentBrain.hpp>
#include <nlm/SimpleWorld.hpp>
#include <iostream>
#include <string>
#include <vector>

void printUsage() {
    std::cout << "Usage: nlm [options]\n";
    std::cout << "  Options:\n";
    std::cout << "    --help              Show this help message\n";
    std::cout << "    --config <file>     Load configuration from file (default: configs/default.cfg)\n";
    std::cout << "    --steps <n>         Number of simulation steps (default: 1000)\n";
    std::cout << "    --debug             Enable debug output\n";
    std::cout << "    --neural           Run neural-only simulation (no agent)\n";
    std::cout << "    --replay          Resume from checkpoint\n";
    std::cout << "    --checkpoint <file>  Save checkpoint to file\n";
    std::cout << "\nExample: nlm --config myconfig.cfg --steps 5000 --debug\n";
}

void printBanner() {
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║     NLM — 熙然                                                  ║" << std::endl;
    std::cout << "║     Neural Learning Machine                                   ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║     Phase 6: Final Integration                                 ║" << std::endl;
    std::cout << "║     An experimental artificial developmental brain.            ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char** argv) {
    printBanner();
    
    // Default configuration
    std::string configFile = "configs/default.cfg";
    int numSteps = 1000;
    bool debug = false;
    bool runAgent = true;
    std::string checkpointFile;
    bool resumeFromCheckpoint = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printUsage();
            return 0;
        } else if (arg == "--debug") {
            debug = true;
        } else if (arg == "--neural") {
            runAgent = false;
        } else if (arg == "--replay") {
            resumeFromCheckpoint = true;
        } else if (arg.substr(0, 12) == "--checkpoint") {
            if (arg.find('=') != std::string::npos) {
                checkpointFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                checkpointFile = argv[++i];
            }
        } else if (arg.substr(0, 8) == "--config") {
            if (arg.find('=') != std::string::npos) {
                configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                configFile = argv[++i];
            }
        } else if (arg.substr(0, 7) == "--steps") {
            if (arg.find('=') != std::string::npos) {
                numSteps = std::stoi(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                numSteps = std::stoi(argv[++i]);
            }
        }
    }
    
    std::cout << "Loading configuration from: " << configFile << std::endl;
    
    // Create and configure brain
    auto config = std::make_shared<nlm::Config>();
    if (config->loadFromFile(configFile)) {
        std::cout << "✓ Configuration loaded successfully\n" << std::endl;
    } else {
        std::cout << "✗ Failed to load configuration, using defaults\n" << std::endl;
    }
    
    // Optionally set checkpoint if provided
    if (!checkpointFile.empty()) {
        config->set("checkpoint_file", checkpointFile);
    }
    
    // Create brain
    auto brain = std::make_shared<nlm::Brain>(config);
    
    if (!brain->initialize()) {
        std::cout << "✗ Failed to initialize brain!\n" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized with " << brain->getTotalNeuronCount() << " neurons\n" << std::endl;
    
    // If resuming from checkpoint, load it
    if (resumeFromCheckpoint) {
        std::string checkpointPath = configFile.substr(0, configFile.find_last_of('.')) + ".nlm_checkpoint";
        if (brain->load(checkpointPath)) {
            std::cout << "✓ Checkpoint loaded successfully\n" << std::endl;
        } else {
            std::cout << "✗ Failed to load checkpoint, starting fresh\n" << std::endl;
        }
    }
    
    if (runAgent) {
        // Run agent simulation
        auto agent = std::make_shared<nlm::AgentBrain>(brain);
        
        // Create and configure world
        auto world = std::make_shared<nlm::SimpleWorld>();
        world->configure(20.0, 20.0, 16, 16);
        world->reset();
        world->setAgentStart(10.0, 10.0);
        
        agent->initialize(*world);
        
        // Enable learning features
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        // Run simulation loop
        std::cout << "Running agent simulation for " << numSteps << " steps...\n" << std::endl;
        
        for (int step = 0; step < numSteps; ++step) {
            // Update world
            world->update(0.1);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input in brain
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(step);
            
            // Decode motor command
            auto motorCmd = agent->decodeMotorCommand();
            
            // Apply action to world
            auto result = world->applyMotorCommand(motorCmd, world->getSimulationTime());
            
            // Apply reward modulation
            agent->applyRewardModulation(result.reward, 0.0);
            
            // Update development
            agent->updateDevelopment(0.1);
            
            // Print progress
            if (debug || (step % 100 == 0)) {
                std::cout << "Step " << step << ": ";
                std::cout << "Firing: " << brain->getFiringNeuronCount() << ", ";
                std::cout << "Spikes: " << brain->getTotalSpikeCount() << ", ";
                std::cout << "Curiosity: " << agent->getCuriosityLevel() << ", ";
                std::cout << "Novelty: " << agent->getNoveltyLevel() << ", ";
                std::cout << "Dev: " << brain->getDevelopmentalStage() << "; ";
                std::cout << "Reward: " << result.reward << "; ";
                if (result.success) {
                    std::cout << "✓ ";
                } else {
                    std::cout << "✗ ";
                }
                std::cout << result.message << std::endl;
            }
            
            // Save checkpoint
            if (!checkpointFile.empty() && step % 100 == 0) {
                std::string checkpointPath = checkpointFile;
                if (step > 0) {
                    // Add step number to filename
                    checkpointPath = checkpointFile.substr(0, checkpointFile.find_last_of('.')) + 
                                    "_step" + std::to_string(step) + 
                                    checkpointFile.substr(checkpointFile.find_last_of('.'));
                }
                brain->save(checkpointPath);
            }
            
            if (step >= 999) {
                break;
            }
        }
        
    } else {
        // Neural-only simulation
        std::cout << "Running neural-only simulation for " << numSteps << " steps...\n" << std::endl;
        
        for (int step = 0; step < numSteps; ++step) {
            brain->step(step);
            
            if (debug || (step % 100 == 0)) {
                std::cout << "Step " << step << ": ";
                std::cout << "Firing: " << brain->getFiringNeuronCount() << ", ";
                std::cout << "Spikes: " << brain->getTotalSpikeCount() << ", ";
                std::cout << "E/I: " << brain->getExcitationInhibitionRatio() << "; ";
                std::cout << "Dev: " << brain->getDevelopmentalStage() << std::endl;
            }
            
            // Save checkpoint
            if (!checkpointFile.empty() && step % 100 == 0) {
                std::string checkpointPath = checkpointFile;
                if (step > 0) {
                    checkpointPath = checkpointFile.substr(0, checkpointFile.find_last_of('.')) + 
                                    "_step" + std::to_string(step) + 
                                    checkpointFile.substr(checkpointFile.find_last_of('.'));
                }
                brain->save(checkpointPath);
            }
        }
    }
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "Simulation complete!\n";
    std::cout << "Final stats:\n";
    std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << std::endl;
    std::cout << "  Excitation/Inhibition ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    std::cout << "  Developmental stage: " << brain->getDevelopmentalStage() << std::endl;
    
    if (runAgent) {
        auto agent = std::make_shared<nlm::AgentBrain>(brain);
        std::cout << "  Curiosity level: " << agent->getCuriosityLevel() << std::endl;
        std::cout << "  Novelty level: " << agent->getNoveltyLevel() << std::endl;
    }
    
    std::cout << "\n" << std::string(60, '=') << std::endl;
    
    return 0;
}