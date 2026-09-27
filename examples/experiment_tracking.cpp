// NLM Experiment Tracking Example
// ================================

// Comprehensive experiment tracking example demonstrating NLM's
// capabilities for managing long-running experiments with persistence,
// reproducibility, and comprehensive logging.

#include "TrainingCore.hpp"
#include "TrainingComponents.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <nlohmann/json.hpp>
#include <map>
#include <algorithm>

using json = nlohmann::json;

class ExperimentSession {
public:
    struct SessionInfo {
        std::string sessionId;
        std::string description;
        std::chrono::system_clock::time_point startTime;
        std::chrono::system_clock::time_point endTime;
        std::string status; // active, completed, paused, failed
        
        std::map<std::string, std::string> metadata;
    };
    
    struct ExperimentResult {
        int episode;
        double reward;
        bool success;
        std::chrono::system_clock::time_point timestamp;
        std::map<std::string, double> metrics;
    };
    
    ExperimentSession(const std::string& description = "") {
        sessionId = "session_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
        this->description = description;
        status = "active";
        startTime = std::chrono::system_clock::now();
        
        std::cout << "Initialized experiment session: " << sessionId << std::endl;
        std::cout << "Description: " << description << std::endl;
    }
    
    void saveBrainState(std::shared_ptr<nlm::Brain> brain, const std::string& filename) {
        try {
            brain->save(filename);
            std::cout << "✓ Brain state saved: " << filename << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "✗ Failed to save brain state: " << e.what() << std::endl;
            throw;
        }
    }
    
    void loadBrainState(std::shared_ptr<nlm::Brain> brain, const std::string& filename) {
        try {
            brain->load(filename);
            std::cout << "✓ Brain state loaded: " << filename << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "✗ Failed to load brain state: " << e.what() << std::endl;
            throw;
        }
    }
    
    void logExperimentEvent(const std::string& eventType, const json& data) {
        ExperimentResult result;
        result.timestamp = std::chrono::system_clock::now();
        result.eventType = eventType;
        result.data = data;
        
        results.push_back(result);
        
        // Save event to timeline file
        std::string filename = "experiment_timeline_" + sessionId + ".json";
        std::ofstream file(filename, std::ios::app);
        if (file.is_open()) {
            file << result.timestamp.time_since_epoch().count() << ":" << eventType << ":" << data.dump() << std::endl;
            file.close();
        }
        
        std::cout << "✓ Experiment event logged: " << eventType << std::endl;
    }
    
    void createCheckpoint(std::shared_ptr<nlm::Brain> brain, int episode, const json& metrics) {
        std::string checkpoint_file = "checkpoint_ep" + std::to_string(episode) + ".nlm";
        saveBrainState(brain, checkpoint_file);
        
        logExperimentEvent("checkpoint", {
            {"episode", episode},
            {"checkpoint_file", checkpoint_file},
            {"metrics", metrics}
        });
    }
    
    void completeSession() {
        endTime = std::chrono::system_clock::now();
        status = "completed";
        
        // Generate final report
        generateSessionReport();
        
        std::cout << "✓ Experiment session completed successfully" << std::endl;
        std::cout << "✓ Session duration: " << std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count() << " seconds" << std::endl;
    }
    
    void generateSessionReport() {
        json report;
        
        report["session_id"] = sessionId;
        report["description"] = description;
        report["status"] = status;
        report["duration_seconds"] = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
        
        // Analyze results
        json analysis = analyzeResults();
        report["analysis"] = analysis;
        
        // Save report
        std::string report_file = "experiment_report_" + sessionId + ".json";
        std::ofstream file(report_file);
        file << report.dump(2);
        file.close();
        
        std::cout << "✓ Experiment report generated: " << report_file << std::endl;
    }
    
    json analyzeResults() {
        json analysis;
        
        if (results.empty()) {
            analysis["status"] = "no_data";
            return analysis;
        }
        
        std::vector<double> rewards;
        std::vector<bool> successes;
        
        for (const auto& result : results) {
            if (result.eventType == "episode_completed") {
                rewards.push_back(result.reward);
                successes.push_back(result.success);
            }
        }
        
        if (!rewards.empty()) {
            double total_reward = std::accumulate(rewards.begin(), rewards.end(), 0.0);
            double avg_reward = total_reward / rewards.size();
            
            double success_rate = std::accumulate(successes.begin(), successes.end(), 0.0) / successes.size();
            
            analysis["total_episodes"] = rewards.size();
            analysis["total_reward"] = total_reward;
            analysis["average_reward"] = avg_reward;
            analysis["success_rate"] = success_rate;
            analysis["best_reward"] = *std::max_element(rewards.begin(), rewards.end());
            analysis["worst_reward"] = *std::min_element(rewards.begin(), rewards.end());
            
            // Calculate learning trend
            if (rewards.size() >= 2) {
                std::vector<double> first_half(rewards.begin(), rewards.begin() + rewards.size() / 2);
                std::vector<double> second_half(rewards.begin() + rewards.size() / 2, rewards.end());
                
                double first_avg = std::accumulate(first_half.begin(), first_half.end(), 0.0) / first_half.size();
                double second_avg = std::accumulate(second_half.begin(), second_half.end(), 0.0) / second_half.size();
                
                double trend = second_avg - first_avg;
                
                if (trend > 1.0) analysis["learning_trend"] = "strongly_improving";
                else if (trend > 0.1) analysis["learning_trend"] = "improving";
                else if (trend > -0.1) analysis["learning_trend"] = "stable";
                else if (trend > -1.0) analysis["learning_trend"] = "declining";
                else analysis["learning_trend"] = "strongly_declining";
            }
        }
        
        return analysis;
    }
    
private:
    std::string sessionId;
    std::string description;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::string status;
    std::vector<ExperimentResult> results;
};

