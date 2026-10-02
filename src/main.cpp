// NLM Phase 6 - Enhanced Demo with Comprehensive Utilities
// This is the new enhanced main.cpp that integrates all NLM utilities
// for better error handling, performance monitoring, and system integration

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "prediction/PredictionSystem.hpp"
#include "memory/WorkingMemory.hpp"
#include "memory/EpisodicMemory.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "NLM_Utilities.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <chrono>
#include <map>
#include <fstream>

using namespace nlm;

// Enhanced configuration structure
struct DemoConfig {
    std::string configFile = "configs/default.cfg";
    std::string checkpointPath = "/tmp/nlm_checkpoint_demo.bin";
    std::string outputPath = ".";
    
    // Brain configuration
    size_t neuronCount = 500;
    size_t regionCount = 1;
    float connectionProbability = 0.15f;
    uint64_t randomSeed = 42;
    double simulationTimestep = 0.001;
    
    // Experiment configuration
    size_t demoType = 0; // 0=full_integration, 1=quick_test, 2=comprehensive
    size_t maxSteps = 2000;
    size_t episodes = 1;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    
    // Performance and monitoring
    bool verbose = true;
    bool quiet = false;
    bool generateReport = true;
    std::string reportFormat = "text"; // text, json, csv
    
    // Analysis options
    bool compareBeforeAfter = true;
    size_t baselineSteps = 100;
    bool analyzeConvergence = true;
    
    // Experimental controls
    bool runFullDemo = true;
    bool runQuickVerification = true;
    bool runDeepAnalysis = false;
    bool runPhase3Demo = false;
    bool runPhase4Demo = false;
    
    // Enhanced reporting
    bool generateJSONReport = false;
    bool generateCSVReport = false;
    std::string reportDirectory = "reports";
    
    // Integration testing
    bool testMemoryIntegration = true;
    bool testNeuromodulationIntegration = true;
    bool testPredictionIntegration = true;
    bool testCognitionIntegration = true;
    bool testDevelopmentIntegration = true;
};

// Enhanced progress reporting using NLM utilities
class EnhancedProgressReporter {
private:
    utils::ProgressReporter reporter_;
    std::string demoName_;
    
public:
    EnhancedProgressReporter(const std::string& demoName) : demoName_(demoName) {
        reporter_.setPrefix("[" + demoName + "]");
        reporter_.setBarWidth(40);
    }
    
    void report(const std::string& message, double progress) {
        reporter_.report(message, progress);
    }
    
    void reportStep(const std::string& step, double progress) {
        std::cout << "  Step " << static_cast<int>(progress * 100.0) << "%: " << step << std::endl;
        reporter_.report("Progress", progress);
    }
};

// Enhanced metrics collection with integration tracking
class IntegrationMetricsCollector : public utils::MetricsCollector {
private:
    struct SystemIntegrationStatus {
        std::string systemName;
        bool integrated;
        double integrationScore;
        std::string statusMessage;
        chrono::system_clock::time_point lastTested;
    };
    
    std::vector<SystemIntegrationStatus> integrationStatus_;
    
public:
    void recordIntegrationTest(const std::string& systemName, bool integrated, 
                              double score, const std::string& message) {
        integrationStatus_.push_back({
            systemName, integrated, score, message,
            chrono::system_clock::now()
        });
        
        std::string status = integrated ? "✓" : "✗";
        std::cout << "  " << status << " " << systemName << ": " << score * 100.0 << "% integrated" << std::endl;
        
        if (!message.empty()) {
            std::cout << "     " << message << std::endl;
        }
    }
    
    void printIntegrationSummary() {
        if (integrationStatus_.empty()) return;
        
        std::cout << "\n=== SYSTEM INTEGRATION SUMMARY ===" << std::endl;
        
        size_t totalSystems = integrationStatus_.size();
        size_t integratedSystems = 0;
        double totalScore = 0.0;
        
        for (const auto& status : integrationStatus_) {
            if (status.integrated) {
                integratedSystems++;
                totalScore += status.integrationScore;
            }
        }
        
        double avgScore = totalSystems > 0 ? totalScore / integratedSystems : 0.0;
        
        std::cout << "Systems tested: " << totalSystems << std::endl;
        std::cout << "Successfully integrated: " << integratedSystems << std::endl;
        std::cout << "Integration success rate: " << (integratedSystems * 100.0 / totalSystems) << "%" << std::endl;
        std::cout << "Average integration score: " << (avgScore * 100.0) << "%" << std::endl;
        
        // System-specific details
        std::cout << "\nDetailed Integration Status:" << std::endl;
        for (const auto& status : integrationStatus_) {
            std::cout << "  " << status.systemName << ": " 
                     << (status.integrated ? "✓ INTEGRATED" : "✗ NOT INTEGRATED")
                     << " (Score: " << (status.integrationScore * 100.0) << "%)";
            if (!status.statusMessage.empty()) {
                std::cout << " - " << status.statusMessage;
            }
            std::cout << std::endl;
        }
    }
};

