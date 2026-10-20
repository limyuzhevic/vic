/**
 * Debug and Visualization Demo
 * 
 * This demo showcases the enhanced visualization and debugging capabilities
 * for the NLM project. It demonstrates real-time brain activity monitoring,
 * world simulation visualization, and comprehensive debugging tools.
 * 
 * The demo integrates the VisualizationInterface and DebugInterface classes
 * to provide a comprehensive debugging and visualization experience.
 */

#include "visualization/VisualizationInterface.hpp"
#include "debugging/DebugInterface.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <iomanip>

using namespace nlm;

void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║    NLM — 熙然                                                ║
    ║    Neural Learning Machine                                   ║
    ║                                                               ║
    ║    Debug and Visualization Demo                             ║
    ║                                                               ║
    ║    Enhanced Visualization and Debugging Tools                ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

void createDemoConfiguration(std::shared_ptr<Config> config) {
    // Set configuration for the demo
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity parameters
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
}

void runBrainActivityVisualizationDemo() {
    std::cout << "\n=== Brain Activity Visualization Demo ===" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create configuration
    auto config = std::make_shared<Config>();
    createDemoConfiguration(config);
    
    // Create brain
    std::cout << "Creating brain with " << config->getOr<int64_t>("neuron_count", 500) << " neurons..." << std::endl;
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return;
    }
    
    // Initialize visualization interface
    VisualizationInterface viz;
    if (!viz.initialize()) {
        std::cerr << "ERROR: Failed to initialize visualization interface!" << std::endl;
        return;
    }
    
    // Initialize debug interface
    DebugInterface debug;
    if (!debug.initialize()) {
        std::cerr << "ERROR: Failed to initialize debug interface!" << std::endl;
        return;
    }
    
    // Configure visualization and debug interfaces
    viz.setUpdateRate(10.0); // 10 Hz update rate
    viz.setVisualizationMode(VisualizationMode::RealTime);
    
    debug.setDebugMode(DebugMode::Detailed);
    debug.setMonitoringMode(MonitoringMode::Active);
    debug.enableRealTimeDebugging(true);
    debug.enableMemoryLeakDetection(true);
    debug.enablePerformanceProfiling(true);
    debug.enableErrorDetection(true);
    
    std::cout << "Starting brain activity visualization..." << std::endl;
    std::cout << "Press Ctrl+C to stop the demo." << std::endl;
    
    // Record start time
    auto startTime = std::chrono::steady_clock::now();
    SimulationStep step = 0;
    double lastFPSLog = 0.0;
    
    // Main visualization loop
    while (true) {
        auto loopStart = std::chrono::steady_clock::now();
        
        // Get current debug step for visualization
        debug.setDebugStep(step);
        
        // Update brain state
        brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
        
        // Inject some input to stimulate activity
        if (step % 100 == 0) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        // Get performance metrics for debugging
        DebugStepInfo debugStep = debug.getCurrentDebugStep();
        
        // Create visualization data
        std::vector<NeuronActivityData> activityData;
        size_t activeNeuronCount = brain->getFiringNeuronCount();
        float averageFiringRate = brain->getAverageFiringRate();
        
        // Add some demo activity data
        for (size_t i = 0; i < std::min(size_t(10), brain->getTotalNeuronCount()); ++i) {
            NeuronActivityData neuronData;
            neuronData.neuronId = NeuronId(static_cast<uint64_t>(i));
            neuronData.membranePotential = (i % 3 == 0) ? 60.0f : 45.0f;
            neuronData.firingRate = (i % 4 == 0) ? 15.0f : 8.0f;
            neuronData.inhibitionLevel = (i % 5 == 0) ? 0.3f : 0.1f;
            neuronData.isActive = (step + i) % 7 == 0;
            activityData.push_back(neuronData);
        }
        
        // Update visualization
        viz.update();
        viz.visualizeBrain(*brain);
        viz.visualizeActivity(*brain, activityData);
        
        // Add time series data for debugging
        debug.addTimeSeriesMetric("steps", static_cast<double>(step));
        debug.addTimeSeriesMetric("firing_rate", static_cast<double>(averageFiringRate));
        debug.addTimeSeriesMetric("active_neurons", static_cast<double>(activeNeuronCount));
        
        // Generate debug info
        DebugInfo info;
        info.componentName = "BrainVisualization";
        info.currentState = "Running";
        info.isHealthy = true;
        
        if (activeNeuronCount < 10) {
            info.warnings.push_back("Low neural activity");
        }
        if (averageFiringRate > 20.0) {
            info.warnings.push_back("High firing rate detected");
        }
        
        debug.addDebugInfo(info);
        
        // Log performance metrics periodically
        double fps = viz.getFPS();
        if (step - lastFPSLog >= 1000) {
            std::cout << "Step: " << step 
                     << " | FPS: " << std::fixed << std::setprecision(1) << fps
                     << " | Active Neurons: " << activeNeuronCount
                     << " | Firing Rate: " << std::setprecision(2) << averageFiringRate << " Hz"
                     << " | Memory Leaks: " << debug.detectMemoryLeaks().size()
                     << std::endl;
            lastFPSLog = step;
        }
        
        // Check for memory leaks
        if (step % 5000 == 0) {
            std::vector<MemoryAllocation> leaks = debug.detectMemoryLeaks();
            if (!leaks.empty()) {
                std::cout << "WARNING: Potential memory leaks detected: " << leaks.size() << std::endl;
                for (const auto& leak : leaks) {
                    std::cout << "  Leak at " << leak.allocationSite << " (" << leak.size << " bytes)" << std::endl;
                }
            }
        }
        
        // Advance simulation
        step++;
        
        // Break condition for demo
        if (step >= 10000) {
            break;
        }
        
        // Calculate and maintain frame rate
        auto loopEnd = std::chrono::steady_clock::now();
        auto loopDuration = std::chrono::duration_cast<std::chrono::milliseconds>(loopEnd - loopStart);
        double targetFrameTime = 1000.0 / viz.getUpdateRate();
        
        if (loopDuration.count() < targetFrameTime) {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(static_cast<int>(targetFrameTime - loopDuration.count()))
            );
        }
    }
    
    // Demo completed
    auto endTime = std::chrono::steady_clock::now();
    auto totalDuration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    std::cout << "\n=== Demo Completed ===" << std::endl;
    std::cout << "Total simulation steps: " << step << std::endl;
    std::cout << "Total duration: " << totalDuration.count() << " seconds" << std::endl;
    std::cout << "Average FPS: " << std::fixed << std::setprecision(1 << step / totalDuration.count() << std::endl;
    
    // Generate final report
    std::cout << "\n=== Final Debug Report ===" << std::endl;
    std::cout << debug.generateDebugReport() << std::endl;
    
    // Export debug data
    debug.exportDebugData("demo_debug_data.json");
    std::cout << "Debug data exported to: demo_debug_data.json" << std::endl;
}

