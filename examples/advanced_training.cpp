// NLM Advanced Training Example
// =============================

// Advanced training example demonstrating the NLM Training Module
// with comprehensive features including callbacks, monitoring, and persistence.

#include "TrainingCore.hpp"
#include "TrainingComponents.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class AdvancedTrainingExample {
public:
    void runExample() {
        std::cout << "=== NLM Advanced Training Example ===" << std::endl;
        std::cout << "This example demonstrates advanced training with comprehensive features." << std::endl;
        std::cout << std::endl;
        
        // Setup training environment
        auto trainer = std::make_shared<nlm::TrainingCore>();
        auto brain = createAdvancedBrain();
        auto world = createAdvancedWorld();
        auto agent = createAdvancedAgent(brain);
        
        // Create advanced training configuration
        auto config = createAdvancedTrainingConfig();
        
        // Setup advanced callbacks
        auto performance_callback = std::make_shared<AdvancedPerformanceCallback>();
        auto features_callback = std::make_shared<AdvancedFeaturesCallback>();
        auto monitoring_callback = std::make_shared<AdvancedMonitoringCallback>();
        
        // Enable advanced features
        enableAdvancedFeatures(brain, world, agent);
        
        std::cout << "Advanced training environment setup complete." << std::endl;
        std::cout << "Training configuration:" << std::endl;
        std::cout << "  Max Episodes: " << config->maxEpisodes << std::endl;
        std::cout << "  Learning Rate: " << config->learningRate << std::endl;
        std::cout << "  Exploration Rate: " << config->explorationRate << std::endl;
        std::cout << "  Reward Type: " << (config->rewardConfig.type == nlm::RewardConfig::Type::WEIGHTED ? "WEIGHTED" : "SIMPLE") << std::endl;
        std::cout << "  Early Stopping: " << (config->earlyStopping ? "Yes" : "No") << std::endl;
        std::cout << std::endl;
        
        // Run advanced training with monitoring
        std::cout << "Starting advanced training with comprehensive monitoring..." << std::endl;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        bool training_complete = trainer->trainAgent(brain, world, agent, config, {performance_callback, features_callback, monitoring_callback});
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
        
        // Display training results
        std::cout << std::endl;
        std::cout << "=== Training Results ===" << std::endl;
        
        auto performance_summary = performance_callback->getPerformanceSummary();
        std::cout << "Training completed: " << (training_complete ? "Yes" : "No") << std::endl;
        std::cout << "Episodes completed: " << performance_summary['episodes_completed'] << std::endl;
        std::cout << "Average reward: " << performance_summary['average_reward'] << std::endl;
        std::cout << "Best reward: " << performance_summary['best_reward'] << std::endl;
        std::cout << "Average loss: " << performance_summary['average_loss'] << std::endl;
        std::cout << "Training duration: " << duration.count() << " seconds" << std::endl;
        std::cout << "Success rate: " << performance_summary['episodes_successful'] / performance_summary['episodes_completed'] * 100 << "%" << std::endl;
        
        // Save advanced training results
        saveAdvancedTrainingResults(performance_summary, duration, trainer, brain, agent);
        
        // Generate advanced analysis
        generateAdvancedAnalysis(performance_summary, features_callback, monitoring_callback);
        
        std::cout << std::endl;
        std::cout << "✓ Advanced training example completed successfully!" << std::endl;
    }
    
