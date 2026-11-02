// Advanced Neural Simulation Features
// Real-world agent-based simulation with advanced learning capabilities

#include "agent/AgentBody.hpp"
#include "agent/AgentBrain.hpp"
#include "brain/Brain.hpp"
#include "world/SimpleWorld.hpp"
#include "core/Logger/Logger.hpp"
#include <memory>
#include <vector>
#include <chrono>
#include <random>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>
#include <algorithm>
#include <unordered_map>

using namespace std::chrono;

namespace nlm {

class AdvancedAgent {
public:
    struct LearningRecord {
        double timestamp;
        std::string action;
        float reward;
        float energy;
        std::vector<float> sensoryState;
        float curiosity;
        float novelty;
        float confidence;
        
        LearningRecord(double ts, const std::string& act, float r, float e,
                      const std::vector<float>& sensory, float cur, float nov, float conf)
            : timestamp(ts), action(act), reward(r), energy(e), sensoryState(sensory),
              curiosity(cur), novelty(nov), confidence(conf) {}
    };
    
    struct PerformanceMetrics {
        double totalSteps;
        double totalActions;
        double totalReward;
        double averageEnergy;
        double explorationRate;
        double learningEfficiency;
        double survivalRate;
        std::map<std::string, double> actionStatistics;
        
        PerformanceMetrics() : totalSteps(0), totalActions(0), totalReward(0),
                              averageEnergy(0), explorationRate(0), learningEfficiency(0),
                              survivalRate(0) {}
    };
    
    AdvancedAgent(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world)
        : brain_(brain), world_(world), agentBody_(), agentBrain_(brain),
          learningEnabled_(true), adaptationEnabled_(true),
          socialLearningEnabled_(false), curiosityEnabled_(true),
          developmentEnabled_(true), currentStep_(0), episodeCount_(0),
          survivalSteps_(0), maxSurvivalSteps_(1000) {}
    
    // Initialize agent in world
    bool initialize() {
        NLM_LOG_INFO("Initializing Advanced Agent...");
        
        // Place agent in world
        agentBody_.reset(new AgentBody());
        agentBody_->x = world_->getWidth() * 0.5f;
        agentBody_->y = world_->getHeight() * 0.5f;
        agentBody_->orientation = 0.0f;
        agentBody_->energy = 1.0f;
        agentBody_->health = 1.0f;
        agentBody_->age = 0.0f;
        agentBody_->isMoving = false;
        agentBody_->isTurning = false;
        
        // Initialize agent brain
        agentBrain_->initialize(*world_);
        
        // Reset learning state
        learningHistory_.clear();
        currentActions_.clear();
        
        // Reset performance metrics
        performanceMetrics_ = PerformanceMetrics();
        
        NLM_LOG_INFO("Advanced Agent initialized successfully");
        return true;
    }
    
    // Run one simulation step
    bool step(double timestep) {
        currentStep_++;
        performanceMetrics_.totalSteps++;
        
        // Update agent development
        if (developmentEnabled_) {
            agentBrain_->updateDevelopment(timestep);
            agentBody_->age += timestep;
            
            // Reduce energy over time (metabolic cost)
            agentBody_->energy = std::max(0.0f, agentBody_->energy - timestep * 0.001f);
        }
        
        // Get current sensory percept
        SensoryPercept percept = world_->getPercept(agentBody_);
        
        // Process sensory input
        agentBrain_->processSensoryInput(percept);
        
        // Decode motor command
        MotorCommand motorCommand = agentBrain_->decodeMotorCommand();
        
        // Execute action
        ActionResult result = executeAction(motorCommand, timestep);
        
        // Apply reward modulation
        float reward = calculateReward(result);
        agentBrain_->applyRewardModulation(reward, agentBrain_->getPredictionError());
        
        // Record learning experience
        if (learningEnabled_) {
            LearningRecord record(
                currentStep_ * timestep,
                motorCommandToString(motorCommand),
                reward,
                agentBody_->energy,
                percept.getVision(),
                agentBrain_->getCuriosityLevel(),
                agentBrain_->getNoveltyLevel(),
                agentBrain_->getSelfModelConfidence()
            );
            learningHistory_.push_back(record);
            updatePerformanceMetrics(record);
        }
        
        // Update development based on experience
        if (adaptationEnabled_) {
            adaptBasedOnExperience(record);
        }
        
        // Check survival
        if (agentBody_->energy <= 0.0f || agentBody_->health <= 0.0f) {
            return false; // Agent died
        }
        
        return true;
    }
    
