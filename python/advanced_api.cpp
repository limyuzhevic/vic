#include <pybind11/pybind11.h>
#include "../src/brain/Brain.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"

namespace py = pybind11;

void add_advanced_api_examples(py::module_& m) {
    // Example 1: Basic Advanced Brain Configuration
    m.def("advanced_brain_example", []() {
        std::cout << "=== Advanced Brain Configuration Example ===\n";
        
        // Create configuration using advanced API
        auto config = nlm::python::AdvancedBrainConfiguration::createConfig({
            {"neuron_count", 2000},
            {"simulation_timestep", 0.001f},
            {"enable_plasticity", true},
            {"plasticity_learning_rate", 0.01f},
            {"enable_neuromodulation", true},
            {"enable_curiosity", true},
            {"enable_novelty", true},
            {"enable_prediction_error", true}
        });
        
        std::cout << "Created configuration with " << config->getKeys().size() << " parameters\n";
        
        // Apply preset
        auto presetConfig = nlm::python::AdvancedBrainConfiguration::applyPreset("research");
        std::cout << "Applied 'research' preset\n";
        
        // Validate configuration
        auto schema = nlm::python::AdvancedBrainConfiguration::getDefaultBrainSchema();
        bool isValid = nlm::python::AdvancedBrainConfiguration::validateConfig(config, schema);
        std::cout << "Configuration is " << (isValid ? "valid" : "invalid") << "\n";
        
        return "Advanced brain configuration example completed";
    }, "Basic advanced brain configuration example");
    
    // Example 2: Reinforcement Learning with Advanced Agent
    m.def("advanced_agent_example", []() {
        std::cout << "\n=== Advanced Agent Reinforcement Learning Example ===\n";
        
        // Create default config and brain
        auto config = std::make_shared<nlm::Config>();
        config->setInt("neuron_count", 1000);
        auto brain = std::make_shared<nlm::Brain>(config);
        
        // Create agent brain
        auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
        
        // Configure advanced agent with RL parameters
        nlm::python::AdvancedAgent::RLSimulationParameters rlParams;
        rlParams.algorithm = nlm::python::AdvancedAgent::RLAlgorithm::DEEP_Q_LEARNING;
        rlParams.learningRate = 0.001;
        rlParams.discountFactor = 0.99;
        rlParams.explorationRate = 0.1;
        rlParams.explorationDecayRate = 0.0001;
        rlParams.replayBufferSize = 10000;
        rlParams.batchSize = 32;
        
        // Create advanced agent
        nlm::python::AdvancedAgent advancedAgent(agentBrain, rlParams);
        
        std::cout << "Created advanced agent with DQN algorithm\n";
        std::cout << "Replay buffer size: " << advancedAgent.getExperienceCount() << "\n";
        
        // Simulate some learning
        for (int i = 0; i < 10; ++i) {
            // Generate random experience
            std::vector<float> state(10, static_cast<float>(rand()) / RAND_MAX);
            int action = rand() % 4;
            double reward = (rand() % 100) / 100.0;
            std::vector<float> nextState(10, static_cast<float>(rand()) / RAND_MAX);
            bool done = (rand() % 10) == 0;
            
            advancedAgent.addExperience(state, action, reward, nextState, done);
        }
        
        // Perform learning step
        advancedAgent.performRLStep();
        
        // Get learning statistics
        auto stats = advancedAgent.getLearningStatistics();
        std::cout << "Learning statistics:\n";
        std::cout << "  Total rewards: " << stats.totalRewards << "\n";
        std::cout << "  Average reward: " << stats.averageReward << "\n";
        std::cout << "  Steps learned: " << stats.stepsLearned << "\n";
        
        return "Advanced agent example completed";
    }, "Advanced reinforcement learning agent example");
    
    // Example 3: Real-time Monitoring
    m.def("monitoring_example", []() {
        std::cout << "\n=== Real-time Monitoring Example ===\n";
        
        // Create basic brain
        auto config = std::make_shared<nlm::Config>();
        config->setInt("neuron_count", 500);
        auto brain = std::make_shared<nlm::Brain>(config);
        brain->initialize();
        
        // Configure monitoring
        nlm::python::MonitoringConfig monitoringConfig;
        monitoringConfig.enableMetricsStreaming = true;
        monitoringConfig.metricsStreamIntervalMs = 100.0;
        monitoringConfig.enableRealTimeVisualization = true;
        monitoringConfig.enablePerformanceProfiling = true;
        monitoringConfig.enableActivityPatternAnalysis = true;
        
        nlm::python::MonitoringSystem::initializeMonitoring(monitoringConfig);
        
        // Start metrics streaming
        nlm::python::MonitoringSystem::startMetricsStreaming(brain, 100.0);
        
        std::cout << "Started real-time monitoring with metrics streaming\n";
        
        // Simulate some monitoring
        for (int i = 0; i < 5; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            
            // Get current metrics
            auto metrics = nlm::python::MonitoringSystem::getCurrentMetrics(brain);
            std::cout << "Step " << (i + 1) << ": " << metrics.size() << " metrics collected\n";
            
            // Get brain state visualization
            auto visualization = nlm::python::MonitoringSystem::getBrainStateVisualization(brain);
            std::cout << "Regional activity: " << visualization.regionalActivity.size() << " regions\n";
            
            // Get performance profile
            auto profile = nlm::python::MonitoringSystem::getPerformanceProfile(brain);
            std::cout << "Average step time: " << profile.averageStepTime << " ms\n";
        }
        
        // Stop monitoring
        nlm::python::MonitoringSystem::stopMetricsStreaming();
        nlm::python::MonitoringSystem::shutdownMonitoring();
        
        std::cout << "Monitoring example completed\n";
        
        return "Real-time monitoring example completed";
    }, "Real-time monitoring example");
    
    // Example 4: Advanced Neural Network Operations
    m.def("neural_operations_example", []() {
        std::cout << "\n=== Advanced Neural Network Operations Example ===\n";
        
        // Create brain
        auto config = std::make_shared<nlm::Config>();
        config->setInt("neuron_count", 1000);
        auto brain = std::make_shared<nlm::Brain>(config);
        
        // Create custom neural population
        nlm::python::AdvancedNeuralOperations::CustomPopulationConfig popConfig;
        popConfig.name = "sensory_population";
        popConfig.neuronType = nlm::NeuronType::Sensory;
        popConfig.neuronCount = 100;
        popConfig.connectionPattern = "random";
        popConfig.connectionDensity = 0.1f;
        popConfig.meanSynapticStrength = 1.0f;
        
        std::vector<nlm::PopulationId> populationIds = 
            nlm::python::AdvancedNeuralOperations::createCustomNeuralPopulation(brain, "default_region", popConfig);
        
        std::cout << "Created custom neural population with " << populationIds.size() << " populations\n";
        
        // Create neural population
        std::vector<nlm::PopulationId> neuralPopIds = 
            nlm::python::AdvancedNeuralOperations::createNeuralPopulation(brain, "motor_region", 
                                                                        nlm::NeuronType::Motor, 200);
        
        std::cout << "Created neural population with " << neuralPopIds.size() << " populations\n";
        
        // Perform network analysis
        auto graphMetrics = nlm::python::AdvancedNeuralOperations::computeGraphMetrics(brain);
        std::cout << "Network metrics:\n";
        for (const auto& metric : graphMetrics) {
            std::cout << "  " << metric.first << ": " << metric.second << "\n";
        }
        
        // Optimize plasticity
        auto plasticityParams = nlm::python::AdvancedNeuralOperations::optimizePlasticityForLearning(brain);
        std::cout << "Optimized plasticity for learning with learning rate: " << plasticityParams.learningRate << "\n";
        
        // Set developmental stage
        nlm::python::AdvancedNeuralOperations::advanceDevelopmentStage(brain, nlm::DevelopmentalStage::CriticalPeriod);
        std::cout << "Advanced brain to critical period\n";
        
        // Network statistics
        auto stats = nlm::python::AdvancedNeuralOperations::getNetworkStatistics(brain);
        std::cout << "Network statistics:\n";
        for (const auto& stat : stats) {
            std::cout << "  " << stat.first << ": " << stat.second << "\n";
        }
        
        std::cout << "Neural network operations example completed\n";
        
        return "Advanced neural network operations example completed";
    }, "Advanced neural network operations example");
    
    // Example 5: Integration with Python ML Libraries
    m.def("integration_example", []() {
        std::cout << "\n=== Integration with Python ML Libraries Example ===\n";
        
        // Test NumPy integration
        bool numpyAvailable = nlm::python::IntegrationSystem::isNumPyAvailable();
        std::cout << "NumPy available: " << (numpyAvailable ? "yes" : "no") << "\n";
        
        if (numpyAvailable) {
            // Create NumPy array
            py::array_t<float> numpyArray = nlm::python::IntegrationSystem::createNumPyArray({100, 10});
            std::cout << "Created NumPy array with shape: " << numpyArray.shape()[0] << "x" << numpyArray.shape()[1] << "\n";
            
            // Convert from NLM format
            std::vector<std::vector<float>> data2D(100, std::vector<float>(10, 0.5f));
            auto numpyFrom2D = nlm::python::IntegrationSystem::convertToNumPyFormat(data2D);
            std::cout << "Converted 2D data to NumPy format\n";
            
            // Convert to NLM format
            auto converted2D = nlm::python::IntegrationSystem::convertFromNumPyFormat2D(numpyFrom2D);
            std::cout << "Converted back to 2D format with size: " << converted2D.size() << "x" << converted2D[0].size() << "\n";
        }
        
        // Test SciPy integration
        bool scipyAvailable = nlm::python::IntegrationSystem::isSciPyAvailable();
        std::cout << "SciPy available: " << (scipyAvailable ? "yes" : "no") << "\n";
        
        if (scipyAvailable) {
            // Create sample data
            py::array_t<float> data = py::array_t<float>({50, 5});
            py::buffer_info buf = data.request();
            float* ptr = (float*)buf.ptr;
            for (size_t i = 0; i < buf.size; ++i) {
                ptr[i] = static_cast<float>(rand()) / RAND_MAX;
            }
            
            // Compute statistical analysis
            auto stats = nlm::python::IntegrationSystem::computeStatisticalAnalysis(data);
            std::cout << "Statistical analysis result: " << stats.result << " (" << stats.confidence << ")\n";
        }
        
        // Test data export/import
        std::string jsonFilename = "brain_export_test.json";
        nlm::python::DataExportConfig exportConfig = 
            nlm::python::IntegrationSystem::exportBrainToJSON(nullptr, jsonFilename);
        std::cout << "Export brain to JSON: " << (exportConfig.success ? "success" : "failed") << "\n";
        
        std::string csvFilename = "data_export_test.csv";
        std::vector<std::vector<double>> testData = {{1.0, 2.0, 3.0}, {4.0, 5.0, 6.0}};
        nlm::python::DataExportConfig csvConfig = 
            nlm::python::IntegrationSystem::exportDataToCSV(csvFilename, testData);
        std::cout << "Export data to CSV: " << (csvConfig.success ? "success" : "failed") << "\n";
        
        // Test serialization
        std::string serialized = nlm::python::IntegrationSystem::serializeToJSON(testData);
        std::cout << "Serialized data to JSON with size: " << serialized.size() << " bytes\n";
        
        std::cout << "Integration example completed\n";
        
        return "Integration with Python ML libraries example completed";
    }, "Integration with Python ML libraries example");
    
    // Example 6: Advanced Neuroscience Features
    m.def("neuroscience_example", []() {
        std::cout << "\n=== Advanced Neuroscience Features Example ===\n";
        
        // Create brain
        auto config = std::make_shared<nlm::Config>();
        config->setInt("neuron_count", 800);
        auto brain = std::make_shared<nlm::Brain>(config);
        
        // Register custom neuromodulator
        nlm::python::AdvancedNeuroscience::NeuromodulatorConfig neuromodConfig;
        neuromodConfig.name = "creativity_modulator";
        neuromodConfig.type = "custom";
        neuromodConfig.baselineLevel = 0.5;
        neuromodConfig.sensitivity = 2.0;
        neuromodConfig.gain = 1.0;
        neuromodConfig.decayRate = 0.1;
        neuromodConfig.timeConstant = 0.5;
        neuromodConfig.isEnabled = true;
        
        bool neuromodulatorRegistered = nlm::python::AdvancedNeuroscience::registerCustomNeuromodulator(neuromConfig);
        std::cout << "Registered custom neuromodulator: " << (neuromulatorRegistered ? "yes" : "no") << "\n";
        
        // Compute novelty and curiosity
        std::vector<float> state1(10, 0.1f);
        std::vector<float> state2(10, 0.9f);
        
        auto noveltyResult = nlm::python::AdvancedNeuroscience::computeNoveltyScore(brain, state1, state2);
        std::cout << "Novelty score: " << noveltyResult.noveltyScore << " (surprise: " << noveltyResult.surpriseValue << ")\n";
        
        auto curiosityResult = nlm::python::AdvancedNeuroscience::computeCuriosityScore(brain, state1);
        std::cout << "Curiosity score: " << curiosityResult.curiosityScore << " (exploration: " << curiosityResult.explorationDrive << ")\n";
        
        // Record memory and trigger consolidation
        std::vector<float> memoryTrace(20, static_cast<float>(rand()) / RAND_MAX);
        bool memoryRecorded = nlm::python::AdvancedNeuroscience::recordMemory(brain, memoryTrace, 0.8);
        std::cout << "Recorded memory: " << (memoryRecorded ? "yes" : "no") << "\n";
        
        auto consolidationResult = nlm::python::AdvancedNeuroscience::triggerMemoryConsolidation(brain, 0.7);
        std::cout << "Consolidation result: " << (consolidationResult.success ? "success" : "failed") << "\n";
        std::cout << "Consolidated " << consolidationResult.consolidatedMemoryIndices.size() << " memories\n";
        
        // Generate reports
        std::string noveltyReport = nlm::python::AdvancedNeuroscience::generateNoveltyReport(noveltyResult);
        std::string curiosityReport = nlm::python::AdvancedNeuroscience::generateCuriosityReport(curiosityResult);
        std::string memoryReport = nlm::python::AdvancedNeuroscience::generateMemoryReport(brain);
        
        std::cout << "Neuroscience example completed\n";
        
        return "Advanced neuroscience features example completed";
    }, "Advanced neuroscience features example");
    
    // Example 7: Performance Optimization
    m.def("performance_example", []() {
        std::cout << "\n=== Performance Optimization Example ===\n";
        
        // Create brain
        auto config = std::make_shared<nlm::Config>();
        config->setInt("neuron_count", 2000);
        auto brain = std::make_shared<nlm::Brain>(config);
        
        // Configure performance optimization
        nlm::python::PerformanceOptimizationConfig perfConfig;
        perfConfig.enableBatchProcessing = true;
        perfConfig.enableParallelExecution = true;
        perfConfig.enableMemoryOptimization = true;
        perfConfig.enableResourceMonitoring = true;
        perfConfig.batchProcessingThreshold = 1000;
        perfConfig.minBatchSize = 100;
        perfConfig.maxParallelThreads = std::thread::hardware_concurrency();
        
        nlm::python::PerformanceOptimizer::updatePerformanceOptimizationConfig(perfConfig);
        
        // Enable batch processing
        nlm::python::PerformanceOptimizer::enableBatchProcessing(brain, true);
        
        // Enable parallel execution
        nlm::python::PerformanceOptimizer::enableParallelExecution(brain, true);
        
        // Enable memory optimization
        nlm::python::PerformanceOptimizer::enableMemoryOptimization(brain, true);
        
        std::cout << "Performance optimization enabled\n";
        std::cout << "Available parallel threads: " << nlm::python::PerformanceOptimizer::getAvailableParallelThreads(brain) << "\n";
        
        // Get performance metrics
        auto metrics = nlm::python::PerformanceOptimizer::getPerformanceMetrics(brain);
        std::cout << "Performance metrics collected: " << metrics.size() << " metrics\n";
        
        // Get memory statistics
        auto memoryStats = nlm::python::PerformanceOptimizer::getMemoryStatistics(brain);
        std::cout << "Memory usage: " << memoryStats.usedMemoryMB << "MB / " << memoryStats.totalMemoryMB << "MB\n";
        std::cout << "Memory efficiency: " << memoryStats.memoryUtilizationPercent << "%\n";
        
        // Get resource usage
        auto resourceUsage = nlm::python::PerformanceOptimizer::getResourceUsage(brain);
        std::cout << "Resource usage:\n";
        std::cout << "  CPU: " << resourceUsage.cpuUsagePercent << "%\n";
        std::cout << "  Memory: " << resourceUsage.memoryUsagePercent << "%\n";
        std::cout << "  Disk: " << resourceUsage.diskUsagePercent << "%\n";
        
        // Start performance profiling
        nlm::python::PerformanceOptimizer::startPerformanceProfiling(brain);
        
        // Simulate some work
        std::vector<double> stepTimes;
        for (int i = 0; i < 5; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            double stepTime = nlm::python::PerformanceOptimizer::getAverageStepTime(brain);
            stepTimes.push_back(stepTime);
        }
        
        // Stop performance profiling
        nlm::python::PerformanceOptimizer::stopPerformanceProfiling(brain);
        
        // Get final performance statistics
        double avgStepTime = std::accumulate(stepTimes.begin(), stepTimes.end(), 0.0) / stepTimes.size();
        std::cout << "Average step time: " << avgStepTime << " ms\n";
        
        // Generate performance report
        std::string report = nlm::python::PerformanceOptimizer::generatePerformanceReport(brain);
        std::cout << "Performance report generated\n";
        
        std::cout << "Performance optimization example completed\n";
        
        return "Performance optimization example completed";
    }, "Performance optimization example");
    
    // Example 8: Complete Integrated Example
    m.def("complete_example", []() {
        std::cout << "\n=== Complete Integrated Example ===\n";
        
        // Create advanced configuration
        auto config = nlm::python::AdvancedBrainConfiguration::createConfig({
            {"neuron_count", 3000},
            {"simulation_timestep", 0.0005f},
            {"enable_plasticity", true},
            {"enable_neuromodulation", true},
            {"enable_curiosity", true},
            {"enable_novelty", true},
            {"enable_prediction_error", true},
            {"enable_structural_plasticity", true},
            {"enable_development", true}
        });
        
        std::cout << "Created advanced brain configuration\n";
        
        // Create brain
        auto brain = std::make_shared<nlm::Brain>(config);
        
        // Create agent brain
        auto agentBrain = std::make_shared<nlm::AgentBrain>(brain);
        
        // Configure monitoring
        nlm::python::MonitoringConfig monitoringConfig;
        monitoringConfig.enableMetricsStreaming = true;
        monitoringConfig.metricsStreamIntervalMs = 50.0;
        monitoringConfig.enableRealTimeVisualization = true;
        monitoringConfig.enablePerformanceProfiling = true;
        monitoringConfig.enableActivityPatternAnalysis = true;
        
        nlm::python::MonitoringSystem::initializeMonitoring(monitoringConfig);
        
        // Create advanced agent
        nlm::python::AdvancedAgent::RLSimulationParameters rlParams;
        rlParams.algorithm = nlm::python::AdvancedAgent::RLAlgorithm::DEEP_Q_LEARNING;
        rlParams.learningRate = 0.001;
        rlParams.discountFactor = 0.99;
        rlParams.explorationRate = 0.1;
        rlParams.replayBufferSize = 5000;
        
        nlm::python::AdvancedAgent advancedAgent(agentBrain, rlParams);
        
        // Start learning simulation
        advancedAgent.startLearning();
        
        std::cout << "Started advanced learning simulation\n";
        
        // Run simulation steps
        for (int step = 0; step < 10; ++step) {
            std::cout << "Step " << (step + 1) << "/10\n";
            
            // Generate random experience
            std::vector<float> state(20, static_cast<float>(rand()) / RAND_MAX);
            int action = rand() % 8;
            double reward = (rand() % 200) / 100.0 - 1.0; // Reward between -1 and 1
            std::vector<float> nextState(20, static_cast<float>(rand()) / RAND_MAX);
            bool done = (rand() % 20) == 0;
            
            advancedAgent.addExperience(state, action, reward, nextState, done);
            advancedAgent.performRLStep();
            
            // Get metrics
            auto metrics = nlm::python::MonitoringSystem::getCurrentMetrics(brain);
            double avgReward = advancedAgent.getLearningStatistics().averageReward;
            std::cout << "  Average reward: " << avgReward << "\n";
            
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        
        // Stop learning
        advancedAgent.stopLearning();
        
        // Generate comprehensive reports
        std::string brainConfigReport = "Brain configuration created with " + std::to_string(config->getKeys().size()) + " parameters";
        auto learningStats = advancedAgent.getLearningStatistics();
        std::string learningReport = "Learning completed - Total rewards: " + std::to_string(learningStats.totalRewards);
        
        auto performanceProfile = nlm::python::MonitoringSystem::getPerformanceProfile(brain);
        std::string performanceReport = "Performance - Average step time: " + std::to_string(performanceProfile.averageStepTime) + " ms";
        
        std::cout << "\n=== Simulation Summary ===\n";
        std::cout << "Brain Configuration: " << brainConfigReport << "\n";
        std::cout << "Learning Results: " << learningReport << "\n";
        std::cout << "Performance Metrics: " << performanceReport << "\n";
        
        // Cleanup
        nlm::python::MonitoringSystem::stopMetricsStreaming();
        nlm::python::MonitoringSystem::shutdownMonitoring();
        
        std::cout << "\nComplete integrated example completed successfully\n";
        
        return "Complete integrated example completed";
    }, "Complete integrated example");
}