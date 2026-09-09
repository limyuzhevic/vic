#ifndef NLM_AGENTBODY_HPP
#define NLM_AGENTBODY_HPP

#include <vector>
#include <string>
#include "Brain.hpp"
#include "SensoryPercept.hpp"
#include "Action.hpp"

namespace nlm {

class AgentBody {
public:
    AgentBody(std::shared_ptr<Brain> brain);
    ~AgentBody();
    
    void initialize();
    void update(double timestep);
    
    // Sensory processing
    void processSensoryInput(const SensoryPercept& percept);
    const SensoryPercept& getCurrentPercept() const;
    
    // Motor output
    Action getAction() const;
    void setAction(Action action);
    
    // State
    float getEnergy() const;
    void setEnergy(float energy);
    bool isAlive() const;
    void setAlive(bool alive);
    
    // Reward and learning
    float getTotalReward() const;
    void addReward(float reward);
    float getLearningRate() const;
    void setLearningRate(float rate);
    
    // Development
    double getDevelopmentalStage() const;
    void setDevelopmentalStage(double stage);
    
    // Statistics
    size_t getStepCount() const;
    void incrementStepCount();
    float getCuriosityLevel() const;
    void setCuriosityLevel(float level);
    
    // Configuration
    void configure(const std::string& key, const std::string& value);
    std::string getConfig(const std::string& key) const;
    
private:
    std::shared_ptr<Brain> brain_;
    
    // State
    SensoryPercept currentPercept_;
    Action currentAction_;
    float energy_;
    bool alive_;
    float totalReward_;
    float learningRate_;
    double developmentalStage_;
    size_t stepCount_;
    float curiosityLevel_;
    
    // Configuration
    std::map<std::string, std::string> config_;
};

} // namespace nlm

#endif // NLM_AGENTBODY_HPP