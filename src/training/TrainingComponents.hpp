#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <chrono>

// Include NLM-specific headers
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../agent/AgentBrain.hpp"
#include "../environment/Environment.hpp"
#include "../experiments/Experiment.hpp"
#include "../performance/CheckpointSystem.hpp"

namespace nlm {

// Training configuration classes
class SimpleTrainingConfig {
public:
    SimpleTrainingConfig() = default;
    ~SimpleTrainingConfig() = default;
    
    // RL pattern configurations
    struct RLPattern {
        float gamma = 0.99f;        // Discount factor
        float epsilon = 0.1f;       // Exploration rate
        float learningRate = 0.001f; // Learning rate
        int maxSteps = 1000;        // Max steps per episode
        int updateInterval = 10;     // Network update frequency
        bool usePrioritizedExperience = false;
    };
    
    // Supervised learning pattern
    struct SupervisedPattern {
        int epochs = 100;
        float learningRate = 0.001f;
        float validationSplit = 0.2f;
        bool earlyStopping = true;
        int patience = 10;
    };
    
    // Unsupervised learning pattern
    struct UnsupervisedPattern {
        int epochs = 50;
        float reconstructionLossWeight = 1.0f;
        float regularizationWeight = 0.01f;
        bool useNegativeSamples = true;
    };
    
    // Hybrid training pattern
    struct HybridPattern {
        float rlWeight = 0.7f;      // Weight for RL component
        float slWeight = 0.3f;      // Weight for SL component
        int interleavedEpisodes = 10; // Episodes per interleaved training
    };
    
    // Pattern selection
    enum PatternType {
        REINFORCEMENT_LEARNING,
        SUPERVISED_LEARNING,
        UNSUPERVISED_LEARNING,
        HYBRID
    } patternType = REINFORCEMENT_LEARNING;
    
    // Pattern-specific configurations
    RLPattern rlPattern;
    SupervisedPattern slPattern;
    UnsupervisedPattern usPattern;
    HybridPattern hybridPattern;
    
    // Common configuration parameters
    int maxEpisodes = 1000;
    float targetSuccessRate = 0.95f;
    float targetPerformance = 100.0f;
    bool saveCheckpoints = true;
    int checkpointInterval = 100;
    
    // Environment interaction
    float timeStep = 0.1f;
    bool enableDebugging = false;
    
    // Methods to enable/disable patterns
    void enableRLPattern() { patternType = REINFORCEMENT_LEARNING; }
    void enableSLPattern() { patternType = SUPERVISED_LEARNING; }
    void enableUSPattern() { patternType = UNSUPERVISED_LEARNING; }
    void enableHybridPattern() { patternType = HYBRID; }
    
    PatternType getPatternType() const { return patternType; }
};

// Advanced training features
class AdvancedTrainingFeatures {
public:
    AdvancedTrainingFeatures() = default;
    ~AdvancedTrainingFeatures() = default;
    
    // Curiosity-driven exploration
    struct CuriosityConfig {
        float explorationBonus = 0.1f;
        bool enableIntrinsicMotivation = true;
        float noveltyThreshold = 0.5f;
        float predictionErrorWeight = 0.5f;
    } curiosityConfig;
    
    // Curriculum learning
    struct CurriculumConfig {
        bool enableCurriculum = false;
        int initialDifficulty = 1;
        int maxDifficulty = 10;
        float successThreshold = 0.8f;
        float difficultyIncrement = 0.1f;
    } curriculumConfig;
    
    // Transfer learning
    struct TransferConfig {
        bool enableTransfer = false;
        std::string sourceTask = "";
        float transferWeight = 0.5f;
        bool fineTuneLastLayers = true;
    } transferConfig;
    
    // Meta-learning
    struct MetaLearningConfig {
        bool enableMetaLearning = false;
        int metaSteps = 10;
        float metaLearningRate = 0.01f;
        float taskDiversityWeight = 0.5f;
    } metaLearningConfig;
    
    // Methods to enable advanced features
    void enableCuriosityDrivenExploration() { curiosityConfig.enableIntrinsicMotivation = true; }
    void enableCurriculumLearning() { curriculumConfig.enableCurriculum = true; }
    void enableTransferLearning(const std::string& sourceTask) { 
        transferConfig.enableTransfer = true; 
        transferConfig.sourceTask = sourceTask;
    }
    void enableMetaLearning() { metaLearningConfig.enableMetaLearning = true; }
    
