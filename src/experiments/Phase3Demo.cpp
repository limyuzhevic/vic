#include "Phase3Experiment.hpp"
#include "../agent/AgentBrain.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "ExperimentBase.hpp"
#include "ExperimentConfig.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>

namespace nlm {

// Main Phase 3 demonstration
// Shows: experience -> neural activity -> action -> consequence -> learning
class Phase3Demo : public ExperimentBase {
public:
    Phase3Demo() : ExperimentBase("Phase3Demo") {
        logger_ = std::make_unique<ExperimentLogger>("phase3_demo.log");
    }
    
    ~Phase3Demo() = default;
    
    void run(int numEpisodes = 5, int stepsPerEpisode = 200) {
        // Use the new infrastructure
        setStatus("Initializing Phase 3 Demo");
        setProgress(0.0f);
        
        // Create configuration using new config system
        auto config = ExperimentConfig::createPhase3Config(numEpisodes, stepsPerEpisode);
        
        // Run the experiment using the new Phase3Experiment class
        Phase3Experiment experiment("Phase3Demo");
        Phase3Results results = experiment.runExperiment(brain_.get(), numEpisodes, stepsPerEpisode);
        
        // Store results using new infrastructure
        storeResult("phase3_results", results);
        
        // Log results
        logger_->logPhase3Header();
        logger_->log("Phase 3 Demo completed successfully!");
        
        // Print summary
        printSummary(results);
        
        setStatus("Completed");
        setProgress(100.0f);
    }
    
    void printSummary(const Phase3Results& results) {
        std::cout << "\nPhase 3 Demo Summary:\n";
        std::cout << "  Total Reward: " << results.totalReward << "\n";
        std::cout << "  Total Steps: " << results.steps << "\n";
        std::cout << "  Final Novelty: " << results.finalNovelty << "\n";
        std::cout << "  Final Curiosity: " << results.finalCuriosity << "\n";
        std::cout << "  Prediction Error: " << results.predictionError << "\n";
        std::cout << "  Developmental Age: " << results.developmentalAge << "\n";
        std::cout << "  Developmental Stage: " << results.developmentalStage << "\n";
        std::cout << "  Total Synapses: " << results.totalSynapses << "\n";
        std::cout << "  Average Firing Rate: " << results.averageFiringRate << "\n";
        std::cout << "  Action Counts:\n";
        std::cout << "    Forward: " << results.forwardCount << "\n";
        std::cout << "    Backward: " << results.backwardCount << "\n";
        std::cout << "    Left: " << results.turnLeftCount << "\n";
        std::cout << "    Right: " << results.turnRightCount << "\n";
        std::cout << "    Interact: " << results.interactCount << "\n";
        std::cout << "    Wait: " << results.waitCount << "\n";
    }
    
private:
    std::unique_ptr<ExperimentLogger> logger_;
    std::unique_ptr<Brain> brain_;
    
    void configureEnvironment(std::shared_ptr<Config> config) {
        config->set("environment_width", 20);
        config->set("environment_height", 20);
        config->set("vision_width", 16);
        config->set("vision_height", 16);
    }
    
    void configureBrain(std::shared_ptr<Config> config) {
        config->set("neuron_count", 500);
        config->set("region_count", 2);
        config->set("connection_probability", 0.1f);
        config->set("random_seed", 42u);
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
    }
    
    const char* motorCommandToString(MotorCommand cmd) {
        switch (cmd) {
            case MotorCommand::MoveForward: return "MoveForward";
            case MotorCommand::MoveBackward: return "MoveBackward";
            case MotorCommand::TurnLeft: return "TurnLeft";
            case MotorCommand::TurnRight: return "TurnRight";
            case MotorCommand::LookLeft: return "LookLeft";
            case MotorCommand::LookRight: return "LookRight";
            case MotorCommand::Interact: return "Interact";
            case MotorCommand::Wait: return "Wait";
            default: return "Unknown";
        }
    }
    
    const char* stageToString(DevelopmentalStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
};

} // namespace nlm

// Main entry point for Phase 3 demo
int main(int argc, char* argv[]) {
    int numEpisodes = 5;
    int stepsPerEpisode = 200;
    
    if (argc > 1) {
        numEpisodes = std::atoi(argv[1]);
    }
    if (argc > 2) {
        stepsPerEpisode = std::atoi(argv[2]);
    }
    
    nlm::Phase3Demo demo;
    demo.run(numEpisodes, stepsPerEpisode);
    
    return 0;
}