void runWorldSimulationVisualizationDemo() {
    std::cout << "\n=== World Simulation Visualization Demo ===" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create configuration
    auto config = std::make_shared<Config>();
    createDemoConfiguration(config);
    
    // Create brain
    std::cout << "Creating brain..." << std::endl;
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return;
    }
    
    // Create world
    std::cout << "Creating world (16x16)..." << std::endl;
    SimpleWorld world;
    world.initialize(16, 16);
    
    // Create agent
    std::cout << "Creating agent..." << std::endl;
    AgentBrain agent(brain);
    agent.initialize(world);
    agent.enableRewardModulation(true);
    agent.enableCuriosity(true);
    
    // Initialize visualization interface
    VisualizationInterface viz;
    if (!viz.initialize()) {
        std::cerr << "ERROR: Failed to initialize visualization interface!" << std::endl;
        return;
    }
    
    // Initialize debug interface
    DebugInterface debug;
    if (!debug.initialize()) {
        std::cerr << "ERROR: Failed to initialize debug interface!" << std::endl;
        return;
    }
    
    // Configure interfaces
    viz.setUpdateRate(5.0); // 5 Hz for world simulation
    viz.setVisualizationMode(VisualizationMode::WorldSimulation);
    
    debug.setDebugMode(DebugMode::Detailed);
    debug.setMonitoringMode(MonitoringMode::Active);
    
    std::cout << "Starting world simulation visualization..." << std::endl;
    std::cout << "Simulation will run for 2000 steps." << std::endl;
    
    // Run simulation
    float totalReward = 0.0f;
    size_t step = 0;
    const size_t maxSteps = 2000;
    
    for (step = 0; step < maxSteps; ++step) {
        // Get observation from world
        SensoryPercept percept = world.observe(brain->getRegion(RegionId(0)).get());
        
        // Process sensory input
        agent.processSensoryInput(percept);
        
        // Brain step
        brain->step(step, step * config->getOr<double>("simulation_timestep", 0.001));
        
        // Get motor command
        MotorCommand cmd = agent.decodeMotorCommand();
        
        // Apply action to world
        world.applyAction(brain->getRegion(RegionId(0)).get(), cmd);
        
        // Compute reward
        float reward = world.computeReward(brain->getRegion(RegionId(0)).get());
        totalReward += reward;
        
        // Apply reward modulation
        agent.applyRewardModulation(reward, 0.0f);
        
        // Update development
        agent.updateDevelopment(0.001);
        
        // Update visualization every 10 steps
        if (step % 10 == 0) {
            viz.update();
            viz.visualizeWorld(world);
            viz.visualizeAgent(agent, world);
            
            // Debug interface updates
            debug.update();
            debug.addTimeSeriesMetric("reward", static_cast<double>(reward));
            debug.addTimeSeriesMetric("total_reward", static_cast<double>(totalReward));
        }
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            std::cout << "Step: " << step 
                     << " | Reward: " << std::setprecision(2) << (totalReward / (step + 1))
                     << " | Firing: " << std::setprecision(2) << brain->getAverageFiringRate() << " Hz"
                     << " | Agent State: " << (agent.getCuriosityLevel() > 0.5 ? "Exploring" : "Exploiting")
                     << std::endl;
        }
        
        // Create heat map data for environment
        if (step % 500 == 0) {
            HeatMapData heatMap = viz.generateWorldHeatMap(world);
            // In real implementation, we would visualize this heat map
        }
    }
    
    std::cout << "\n=== World Simulation Demo Completed ===" << std::endl;
    std::cout << "Total steps: " << step << std::endl;
    std::cout << "Total reward: " << std::fixed << std::setprecision(2) << totalReward << std::endl;
    std::cout << "Average reward per step: " << (totalReward / step) << std::endl;
    
    // Generate final report
    std::cout << "\n=== Final World Simulation Report ===" << std::endl;
    std::cout << debug.generateDebugReport() << std::endl;
    
    // Export debug data
    debug.exportDebugData("world_simulation_debug.json");
    std::cout << "Debug data exported to: world_simulation_debug.json" << std::endl;
}