    // Execute motor command
    ActionResult executeAction(MotorCommand command, double timestep) {
        ActionResult result;
        result.reward = 0.0f;
        result.success = false;
        result.message = "Action executed";
        
        switch (command) {
            case MotorCommand::MoveForward:
                result = moveForward(timestep);
                break;
            case MotorCommand::MoveBackward:
                result = moveBackward(timestep);
                break;
            case MotorCommand::TurnLeft:
                result = turnLeft(timestep);
                break;
            case MotorCommand::TurnRight:
                result = turnRight(timestep);
                break;
            case MotorCommand::LookLeft:
                result = lookLeft(timestep);
                break;
            case MotorCommand::LookRight:
                result = lookRight(timestep);
                break;
            case MotorCommand::Interact:
                result = interact(timestep);
                break;
            case MotorCommand::Wait:
                result = wait(timestep);
                break;
        }
        
        return result;
    }
    
    // Move forward
    ActionResult moveForward(double timestep) {
        ActionResult result;
        result.success = true;
        
        float speed = 0.5f * agentBody_->energy; // Energy-dependent speed
        float dx = speed * std::cos(agentBody_->orientation);
        float dy = speed * std::sin(agentBody_->orientation);
        
        // Check for collision
        std::vector<float> newPosition = {agentBody_->x + dx, agentBody_->y + dy};
        if (world_->isValidPosition(newPosition)) {
            agentBody_->x = newPosition[0];
            agentBody_->y = newPosition[1];
            agentBody_->isMoving = true;
            agentBody_->lastActionTime = currentStep_ * 0.001f;
            
            // Consume energy
            agentBody_->energy = std::max(0.0f, agentBody_->energy - speed * 0.001f);
            
            // Check for resources
            auto objects = world_->getObjectsAt(newPosition);
            for (const auto& obj : objects) {
                if (obj.type == WorldObjectType::Resource) {
                    result.reward += 10.0f; // Resource reward
                    world_->removeObject(obj.position);
                } else if (obj.type == WorldObjectType::Hazard) {
                    result.reward -= 5.0f; // Hazard penalty
                    agentBody_->health = std::max(0.0f, agentBody_->health - 0.1f);
                }
            }
        } else {
            result.success = false;
            result.message = "Collision detected";
        }
        
        return result;
    }
    
    // Move backward
    ActionResult moveBackward(double timestep) {
        ActionResult result;
        result.success = true;
        
        float speed = 0.3f * agentBody_->energy * 0.5f; // Slower, safer
        float dx = -speed * std::cos(agentBody_->orientation);
        float dy = -speed * std::sin(agentBody_->orientation);
        
        std::vector<float> newPosition = {agentBody_->x + dx, agentBody_->y + dy};
        if (world_->isValidPosition(newPosition)) {
            agentBody_->x = newPosition[0];
            agentBody_->y = newPosition[1];
            agentBody_->isMoving = true;
            
            // Less energy consumption for backward movement
            agentBody_->energy = std::max(0.0f, agentBody_->energy - speed * 0.0005f);
        } else {
            result.success = false;
            result.message = "Collision detected";
        }
        
        return result;
    }
    
    // Turn left
    ActionResult turnLeft(double timestep) {
        ActionResult result;
        result.success = true;
        
        float turnSpeed = 0.5f * agentBody_->energy;
        agentBody_->orientation -= turnSpeed * timestep;
        agentBody_->isTurning = true;
        agentBody_->angularVelocity = -turnSpeed;
        
        return result;
    }
    