private:
    std::shared_ptr<nlm::Brain> createAdvancedBrain() {
        auto config = nlm::createDefaultConfig();
        
        // Configure advanced brain parameters
        config->set("brain.neuron_count", 2000);
        config->set("brain.synapse_density", 0.15);
        config->set("brain.excitatory_neurons", 1700);
        config->set("brain.inhibitory_neurons", 300);
        config->set("brain.v_thresh", -45.0);
        config->set("brain.tau_mem", 25.0);
        config->set("brain.g_leak", 0.3);
        
        auto brain = nlm::createBrain(config);
        brain->initialize();
        
        return brain;
    }
    
    std::shared_ptr<nlm::World> createAdvancedWorld() {
        auto world = nlm::createSimpleWorld();
        
        // Configure advanced world parameters
        world->configure(
            30,  // width
            30,  // height
            12,  // visionWidth
            12,  // visionHeight
            0.05 // timestep
        );
        
        world->reset();
        world->setAgentStart(15.0, 15.0);
        
        return world;
    }
    
    std::shared_ptr<nlm::AgentBrain> createAdvancedAgent(std::shared_ptr<nlm::Brain> brain) {
        auto agent = nlm::createAgentBrain(brain);
        agent->initialize(nullptr);  // Will be initialized properly later
        return agent;
    }
    
    std::shared_ptr<nlm::TrainingConfig> createAdvancedTrainingConfig() {
        auto config = std::make_shared<nlm::TrainingConfig>();
        
        // Advanced training parameters
        config->maxEpisodes = 1000;
        config->maxStepsPerEpisode = 300;
        config->timeStep = 0.05;
        config->learningRate = 0.002;
        config->discountFactor = 0.995;
        config->explorationRate = 0.2;
        
        // Advanced reward configuration
        config->rewardConfig.type = nlm::RewardConfig::Type::WEIGHTED;
        config->rewardConfig.weighted.stepCompletion = 1.0;
        config->rewardConfig.weighted.novelty = 0.8;
        config->rewardConfig.weighted.survival = 1.2;
        config->rewardConfig.weighted.efficiency = 1.5;
        config->rewardConfig.weighted.curiosity = 1.0;
        config->rewardConfig.weighted.predictionError = 0.7;
        config->rewardConfig.weighted.social = 0.6;
        config->rewardConfig.weighted.competition = 0.5;
        
        // Advanced stopping criteria
        config->earlyStopping = true;
        config->patience = 50;
        config->minPerformance = 0.9;
        config->improvementThreshold = 0.005;
        
        // Performance monitoring
        config->checkpointFrequency = 100;
        config->saveCheckpoints = true;
        config->logFrequency = 10;
        
        return config;
    }
    
    void enableAdvancedFeatures(std::shared_ptr<nlm::Brain> brain, std::shared_ptr<nlm::World> world, std::shared_ptr<nlm::AgentBrain> agent) {
        // Enable advanced brain features
        brain->enableFeatures(
            true,  // reward_modulation
            true,  // structural_plasticity
            true,  // development
            true   // curiosity
        );
        
        // Enable advanced world features
        world->enableFeatures(
            true,  // exploration
            true,  // reward
            true,  // novelty
            true,  // competition
            true   // social
        );
        
        // Enable advanced agent features
        agent->enableLearning(true);
    }
    
    void saveAdvancedTrainingResults(
        const nlohmann::json& performance_summary,
        const std::chrono::seconds& duration,
        std::shared_ptr<nlm::TrainingCore> trainer,
        std::shared_ptr<nlm::Brain> brain,
        std::shared_ptr<nlm::AgentBrain> agent
    ) {
        // Create results directory
        std::filesystem::create_directory("advanced_training_results");
        
        // Save performance summary
        json results_json = {
            {"training_complete", true},
            {"duration_seconds", duration.count()},
            {"performance_summary", performance_summary}
        };
        
        std::ofstream results_file("advanced_training_results/training_results.json");
        results_file << results_json.dump(2);
        results_file.close();
        
        // Save brain state
        brain->save("advanced_training_results/brain_state.nlm");
        
        // Save agent state
        agent->save("advanced_training_results/agent_state.nlm");
        
        std::cout << "✓ Advanced training results saved to advanced_training_results/" << std::endl;
    }
    
    void generateAdvancedAnalysis(
        const nlohmann::json& performance_summary,
        std::shared_ptr<AdvancedFeaturesCallback> features_callback,
        std::shared_ptr<AdvancedMonitoringCallback> monitoring_callback
    ) {
        std::cout << std::endl;
        std::cout << "=== Advanced Analysis ===" << std::endl;
        
        // Performance analysis
        double avg_reward = performance_summary["average_reward"];
        double success_rate = performance_summary["episodes_successful"] / performance_summary["episodes_completed"];
        
        std::cout << "Performance Analysis:" << std::endl;
        std::cout << "  Average reward: " << avg_reward << std::endl;
        std::cout << "  Success rate: " << success_rate * 100 << "%" << std::endl;
        std::cout << "  Efficiency score: " << (avg_reward * success_rate) << std::endl;
        
        // Feature analysis
        std::cout << std::endl;
        std::cout << "Feature Usage:" << std::endl;
        for (const auto& [feature, usage] : features_callback->getFeatureUsage()) {
            std::cout << "  " << feature << ": " << usage << std::endl;
        }
        
        // Monitoring analysis
        std::cout << std::endl;
        std::cout << "Monitoring Summary:" << std::endl;
        std::cout << "  Data points collected: " << monitoring_callback->getDataPointsCount() << std::endl;
        std::cout << "  Average step time: " << monitoring_callback->getAverageStepTime() << " ms" << std::endl;
        std::cout << "  Data quality score: " << monitoring_callback->getDataQualityScore() << std::endl;
        
        // Recommendations
        std::cout << std::endl;
        std::cout << "Recommendations:" << std::endl;
        if (avg_reward > 50.0) {
            std::cout << "  ✓ High performance achieved - consider scaling up" << std::endl;
        } else {
            std::cout << "  ⚠ Performance could be improved - investigate reward function" << std::endl;
        }
        
        if (success_rate > 0.8) {
            std::cout << "  ✓ High success rate - good task alignment" << std::endl;
        } else {
            std::cout << "  ⚠ Low success rate - consider task difficulty adjustment" << std::endl;
        }
        
        if (features_callback->getFeatureUsage().find("curiosity") != features_callback->getFeatureUsage().end()) {
            std::cout << "  ✓ Curiosity-driven exploration active - good for exploration tasks" << std::endl;
        } else {
            std::cout << "  ⚠ Curiosity-driven exploration inactive - consider enabling" << std::endl;
        }
    }
};