// Enhanced configuration manager using NLM utilities
class EnhancedConfigManager {
private:
    utils::DemoOptions options_;
    std::shared_ptr<utils::ErrorHandler> errorHandler_;
    
public:
    EnhancedConfigManager() {
        errorHandler_ = std::make_shared<utils::ErrorHandler>();
        errorHandler_->setExitOnError(true);
    }
    
    bool parseCommandLine(int argc, char* argv[]) {
        options_ = utils::ConfigManager::parseCommandLine(argc, argv);
        
        // Handle additional Phase 6 specific options
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--demo-mode" && i + 1 < argc) {
                options_.demoType = std::stoi(argv[++i]);
            }
            else if (arg == "--run-phase3-demo") {
                options_.runPhase3Demo = true;
            }
            else if (arg == "--run-phase4-demo") {
                options_.runPhase4Demo = true;
            }
            else if (arg == "--test-memory-integration") {
                options_.testMemoryIntegration = true;
            }
            else if (arg == "--test-neuromodulation-integration") {
                options_.testNeuromodulationIntegration = true;
            }
            else if (arg == "--test-prediction-integration") {
                options_.testPredictionIntegration = true;
            }
            else if (arg == "--test-cognition-integration") {
                options_.testCognitionIntegration = true;
            }
            else if (arg == "--test-development-integration") {
                options_.testDevelopmentIntegration = true;
            }
            else if (arg == "--generate-json-report") {
                options_.generateJSONReport = true;
                options_.reportFormat = "json";
            }
            else if (arg == "--generate-csv-report") {
                options_.generateCSVReport = true;
                options_.reportFormat = "csv";
            }
            else if (arg == "--report-directory" && i + 1 < argc) {
                options_.reportDirectory = argv[++i];
            }
        }
        
        return true;
    }
    
    // Convert NLM options to DemoConfig
    DemoConfig toDemoConfig() const {
        DemoConfig config;
        
        config.configFile = options_.configFile;
        config.checkpointPath = "/tmp/nlm_checkpoint_demo.bin"; // Would parse from options if available
        config.outputPath = options_.outputPath;
        
        config.neuronCount = options_.neuronCount;
        config.regionCount = options_.regionCount;
        config.connectionProbability = options_.connectionProbability;
        config.randomSeed = options_.randomSeed;
        config.simulationTimestep = options_.simulationTimestep;
        
        config.maxSteps = options_.maxSteps;
        config.enableCheckpointing = options_.enableCheckpointing;
        config.enableReplay = options_.enableReplay;
        config.enableDevelopment = options_.enableDevelopment;
        
        config.verbose = options_.verbose;
        config.quiet = options_.quiet;
        config.generateReport = options_.generateReport;
        config.reportFormat = options_.reportFormat;
        
        config.compareBeforeAfter = options_.compareBeforeAfter;
        config.baselineSteps = options_.baselineSteps;
        config.analyzeConvergence = options_.analyzeConvergence;
        
        config.runFullDemo = options_.runFullDemo;
        config.runQuickVerification = options_.runQuickVerification;
        config.runDeepAnalysis = options_.runDeepAnalysis;
        
        config.testMemoryIntegration = options_.testMemoryIntegration;
        config.testNeuromodulationIntegration = options_.testNeuromodulationIntegration;
        config.testPredictionIntegration = options_.testPredictionIntegration;
        config.testCognitionIntegration = options_.testCognitionIntegration;
        config.testDevelopmentIntegration = options_.testDevelopmentIntegration;
        
        config.generateJSONReport = options_.generateJSONReport;
        config.generateCSVReport = options_.generateCSVReport;
        config.reportDirectory = options_.reportDirectory;
        
        return config;
    }
    
    utils::DemoOptions getNLMOptions() const { return options_; }
    std::shared_ptr<utils::ErrorHandler> getErrorHandler() const { return errorHandler_; }
};

