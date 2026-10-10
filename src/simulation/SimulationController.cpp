#include <nlm/simulation/SimulationController.hpp>
#include <nlm/core/Config/Config.hpp>
#include <nlm/brain/Brain.hpp>
#include <nlm/world/SimpleWorld.hpp>
#include <nlm/agent/AgentBrain.hpp>
#include <nlm/core/Logger/Logger.hpp>
#include <nlm/experiments/ExperimentRunner.hpp>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>

namespace nlm {

SimulationController::SimulationController() 
    : state_(std::make_unique<SimulationState>()),
      parameters_(std::make_unique<SimulationParameters>()),
      manager_(std::make_unique<SimulationManager>()),
      registry_(std::make_unique<SimulationRegistry>()),
      running_(false),
      paused_(false),
      shouldStep_(false),
      speed_(1.0),
      timeScale_(1.0),
      currentStep_(0),
      startTime_(0),
      lastUpdateTime_(0),
      totalSteps_(0),
      framesToSkip_(0),
      maxFramesToSkip_(10),
      conditionalExecutionEnabled_(false),
      eventDrivenSimulation_(false) {
    
    // Initialize logger
    logger_ = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger_->addLogger(consoleLogger);
    Logger::setGlobal(logger_);
    
    LOG_INFO("SimulationController initialized");
}

SimulationController::SimulationController(const SimulationParameters& params)
    : state_(std::make_unique<SimulationState>()),
      parameters_(std::make_unique<SimulationParameters>(params)),
      manager_(std::make_unique<SimulationManager>()),
      registry_(std::make_unique<SimulationRegistry>()),
      running_(false),
      paused_(false),
      shouldStep_(false),
      speed_(params.speed),
      timeScale_(params.timeScale),
      currentStep_(0),
      startTime_(0),
      lastUpdateTime_(0),
      totalSteps_(0),
      framesToSkip_(0),
      maxFramesToSkip_(params.maxFramesToSkip),
      conditionalExecutionEnabled_(params.conditionalExecutionEnabled),
      eventDrivenSimulation_(params.eventDrivenSimulation) {
    
    // Initialize logger
    logger_ = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger_->addLogger(consoleLogger);
    Logger::setGlobal(logger_);
    
    LOG_INFO("SimulationController initialized with custom parameters");
}

SimulationController::~SimulationController() {
    LOG_INFO("SimulationController destroyed");
}

bool SimulationController::initialize() {
    LOG_INFO("Initializing simulation controller");
    
    try {
        // Load configuration from file if specified
        if (!parameters_->configFilePath.empty()) {
            config_ = std::make_shared<Config>();
            if (config_->loadFromFile(parameters_->configFilePath)) {
                LOG_INFO("Loaded configuration from: " + parameters_->configFilePath);
            } else {
                LOG_WARNING("Failed to load configuration from: " + parameters_->configFilePath);
                config_ = std::make_shared<Config>();
            }
        } else {
            config_ = std::make_shared<Config>();
        }
        
        // Apply configuration overrides
        config_->loadFromArgs(0, nullptr);
        
        // Set default values for Phase 6 simulation
        config_->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config_->set("simulation_timestep", 0.001, ConfigSource::Default);
        config_->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config_->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config_->set("connection_probability", 0.1f, ConfigSource::Default);
        
        // Create brain with configuration
        brain_ = std::make_shared<Brain>(config_);
        
        if (!brain_->initialize()) {
            LOG_ERROR("Failed to initialize brain!");
            return false;
        }
        
        LOG_INFO("Brain initialized successfully");
        
        // Create world if needed
        if (parameters_->createWorld) {
            world_ = std::make_shared<SimpleWorld>();
            world_->configure(
                parameters_->worldWidth,
                parameters_->worldHeight,
                parameters_->visionWidth,
                parameters_->visionHeight
            );
            
            if (parameters_->createAgent) {
                world_->setAgentStart(
                    parameters_->agentStartX,
                    parameters_->agentStartY
                );
            }
            
            LOG_INFO("World configured successfully");
        }
        
        // Create agent brain if needed
        if (parameters_->createAgentBrain && brain_) {
            agentBrain_ = std::make_shared<AgentBrain>(brain_);
            
            if (world_) {
                agentBrain_->initialize(world_);
            }
            
            LOG_INFO("Agent brain initialized successfully");
        }
        
        // Initialize performance analyzers if needed
        if (parameters_->enablePerformanceAnalysis) {
            performanceAnalyzer_ = std::make_shared<PerformanceAnalyzer>();
            LOG_INFO("Performance analyzer initialized");
        }
        
        // Initialize state analyzers if needed
        if (parameters_->enableStateAnalysis) {
            stateAnalyzer_ = std::make_shared<StateAnalyzer>();
            LOG_INFO("State analyzer initialized");
        }
        
        // Initialize behavioral analyzers if needed
        if (parameters_->enableBehavioralAnalysis) {
            behavioralAnalyzer_ = std::make_shared<BehavioralAnalyzer>();
            LOG_INFO("Behavioral analyzer initialized");
        }
        
        // Initialize visualizers if needed
        if (parameters_->enableVisualization) {
            visualizer_ = std::make_shared<RealTimeVisualizer>();
            animationController_ = std::make_shared<AnimationController>();
            dataVisualizer_ = std::make_shared<DataVisualizer>();
            comparisonVisualizer_ = std::make_shared<ComparisonVisualizer>();
            
            LOG_INFO("Visualizers initialized");
        }
        
        // Initialize branch simulation if needed
        if (parameters_->enableBranchSimulation) {
            branchSimulation_ = std::make_shared<BranchSimulation>();
            LOG_INFO("Branch simulation initialized");
        }
        
        // Initialize parallel simulation if needed
        if (parameters_->enableParallelSimulation) {
            parallelSimulation_ = std::make_shared<ParallelSimulation>();
            LOG_INFO("Parallel simulation initialized");
        }
        
        // Record initial state
        state_->recordStep(currentStep_, std::chrono::high_resolution_clock::now());
        
        running_ = true;
        paused_ = false;
        shouldStep_ = false;
        
        LOG_INFO("Simulation controller initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERROR("Error initializing simulation controller: " + std::string(e.what()));
        return false;
    }
}

void SimulationController::run() {
    if (!running_) {
        LOG_ERROR("Simulation not running! Call initialize() first.");
        return;
    }
    
    LOG_INFO("Starting simulation run");
    startTime_ = std::chrono::high_resolution_clock::now();
    lastUpdateTime_ = startTime_;
    
    try {
        while (running_ && !shouldTerminate_) {
            // Handle pause state
            if (paused_) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }
            
            // Handle step-by-step execution
            if (shouldStep_) {
                executeStep();
                shouldStep_ = false;
                
                // Record step
                state_->recordStep(currentStep_, std::chrono::high_resolution_clock::now());
                
                // Notify visualizers
                if (visualizer_) {
                    visualizer_->updateStep(currentStep_, state_);
                }
                
                // Perform analysis if enabled
                if (performanceAnalyzer_) {
                    performanceAnalyzer_->analyzeStep(currentStep_, brain_, world_);
                }
                
                if (stateAnalyzer_) {
                    stateAnalyzer_->analyzeState(currentStep_, state_);
                }
                
                if (behavioralAnalyzer_) {
                    behavioralAnalyzer_->analyzeBehavior(currentStep_, agentBrain_);
                }
                
                LOG_INFO("Step " + std::to_string(currentStep_) + " completed");
                continue;
            }
            
            // Handle fast-forward simulation
            if (framesToSkip_ > 0) {
                executeFastForward(framesToSkip_);
                framesToSkip_ = 0;
                continue;
            }
            
            // Execute simulation step
            executeStep();
            
            // Record step
            state_->recordStep(currentStep_, std::chrono::high_resolution_clock::now());
            
            // Perform analysis if enabled
            if (performanceAnalyzer_) {
                performanceAnalyzer_->analyzeStep(currentStep_, brain_, world_);
            }
            
            if (stateAnalyzer_) {
                stateAnalyzer_->analyzeState(currentStep_, state_);
            }
            
            if (behavioralAnalyzer_) {
                behavioralAnalyzer_->analyzeBehavior(currentStep_, agentBrain_);
            }
            
            // Update visualizers if enabled
            if (visualizer_) {
                visualizer_->updateStep(currentStep_, state_);
            }
            
            LOG_INFO("Step " + std::to_string(currentStep_) + " completed");
        }
        
    } catch (const std::exception& e) {
        LOG_ERROR("Error during simulation run: " + std::string(e.what()));
    }
    
    LOG_INFO("Simulation run completed");
}

void SimulationController::step() {
    shouldStep_ = true;
    LOG_INFO("Step requested");
}

void SimulationController::pause() {
    if (!paused_) {
        paused_ = true;
        LOG_INFO("Simulation paused");
    }
}

void SimulationController::resume() {
    if (paused_) {
        paused_ = false;
        LOG_INFO("Simulation resumed");
    }
}

void SimulationController::stop() {
    running_ = false;
    paused_ = false;
    shouldStep_ = false;
    LOG_INFO("Simulation stopped");
}

void SimulationController::terminate() {
    shouldTerminate_ = true;
    stop();
    LOG_INFO("Simulation terminated");
}

void SimulationController::update() {
    if (!running_ || paused_) {
        return;
    }
    
    // Calculate elapsed time
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration<double>(currentTime - lastUpdateTime_).count() * timeScale_ * speed_;
    
    // Update simulation time
    currentSimulationTime_ += elapsed;
    lastUpdateTime_ = currentTime;
    
    // Execute simulation step
    executeStep();
    
    // Record step
    state_->recordStep(currentStep_, currentTime);
    
    // Perform analysis if enabled
    if (performanceAnalyzer_) {
        performanceAnalyzer_->analyzeStep(currentStep_, brain_, world_);
    }
    
    if (stateAnalyzer_) {
        stateAnalyzer_->analyzeState(currentStep_, state_);
    }
    
    if (behavioralAnalyzer_) {
        behavioralAnalyzer_->analyzeBehavior(currentStep_, agentBrain_);
    }
    
    // Update visualizers if enabled
    if (visualizer_) {
        visualizer_->updateStep(currentStep_, state_);
    }
}

void SimulationController::fastForward(int frames) {
    if (frames > 0) {
        framesToSkip_ = std::min(frames, maxFramesToSkip_);
        LOG_INFO("Fast-forward requested: " + std::to_string(framesToSkip_) + " frames");
    }
}

defines SimulationController::setSpeed(double speed) {
    speed_ = std::max(0.0, std::min(speed, 10.0)); // Limit speed to reasonable range
    LOG_INFO("Speed set to: " + std::to_string(speed_));
}

defines SimulationController::setTimeScale(double scale) {
    timeScale_ = std::max(0.01, std::min(scale, 10.0)); // Limit time scale
    LOG_INFO("Time scale set to: " + std::to_string(timeScale_));
}

void SimulationController::executeStep() {
    try {
        if (!brain_) {
            return;
        }
        
        // Create sensory input if world is available
        std::shared_ptr<SensoryPercept> percept;
        if (world_) {
            percept = world_->getSensoryPercept();
            
            // Process sensory input with agent brain if available
            if (agentBrain_ && percept) {
                agentBrain_->processSensoryInput(percept);
                
                // Decode motor command
                Action motorCmd = agentBrain_->decodeMotorCommand();
                
                // Apply motor command to world
                world_->applyMotorCommand(motorCmd, currentSimulationTime_);
            }
            
            // Update world
            world_->update(0.001);
        }
        
        // Update agent development if available
        if (agentBrain_) {
            agentBrain_->updateDevelopment(0.001);
        }
        
        // Run brain step
        brain_->step(currentStep_);
        
        // Increment step counter
        currentStep_++;
        totalSteps_++;
        
        // Handle conditional execution
        if (conditionalExecutionEnabled_) {
            handleConditionalExecution();
        }
        
        // Handle event-driven simulation
        if (eventDrivenSimulation_) {
            handleEventDrivenSimulation();
        }
        
    } catch (const std::exception& e) {
        LOG_ERROR("Error executing simulation step: " + std::string(e.what()));
    }
}

void SimulationController::executeFastForward(int frames) {
    try {
        LOG_INFO("Executing fast-forward for " + std::to_string(frames) + " frames");
        
        for (int frame = 0; frame < frames; ++frame) {
            executeStep();
        }
        
        LOG_INFO("Fast-forward completed: " + std::to_string(frames) + " frames executed");
        
    } catch (const std::exception& e) {
        LOG_ERROR("Error during fast-forward simulation: " + std::string(e.what()));
    }
}

void SimulationController::handleConditionalExecution() {
    // Implementation for conditional execution
    // This can be extended based on specific requirements
}

void SimulationController::handleEventDrivenSimulation() {
    // Implementation for event-driven simulation
    // This can be extended based on specific requirements
}

void SimulationController::loadConfiguration(const std::string& filePath) {
    if (!config_) {
        config_ = std::make_shared<Config>();
    }
    
    if (config_->loadFromFile(filePath)) {
        LOG_INFO("Loaded configuration from: " + filePath);
        
        // Apply configuration to simulation
        if (brain_) {
            brain_->setConfig(config_);
        }
    } else {
        LOG_ERROR("Failed to load configuration from: " + filePath);
    }
}

void SimulationController::saveState(const std::string& filePath) {
    if (state_) {
        state_->saveToFile(filePath);
        LOG_INFO("Simulation state saved to: " + filePath);
    }
}

void SimulationController::loadState(const std::string& filePath) {
    if (state_) {
        if (state_->loadFromFile(filePath)) {
            LOG_INFO("Simulation state loaded from: " + filePath);
        } else {
            LOG_ERROR("Failed to load simulation state from: " + filePath);
        }
    }
}

void SimulationController::createBranch(const std::string& name) {
    if (branchSimulation_) {
        branchSimulation_->createBranch(name, state_);
        LOG_INFO("Branch created: " + name);
    }
}

void SimulationController::switchBranch(const std::string& name) {
    if (branchSimulation_) {
        branchSimulation_->switchBranch(name);
        LOG_INFO("Switched to branch: " + name);
    }
}

void SimulationController::runParallel() {
    if (parallelSimulation_) {
        parallelSimulation_->run(brain_, world_);
        LOG_INFO("Parallel simulation executed");
    }
}

void SimulationController::generateReport(const std::string& filePath) {
    std::stringstream report;
    
    report << "=== Simulation Report ===\n";
    report << "Total Steps: " << totalSteps_ << "\n";
    report << "Current Step: " << currentStep_ << "\n";
    report << "Simulation Time: " << currentSimulationTime_ << "s\n";
    report << "Speed: " << speed_ << "x\n";
    report << "Time Scale: " << timeScale_ << "x\n";
    report << "Paused: " << (paused_ ? "Yes" : "No") << "\n";
    
    if (performanceAnalyzer_) {
        report << "\nPerformance Analysis:\n";
        report << performanceAnalyzer_->generateReport();
    }
    
    if (stateAnalyzer_) {
        report << "\nState Analysis:\n";
        report << stateAnalyzer_->generateReport();
    }
    
    if (behavioralAnalyzer_) {
        report << "\nBehavioral Analysis:\n";
        report << behavioralAnalyzer_->generateReport();
    }
    
    if (visualizer_) {
        report << "\nVisualization Data:\n";
        report << visualizer_->generateData();
    }
    
    // Write report to file
    std::ofstream outFile(filePath);
    if (outFile.is_open()) {
        outFile << report.str();
        outFile.close();
        LOG_INFO("Report generated: " + filePath);
    } else {
        LOG_ERROR("Failed to generate report: " + filePath);
    }
}

bool SimulationController::isRunning() const {
    return running_;
}

bool SimulationController::isPaused() const {
    return paused_;
}

double SimulationController::getSpeed() const {
    return speed_;
}

double SimulationController::getTimeScale() const {
    return timeScale_;
}

int SimulationController::getCurrentStep() const {
    return currentStep_;
}

double SimulationController::getSimulationTime() const {
    return currentSimulationTime_;
}

int SimulationController::getTotalSteps() const {
    return totalSteps_;
}

std::shared_ptr<Brain> SimulationController::getBrain() const {
    return brain_;
}

std::shared_ptr<SimpleWorld> SimulationController::getWorld() const {
    return world_;
}

std::shared_ptr<AgentBrain> SimulationController::getAgentBrain() const {
    return agentBrain_;
}

std::shared_ptr<SimulationState> SimulationController::getState() const {
    return state_;
}

std::shared_ptr<PerformanceAnalyzer> SimulationController::getPerformanceAnalyzer() const {
    return performanceAnalyzer_;
}

std::shared_ptr<StateAnalyzer> SimulationController::getStateAnalyzer() const {
    return stateAnalyzer_;
}

std::shared_ptr<BehavioralAnalyzer> SimulationController::getBehavioralAnalyzer() const {
    return behavioralAnalyzer_;
}

std::shared_ptr<RealTimeVisualizer> SimulationController::getVisualizer() const {
    return visualizer_;
}

std::shared_ptr<AnimationController> SimulationController::getAnimationController() const {
    return animationController_;
}

std::shared_ptr<DataVisualizer> SimulationController::getDataVisualizer() const {
    return dataVisualizer_;
}

std::shared_ptr<ComparisonVisualizer> SimulationController::getComparisonVisualizer() const {
    return comparisonVisualizer_;
}

std::shared_ptr<Logger> SimulationController::getLogger() const {
    return logger_;
}