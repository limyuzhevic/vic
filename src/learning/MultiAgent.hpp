#ifndef MULTI_AGENT_HPP
#define MULTI_AGENT_HPP

#include <vector>
#include <string>
#include <unordered_map>

namespace nlm {

class Brain;

// Action types for multi-agent systems
enum class ActionType {
    Wait,
    MoveForward,
    MoveBackward,
    TurnLeft,
    TurnRight,
    Interact,
    Share,
    Compete,
    Coordinate
};

class MultiAgent {
public:
    MultiAgent();
    ~MultiAgent();
    
    void initialize(Brain* brain);
    void addAgent(const std::string& agentID,
                  const std::vector<float>& initialPosition);
    void updateAgent(const std::string& agentID,
                     const std::vector<float>& state,
                     const std::vector<float>& action);
    void updateCompetitor(const std::string& agentID,
                         const std::vector<float>& competitorState,
                         const std::vector<float>& reward);
    void computeCoordination(const std::vector<std::string>& agentIDs);
    void updateResources(const std::vector<std::string>& agentIDs,
                        const std::vector<std::string>& resourceIDs,
                        const std::vector<std::vector<float>>& resourceAmounts);
    void update(TimestepDuration dt);
    
    // Configuration methods
    void setCompetitionParameters(const CompetitionModel::CompetitionParameters& params);
    void setCooperationParameters(const CooperationModel::CooperationParameters& params);
    void setCommunicationParameters(const CommunicationProtocol::ProtocolParameters& params);
    void setResourceParameters(const ResourceManager::ResourceParameters& params);
    
    // Status and control
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Agent state
    TimestepDuration currentStep_;
    Brain* brain_;
};

// Internal model classes (for implementation)
class CompetitionModel;
class CooperationModel;
class CommunicationProtocol;
class ResourceManager;

} // namespace nlm

#endif // MULTI_AGENT_HPP
