#include "BaseExperiment.hpp"
#include "Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <memory>

namespace nlm {

class Phase6IntegrationExperiment : public BaseExperiment {
public:
    Phase6IntegrationExperiment(const std::string& name = "Phase6Integration")
        : BaseExperiment(name, "Comprehensive integration test of all NLM brain systems") {
        
        // Set default configuration values
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        setConfig(config);
    }
    
    bool initialize() override {
        try {
            // Validate configuration
            if (!getConfig()) {
                throw std::runtime_error("No configuration provided");
            }
            
            std::cout << "=== Phase 6 Integration Experiment ===" << std::endl;
            std::cout << "Name: " << getName() << std::endl;
            std::cout << "Description: " << getDescription() << std::endl;
            
            // Create and validate configuration
            auto appConfig = ConfigManager::getInstance().getAppConfig();
            std::cout << "Configuration validated successfully" << std::endl;
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Initialization failed: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool run() override {
        try {
            std::cout << "Starting Phase 6 integration experiment..." << std::endl;
            
            // Initialize configuration manager
            ConfigManager::getInstance().setConfig(getConfig());
            
            // Create experiment using the existing Phase6IntegratedExperiment
            Phase6IntegratedExperiment experiment;
            
            // Configure experiment parameters
            Phase6Config config;
            config.neuronCount = getConfigValue<int>("neuron_count", 500);
            config.maxSteps = getConfigValue<int>("max_steps", 2000);
            config.enableCheckpointing = getConfigValue<bool>("enable_checkpointing", true);
            config.enableReplay = getConfigValue<bool>("enable_replay", true);
            config.enableDevelopment = getConfigValue<bool>("enable_development", true);
            
            // Run experiment with progress reporting
            setProgressCallback([](double progress) {
                static int last_percent = -1;
                int percent = static_cast<int>(progress * 100);
                if (percent != last_percent) {
                    std::cout << "\rProgress: " << percent << "%";
                    std::cout.flush();
                    last_percent = percent;
                }
            });
            
            startTiming();
            
            auto result = experiment.run(config);
            
            std::cout << std::endl << std::endl;
            std::cout << "=== EXPERIMENT RESULTS ===" << std::endl;
            std::cout << "Total reward: " << result.totalReward << std::endl;
            std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
            std::cout << "Episodes stored: " << result.memoryEpisodesStored << std::endl;
            std::cout << "Dopamine level: " << result.dopamineLevel << std::endl;
            std::cout << std::endl;
            std::cout << "=== INTEGRATION STATUS ===" << std::endl;
            std::cout << "Working Memory: " << (result.memoryWorkingMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Episodic Memory: " << (result.memoryEpisodicMemoryIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Neuromodulation: " << (result.neuromodulationIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Prediction: " << (result.predictionIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Development: " << (result.developmentIntegrated ? "CONNECTED" : "DISCONNECTED") << std::endl;
            std::cout << "Checkpointing: " << (result.checkpointingWorks ? "WORKING" : "NOT WORKING") << std::endl;
            std::cout << std::endl;
            std::cout << "Wall clock time: " << result.totalWallClockTime << "s" << std::endl;
            
            // Check if all systems are integrated
            bool success = result.memoryWorkingMemoryIntegrated &&
                          result.memoryEpisodicMemoryIntegrated &&
                          result.neuromodulationIntegrated &&
                          result.predictionIntegrated &&
                          result.developmentIntegrated &&
                          result.checkpointingWorks;
            
            if (success) {
                std::cout << "✓ ALL SYSTEMS SUCCESSFULLY INTEGRATED" << std::endl;
            } else {
                std::cout << "✗ SOME SYSTEMS FAILED INTEGRATION" << std::endl;
            }
            
            return success;
            
        } catch (const std::exception& e) {
            std::cerr << "Experiment execution failed: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool cleanup() override {
        std::cout << "Cleaning up experiment resources..." << std::endl;
        ConfigManager::getInstance().reset();
        return true;
    }
    
    // Advanced experiment capabilities
    void enableVisualization(bool enable) {
        visualization_enabled_ = enable;
    }
    
    void setCustomCheckpointPath(const std::string& path) {
        checkpoint_path_ = path;
        setConfigValue<std::string>("checkpoint_path", path);
    }
    
    // Get experiment results as structured data
    struct ExperimentResults {
        double totalReward = 0.0;
        double avgFiringRate = 0.0;
        size_t memoryEpisodesStored = 0;
        double noveltyLevel = 0.0;
        double curiosityLevel = 0.0;
        double dopamineLevel = 0.0;
        double totalTime = 0.0;
        bool workingMemoryIntegrated = false;
        bool episodicMemoryIntegrated = false;
        bool neuromodulationIntegrated = false;
        bool predictionIntegrated = false;
        bool developmentIntegrated = false;
        bool checkpointingWorks = false;
        std::string statusMessage;
    };
    
    ExperimentResults getResults() const {
        return last_results_;
    }
    
protected:
    void setLastResults(const ExperimentResults& results) {
        last_results_ = results;
    }
    
private:
    bool visualization_enabled_ = false;
    std::string checkpoint_path_ = "/tmp/nlm_checkpoint.bin";
    ExperimentResults last_results_;
};

} // namespace nlm