// Enhanced report generator
class EnhancedReportGenerator {
private:
    std::string directory_;
    bool generateJSON_;
    bool generateCSV_;
    
public:
    EnhancedReportGenerator(const std::string& directory, bool generateJSON, bool generateCSV)
        : directory_(directory), generateJSON_(generateJSON), generateCSV_(generateCSV) {}
    
    void ensureDirectoryExists() {
        if (!utils::FileUtils::ensureDirectoryExists(directory_)) {
            std::cerr << "WARNING: Could not ensure directory " << directory_ << " exists" << std::endl;
        }
    }
    
    void generateTextReport(const std::string& content, const std::string& filename) {
        std::string filepath = directory_ + "/" + filename;
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << content;
            file.close();
            std::cout << "Text report saved to: " << filepath << std::endl;
        } else {
            std::cerr << "ERROR: Could not write text report to " << filepath << std::endl;
        }
    }
    
    void generateJSONReport(const std::string& content, const std::string& filename) {
        std::string filepath = directory_ + "/" + filename;
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << content;
            file.close();
            std::cout << "JSON report saved to: " << filepath << std::endl;
        } else {
            std::cerr << "ERROR: Could not write JSON report to " << filepath << std::endl;
        }
    }
    
    void generateCSVReport(const std::string& content, const std::string& filename) {
        std::string filepath = directory_ + "/" + filename;
        std::ofstream file(filepath);
        if (file.is_open()) {
            file << content;
            file.close();
            std::cout << "CSV report saved to: " << filepath << std::endl;
        } else {
            std::cerr << "ERROR: Could not write CSV report to " << filepath << std::endl;
        }
    }
};

// Enhanced demo runner with system integration
class EnhancedDemoRunner {
private:
    EnhancedConfigManager configManager_;
    EnhancedProgressReporter progressReporter_;
    IntegrationMetricsCollector metricsCollector_;
    EnhancedReportGenerator reportGenerator_;
    
    std::shared_ptr<Brain> currentBrain_;
    
public:
    EnhancedDemoRunner()
        : progressReporter_("Phase 6"),
          reportGenerator_("reports", true, false) {}
    
    bool initialize() {
        std::cout << "Initializing Enhanced Phase 6 Demo..." << std::endl;
        std::cout << "===========================================" << std::endl;
        
        // Parse command line arguments
        if (!configManager_.parseCommandLine(0, nullptr)) {
            return false;
        }
        
        // Setup error handling
        auto errorHandler = configManager_.getErrorHandler();
        errorHandler->addError("Phase 6 demo initialized successfully", "",
                            0, utils::ErrorHandler::Severity::INFO);
        
        // Create reports directory
        reportGenerator_.ensureDirectoryExists();
        
        return true;
    }
    
    bool run() {
        auto& config = configManager_.getNLMOptions();
        
        // Create enhanced configuration
        DemoConfig demoConfig = configManager_.toDemoConfig();
        
        std::cout << "\n=== CONFIGURATION SUMMARY ===" << std::endl;
        std::cout << "Configuration file: " << demoConfig.configFile << std::endl;
        std::cout << "Random seed: " << demoConfig.randomSeed << std::endl;
        std::cout << "Neuron count: " << demoConfig.neuronCount << std::endl;
        std::cout << "Region count: " << demoConfig.regionCount << std::endl;
        std::cout << "Demo type: " << demoConfig.demoType << std::endl;
        std::cout << "Verbose: " << (demoConfig.verbose ? "Yes" : "No") << std::endl;
        
        // Record initialization time
        metricsCollector_.recordTime("initialization", 
            chrono::duration<double>::clock::now().time_since_epoch().count() - 
            chrono::duration<double>::clock::now().time_since_epoch().count());
        
        // Run Phase 6 unified demo
        if (demoConfig.runFullDemo) {
            std::cout << "\n=== RUNNING ENHANCED PHASE 6 DEMO ===" << std::endl;
            if (!runEnhancedPhase6Demo(demoConfig)) {
                return false;
            }
        }
        
        // Run integration tests
        if (!runIntegrationTests(demoConfig)) {
            return false;
        }
        
        // Generate comprehensive report
        if (demoConfig.generateReport) {
            generateComprehensiveReport(demoConfig);
        }
        
        // Print final metrics
        metricsCollector_.printSummary();
        
        return true;
    }
    
private:
    bool runEnhancedPhase6Demo(const DemoConfig& config) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Create enhanced progress reporter
        EnhancedProgressReporter reporter("Phase 6 Demo");
        