    // Get current configuration
    bool isCuriosityEnabled() const { return curiosityConfig.enableIntrinsicMotivation; }
    bool isCurriculumEnabled() const { return curriculumConfig.enableCurriculum; }
    bool isTransferEnabled() const { return transferConfig.enableTransfer; }
    bool isMetaLearningEnabled() const { return metaLearningConfig.enableMetaLearning; }
};

// Callback implementations
class SimpleTrainingCallback : public TrainingCallback {
public:
    SimpleTrainingCallback() = default;
    ~SimpleTrainingCallback() = default;
    
    // Override pure virtual functions
    void afterEpisode(int episode, float reward, bool success) override {
        if (success) {
            std::cout << "Episode " << episode << " succeeded with reward: " << reward << std::endl;
        }
    }
    
    void afterStep(int episode, int step, float reward, float loss) override {
        if (step % 100 == 0) {
            std::cout << "Episode " << episode << ", Step " << step 
                      << ", Reward: " << reward << ", Loss: " << loss << std::endl;
        }
    }
    
    void onError(const std::string& error) override {
        std::cerr << "Training error: " << error << std::endl;
    }
    
    void onTrainingStart() override {
        std::cout << "Training started" << std::endl;
    }
    
    void onTrainingEnd() override {
        std::cout << "Training ended" << std::endl;
    }
    
    void onCheckpointSaved(const std::string& path) override {
        std::cout << "Checkpoint saved to: " << path << std::endl;
    }
};

// Common training pattern implementations
class CommonTrainingPatterns {
public:
    CommonTrainingPatterns() = default;
    ~CommonTrainingPatterns() = default;
    
    // Reinforcement learning pattern implementation
    static bool runReinforcementLearningPattern(
        std::shared_ptr<TrainingCore> trainer,
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        const SimpleTrainingConfig::RLPattern& config,
        std::shared_ptr<TrainingCallback> callback) {
        
        std::cout << "Running Reinforcement Learning Pattern" << std::endl;
        
        TrainingConfig trainingConfig;
        trainingConfig.maxEpisodes = 1000;
        trainingConfig.learningRate = config.learningRate;
        trainingConfig.discountFactor = config.gamma;
        trainingConfig.explorationRate = config.epsilon;
        
        // Enable advanced features
        if (agent->isCuriosityEnabled()) {
            // Curiosity-driven exploration
            trainer->addCuriosityDrivenExplorationPattern(brain, world);
        }
        
        return trainer->trainAgent(brain, world, agent, trainingConfig, {callback});
    }
    
    // Supervised learning pattern implementation
    static bool runSupervisedLearningPattern(
        std::shared_ptr<TrainingCore> trainer,
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        const SimpleTrainingConfig::SupervisedPattern& config,
        std::shared_ptr<TrainingCallback> callback) {
        
        std::cout << "Running Supervised Learning Pattern" << std::endl;
        
        TrainingConfig trainingConfig;
        trainingConfig.maxEpisodes = config.epochs;
        trainingConfig.learningRate = config.learningRate;
        
        // TODO: Implement supervised learning specific logic
        return trainer->trainAgent(brain, world, agent, trainingConfig, {callback});
    }
    
    // Unsupervised learning pattern implementation
    static bool runUnsupervisedLearningPattern(
        std::shared_ptr<TrainingCore> trainer,
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        const SimpleTrainingConfig::UnsupervisedPattern& config,
        std::shared_ptr<TrainingCallback> callback) {
        
        std::cout << "Running Unsupervised Learning Pattern" << std::endl;
        
        TrainingConfig trainingConfig;
        trainingConfig.maxEpisodes = config.epochs;
        
        // TODO: Implement unsupervised learning specific logic
        return trainer->trainAgent(brain, world, agent, trainingConfig, {callback});
    }
    
    // Hybrid training pattern implementation
    static bool runHybridTrainingPattern(
        std::shared_ptr<TrainingCore> trainer,
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        const SimpleTrainingConfig::HybridPattern& config,
        std::shared_ptr<TrainingCallback> callback) {
        
        std::cout << "Running Hybrid Training Pattern" << std::endl;
        
        TrainingConfig trainingConfig;
        trainingConfig.maxEpisodes = 500;
        
        // TODO: Implement hybrid training specific logic
        return trainer->trainAgent(brain, world, agent, trainingConfig, {callback});
    }
};

// Save/Load training module
class TrainingPersistence {
public:
    TrainingPersistence() = default;
    ~TrainingPersistence() = default;
    
