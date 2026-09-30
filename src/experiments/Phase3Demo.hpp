// Header file for Phase3Demo
// NLM Phase 3: World Interaction Demonstration
// Shows: experience -> neural activity -> action -> consequence -> learning

#pragma once

#include "experiments/Phase3Experiment.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/SensoryPercept.hpp"
#include "world/SimpleWorld.hpp"
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <string>
#include <vector>

namespace nlm {

class Phase3Demo {
public:
    Phase3Demo();
    ~Phase3Demo();
    
    // Run the Phase 3 world interaction demonstration
    // numEpisodes: Number of episodes to run (default: 5)
    // stepsPerEpisode: Steps per episode (default: 200)
    void run(int numEpisodes = 5, int stepsPerEpisode = 200);

private:
    // Configure the simulation environment
    void configureEnvironment(std::shared_ptr<Config> config);
    
    // Configure the brain parameters
    void configureBrain(std::shared_ptr<Config> config);
    
    // Convert MotorCommand to human-readable string
    const char* motorCommandToString(MotorCommand cmd);
    
    // Convert DevelopmentalStage to human-readable string
    const char* stageToString(DevelopmentalStage stage);
};

} // namespace nlm