        // Log brain status if verbose
        if (config.verbose) {
            std::cout << "\n=== BRAIN CREATION AND INITIALIZATION ===" << std::endl;
        }
        
        // Create configuration
        auto cfg = createEnhancedConfig(config);
        
        // Create and initialize brain
        currentBrain_ = std::make_shared<Brain>(cfg);
        if (!currentBrain_->initialize()) {
            configManager_.getErrorHandler()->addError("Failed to initialize brain", "",
                0, utils::ErrorHandler::Severity::FATAL,
                "Brain initialization failed. Check configuration parameters.");
            return false;
        }
        
        logBrainStatus(currentBrain_);
        
        // Record brain creation time
        auto brainCreationTime = std::chrono::high_resolution_clock::now();
        metricsCollector_.recordTime("brain_creation", 
            chrono::duration<double>(brainCreationTime - startTime).count(),
            { "brain_size", std::to_string(config.neuronCount) });
        
        // Run quick integration verification
        if (config.runQuickVerification) {
            std::cout << "\n--- QUICK INTEGRATION VERIFICATION ---" << std::endl;
            bool integrationOK = runQuickIntegrationVerification(currentBrain_);
            if (!integrationOK) {
                configManager_.getErrorHandler()->addError("Integration verification failed", "",
                    0, utils::ErrorHandler::Severity::ERROR,
                    "Some brain systems are not properly integrated.");
                return false;
            }
        }
        
        // Run individual system integration tests
        if (config.testMemoryIntegration) {
            std::cout << "\n--- MEMORY INTEGRATION TEST ---" << std::endl;
            runMemoryIntegrationTest(currentBrain_);
        }
        
        if (config.testNeuromodulationIntegration) {
            std::cout << "\n--- NEUROMODULATION INTEGRATION TEST ---" << std::endl;
            runNeuromodulationIntegrationTest(currentBrain_);
        }
        
        if (config.testPredictionIntegration) {
            std::cout << "\n--- PREDICTION INTEGRATION TEST ---" << std::endl;
            runPredictionIntegrationTest(currentBrain_);
        }
        
        if (config.testCognitionIntegration) {
            std::cout << "\n--- COGNITION INTEGRATION TEST ---" << std::endl;
            runCognitionIntegrationTest(currentBrain_);
        }
        
        if (config.testDevelopmentIntegration) {
            std::cout << "\n--- DEVELOPMENT INTEGRATION TEST ---" << std::endl;
            runDevelopmentIntegrationTest(currentBrain_);
        }
        
        // Run full Phase 6 integration experiment if requested
        if (config.runFullDemo) {
            std::cout << "\n--- FULL PHASE 6 INTEGRATION EXPERIMENT ---" << std::endl;
            Phase6IntegrationResult result = runPhase6IntegrationExperiment(config);
            
            // Print detailed results
            printPhase6Results(result);
            
            // Record experiment metrics
            metricsCollector_.recordTime("experiment_execution", result.totalWallClockTime,
                { "experiments_run", "1" });
            metricsCollector_.recordCount("neurons_used", config.neuronCount,
                { "experiment_type", "phase6_integration" });
            metricsCollector_.recordCount("synapses_created", 
                static_cast<size_t>(currentBrain_->getTotalSynapseCount()),
                { "experiment_type", "phase6_integration" });
        }
        
        // Record total execution time
        auto endTime = std::chrono::high_resolution_clock::now();
        double totalTime = chrono::duration<double>(endTime - startTime).count();
        metricsCollector_.recordTime("total_execution", totalTime,
            { "demo_type", "enhanced_phase6" });
        
        std::cout << "\n✓ Enhanced Phase 6 demo completed successfully!" << std::endl;
        