class AdvancedPerformanceCallback : public nlm::TrainingCallback {
public:
    void afterEpisode(int episode, double reward, bool success) override {
        episode_data.push_back({{"episode", episode}, {"reward", reward}, {"success", success}});
        
        double avg_reward = 0.0;
        if (!episode_data.empty()) {
            double total_reward = 0.0;
            for (const auto& data : episode_data) {
                total_reward += data.at("reward");
            }
            avg_reward = total_reward / episode_data.size();
        }
        
        if (episode % 100 == 0) {
            std::cout << "    Advanced Episode " << episode << ": Reward = " << reward 
                      << ", Success = " << (success ? "Yes" : "No") 
                      << ", Average = " << avg_reward << std::endl;
        }
    }
    
    void afterStep(int episode, int step, double reward, double loss) override {
        step_data.push_back({{"episode", episode}, {"step", step}, {"reward", reward}, {"loss", loss}});
    }
    
    nlohmann::json getPerformanceSummary() {
        nlohmann::json summary;
        
        summary["episodes_completed"] = episode_data.size();
        summary["total_steps"] = step_data.size();
        
        if (!episode_data.empty()) {
            double total_reward = 0.0;
            double best_reward = std::numeric_limits<double>::lowest();
            double worst_reward = std::numeric_limits<double>::max();
            
            for (const auto& data : episode_data) {
                double reward = data.at("reward");
                total_reward += reward;
                
                if (reward > best_reward) {
                    best_reward = reward;
                }
                if (reward < worst_reward) {
                    worst_reward = reward;
                }
            }
            
            summary["average_reward"] = total_reward / episode_data.size();
            summary["best_reward"] = best_reward;
            summary["worst_reward"] = worst_reward;
            
            int successful_episodes = 0;
            for (const auto& data : episode_data) {
                if (data.at("success")) {
                    successful_episodes++;
                }
            }
            
            summary["episodes_successful"] = successful_episodes;
            summary["success_rate"] = static_cast<double>(successful_episodes) / episode_data.size();
        }
        
        return summary;
    }
    
    std::map<std::string, double> getFeatureUsage() {
        return feature_usage;
    }
    
private:
    std::vector<nlohmann::json> episode_data;
    std::vector<nlohmann::json> step_data;
    std::map<std::string, double> feature_usage;
};

class AdvancedFeaturesCallback : public nlm::TrainingCallback {
public:
    void afterEpisode(int episode, double reward, bool success) override {
        // Monitor and adapt advanced features based on performance
        if (success && episode % 100 == 0) {
            std::cout << "  Curriculum progression: Episode " << episode << " successful" << std::endl;
        }
        
        // Track feature usage
        feature_usage["curiosity"] = 0.8;  // Example value
        feature_usage["curriculum"] = 0.6; // Example value
        feature_usage["transfer"] = 0.3;   // Example value
    }
};

class AdvancedMonitoringCallback : public nlm::TrainingCallback {
public:
    void afterStep(int episode, int step, double reward, double loss) override {
        // Collect data for visualization and analysis
        monitoring_data.push_back({
            {"episode", episode},
            {"step", step},
            {"reward", reward},
            {"loss", loss},
            {"timestamp", std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()
            ).count()}
        });
    }
    
    size_t getDataPointsCount() const {
        return monitoring_data.size();
    }
    
    double getAverageStepTime() const {
        if (monitoring_data.size() < 2) return 0.0;
        
        double total_time = 0.0;
        for (size_t i = 1; i < monitoring_data.size(); ++i) {
            double time1 = monitoring_data[i-1].at("timestamp");
            double time2 = monitoring_data[i].at("timestamp");
            total_time += (time2 - time1);
        }
        
        return (total_time / (monitoring_data.size() - 1)) / 1000.0; // Convert to seconds
    }
    
    double getDataQualityScore() const {
        // Simple quality metric based on data completeness
        return std::min(1.0, monitoring_data.size() / 1000.0);
    }
    
private:
    std::vector<nlohmann::json> monitoring_data;
};

int main() {
    try {
        AdvancedTrainingExample example;
        example.runExample();
        
        std::cout << std::endl;
        std::cout << "✓ Advanced training example completed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error in advanced training example: " << e.what() << std::endl;
        return 1;
    }
}
