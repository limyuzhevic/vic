// SimpleAgent class header
// This file provides the header declaration for the SimpleAgent class

#ifndef SIMPLE_AGENT_HPP
#define SIMPLE_AGENT_HPP

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>

// NLM includes
#include "../src/advanced_api.hpp"

/**
 * @brief High-level agent interface for common scenarios
 */
class SimpleAgent {
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::string agentId_;
    bool isInitialized_ = false;
    
public:
    SimpleAgent(std::shared_ptr<Brain> brain, 
                std::shared_ptr<SimpleWorld> world,
                const std::string& agentId = "agent_1");
    
    ~SimpleAgent() = default;
    
    // Core functionality
    bool initialize();
    void update(float dt = 1.0f);
    void reset();
    
    // Neural operations
    void processInput(const FloatArray& input);
    FloatArray getOutput() const;
    double getReward() const;
    void receiveReward(double reward);
    
    // State management
    bool isActive() const;
    std::string getAgentId() const;
    std::shared_ptr<Brain> getBrain() const;
    std::shared_ptr<SimpleWorld> getWorld() const;
    
    // Configuration and setup
    bool configure(const StringMap& config);
    StringMap getConfig() const;
    
    // Performance monitoring
    double getFiringRate() const;
    size_t getSpikeCount() const;
    FloatArray getNeuralActivity() const;
    
    // Learning and development
    void learnFromExperience(const FloatArray& experience);
    void develop(float dt = 1.0f);
    
    // Serialization
    void saveState(const std::string& filepath) const;
    void loadState(const std::string& filepath);
    
    // Debugging
    std::string getDebugInfo() const;
    void profile(float duration = 1.0f);
};

#endif // SIMPLE_AGENT_HPP
