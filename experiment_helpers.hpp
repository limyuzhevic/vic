// Brain factory for consistent test setup
class BrainFactory {
public:
    static std::shared_ptr<Brain> createTestBrain(size_t neuronCount = 100) {
        auto cfg = std::make_shared<Config>();
        cfg->set("neuron_count", neuronCount);
        cfg->set("region_count", 1);
        auto brain = std::make_shared<Brain>(cfg);
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize test brain");
            return nullptr;
        }
        return brain;
    }
};

// Configuration factory for experiment presets
class ExperimentConfigFactory {
public:
    static Phase6Config createQuickTestConfig() {
        Phase6Config config;
        config.neuronCount = 100;
        config.maxSteps = 500;
        config.enableCheckpointing = false;
        config.enableReplay = false;
        config.enableDevelopment = false;
        return config;
    }
    
    static Phase6Config createFullIntegrationConfig() {
        Phase6Config config;
        config.neuronCount = 1000;
        config.maxSteps = 5000;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        config.enableDevelopment = true;
        config.checkpointPath = "./full_integration_checkpoint.bin";
        return config;
    }
};