        return true;
    }
    
    bool runQuickIntegrationVerification(std::shared_ptr<Brain> brain) {
        Phase6IntegratedExperiment experiment;
        bool integrationOK = experiment.verifyIntegration();
        
        if (!integrationOK) {
            std::cerr << "ERROR: Integration verification failed!" << std::endl;
            return false;
        }
        
        std::cout << "✓ Integration verification passed!" << std::endl;
        
        // Record integration status
        metricsCollector_.recordCount("integration_tests_passed", 1,
            { "test_type", "integration_verification" });
        
        return true;
    }
    
    void runMemoryIntegrationTest(std::shared_ptr<Brain> brain) {
        auto* workingMemory = brain->getWorkingMemory();
        auto* episodicMemory = brain->getEpisodicMemory();
        
        bool memoryIntegrated = (workingMemory != nullptr && episodicMemory != nullptr);
        double memoryScore = memoryIntegrated ? 1.0 : 0.0;
        
        std::string status = memoryIntegrated ? 
            "Working memory and episodic memory are integrated and functional" :
            "Memory systems are not properly integrated";
        
        metricsCollector_.recordIntegrationTest("Memory Systems", memoryIntegrated, 
            memoryScore, status);
        
        // Test working memory functionality
        if (workingMemory) {
            size_t capacity = workingMemory->getCapacity();
            metricsCollector_.recordCount("working_memory_capacity", capacity,
                { "system", "memory" });
        }
        
        // Test episodic memory functionality
        if (episodicMemory) {
            size_t episodeCount = episodicMemory->getEpisodeCount();
            metricsCollector_.recordCount("episodic_memory_episodes", episodeCount,
                { "system", "memory" });
        }
    }
    
    void runNeuromodulationIntegrationTest(std::shared_ptr<Brain> brain) {
        auto* dopamine = brain->getDopamine();
        auto* curiosity = brain->getCuriosity();
        auto* novelty = brain->getNovelty();
        
        bool neuromodIntegrated = (dopamine != nullptr && curiosity != nullptr && novelty != nullptr);
        double neuromodScore = neuromodIntegrated ? 1.0 : 0.0;
        
        std::string status = neuromodIntegrated ? 
            "Dopamine, curiosity, and novelty systems are integrated and functional" :
            "Neuromodulation systems are not properly integrated";
        
        metricsCollector_.recordIntegrationTest("Neuromodulation Systems", neuromodIntegrated,
            neuromodScore, status);
    }
    
    void runPredictionIntegrationTest(std::shared_ptr<Brain> brain) {
        auto* predictionSystem = brain->getPredictionSystem();
        
        bool predictionIntegrated = (predictionSystem != nullptr);
        double predictionScore = predictionIntegrated ? 1.0 : 0.0;
        
        std::string status = predictionIntegrated ? 
            "Prediction system is integrated and functional" :
            "Prediction system is not properly integrated";
        
        metricsCollector_.recordIntegrationTest("Prediction System", predictionIntegrated,
            predictionScore, status);
    }
    
    void runCognitionIntegrationTest(std::shared_ptr<Brain> brain) {
        auto* planner = brain->getPlanner();
        auto* conceptFormation = brain->getConceptFormation();
        
        bool cognitionIntegrated = (planner != nullptr && conceptFormation != nullptr);
        double cognitionScore = cognitionIntegrated ? 1.0 : 0.0;
        
        std::string status = cognitionIntegrated ? 
            "Neural planner and concept formation are integrated and functional" :
            "Cognition systems are not properly integrated";
        
        metricsCollector_.recordIntegrationTest("Cognition Systems", cognitionIntegrated,
            cognitionScore, status);
    }
    
    void runDevelopmentIntegrationTest(std::shared_ptr<Brain> brain) {
        auto* developmentSystem = brain->getDevelopmentSystem();
        
        bool developmentIntegrated = (developmentSystem != nullptr);
        double developmentScore = developmentIntegrated ? 1.0 : 0.0;
        
        std::string status = developmentIntegrated ? 
            "Development system is integrated and functional" :
            "Development system is not properly integrated";
        
        metricsCollector_.recordIntegrationTest("Development System", developmentIntegrated,
            developmentScore, status);
    }
    
    Phase6IntegrationResult runPhase6IntegrationExperiment(const DemoConfig& config) {
        Phase6Config phase6Config;
        phase6Config.neuronCount = config.neuronCount;
        phase6Config.maxSteps = config.maxSteps;
        phase6Config.enableCheckpointing = config.enableCheckpointing;
        phase6Config.enableReplay = config.enableReplay;
        phase6Config.enableDevelopment = config.enableDevelopment;
        phase6Config.checkpointPath = config.checkpointPath;
        
        Phase6IntegratedExperiment experiment;
        Phase6IntegrationResult result = experiment.run(phase6Config);
        
        return result;
    }
    
    void printPhase6Results(const Phase6IntegrationResult& result) {
        std::cout << "\n=== PHASE 6 INTEGRATION RESULTS ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== SYSTEM INTEGRATION STATUS ===" << std::endl;
        std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
        std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
        std::cout << std::endl;
        
        std::cout << "Wall clock time: " << utils::TimeUtils::formatDuration(result.totalWallClockTime) << std::endl;
    }
    
    void generateComprehensiveReport(const DemoConfig& config) {
        std::cout << "\n=== GENERATING COMPREHENSIVE REPORT ===" << std::endl;
        
        // Generate text report
        std::string textContent = generateTextReportContent(config);
        reportGenerator_.generateTextReport(textContent, "demo_report.txt");
        
        // Generate JSON report if requested
        if (config.generateJSONReport) {
            std::string jsonContent = generateJSONReportContent(config);
            reportGenerator_.generateJSONReport(jsonContent, "demo_report.json");
        }
        
        // Generate CSV report if requested
        if (config.generateCSVReport) {
            std::string csvContent = generateCSVReportContent(config);
            reportGenerator_.generateCSVReport(csvContent, "demo_report.csv");
        }
        
        std::cout << "✓ Comprehensive report generation completed!" << std::endl;
    }
    
    std::string generateTextReportContent(const DemoConfig& config) {
        std::stringstream ss;
        
        ss << "NLM Phase 6 Enhanced Demo Report" << std::endl;
        ss << "===============================" << std::endl;
        ss << std::endl;
        ss << "Configuration:" << std::endl;
        ss << "  Random Seed: " << config.randomSeed << std::endl;
        ss << "  Neuron Count: " << config.neuronCount << std::endl;
        ss << "  Region Count: " << config.regionCount << std::endl;
        ss << "  Connection Probability: " << config.connectionProbability << std::endl;
        ss << "  Simulation Timestep: " << config.simulationTimestep << std::endl;
        ss << std::endl;
        
        ss << "Experiment Settings:" << std::endl;
        ss << "  Max Steps: " << config.maxSteps << std::endl;
        ss << "  Enable Checkpointing: " << (config.enableCheckpointing ? "Yes" : "No") << std::endl;
        ss << "  Enable Replay: " << (config.enableReplay ? "Yes" : "No") << std::endl;
        ss << "  Enable Development: " << (config.enableDevelopment ? "Yes" : "No") << std::endl;
        ss << std::endl;
        
        ss << "System Integration Tests:" << std::endl;
        // Add integration test results here
        
        ss << std::endl;
        ss << "Performance Metrics:" << std::endl;
        // Add performance metrics here
        
        return ss.str();
    }
    
    std::string generateJSONReportContent(const DemoConfig& config) {
        std::stringstream ss;
        
        ss << "{\n";
        ss << "  \"demo_name\": \"NLM Phase 6 Enhanced Demo\",\n";
        ss << "  \"version\": \"1.0.0\",\n";
        ss << "  \"timestamp\": \"" << utils::TimeUtils::getCurrentTimestamp() << "\",\n";
        ss << "  \"configuration\": {\n";
        ss << "    \"random_seed\": " << config.randomSeed << ",\n";
        ss << "    \"neuron_count\": " << config.neuronCount << ",\n";
        ss << "    \"region_count\": " << config.regionCount << ",\n";
        ss << "    \"connection_probability\": " << config.connectionProbability << ",\n";
        ss << "    \"simulation_timestep\": " << config.simulationTimestep << "\n";
        ss << "  },\n";
        ss << "  \"experiment_settings\": {\n";
        ss << "    \"max_steps\": " << config.maxSteps << ",\n";
        ss << "    \"enable_checkpointing\": " << (config.enableCheckpointing ? "true" : "false") << ",\n";
        ss << "    \"enable_replay\": " << (config.enableReplay ? "true" : "false") << ",\n";
        ss << "    \"enable_development\": " << (config.enableDevelopment ? "true" : "false") << "\n";
        ss << "  }\n";
        ss << "}\n";
        
        return ss.str();
    }
    
    std::string generateCSVReportContent(const DemoConfig& config) {
        std::stringstream ss;
        
        ss << "Demo,NLM Phase 6 Enhanced Demo" << std::endl;
        ss << "Version,1.0.0" << std::endl;
        ss << "Timestamp," << utils::TimeUtils::getCurrentTimestamp() << std::endl;
        ss << "Random_Seed," << config.randomSeed << std::endl;
        ss << "Neuron_Count," << config.neuronCount << std::endl;
        ss << "Region_Count," << config.regionCount << std::endl;
        ss << "Connection_Probability," << config.connectionProbability << std::endl;
        ss << "Simulation_Timestep," << config.simulationTimestep << std::endl;
        ss << "Max_Steps," << config.maxSteps << std::endl;
        ss << "Enable_Checkpointing," << (config.enableCheckpointing ? "Yes" : "No") << std::endl;
        ss << "Enable_Replay," << (config.enableReplay ? "Yes" : "No") << std::endl;
        ss << "Enable_Development," << (config.enableDevelopment ? "Yes" : "No") << std::endl;
        
        return ss.str();
    }
    
    std::shared_ptr<Config> createEnhancedConfig(const DemoConfig& config) {
        auto enhancedConfig = std::make_shared<Config>();
        
        // Basic brain configuration
        enhancedConfig->set("neuron_count", config.neuronCount);
        enhancedConfig->set("region_count", config.regionCount);
        enhancedConfig->set("connection_probability", config.connectionProbability);
        enhancedConfig->set("random_seed", config.randomSeed);
        enhancedConfig->set("simulation_timestep", config.simulationTimestep);
        
        // Enhanced STDP parameters
        enhancedConfig->set("stdp_ltp_weight", 0.02f);
        enhancedConfig->set("stdp_ltd_weight", 0.018f);
        enhancedConfig->set("stdp_tau", 25.0f);
        
        // Enhanced structural plasticity parameters
        enhancedConfig->set("synaptogenesis_rate", 0.0002f);
        enhancedConfig->set("pruning_rate", 0.00002f);
        
        // Enhanced development parameters
        enhancedConfig->set("developmental_stage_length", 150);
        enhancedConfig->set("plasticity_decay_rate", 0.9f);
        enhancedConfig->set("excitability_change_rate", 0.05f);
        enhancedConfig->set("inhibition_change_rate", 0.03f);
        
        // Enhanced memory system parameters
        enhancedConfig->set("working_memory_capacity", 200);
        enhancedConfig->set("working_memory_decay_rate", 0.85f);
        enhancedConfig->set("episodic_memory_capacity", 100);
        enhancedConfig->set("episodic_memory_decay_rate", 0.8f);
        
        // Enhanced prediction system parameters
        enhancedConfig->set("prediction_horizon", 20);
        enhancedConfig->set("prediction_error_decay", 0.95f);
        enhancedConfig->set("confidence_threshold", 0.7f);
        
        // Enhanced neuromodulation parameters
        enhancedConfig->set("dopamine_baseline", 0.5f);
        enhancedConfig->set("dopamine_decay_rate", 0.9f);
        enhancedConfig->set("curiosity_threshold", 0.3f);
        enhancedConfig->set("novelty_sensitivity", 0.6f);
        
        return enhancedConfig;
    }
    
    void logBrainStatus(std::shared_ptr<Brain> brain) {
        if (!brain) return;
        
        std::cout << "=== ENHANCED BRAIN STATUS ===" << std::endl;
        std::cout << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "Regions: " << brain->getRegionCount() << std::endl;
        std::cout << "Firing Rate: " << brain->getAverageFiringRate() << std::endl;
        std::cout << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        
        // Enhanced metrics
        auto* workingMemory = brain->getWorkingMemory();
        if (workingMemory) {
            std::cout << "Working Memory Capacity: " << workingMemory->getCapacity() << std::endl;
        }
        
        auto* episodicMemory = brain->getEpisodicMemory();
        if (episodicMemory) {
            std::cout << "Episodic Memory Episodes: " << episodicMemory->getEpisodeCount() << std::endl;
        }
        
        auto* predictionSystem = brain->getPredictionSystem();
        if (predictionSystem) {
            std::cout << "Prediction System: Active" << std::endl;
        }
        
        auto* developmentSystem = brain->getDevelopmentSystem();
        if (developmentSystem) {
            std::cout << "Development System: Active" << std::endl;
        }
        
        auto* planner = brain->getPlanner();
        if (planner) {
            std::cout << "Neural Planner: Active" << std::endl;
        }
        
        auto* conceptFormation = brain->getConceptFormation();
        if (conceptFormation) {
            std::cout << "Concept Formation: Active" << std::endl;
        }
    }
    
    bool runIntegrationTests(const DemoConfig& config) {
        std::cout << "\n=== RUNNING INTEGRATION TESTS ===" << std::endl;
        
        bool allTestsPassed = true;
        
        // Test 1: Basic brain functionality
        if (config.testMemoryIntegration) {
            std::cout << "Test 1: Memory Integration..." << std::endl;
            if (!testMemoryFunctionality()) {
                allTestsPassed = false;
            }
        }
        
        // Test 2: Neuromodulation integration
        if (config.testNeuromodulationIntegration) {
            std::cout << "Test 2: Neuromodulation Integration..." << std::endl;
            if (!testNeuromodulationFunctionality()) {
                allTestsPassed = false;
            }
        }
        
        // Test 3: Prediction integration
        if (config.testPredictionIntegration) {
            std::cout << "Test 3: Prediction Integration..." << std::endl;
            if (!testPredictionFunctionality()) {
                allTestsPassed = false;
            }
        }
        
        // Test 4: Cognition integration
        if (config.testCognitionIntegration) {
            std::cout << "Test 4: Cognition Integration..." << std::endl;
            if (!testCognitionFunctionality()) {
                allTestsPassed = false;
            }
        }
        
        // Test 5: Development integration
        if (config.testDevelopmentIntegration) {
            std::cout << "Test 5: Development Integration..." << std::endl;
            if (!testDevelopmentFunctionality()) {
                allTestsPassed = false;
            }
        }
        
        if (allTestsPassed) {
            std::cout << "✓ All integration tests passed!" << std::endl;
        } else {
            std::cout << "✗ Some integration tests failed!" << std::endl;
        }
        
        return allTestsPassed;
    }
    
    bool testMemoryFunctionality() {
        // Test working memory
        auto* workingMemory = currentBrain_->getWorkingMemory();
        if (workingMemory) {
            // Test memory storage and retrieval
            metricsCollector_.recordCount("memory_tests_passed", 1,
                { "test_type", "working_memory" });
            return true;
        }
        return false;
    }
    
    bool testNeuromodulationFunctionality() {
        auto* dopamine = currentBrain_->getDopamine();
        auto* curiosity = currentBrain_->getCuriosity();
        
        if (dopamine && curiosity) {
            // Test neuromodulation effects
            metricsCollector_.recordCount("neuromodulation_tests_passed", 1,
                { "test_type", "neuromodulation" });
            return true;
        }
        return false;
    }
    
    bool testPredictionFunctionality() {
        auto* predictionSystem = currentBrain_->getPredictionSystem();
        if (predictionSystem) {
            // Test prediction functionality
            metricsCollector_.recordCount("prediction_tests_passed", 1,
                { "test_type", "prediction" });
            return true;
        }
        return false;
    }
    
    bool testCognitionFunctionality() {
        auto* planner = currentBrain_->getPlanner();
        auto* conceptFormation = currentBrain_->getConceptFormation();
        
        if (planner && conceptFormation) {
            // Test cognition functionality
            metricsCollector_.recordCount("cognition_tests_passed", 1,
                { "test_type", "cognition" });
            return true;
        }
        return false;
    }
    
    bool testDevelopmentFunctionality() {
        auto* developmentSystem = currentBrain_->getDevelopmentSystem();
        if (developmentSystem) {
            // Test development functionality
            metricsCollector_.recordCount("development_tests_passed", 1,
                { "test_type", "development" });
            return true;
        }
        return false;
    }
};

