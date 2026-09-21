#pragma once

#include <memory>
#include <string>
#include <vector>

namespace nlm {

class Brain;

class SystemInitializer {
public:
    static bool initializeAllSystems(std::shared_ptr<Brain> brain);
    
    static void configurePlasticitySystems(std::shared_ptr<Brain> brain);
    static void configureNeuromodulationSystems(std::shared_ptr<Brain> brain);
    static void configureMemorySystems(std::shared_ptr<Brain> brain);
    static void configurePredictionSystems(std::shared_ptr<Brain> brain);
    static void configureCognitionSystems(std::shared_ptr<Brain> brain);
    static void configureDevelopmentSystems(std::shared_ptr<Brain> brain);
    static void configureSensoryMotorSystems(std::shared_ptr<Brain> brain);
    static void configureSimulationClock(std::shared_ptr<Brain> brain);
    
    static bool validateConfiguration(std::shared_ptr<Config> config);
    static void applyPerformanceOptimizations(std::shared_ptr<Brain> brain);
};

} // namespace nlm