    // Turn right
    ActionResult turnRight(double timestep) {
        ActionResult result;
        result.success = true;
        
        float turnSpeed = 0.5f * agentBody_->energy;
        agentBody_->orientation += turnSpeed * timestep;
        agentBody_->isTurning = true;
        agentBody_->angularVelocity = turnSpeed;
        
        return result;
    }
    
    // Look left (sensor activation)
    ActionResult lookLeft(double timestep) {
        ActionResult result;
        result.success = true;
        result.reward = 0.0f; // No immediate reward
        
        // Adjust sensory field for looking
        // This would be implemented based on world sensor system
        // For now, just record the action
        agentBrain_->observeOtherAgent(MotorCommand::LookLeft, {}, {});
        
        return result;
    }
    
    // Look right (sensor activation)
    ActionResult lookRight(double timestep) {
        ActionResult result;
        result.success = true;
        result.reward = 0.0f;
        
        agentBrain_->observeOtherAgent(MotorCommand::LookRight, {}, {});
        
        return result;
    }
    
    // Interact with environment
    ActionResult interact(double timestep) {
        ActionResult result;
        result.success = true;
        
        // Check for objects in interaction range
        std::vector<float> agentPos = {agentBody_->x, agentBody_->y};
        auto nearbyObjects = world_->getObjectsInRange(agentPos, 1.0f);
        
        if (!nearbyObjects.empty()) {
            // Interact with closest object
            auto closest = *std::min_element(nearbyObjects.begin(), nearbyObjects.end(),
                [](const WorldObject& a, const WorldObject& b) {
                    float distA = std::sqrt((a.position[0] - agentPos[0]) * (a.position[0] - agentPos[0]) +
                                          (a.position[1] - agentPos[1]) * (a.position[1] - agentPos[1]));
                    float distB = std::sqrt((b.position[0] - agentPos[0]) * (b.position[0] - agentPos[0]) +
                                          (b.position[1] - agentPos[1]) * (b.position[1] - agentPos[1]));
                    return distA < distB;
                });
            
            if (closest.type == WorldObjectType::Resource) {
                result.reward = 15.0f; // High reward for resource collection
                world_->removeObject(closest.position);
                result.message = "Resource collected";
            } else if (closest.type == WorldObjectType::Hazard) {
                result.reward = -10.0f; // High penalty for hazard contact
                agentBody_->health = std::max(0.0f, agentBody_->health - 0.3f);
                result.message = "Hazard encountered";
            } else if (closest.type == WorldObjectType::Marker) {
                result.reward = 5.0f; // Minor reward for finding marker
                result.message = "Navigation aid found";
            }
        } else {
            result.success = false;
            result.message = "No objects to interact with";
        }
        
        return result;
    }
    
    // Wait (no action)
    ActionResult wait(double timestep) {
        ActionResult result;
        result.success = true;
        
        // Consume energy more slowly
        agentBody_->energy = std::max(0.0f, agentBody_->energy - timestep * 0.0005f);
        
        // Natural recovery of some energy
        agentBody_->energy = std::min(1.0f, agentBody_->energy + timestep * 0.0002f);
        
        return result;
    }
    
    // Calculate reward for action
    float calculateReward(const ActionResult& result) {
        float baseReward = result.reward;
        
        // Apply curiosity bonus
        if (curiosityEnabled_) {
            baseReward += agentBrain_->getCuriosityLevel() * 2.0f;
        }
        
        // Apply novelty bonus
        baseReward += agentBrain_->getNoveltyLevel() * 1.0f;
        
        // Apply developmental modifier
        if (developmentEnabled_) {
            auto stage = agentBrain_->getDevelopmentalStage();
            float plasticityModifier = 1.0f;
            switch (stage) {
                case DevelopmentalStage::Initial: plasticityModifier = 1.5f; break;
                case DevelopmentalStage::CriticalPeriod: plasticityModifier = 1.2f; break;
                case DevelopmentalStage::Maturation: plasticityModifier = 0.8f; break;
                case DevelopmentalStage::Adult: plasticityModifier = 0.5f; break;
            }
            baseReward *= plasticityModifier;
        }
        
        return baseReward;
    }
    