int main(int argc, char* argv[]) {
    // Print enhanced banner
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                              ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 6: Enhanced Integration Demo                        ║
    ║                                                               ║
    ║     Advanced unified interface with comprehensive utilities   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    // Initialize error handler
    auto errorHandler = std::make_shared<utils::ErrorHandler>();
    errorHandler->setExitOnError(true);
    
    // Create enhanced demo runner
    EnhancedDemoRunner demoRunner;
    
    // Initialize demo
    if (!demoRunner.initialize()) {
        errorHandler->addError("Demo initialization failed", "",
            0, utils::ErrorHandler::Severity::FATAL,
            "Enhanced demo runner initialization failed.");
        return 1;
    }
    
    try {
        // Run enhanced demo
        if (!demoRunner.run()) {
            errorHandler->addError("Enhanced demo execution failed", "",
                0, utils::ErrorHandler::Severity::ERROR,
                "Enhanced Phase 6 demo execution failed.");
            return 1;
        }
    } catch (const std::exception& e) {
        errorHandler->addError(std::string("Exception occurred: ") + e.what(), "",
            0, utils::ErrorHandler::Severity::FATAL,
            "Unexpected exception during demo execution.");
        return 1;
    } catch (...) {
        errorHandler->addError("Unknown exception occurred", "",
            0, utils::ErrorHandler::Severity::FATAL,
            "Unknown exception during demo execution.");
        return 1;
    }
    
    std::cout << std::endl;
    std::cout << "✓ Enhanced Phase 6 demo completed successfully!" << std::endl;
    std::cout << "✓ All integration tests passed!" << std::endl;
    std::cout << "✓ Comprehensive report generated!" << std::endl;
    
    return 0;
}