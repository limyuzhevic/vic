// BrainInit.hpp - Brain initialization logic
#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

class BrainInit {
public:
    BrainInit(Brain* brain, std::shared_ptr<Config> config);
    ~BrainInit();
    
    bool initialize();
    
private:
    Brain* brain_;
    std::shared_ptr<Config> config_;
    std::unique_ptr<RandomGenerator> rng_;
    std::unique_ptr<Logger> logger_;
    std::unique_ptr<ConsoleLogger> consoleLogger_;
    std::unique_ptr<SimulationClock> clock_;
    
    void setupLogging();
    void loadConfiguration();
    void setupRandomGenerator();
    void createRegions();
    void createNeurons();
    void setupConnectivity();
    void initializeMemorySystems();
    void initializePredictionSystem();
    void initializeCognitionSystems();
    void initializeNeuromodulation();
    void setupSpikeHandlers();
    void configureCheckpointManager();
};

} // namespace nlm