void runInteractiveCommandLineDemo() {
    std::cout << "\n=== Interactive Command-Line Debugging Demo ===" << std::endl;
    
    // Initialize debug interface
    DebugInterface debug;
    if (!debug.initialize()) {
        std::cerr << "ERROR: Failed to initialize debug interface!" << std::endl;
        return;
    }
    
    debug.setDebugMode(DebugMode::Expert);
    debug.enableCommandLineInterface(true);
    debug.enableDebugOutput(true);
    debug.setOutputLevel(2);
    
    std::cout << "Interactive command-line debugging enabled." << std::endl;
    std::cout << "Type 'help' for available commands." << std::endl;
    std::cout << "Type 'quit' to exit." << std::endl;
    std::cout << std::endl;
    
    std::string command;
    while (true) {
        std::cout << ">>> ";
        std::getline(std::cin, command);
        
        if (command.empty()) {
            continue;
        }
        
        bool commandHandled = debug.executeCommand(command);
        
        if (command == "quit") {
            std::cout << "Exiting..." << std::endl;
            break;
        } else if (!commandHandled) {
            std::cout << "Unknown command. Type 'help' for available commands." << std::endl;
        }
    }
    
    std::cout << "Interactive debugging session ended." << std::endl;
}

int main(int argc, char** argv) {
    printBanner();
    
    std::cout << "Debug and Visualization Demo for NLM" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    // Parse command line arguments
    std::string demoMode = "brain_activity";
    if (argc > 1) {
        demoMode = argv[1];
    }
    
    try {
        if (demoMode == "brain_activity") {
            runBrainActivityVisualizationDemo();
        } else if (demoMode == "world_simulation") {
            runWorldSimulationVisualizationDemo();
        } else if (demoMode == "interactive") {
            runInteractiveCommandLineDemo();
        } else if (demoMode == "all") {
            runBrainActivityVisualizationDemo();
            runWorldSimulationVisualizationDemo();
            runInteractiveCommandLineDemo();
        } else {
            std::cerr << "Unknown demo mode: " << demoMode << std::endl;
            std::cerr << "Available modes: brain_activity, world_simulation, interactive, all" << std::endl;
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\n=== All Demos Completed Successfully ===" << std::endl;
    return 0;
}