class ExperimentManager {
public:
    void createExperiment(const std::string& description, const json& config) {
        std::cout << "Creating experiment: " << description << std::endl;
        
        ExperimentSession session(description);
        sessions.push_back(session);
        
        std::cout << "✓ Experiment created: " << session.getSessionId() << std::endl;
    }
    
    ExperimentSession* getExperiment(const std::string& sessionId) {
        for (auto& session : sessions) {
            if (session.getSessionId() == sessionId) {
                return &session;
            }
        }
        return nullptr;
    }
    
    void listExperiments() {
        std::cout << "Active experiments:" << std::endl;
        for (size_t i = 0; i < sessions.size(); ++i) {
            std::cout << "  " << i + 1 << ". " << sessions[i].getSessionId() 
                      << " - " << sessions[i].getDescription() 
                      << " (" << sessions[i].getStatus() << ")" << std::endl;
        }
    }
    
    void runExperiment(const std::string& sessionId, std::shared_ptr<nlm::Brain> brain, std::shared_ptr<nlm::World> world, std::shared_ptr<nlm::AgentBrain> agent) {
        auto session = getExperiment(sessionId);
        if (!session) {
            std::cerr << "Experiment not found: " << sessionId << std::endl;
            return;
        }
        
        std::cout << "Running experiment: " << sessionId << std::endl;
        
        // Create training configuration
        auto config = std::make_shared<nlm::TrainingConfig>();
        config->maxEpisodes = 100;
        config->learningRate = 0.001;
        config->explorationRate = 0.1;
        config->earlyStopping = true;
        config->patience = 20;
        
        // Create callback for experiment tracking
        class ExperimentTrackingCallback : public nlm::TrainingCallback {
        public:
            std::shared_ptr<ExperimentSession> session;
            
            void afterEpisode(int episode, double reward, bool success) override {
                session->logExperimentEvent("episode_completed", {
                    {"episode", episode},
                    {"reward", reward},
                    {"success", success},
                    {"timestamp", std::chrono::system_clock::now().time_since_epoch().count()}
                });
                
                // Create checkpoint every 20 episodes
                if (episode % 20 == 0) {
                    session->createCheckpoint(nullptr, episode, {
                        {"average_reward", reward},
                        {"success_rate", success ? 1.0 : 0.0}
                    });
                }
            }
        };
        
        auto callback = std::make_shared<ExperimentTrackingCallback>();
        callback->session = session;
        
        // Run training
        bool training_complete = trainer->trainAgent(brain, world, agent, config, {callback});
        
        if (training_complete) {
            session->completeSession();
            std::cout << "✓ Experiment completed successfully: " << sessionId << std::endl;
        } else {
            std::cout << "⚠ Experiment stopped early: " << sessionId << std::endl;
        }
    }
    
private:
    std::vector<ExperimentSession> sessions;
    std::shared_ptr<nlm::TrainingCore> trainer = std::make_shared<nlm::TrainingCore>();
};

int main() {
    try {
        ExperimentManager manager;
        
        std::cout << "=== NLM Experiment Tracking System ===" << std::endl;
        std::cout << "This system provides comprehensive experiment management" << std::endl;
        std::cout << "including session tracking, state persistence, and analysis." << std::endl;
        std::cout << std::endl;
        
        // Create an experiment
        json experiment_config = {
            {"description", "Curiosity-driven learning experiment"},
            {"brain_config", {"neuron_count", 1000, "complexity", 0.7}},
            {"world_config", {"width", 20, "height", 20}},
            {"training_config", {"max_episodes", 100, "learning_rate", 0.001}}
        };
        
        manager.createExperiment("Curiosity-driven learning experiment", experiment_config);
        
        // List experiments
        manager.listExperiments();
        
        // Note: In a real implementation, you would create actual NLM components
        // and run the experiment with proper setup.
        
        std::cout << std::endl;
        std::cout << "✓ Experiment tracking system initialized successfully!" << std::endl;
        std::cout << "✓ Ready to run experiments with full tracking and persistence." << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error in experiment tracking system: " << e.what() << std::endl;
        return 1;
    }
}
