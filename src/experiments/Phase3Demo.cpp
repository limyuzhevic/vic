// NLM Phase 3 Demo - Refactored for better modularity
// Main Phase 3 demonstration: shows experience -> neural activity -> action -> consequence -> learning

#include "Phase3Experiment.hpp"
#include "../agent/AgentBrain.hpp"
#include "../agent/SensoryPercept.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <set>
#include <utility>

namespace nlm {

class EpisodeRunner {
public:
    EpisodeRunner(SimpleWorld& world, AgentBrain& agentBrain, Brain& brain)
        : world_(world), agentBrain_(agentBrain), brain_(brain) {}
    
    ExperimentMetrics runEpisode(int episode, int stepsPerEpisode) {
        std::cout << "--- Episode " << episode << " started ---\n";
        
        // Reset world
        world_.reset();
        agentBrain_.reset();
        brain_.reset();
        
        ExperimentMetrics metrics;
        metrics.episode = episode;
        
        // Phase 5 navigation/interact metrics: visited 1x1 cells,
        // interact outcomes from the world's own result message
        // (consumed_resource / hit_hazard / no_object — not a label
        // given to the brain), and forward/backward oscillation
        // (a Fwd step immediately followed by Bwd, or vice versa).
        std::set<std::pair<int, int>> visitedCells;
        int resourcesConsumed = 0, hazardsHit = 0, emptyInteracts = 0;
        int tooSoonCount = 0;
        int oscillations = 0;
        MotorCommand prevMove = MotorCommand::Wait;
        bool hasPrevMove = false;
        
        float episodeReward = 0.0f;
        // Monotonic brain-step counter for this episode. The old code
        // reused neuralStep 0..9 for every action, so delayed spikes
        // scheduled for step 10+ could never be delivered and
        // accumulated forever in the spike system.
        uint64_t brainStepCounter = 0;
        
        // Run steps
        for (int step = 0; step < stepsPerEpisode; ++step) {
            // 1. Get sensory percept from world
            const SensoryPercept& percept = world_.getSensoryPercept();
            
            // 2. Inject sensory input into brain
            agentBrain_.processSensoryInput(percept);
            
            // 3. Simulate brain (multiple steps per action)
            for (int neuralStep = 0; neuralStep < 10; ++neuralStep) {
                (void)neuralStep;
                brain_.step(brainStepCounter++);
            }
            
            // 4. Decode motor command from brain activity
            MotorCommand cmd = agentBrain_.decodeMotorCommand();
            
            // Instrumentation (opt-in, off by default): log the exact
            // quantities the Wait-default hypothesis depends on, for
            // episode 0 only. Set NLM_PHASE3_INSTRUMENT=1 to enable.
            if (episode == 0 && std::getenv("NLM_PHASE3_INSTRUMENT")) {
                auto acts = agentBrain_.getMotorGroupActivities();
                size_t visionSeen = 0;
                for (float v : percept.getVision()) {
                    if (v < 0.999f) ++visionSeen;
                }
                std::cout << "INSTR step=" << step
                          << " firing=" << brain_.getFiringNeuronCount()
                          << " avgRate=" << brain_.getAverageFiringRate()
                          << " visionSeen=" << visionSeen << "/"
                          << percept.getVision().size();
                for (const auto& a : acts) {
                    std::cout << " " << a.name << "=" << a.activity
                              << "/rate=" << a.firingRate
                              << "(n=" << a.size << ",refr=" << a.refractoryCount << ")";
                }
                std::cout << " cmd=" << motorCommandToString(cmd) << "\n";
            }
            
            // 5. Apply motor command to world
            ActionResult result = world_.applyMotorCommand(cmd, world_.getSimulationTime());
            
            // 6. Apply reward modulation
            agentBrain_.applyRewardModulation(result.reward, 0.0f);
            
            // 7. Update world — Phase 6 pacing fix: one decision
            // is one world action interval (0.1 s). The old
            // 0.01 s step meant the world's 0.1 s action gate
            // rejected 181/200 decisions as "too soon", so the
            // brain was judged on actions it never took. The
            // world gate itself is unchanged; the demo now
            // actually steps the world by the interval the
            // world's own physics (3.0 units/s, 2.0 rad/s) is
            // defined over.
            world_.update(0.1);
            
            // 8. Update development
            agentBrain_.updateDevelopment(0.1);
            
            // Track metrics
            episodeReward += result.reward;
            metrics.steps++;
            metrics.totalReward += result.reward;
            metrics.totalSpikes += brain_.getFiringNeuronCount();
            
            // Phase 5 metrics: coverage, interact outcome, oscillation.
            {
                const auto& body = world_.getAgentBody();
                visitedCells.insert({static_cast<int>(std::floor(body.x)),
                                     static_cast<int>(std::floor(body.y))});
            }
            if (result.message == "too soon") ++tooSoonCount;
            if (cmd == MotorCommand::Interact) {
                if (result.message == "consumed_resource") ++resourcesConsumed;
                else if (result.message == "hit_hazard") ++hazardsHit;
                else ++emptyInteracts;
            }
            if (cmd == MotorCommand::MoveForward || cmd == MotorCommand::MoveBackward) {
                if (hasPrevMove && prevMove != cmd) ++oscillations;
                prevMove = cmd;
                hasPrevMove = true;
            }
            
            // Count actions (Look actions are not Wait)
            switch (cmd) {
                case MotorCommand::MoveForward: metrics.forwardCount++; break;
                case MotorCommand::MoveBackward: metrics.backwardCount++; break;
                case MotorCommand::TurnLeft: metrics.turnLeftCount++; break;
                case MotorCommand::TurnRight: metrics.turnRightCount++; break;
                case MotorCommand::Interact: metrics.interactCount++; break;
                case MotorCommand::LookLeft: metrics.lookLeftCount++; break;
                case MotorCommand::LookRight: metrics.lookRightCount++; break;
                default: metrics.waitCount++; break;
            }
            
            // Print periodic updates
            if (step % 50 == 0) {
                std::cout << "  Step " << step 
                          << ": reward=" << result.reward
                          << ", action=" << motorCommandToString(cmd)
                          << ", neuromod=" << agentBrain_.getNeuromodulationLevel()
                          << ", novelty=" << agentBrain_.getNoveltyLevel()
                          << ", energy=" << world_.getAgentBody().energy
                          << "\n";
            }
        }
        
        // Record final metrics
        metrics.finalNovelty = agentBrain_.getNoveltyLevel();
        metrics.finalCuriosity = agentBrain_.getCuriosityLevel();
        metrics.finalEnergy = world_.getAgentBody().energy;
        metrics.predictionError = agentBrain_.getPredictionError();
        metrics.developmentalAge = agentBrain_.getBrain()->getTotalSynapseCount();  // Use as proxy
        metrics.developmentalStage = stageToString(agentBrain_.getDevelopmentalStage());
        metrics.totalSynapses = brain_.getTotalSynapseCount();
        metrics.averageFiringRate = brain_.getAverageFiringRate();
        
        // Phase 5 navigation/interact metrics summary
        metrics.visitedCells = visitedCells.size();
        metrics.resourcesConsumed = resourcesConsumed;
        metrics.hazardsHit = hazardsHit;
        metrics.emptyInteracts = emptyInteracts;
        metrics.oscillations = oscillations;
        metrics.tooSoonCount = tooSoonCount;
        
        std::cout << "--- Episode " << episode << " ended ---\n";
        std::cout << "  Steps: " << metrics.steps << "\n";
        std::cout << "  Total Reward: " << metrics.totalReward << "\n";
        std::cout << "  Final Energy: " << metrics.finalEnergy << "\n";
        std::cout << "  Synapses: " << metrics.totalSynapses << "\n";
        std::cout << "  Action counts: fwd=" << metrics.forwardCount
                  << ", bwd=" << metrics.backwardCount
                  << ", left=" << metrics.turnLeftCount
                  << ", right=" << metrics.turnRightCount
                  << ", interact=" << metrics.interactCount
                  << ", wait=" << metrics.waitCount
                  << ", lookL=" << metrics.lookLeftCount
                  << ", lookR=" << metrics.lookRightCount
                  << ", avgFiringRate=" << metrics.averageFiringRate
                  << "\n";
        std::cout << "  Phase5 metrics: visitedCells=" << metrics.visitedCells
                  << ", resources=" << metrics.resourcesConsumed
                  << ", hazards=" << metrics.hazardsHit
                  << ", emptyInteracts=" << metrics.emptyInteracts
                  << ", oscillations=" << metrics.oscillations
                  << ", finalNovelty=" << metrics.finalNovelty
                  << ", finalCuriosity=" << metrics.finalCuriosity
                  << ", tooSoon=" << metrics.tooSoonCount
                  << ", executed=" << (stepsPerEpisode - metrics.tooSoonCount)
                  << ", reflexBonus=" << agentBrain_.getReflexBonusCount()
                  << ", reflexSuppress=" << agentBrain_.getReflexSuppressCount()
                  << ", wtaPulses=" << agentBrain_.getWtaPulseCount()
                  << ", plannerOverride=" << agentBrain_.getPlannerOverrideCount()
                  << ", plannerAgree=" << agentBrain_.getPlannerAgreementCount()
                  << "/" << agentBrain_.getPlannerDecisionCount()
                  << ", conceptBias=" << agentBrain_.getConceptBiasCount()
                  << ", selfBias=" << agentBrain_.getSelfBiasCount()
                  << ", socialBias=" << agentBrain_.getSocialBiasCount()
                  << ", concepts=" << agentBrain_.getConceptCount()
                  << ", selfConf=" << agentBrain_.getSelfModelConfidence()
                  << ", wmTraces=" << (brain_.getWorkingMemory() ? brain_.getWorkingMemory()->getActiveTraces() : 0)
                  << "\n\n";
        
        return metrics;
    }
    
private:
    SimpleWorld& world_;
    AgentBrain& agentBrain_;
    Brain& brain_;
    
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

// Main Phase 3 demonstration
// Shows: experience -> neural activity -> action -> consequence -> learning

class Phase3Demo {
public:
    Phase3Demo() {}
    ~Phase3Demo() {}
    