    // Save training session
    static bool saveTrainingSession(
        const std::string& filepath,
        std::shared_ptr<TrainingCore> trainer,
        std::shared_ptr<Brain> brain,
        std::shared_ptr<AgentBrain> agent,
        int episode,
        const std::vector<std::shared_ptr<Experiment>>& experiments) {
        
        try {
            // Create checkpoint file
            std::ofstream file(filepath, std::ios::binary | std::ios::trunc);
            if (!file) {
                std::cerr << "Failed to create checkpoint file: " << filepath << std::endl;
                return false;
            }
            
            // Save metadata
            uint32_t version = 1;
            file.write(reinterpret_cast<const char*>(&version), sizeof(uint32_t));
            
            int32_t episodeCount = static_cast<int32_t>(experiments.size());
            file.write(reinterpret_cast<const char*>(&episodeCount), sizeof(int32_t));
            
            // Save experiment data
            for (const auto& experiment : experiments) {
                // Save experiment metadata
                std::string name = experiment->getName();
                uint64_t seed = experiment->getSeed();
                
                int32_t nameLength = static_cast<int32_t>(name.length());
                file.write(reinterpret_cast<const char*>(&nameLength), sizeof(int32_t));
                file.write(name.c_str(), nameLength);
                
                file.write(reinterpret_cast<const char*>(&seed), sizeof(uint64_t));
            }
            
            // Save brain state using CheckpointSystem
            if (CheckpointSystem::saveBrainState(file, brain)) {
                std::cout << "Brain state saved to: " << filepath << std::endl;
            } else {
                std::cerr << "Failed to save brain state" << std::endl;
                return false;
            }
            
            // Save agent state
            if (agent->saveToFile(file)) {
                std::cout << "Agent state saved to: " << filepath << std::endl;
            } else {
                std::cerr << "Failed to save agent state" << std::endl;
                return false;
            }
            
            // Save current episode
            file.write(reinterpret_cast<const char*>(&episode), sizeof(int));
            
            file.close();
            
            std::cout << "Training session saved to: " << filepath << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Error saving training session: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Load training session
    static bool loadTrainingSession(
        const std::string& filepath,
        std::shared_ptr<TrainingCore> trainer,
        std::shared_ptr<Brain> brain,
        std::shared_ptr<AgentBrain> agent,
        int& episode,
        std::vector<std::shared_ptr<Experiment>>& experiments) {
        
        try {
            std::ifstream file(filepath, std::ios::binary);
            if (!file) {
                std::cerr << "Failed to open checkpoint file: " << filepath << std::endl;
                return false;
            }
            
            // Read metadata
            uint32_t version;
            file.read(reinterpret_cast<char*>(&version), sizeof(uint32_t));
            
            int32_t episodeCount;
            file.read(reinterpret_cast<char*>(&episodeCount), sizeof(int32_t));
            
            // Load experiment data
            experiments.clear();
            for (int32_t i = 0; i < episodeCount; ++i) {
                // Read experiment metadata
                int32_t nameLength;
                file.read(reinterpret_cast<char*>(&nameLength), sizeof(int32_t));
                
                std::string name(nameLength, ' ');
                file.read(&name[0], nameLength);
                
                uint64_t seed;
                file.read(reinterpret_cast<char*>(&seed), sizeof(uint64_t));
                
                // Create experiment
                auto experiment = std::make_shared<Experiment>(name);
                experiment->setSeed(seed);
                experiments.push_back(experiment);
            }
            
            // Load brain state
            if (CheckpointSystem::loadBrainState(file, brain)) {
                std::cout << "Brain state loaded from: " << filepath << std::endl;
            } else {
                std::cerr << "Failed to load brain state" << std::endl;
                return false;
            }
            
            // Load agent state
            if (agent->loadFromFile(file)) {
                std::cout << "Agent state loaded from: " << filepath << std::endl;
            } else {
                std::cerr << "Failed to load agent state" << std::endl;
                return false;
            }
            
            // Load current episode
            file.read(reinterpret_cast<char*>(&episode), sizeof(int));
            
            file.close();
            
            std::cout << "Training session loaded from: " << filepath 
                      << " (resuming from episode " << episode << ")" << std::endl;
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Error loading training session: " << e.what() << std::endl;
            return false;
        }
    }
};

} // namespace nlm