    // Convert motor command to string
    std::string motorCommandToString(MotorCommand command) {
        switch (command) {
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
    
    // Adapt based on experience
    void adaptBasedOnExperience(const LearningRecord& record) {
        // Modify brain parameters based on experience
        float learningRate = 0.01f;
        
        // Adjust curiosity threshold based on experience
        if (record.novelty > 0.5f && record.reward > 5.0f) {
            // High novelty with positive reward increases curiosity
            // This would modify internal brain parameters
        }
        
        // Adjust exploration rate based on learning efficiency
        if (record.confidence > 0.8f) {
            // High confidence reduces need for exploration
            // This would affect curiosity levels
        }
        
        // Update developmental stage based on experience
        if (record.energy < 0.1f) {
            // Low energy may trigger developmental responses
            // This would affect plasticity
        }
    }
    
    // Update performance metrics
    void updatePerformanceMetrics(const LearningRecord& record) {
        performanceMetrics_.totalActions++;
        performanceMetrics_.totalReward += record.reward;
        
        // Update action statistics
        performanceMetrics_.actionStatistics[record.action] += 1.0f;
        
        // Calculate average energy
        static double energySum = 0.0;
        static size_t energyCount = 0;
        energySum += record.energy;
        energyCount++;
        performanceMetrics_.averageEnergy = energySum / energyCount;
        
        // Calculate exploration rate
        double explorationActions = 0.0;
        for (const auto& action : performanceMetrics_.actionStatistics) {
            if (action.first.find("Turn") != std::string::npos || 
                action.first.find("Look") != std::string::npos) {
                explorationActions += action.second;
            }
        }
        performanceMetrics_.explorationRate = explorationActions / performanceMetrics_.totalActions;
        
        // Calculate learning efficiency
        double totalExperience = performanceMetrics_.totalActions;
        double learningGain = performanceMetrics_.totalReward;
        performanceMetrics_.learningEfficiency = learningGain / std::max(totalExperience, 1.0);
        
        // Calculate survival rate
        survivalSteps_++;
        performanceMetrics_.survivalRate = static_cast<double>(survivalSteps_) / maxSurvivalSteps_;
    }
    
    // Get agent state
    struct AgentState {
        float x, y, orientation;
        float energy, health, age;
        bool isMoving, isTurning;
        double lastActionTime;
        std::vector<LearningRecord> learningHistory;
        PerformanceMetrics performance;
    };
    
    AgentState getState() const {
        AgentState state;
        state.x = agentBody_->x;
        state.y = agentBody_->y;
        state.orientation = agentBody_->orientation;
        state.energy = agentBody_->energy;
        state.health = agentBody_->health;
        state.age = agentBody_->age;
        state.isMoving = agentBody_->isMoving;
        state.isTurning = agentBody_->isTurning;
        state.lastActionTime = agentBody_->lastActionTime;
        state.learningHistory = learningHistory_;
        state.performance = performanceMetrics_;
        return state;
    }
    
    // Set agent state (for checkpointing)
    void setState(const AgentState& state) {
        agentBody_->x = state.x;
        agentBody_->y = state.y;
        agentBody_->orientation = state.orientation;
        agentBody_->energy = state.energy;
        agentBody_->health = state.health;
        agentBody_->age = state.age;
        agentBody_->isMoving = state.isMoving;
        agentBody_->isTurning = state.isTurning;
        agentBody_->lastActionTime = state.lastActionTime;
        
        learningHistory_ = state.learningHistory;
        performanceMetrics_ = state.performance;
        
        // Note: AgentBrain state would need to be managed separately
    }
    
    // Save learning history to file
    bool saveLearningHistory(const std::string& filepath) {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "Learning History for Advanced Agent\n";
        file << "=====================================\n\n";
        
        file << "Performance Metrics:\n";
        file << "  Total Steps: " << performanceMetrics_.totalSteps << "\n";
        file << "  Total Actions: " << performanceMetrics_.totalActions << "\n";
        file << "  Total Reward: " << performanceMetrics_.totalReward << "\n";
        file << "  Average Energy: " << performanceMetrics_.averageEnergy << "\n";
        file << "  Exploration Rate: " << performanceMetrics_.explorationRate << "\n";
        file << "  Learning Efficiency: " << performanceMetrics_.learningEfficiency << "\n";
        file << "  Survival Rate: " << performanceMetrics_.survivalRate << "\n\n";
        
        file << "Action Statistics:\n";
        for (const auto& action : performanceMetrics_.actionStatistics) {
            file << "  " << action.first << ": " << action.second << "\n";
        }
        
        file << "\nLearning Episodes:\n";
        for (size_t i = 0; i < learningHistory_.size(); ++i) {
            const auto& record = learningHistory_[i];
            file << "Episode " << i << ":\n";
            file << "  Timestamp: " << record.timestamp << "\n";
            file << "  Action: " << record.action << "\n";
            file << "  Reward: " << record.reward << "\n";
            file << "  Energy: " << record.energy << "\n";
            file << "  Curiosity: " << record.curiosity << "\n";
            file << "  Novelty: " << record.novelty << "\n";
            file << "  Confidence: " << record.confidence << "\n\n";
        }
        
        NLM_LOG_INFO("Learning history saved to: " + filepath);
        return true;
    }
    
    // Load learning history from file
    bool loadLearningHistory(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        LearningRecord* currentRecord = nullptr;
        size_t currentEpisode = 0;
        
        while (std::getline(file, line)) {
            if (line.find("Episode ") == 0) {
                // Start of new episode
                currentRecord = new LearningRecord(0.0, "", 0.0f, 0.0f, {}, 0.0f, 0.0f, 0.0f);
                currentEpisode++;
            } else if (line.find("  Timestamp: ") == 0) {
                if (currentRecord) {
                    currentRecord->timestamp = std::stod(line.substr(14));
                }
            } else if (line.find("  Action: ") == 0) {
                if (currentRecord) {
                    currentRecord->action = line.substr(10);
                }
            } else if (line.find("  Reward: ") == 0) {
                if (currentRecord) {
                    currentRecord->reward = std::stof(line.substr(10));
                }
            } else if (line.find("  Energy: ") == 0) {
                if (currentRecord) {
                    currentRecord->energy = std::stof(line.substr(10));
                }
            } else if (line.find("  Curiosity: ") == 0) {
                if (currentRecord) {
                    currentRecord->curiosity = std::stof(line.substr(12));
                }
            } else if (line.find("  Novelty: ") == 0) {
                if (currentRecord) {
                    currentRecord->novelty = std::stof(line.substr(11));
                }
            } else if (line.find("  Confidence: ") == 0) {
                if (currentRecord) {
                    currentRecord->confidence = std::stof(line.substr(13));
                    learningHistory_.push_back(*currentRecord);
                    delete currentRecord;
                    currentRecord = nullptr;
                }
            }
        }
        
        NLM_LOG_INFO("Learning history loaded from: " + filepath);
        return true;
    }
    
    // Get agent brain
    AgentBrain* getAgentBrain() {
        return &agentBrain_;
    }
    
    // Get agent body
    AgentBody* getAgentBody() {
        return agentBody_.get();
    }
    
    // Get brain
    Brain* getBrain() {
        return brain_.get();
    }
    
    // Get world
    SimpleWorld* getWorld() {
        return world_.get();
    }
    
    // Reset agent
    void reset() {
        currentStep_ = 0;
        episodeCount_ = 0;
        survivalSteps_ = 0;
        
        // Reset learning history
        learningHistory_.clear();
        
        // Reset performance metrics
        performanceMetrics_ = PerformanceMetrics();
        
        // Reset agent body
        if (agentBody_) {
            agentBody_->reset();
        }
        
        NLM_LOG_INFO("Advanced Agent reset completed");
    }
    
    // Generate report
    std::string generateReport() {
        std::stringstream report;
        
        report << "╔═══════════════════════════════════════════════════════════════╗\n";
        report << "║                    ADVANCED AGENT REPORT                           ║\n";
        report << "╚═══════════════════════════════════════════════════════════════╝\n\n";
        
        report << "Agent State:\n";
        report << "  Position: (" << agentBody_->x << ", " << agentBody_->y << ")\n";
        report << "  Orientation: " << agentBody_->orientation << " radians\n";
        report << "  Energy: " << agentBody_->energy << " / " << 1.0f << "\n";
        report << "  Health: " << agentBody_->health << " / " << 1.0f << "\n";
        report << "  Age: " << agentBody_->age << " steps\n";
        report << "  Moving: " << (agentBody_->isMoving ? "Yes" : "No") << "\n";
        report << "  Turning: " << (agentBody_->isTurning ? "Yes" : "No") << "\n\n";
        
        report << "Performance Metrics:\n";
        report << "  Total Steps: " << performanceMetrics_.totalSteps << "\n";
        report << "  Total Actions: " << performanceMetrics_.totalActions << "\n";
        report << "  Total Reward: " << performanceMetrics_.totalReward << "\n";
        report << "  Average Energy: " << performanceMetrics_.averageEnergy << "\n";
        report << "  Exploration Rate: " << performanceMetrics_.explorationRate << "\n";
        report << "  Learning Efficiency: " << performanceMetrics_.learningEfficiency << "\n";
        report << "  Survival Rate: " << performanceMetrics_.survivalRate << "\n\n";
        
        report << "Action Statistics:\n";
        for (const auto& action : performanceMetrics_.actionStatistics) {
            report << "  " << action.first << ": " << action.second << " ("
                   << (action.second / performanceMetrics_.totalActions * 100.0) << "%)\n";
        }
        
        report << "\nLearning Experience:\n";
        report << "  Episodes: " << learningHistory_.size() << "\n";
        report << "  Current Step: " << currentStep_ << "\n";
        report << "  Episode Count: " << episodeCount_ << "\n";
        report << "  Survival Steps: " << survivalSteps_ << "\n";
        
        return report.str();
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::unique_ptr<AgentBody> agentBody_;
    AgentBrain agentBrain_;
    
    bool learningEnabled_;
    bool adaptationEnabled_;
    bool socialLearningEnabled_;
    bool curiosityEnabled_;
    bool developmentEnabled_;
    
    size_t currentStep_;
    size_t episodeCount_;
    size_t survivalSteps_;
    size_t maxSurvivalSteps_;
    
    std::vector<LearningRecord> learningHistory_;
    PerformanceMetrics performanceMetrics_;
};

class AdvancedSimulation {
public:
    struct SimulationConfig {
        std::string worldType;
        std::string agentType;
        std::string learningMode;
        double timestep;
        double duration;
        int numAgents;
        bool enableMultiAgent;
        bool enableEnvironmentInteraction;
        bool enableSocialLearning;
        bool enableDevelopment;
        std::string outputFile;
        bool saveAgentStates;
        bool saveLearningHistory;
        bool savePerformanceMetrics;
    };
    
    AdvancedSimulation() : running(false), simulationTime(0.0), stepCount(0) {}
    
    // Configure simulation
    void configure(const SimulationConfig& config) {
        config_ = config;
        NLM_LOG_INFO("Advanced Simulation configured");
    }
    
    // Add agent to simulation
    void addAgent(std::unique_ptr<AdvancedAgent> agent) {
        agents_.push_back(std::move(agent));
        NLM_LOG_INFO("Agent added to simulation (total: " + std::to_string(agents_.size()) + ")");
    }
    
    // Start simulation
    void start() {
        if (agents_.empty()) {
            NLM_LOG_ERROR("No agents to simulate");
            return;
        }
        
        running = true;
        startTime = high_resolution_clock::now();
        
        NLM_LOG_INFO("Advanced Simulation started");
        NLM_LOG_INFO("  Agents: " + std::to_string(agents_.size()) << "\n";
        NLM_LOG_INFO("  World Type: " << config_.worldType << "\n";
        NLM_LOG_INFO("  Learning Mode: " << config_.learningMode << "\n";
        NLM_LOG_INFO("  Duration: " << config_.duration << " seconds\n";
        
        // Initialize agents
        for (auto& agent : agents_) {
            if (!agent->initialize()) {
                NLM_LOG_ERROR("Failed to initialize agent");
            }
        }
        
        // Run simulation loop
        while (running && simulationTime < config_.duration) {
            step(config_.timestep);
        }
        
        stop();
    }
    
    // Run one simulation step
    void step(double timestep) {
        if (!running) return;
        
        // Update all agents
        for (auto& agent : agents_) {
            // Execute agent behavior
            agent->step(timestep);
            
            // Check if agent is still alive
            if (agent->getAgentBody()->energy <= 0.0f || 
                agent->getAgentBody()->health <= 0.0f) {
                // Agent died - remove from simulation
                agent.reset();
            }
        }
        
        // Remove dead agents
        agents_.erase(
            std::remove_if(agents_.begin(), agents_.end(),
                [](const std::unique_ptr<AdvancedAgent>& agent) {
                    return agent == nullptr;
                }),
            agents_.end()
        );
        
        // Update simulation time
        simulationTime += timestep;
        stepCount++;
        
        // Log progress periodically
        if (stepCount % 100 == 0) {
            NLM_LOG_INFO("Step " << stepCount << ", Time: " << simulationTime << "s, Agents: " << agents_.size());
        }
        
        // Check if simulation should end
        if (simulationTime >= config_.duration || agents_.empty()) {
            running = false;
        }
    }
    
    // Stop simulation
    void stop() {
        running = false;
        auto endTime = high_resolution_clock::now();
        double duration = duration<double>(endTime - startTime).count();
        
        NLM_LOG_INFO("Advanced Simulation stopped");
        NLM_LOG_INFO("  Total Duration: " << duration << " seconds");
        NLM_LOG_INFO("  Total Steps: " << stepCount);
        NLM_LOG_INFO("  Final Agents: " << agents_.size());
        
        // Generate final reports
        generateFinalReports();
        
        // Save agent states if requested
        if (config_.saveAgentStates) {
            saveAgentStates();
        }
        
        // Save learning history if requested
        if (config_.saveLearningHistory) {
            saveLearningHistory();
        }
        
        // Save performance metrics if requested
        if (config_.savePerformanceMetrics) {
            savePerformanceMetrics();
        }
    }
    
    // Generate final reports
    void generateFinalReports() {
        NLM_LOG_INFO("╔═══════════════════════════════════════════════════════════════╗");
        NLM_LOG_INFO("║                    SIMULATION FINAL REPORTS                        ║");
        NLM_LOG_INFO("╚═══════════════════════════════════════════════════════════════╝");
        
        for (size_t i = 0; i < agents_.size(); ++i) {
            const auto& agent = agents_[i];
            if (agent) {
                NLM_LOG_INFO("\nAgent " << i << " Report:");
                NLM_LOG_INFO(agent->generateReport().c_str());
            }
        }
        
        // Generate summary report
        NLM_LOG_INFO("\nSimulation Summary:");
        NLM_LOG_INFO("  Total Agents: " << agents_.size());
        NLM_LOG_INFO("  Total Steps: " << stepCount);
        NLM_LOG_INFO("  Total Simulation Time: " << simulationTime << "s");
        
        double averageStepsPerAgent = agents_.empty() ? 0.0 : 
            static_cast<double>(stepCount) / agents_.size();
        NLM_LOG_INFO("  Average Steps per Agent: " << averageStepsPerAgent);
    }
    
    // Save agent states
    void saveAgentStates() {
        NLM_LOG_INFO("Saving agent states to: " << config_.outputFile << "_agents.json");
        
        // Create JSON structure for agent states
        std::stringstream jsonStream;
        jsonStream << "{\n";
        jsonStream << "  \"agents\": [\n";
        
        for (size_t i = 0; i < agents_.size(); ++i) {
            const auto& agent = agents_[i];
            if (agent) {
                auto state = agent->getState();
                jsonStream << "    {\n";
                jsonStream << "      \"id\": " << i << ",\n";
                jsonStream << "      \"position\": [" << state.x << ", " << state.y << "],\n";
                jsonStream << "      \"orientation\": " << state.orientation << ",\n";
                jsonStream << "      \"energy\": " << state.energy << ",\n";
                jsonStream << "      \"health\": " << state.health << ",\n";
                jsonStream << "      \"age\": " << state.age << ",\n";
                jsonStream << "      \"learningEpisodes\": " << state.learningHistory.size() << "\n";
                
                if (i < agents_.size() - 1) {
                    jsonStream << "    },\n";
                } else {
                    jsonStream << "    }\n";
                }
            }
        }
        
        jsonStream << "  ],\n";
        jsonStream << "  \"simulationTime\": " << simulationTime << ",\n";
        jsonStream << "  \"stepCount\": " << stepCount << "\n";
        jsonStream << "}\n";
        
        // Write to file
        std::ofstream file(config_.outputFile + "_agents.json");
        if (file.is_open()) {
            file << jsonStream.str();
            NLM_LOG_INFO("Agent states saved successfully");
        } else {
            NLM_LOG_ERROR("Failed to save agent states");
        }
    }
    
    // Save learning history
    void saveLearningHistory() {
        NLM_LOG_INFO("Saving learning history to: " << config_.outputFile << "_learning.json");
        
        for (size_t i = 0; i < agents_.size(); ++i) {
            const auto& agent = agents_[i];
            if (agent) {
                std::string filename = config_.outputFile + "_agent_" + std::to_string(i) + "_learning.json";
                agent->saveLearningHistory(filename);
            }
        }
    }
    
    // Save performance metrics
    void savePerformanceMetrics() {
        NLM_LOG_INFO("Saving performance metrics to: " << config_.outputFile << "_performance.json");
        
        // Create performance metrics JSON
        std::stringstream jsonStream;
        jsonStream << "{\n";
        jsonStream << "  \"simulationTime\": " << simulationTime << ",\n";
        jsonStream << "  \"stepCount\": " << stepCount << ",\n";
        jsonStream << "  \"agentCount\": " << agents_.size() << ",\n";
        jsonStream << "  \"agents\": [\n";
        
        for (size_t i = 0; i < agents_.size(); ++i) {
            const auto& agent = agents_[i];
            if (agent) {
                auto state = agent->getState();
                jsonStream << "    {\n";
                jsonStream << "      \"id\": " << i << ",\n";
                jsonStream << "      \"totalSteps\": " << state.performance.totalSteps << ",\n";
                jsonStream << "      \"totalActions\": " << state.performance.totalActions << ",\n";
                jsonStream << "      \"totalReward\": " << state.performance.totalReward << ",\n";
                jsonStream << "      \"averageEnergy\": " << state.performance.averageEnergy << ",\n";
                jsonStream << "      \"explorationRate\": " << state.performance.explorationRate << ",\n";
                jsonStream << "      \"learningEfficiency\": " << state.performance.learningEfficiency << ",\n";
                jsonStream << "      \"survivalRate\": " << state.performance.survivalRate << "\n";
                
                if (i < agents_.size() - 1) {
                    jsonStream << "    },\n";
                } else {
                    jsonStream << "    }\n";
                }
            }
        }
        
        jsonStream << "  ]\n";
        jsonStream << "}\n";
        
        // Write to file
        std::ofstream file(config_.outputFile + "_performance.json");
        if (file.is_open()) {
            file << jsonStream.str();
            NLM_LOG_INFO("Performance metrics saved successfully");
        } else {
            NLM_LOG_ERROR("Failed to save performance metrics");
        }
    }
    
private:
    SimulationConfig config_;
    bool running;
    double simulationTime;
    size_t stepCount;
    high_resolution_clock::time_point startTime;
    std::vector<std::unique_ptr<AdvancedAgent>> agents_;
};

} // namespace nlm