    void run(int numEpisodes = 5, int stepsPerEpisode = 200) {
        std::cout << "===========================================\n";
        std::cout << "NLM PHASE 3 - WORLD INTERACTION DEMO\n";
        std::cout << "===========================================\n\n";
        
        // Create configuration
        auto config = std::make_shared<Config>();
        configureEnvironment(config);
        configureBrain(config);
        
        // Create world
        SimpleWorld world;
        world.configure(20.0f, 20.0f, 16, 16);
        world.setRandomSeed(42);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create agent brain interface
        AgentBrain agentBrain(brain);
        agentBrain.initialize(world);
        
        // Configure agent brain
        agentBrain.enableRewardModulation(true);
        agentBrain.enableStructuralPlasticity(true);
        agentBrain.enableDevelopment(true);
        agentBrain.enableCuriosity(true);
        
        std::cout << "Initial brain state:\n";
        std::cout << "  Neurons: " << brain->getTotalNeuronCount() << "\n";
        std::cout << "  Synapses: " << brain->getTotalSynapseCount() << "\n";
        std::cout << "  Sensory input size: " << agentBrain.getSensoryInputSize() << "\n";
        std::cout << "  Motor output size: " << agentBrain.getMotorOutputSize() << "\n\n";
        
        // Metrics tracking
        std::vector<ExperimentMetrics> allMetrics;
        
        // Create episode runner
        EpisodeRunner episodeRunner(world, agentBrain, *brain);
        
        // Run episodes
        for (int episode = 0; episode < numEpisodes; ++episode) {
            ExperimentMetrics metrics = episodeRunner.runEpisode(episode, stepsPerEpisode);
            allMetrics.push_back(metrics);
        }
        
        // Summary
        std::cout << "===========================================\n";
        std::cout << "SUMMARY\n";
        std::cout << "===========================================\n";
        
        if (allMetrics.size() >= 2) {
            const auto& first = allMetrics.front();
            const auto& last = allMetrics.back();
            
            std::cout << "Early episode (0) vs Late episode (" << (allMetrics.size() - 1) << "):\n";
            std::cout << "  Reward: " << first.totalReward << " -> " << last.totalReward << "\n";
            std::cout << "  Forward actions: " << first.forwardCount << " -> " << last.forwardCount << "\n";
            std::cout << "  Exploration (wait): " << first.waitCount << " -> " << last.waitCount << "\n";
            
            // Calculate behavior change
            float rewardChange = last.totalReward - first.totalReward;
            float actionChange = static_cast<float>(last.forwardCount) - static_cast<float>(first.forwardCount);
            
            std::cout << "\nBehavioral change:\n";
            if (rewardChange > 0) {
                std::cout << "  Reward INCREASED by " << rewardChange << " (positive learning)\n";
            } else if (rewardChange < -5) {
                std::cout << "  Reward DECREASED by " << -rewardChange << " (possible exploration or extinction)\n";
            }
            
            if (std::abs(actionChange) > 10) {
                std::cout << "  Forward actions changed by " << actionChange << "\n";
            }
            
            // Synapse change
            if (last.totalSynapses != first.totalSynapses) {
                std::cout << "  Synapses: " << first.totalSynapses << " -> " << last.totalSynapses << "\n";
            }
        }
        
        std::cout << "\nPhase 3 demonstration complete.\n";
        std::cout << "The brain received sensory input, produced actions,\n";
        std::cout << "experienced consequences, and modified its synapses.\n";
    }
    
private:
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
        config->set("random_seed", static_cast<int64_t>(42));
        config->set("simulation_timestep", 0.001);
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
    }
};

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

} // namespace nlm