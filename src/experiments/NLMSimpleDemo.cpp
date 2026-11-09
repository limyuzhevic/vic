// Simple NLM Demo - Demonstrates the core brain-world interaction
// 
// This demo shows the basic principles of NLM:
// 1. Brain processes sensory input
// 2. Brain produces motor actions
// 3. Actions affect the world
// 4. World provides feedback (reward)
// 5. Brain learns from feedback

#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <string>

// Simple helper function to parse command line arguments
std::string getArgValue(int argc, char** argv, const std::string& flag) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        size_t pos = arg.find(flag);
        if (pos != std::string::npos) {
            if (arg.find('=') != std::string::npos) {
                return arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                return argv[++i];
            }
        }
    }
    return "";
}

int main(int argc, char** argv) {
    std::cout << "=== Simple NLM Demo ===\n";
    std::cout << "Demonstrates basic brain-world interaction\n\n";
    
    // Parse simple command line arguments
    int neuronCount = 500;
    int worldSize = 10;
    int simulationSteps = 100;
    bool enableLearning = true;
    
    std::string neuronArg = getArgValue(argc, argv, "--neurons");
    if (!neuronArg.empty()) {
        neuronCount = std::stoi(neuronArg);
    }
    
    std::string worldArg = getArgValue(argc, argv, "--world");
    if (!worldArg.empty()) {
        worldSize = std::stoi(worldArg);
    }
    
    std::string stepsArg = getArgValue(argc, argv, "--steps");
    if (!stepsArg.empty()) {
        simulationSteps = std::stoi(stepsArg);
    }
    
    std::string learnArg = getArgValue(argc, argv, "--learning");
    if (learnArg == "0" || learnArg == "false" || learnArg == "no") {
        enableLearning = false;
    }
    
    // Initialize logging
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    consoleLogger->setUseColors(true);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("Simple NLM Demo Started");
    NLM_LOG_INFO("Configuration:");
    NLM_LOG_INFO("  Neurons: " + std::to_string(neuronCount));
    NLM_LOG_INFO("  World size: " + std::to_string(worldSize) + "x" + std::to_string(worldSize));
    NLM_LOG_INFO("  Simulation steps: " + std::to_string(simulationSteps));
    NLM_LOG_INFO("  Learning enabled: " + std::string(enableLearning ? "true" : "false"));
    
    // Validate configuration parameters
    if (neuronCount <= 0) {
        NLM_LOG_ERROR("Invalid neuron count: " + std::to_string(neuronCount) + ". Must be positive.");
        return 1;
    }
    
    if (worldSize <= 0) {
        NLM_LOG_ERROR("Invalid world size: " + std::to_string(worldSize) + ". Must be positive.");
        return 1;
    }
    
    if (simulationSteps <= 0) {
        NLM_LOG_ERROR("Invalid simulation steps: " + std::to_string(simulationSteps) + ". Must be positive.");
        return 1;
    }
    
    // Log validation results
    NLM_LOG_INFO("Configuration validation passed");
    NLM_LOG_INFO("Note: Large brain sizes and many steps may require significant computational resources");
    
    // Create brain
    NLM_LOG_INFO("Creating brain...");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        NLM_LOG_ERROR("Possible causes: invalid configuration, insufficient memory, or system resource limits");
        return 1;
    }
    
    // Validate brain initialization
    if (brain->getTotalNeuronCount() == 0) {
        NLM_LOG_ERROR("Brain initialized with zero neurons!");
        return 1;
    }
    
    if (brain->getTotalSynapseCount() == 0) {
        NLM_LOG_WARN("Brain has zero synapses. This may indicate connectivity issues.");
    }
    
    // Create world
    NLM_LOG_INFO("Creating world...");
    SimpleWorld world;
    world.configure(static_cast<float>(worldSize), static_cast<float>(worldSize), 8, 8);
    world.setAgentStart(static_cast<float>(worldSize) / 2.0f, static_cast<float>(worldSize) / 2.0f);
    world.reset();
    
    // Create agent interface
    AgentBrain agent(brain);
    agent.initialize(world);
    
    if (enableLearning) {
        agent.enableRewardModulation(true);
        agent.enableCuriosity(true);
        NLM_LOG_INFO("Learning enabled - agent will learn from experience");
    }
    
    // Show initial state
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Initial State ===");
    NLM_LOG_INFO("Brain neurons: " + std::to_string(brain->getTotalNeuronCount()));
    NLM_LOG_INFO("Brain synapses: " + std::to_string(brain->getTotalSynapseCount()));
    NLM_LOG_INFO("Agent sensory input: " + std::to_string(agent.getSensoryInputSize()));
    NLM_LOG_INFO("Agent motor output: " + std::to_string(agent.getMotorOutputSize()));
    
    // Run simulation with better error handling
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Running Simulation ===");
    
    // Check system resources and warn if simulation might be too heavy
    if (simulationSteps > 1000) {
        NLM_LOG_WARN("Large number of simulation steps requested. This may require significant computational resources.");
        NLM_LOG_WARN("Consider running with --steps=1000 or less for faster execution.");
    }
    
    if (neuronCount > 1000) {
        NLM_LOG_WARN("Large brain size requested. This will consume more memory and CPU.");
        NLM_LOG_WARN("Consider running with --neurons=1000 or less for faster execution.");
    }
    
    float totalReward = 0.0f;
    int activeNeurons = 0;
    int decisionChanges = 0;
    int errorCount = 0;
    
    for (int step = 0; step < simulationSteps; ++step) {
        try {
            // Update world with error checking
            world.update(0.1f);
            
            // Validate world state
            if (world.getAgentBody().x < 0 || world.getAgentBody().x >= worldSize ||
                world.getAgentBody().y < 0 || world.getAgentBody().y >= worldSize) {
                NLM_LOG_WARN("Agent position outside world boundaries - this may indicate a simulation issue");
            }
            
            // Get sensory input from world
            const SensoryPercept& percept = world.getSensoryPercept();
            
            // Process sensory input with error checking
            if (!agent.processSensoryInput(percept)) {
                NLM_LOG_ERROR("Failed to process sensory input at step " + std::to_string(step));
                errorCount++;
            }
            
            // Brain thinks (processes information) with error checking
            brain->step(step);
            
            // Decode motor command from brain activity with error checking
            MotorCommand action = agent.decodeMotorCommand();
            
            // Validate action
            if (action < MotorCommand::MoveForward || action > MotorCommand::Wait) {
                NLM_LOG_ERROR("Invalid motor command received at step " + std::to_string(step));
                errorCount++;
            }
            
            // Apply motor command to world with error checking
            ActionResult result = world.applyMotorCommand(action, world.getSimulationTime());
            
            // Validate result
            if (result.reward < -100.0f || result.reward > 100.0f) {
                NLM_LOG_WARN("Unusually large reward value: " + std::to_string(result.reward));
            }
            
            // Get reward and apply learning
            float reward = result.reward;
            totalReward += reward;
            
            if (enableLearning) {
                agent.applyRewardModulation(reward, 0.0f);
            }
            
            // Collect statistics with validation
            if (brain->getFiringNeuronCount() > 0) {
                activeNeurons++;
            }
            
            // Log progress every 20 steps with enhanced error reporting
            if (step % 20 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + 
                            " | Reward: " + std::to_string(reward) +
                            " | Firing neurons: " + std::to_string(brain->getFiringNeuronCount()) +
                            " | Curiosity: " + std::to_string(agent.getCuriosityLevel()) +
                            " | Novelty: " + std::to_string(agent.getNoveltyLevel()));
                
                // Report error count periodically
                if (errorCount > 0 && step % 100 == 0) {
                    NLM_LOG_WARN("Errors encountered: " + std::to_string(errorCount) + " (total)");
                }
            }
            
            // Check if action changed (demonstrates decision making)
            if (step > 0 && action != MotorCommand::Wait) {
                decisionChanges++;
            }
            
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Exception occurred at step " + std::to_string(step) + ": " + std::string(e.what()));
            errorCount++;
            
            // Continue simulation despite error (for demonstration)
            // In production, you might want to terminate early
            continue;
        }
    }
    
    // Final validation and reporting
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Simulation Complete ===");
    
    // Check for simulation errors
    if (errorCount > 0) {
        NLM_LOG_WARN("Simulation completed with " + std::to_string(errorCount) + " errors");
    }
    
    // Validate results
    if (activeNeurons == 0 && simulationSteps > 0) {
        NLM_LOG_WARN("No active neurons were detected during the simulation. This may indicate:")
        NLM_LOG_WARN("  - Insufficient sensory input")
        NLM_LOG_WARN("  - Brain initialization issues")
        NLM_LOG_WARN("  - Learning mechanisms not enabled")
    } else {
        NLM_LOG_INFO("Simulation successful - " + std::to_string(activeNeurons) + " steps with active neurons");
    }
    
    // Show final results
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Simulation Complete ===");
    NLM_LOG_INFO("Total reward: " + std::to_string(totalReward));
    NLM_LOG_INFO("Average firing rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz");
    NLM_LOG_INFO("Steps with active neurons: " + std::to_string(activeNeurons));
    NLM_LOG_INFO("Decision changes: " + std::to_string(decisionChanges));
    NLM_LOG_INFO("Errors encountered: " + std::to_string(errorCount));
    
    // Development stage comparison
    std::string devStage;
    switch (brain->getDevelopmentalStage()) {
        case DevelopmentalStage::Initial: devStage = "Initial"; break;
        case DevelopmentalStage::CriticalPeriod: devStage = "Critical Period"; break;
        case DevelopmentalStage::Maturation: devStage = "Maturation"; break;
        case DevelopmentalStage::Adult: devStage = "Adult"; break;
        case DevelopmentalStage::Aging: devStage = "Aging"; break;
        default: devStage = "Unknown";
    }
    NLM_LOG_INFO("Final development stage: " + devStage);
    NLM_LOG_INFO("Final curiosity level: " + std::to_string(agent.getCuriosityLevel()));
    NLM_LOG_INFO("Final novelty level: " + std::to_string(agent.getNoveltyLevel()));
    
    // Enhanced summary with system health indicators
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== System Health Indicators ===");
    
    float firingRate = brain->getAverageFiringRate();
    if (firingRate < 1.0f) {
        NLM_LOG_INFO("Brain activity: Low (consider increasing learning enabled)");
    } else if (firingRate > 50.0f) {
        NLM_LOG_WARN("Brain activity: High (may indicate high plasticity or learning)");
    } else {
        NLM_LOG_INFO("Brain activity: Normal");
    }
    
    float neuromodulation = agent.getNeuromodulationLevel();
    if (neuromodulation > 0.5f) {
        NLM_LOG_INFO("Neuromodulation level: Active");
    } else {
        NLM_LOG_INFO("Neuromodulation level: Moderate");
    }
    
    int activePopulation = brain->getActiveNeuronCount();
    if (activePopulation > 0) {
        NLM_LOG_INFO("Population activity: Active");
    } else {
        NLM_LOG_WARN("Population activity: Inactive - check connectivity or learning settings");
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Key Concepts Demonstrated ===");
    NLM_LOG_INFO("1. Brain receives sensory input from world");
    NLM_LOG_INFO("2. Brain processes information and makes decisions");
    NLM_LOG_INFO("3. Actions affect the world");
    NLM_LOG_INFO("4. World provides feedback (reward)");
    NLM_LOG_INFO("5. Brain learns from experience");
    NLM_LOG_INFO("6. Neural connections change over time (plasticity)");
    NLM_LOG_INFO("7. Error handling and validation");
    
    if (errorCount > 0) {
        NLM_LOG_WARN("Note: Simulation completed with " + std::to_string(errorCount) + " errors (see logs for details)");
    }
    
    return 0;
}
