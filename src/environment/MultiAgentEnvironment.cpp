// MultiAgentEnvironment Implementation

#include "MultiAgentEnvironment.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <chrono>

namespace nlm {

struct MultiAgentEnvironment::Impl {
    std::vector<std::shared_ptr<AgentBrain>> agents;
    std::vector<size_t> agentIds;
    
    float width;
    float height;
    std::string environmentType;
    
    // Resource management
    struct Resource {
        float x;
        float y;
        float value;
        size_t ownerId;
        bool isDynamic;
        float decayRate;
    };
    
    std::vector<Resource> resources;
    
    // Inter-agent communication
    bool communicationEnabled;
    std::vector<std::vector<float>> communicationStrengths;
    
    // Knowledge transfer
    bool knowledgeTransferEnabled;
    std::vector<std::vector<float>> knowledgeTransferMatrix;
    
    // Random number generator
    std::mt19937 rng;
    std::uniform_real_distribution<float> posDist;
    std::uniform_real_distribution<float> valueDist;
    
    Impl(const std::shared_ptr<Config>& config) : width(100.0f), height(100.0f),
                                                 environmentType("mixed"),
                                                 communicationEnabled(true),
                                                 knowledgeTransferEnabled(true),
                                                 rng(std::chrono::steady_clock::now().time_since_epoch().count()),
                                                 posDist(0.0f, width),
                                                 valueDist(0.0f, 1.0f) {
        
        if (config) {
            width = config->getOr<float>("environment_width", width);
            height = config->getOr<float>("environment_height", height);
            environmentType = config->getOr<std::string>("environment_type", environmentType);
            communicationEnabled = config->getOr<bool>("enable_communication", communicationEnabled);
            knowledgeTransferEnabled = config->getOr<bool>("enable_knowledge_transfer", knowledgeTransferEnabled);
        }
        
        posDist = std::uniform_real_distribution<float>(0.0f, width);
        valueDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
    }
};

MultiAgentEnvironment::MultiAgentEnvironment()
    : pImpl(new Impl(nullptr))
{
}

MultiAgentEnvironment::~MultiAgentEnvironment() = default;

bool MultiAgentEnvironment::initialize(const std::shared_ptr<Config>& config) {
    pImpl = std::make_unique<Impl>(config);
    
    // Generate initial resources
    size_t resourceCount = 10;
    pImpl->resources.reserve(resourceCount);
    for (size_t i = 0; i < resourceCount; ++i) {
        Resource resource;
        resource.x = pImpl->posDist(pImpl->rng);
        resource.y = pImpl->posDist(pImpl->rng);
        resource.value = pImpl->valueDist(pImpl->rng);
        resource.ownerId = static_cast<size_t>(-1);  // No owner initially
        resource.isDynamic = true;
        resource.decayRate = 0.01f;
        pImpl->resources.push_back(resource);
    }
    
    // Initialize communication strengths
    size_t agentCount = pImpl->agents.size();
    pImpl->communicationStrengths.assign(agentCount, std::vector<float>(agentCount, 0.0f));
    
    // Initialize knowledge transfer matrix
    pImpl->knowledgeTransferMatrix.assign(agentCount, std::vector<float>(agentCount, 0.0f));
    
    NLM_LOG_INFO("Multi-agent environment initialized");
    return true;
}

bool MultiAgentEnvironment::addAgent(std::shared_ptr<AgentBrain> agent) {
    if (!agent) return false;
    
    pImpl->agents.push_back(agent);
    pImpl->agentIds.push_back(pImpl->agents.size() - 1);
    
    // Resize communication and knowledge transfer matrices
    size_t newSize = pImpl->agents.size();
    pImpl->communicationStrengths.resize(newSize, std::vector<float>(newSize, 0.0f));
    pImpl->knowledgeTransferMatrix.resize(newSize, std::vector<float>(newSize, 0.0f));
    
    NLM_LOG_INFO("Added agent " + std::to_string(pImpl->agents.size() - 1));
    return true;
}

bool MultiAgentEnvironment::removeAgent(size_t agentId) {
    if (agentId >= pImpl->agents.size()) return false;
    
    pImpl->agents.erase(pImpl->agents.begin() + agentId);
    pImpl->agentIds.erase(pImpl->agentIds.begin() + agentId);
    
    // Resize matrices
    size_t newSize = pImpl->agents.size();
    pImpl->communicationStrengths.resize(newSize);
    for (auto& row : pImpl->communicationStrengths) {
        row.resize(newSize);
    }
    
    pImpl->knowledgeTransferMatrix.resize(newSize);
    for (auto& row : pImpl->knowledgeTransferMatrix) {
        row.resize(newSize);
    }
    
    NLM_LOG_INFO("Removed agent " + std::to_string(agentId));
    return true;
}

std::shared_ptr<AgentBrain> MultiAgentEnvironment::getAgent(size_t agentId) const {
    if (agentId >= pImpl->agents.size()) return nullptr;
    return pImpl->agents[agentId];
}

std::vector<std::shared_ptr<AgentBrain>> MultiAgentEnvironment::getAllAgents() const {
    return pImpl->agents;
}

size_t MultiAgentEnvironment::getAgentCount() const {
    return pImpl->agents.size();
}

void MultiAgentEnvironment::update(double timestep) {
    // Update resources
    for (auto& resource : pImpl->resources) {
        if (resource.isDynamic) {
            resource.value *= (1.0f - resource.decayRate * static_cast<float>(timestep));
            if (resource.value < 0.01f) {
                // Regenerate resource
                resource.x = pImpl->posDist(pImpl->rng);
                resource.y = pImpl->posDist(pImpl->rng);
                resource.value = pImpl->valueDist(pImpl->rng);
                resource.ownerId = static_cast<size_t>(-1);
            }
        }
    }
    
    // Update agent sensory inputs with shared world state
    std::vector<std::shared_ptr<SensoryPercept>> allPercepts;
    for (size_t i = 0; i < pImpl->agents.size(); ++i) {
        auto agent = pImpl->agents[i];
        
        // Create shared percept containing world state
        auto percept = std::make_shared<SensoryPercept>();
        
        // Add resource information
        std::vector<float> resourceInfo;
        for (const auto& resource : pImpl->resources) {
            float distance = std::sqrt(std::pow(resource.x - agent->getBody().x, 2) +
                                     std::pow(resource.y - agent->getBody().y, 2));
            if (distance < 20.0f) {  // Detection radius
                resourceInfo.push_back(resource.value / distance);
            } else {
                resourceInfo.push_back(0.0f);
            }
        }
        
        percept->setInternal(resourceInfo);
        
        // Add agent information
        std::vector<float> agentInfo;
        for (size_t j = 0; j < pImpl->agents.size(); ++j) {
            if (i != j) {
                auto otherAgent = pImpl->agents[j];
                float distance = std::sqrt(std::pow(otherAgent->getBody().x - agent->getBody().x, 2) +
                                         std::pow(otherAgent->getBody().y - agent->getBody().y, 2));
                if (distance < 30.0f) {
                    agentInfo.push_back(1.0f / distance);
                } else {
                    agentInfo.push_back(0.0f);
                }
            } else {
                agentInfo.push_back(0.0f);
            }
        }
        
        percept->setProprioception(agentInfo);
        
        allPercepts.push_back(percept);
    }
    
    // Process sensory inputs and apply inter-agent communication
    for (size_t i = 0; i < pImpl->agents.size(); ++i) {
        auto agent = pImpl->agents[i];
        auto percept = allPercepts[i];
        
        // Apply communication from other agents
        if (pImpl->communicationEnabled) {
            for (size_t j = 0; j < pImpl->agents.size(); ++j) {
                if (i != j && pImpl->communicationStrengths[i][j] > 0.0f) {
                    auto otherAgent = pImpl->agents[j];
                    // Blend sensory inputs based on communication strength
                    auto otherPercept = allPercepts[j];
                    // Simple blending - could be more sophisticated
                }
            }
        }
        
        // Apply knowledge transfer
        if (pImpl->knowledgeTransferEnabled) {
            for (size_t j = 0; j < pImpl->agents.size(); ++j) {
                if (i != j && pImpl->knowledgeTransferMatrix[i][j] > 0.0f) {
                    // Transfer knowledge from agent j to agent i
                    // This would involve copying neural weights or parameters
                }
            }
        }
        
        // Process sensory input
        agent->processSensoryInput(*percept);
        
        // Execute action
        auto action = agent->decodeMotorCommand(agent->getBrain()->produceAction());
        
        // Apply action to agent body
        agent->getBody().x += action.velocityX * static_cast<float>(timestep);
        agent->getBody().y += action.velocityY * static_cast<float>(timestep);
        
        // Apply action to world (environment effects)
        // This would modify resources, etc.
    }
}

std::vector<Action> MultiAgentEnvironment::executeActions() const {
    std::vector<Action> actions;
    actions.reserve(pImpl->agents.size());
    
    for (const auto& agent : pImpl->agents) {
        auto action = agent->decodeMotorCommand(agent->getBrain()->produceAction());
        actions.push_back(action);
    }
    
    return actions;
}

std::vector<std::shared_ptr<SensoryPercept>> MultiAgentEnvironment::getSensoryPercepts() const {
    std::vector<std::shared_ptr<SensoryPercept>> percepts;
    
    for (const auto& agent : pImpl->agents) {
        // Get percept from agent's brain
        auto brain = agent->getBrain();
        auto percept = std::make_shared<SensoryPercept>();
        
        // Create percept based on brain state
        // This is a simplified version - real implementation would extract
        // actual sensory data from brain
        percepts.push_back(percept);
    }
    
    return percepts;
}

std::vector<std::pair<float, float>> MultiAgentEnvironment::getAgentPositions() const {
    std::vector<std::pair<float, float>> positions;
    
    for (const auto& agent : pImpl->agents) {
        positions.emplace_back(agent->getBody().x, agent->getBody().y);
    }
    
    return positions;
}

void MultiAgentEnvironment::setResourcePosition(size_t agentId, float x, float y, float value) {
    if (agentId < pImpl->resources.size()) {
        pImpl->resources[agentId].x = x;
        pImpl->resources[agentId].y = y;
        pImpl->resources[agentId].value = value;
        pImpl->resources[agentId].ownerId = agentId;
    }
}

float MultiAgentEnvironment::getResourceValue(float x, float y) const {
    float maxValue = 0.0f;
    for (const auto& resource : pImpl->resources) {
        float distance = std::sqrt(std::pow(resource.x - x, 2) + std::pow(resource.y - y, 2));
        if (distance < 5.0f) {
            maxValue = std::max(maxValue, resource.value);
        }
    }
    return maxValue;
}

bool MultiAgentEnvironment::isOccupied(float x, float y) const {
    // Check if position is occupied by an agent
    for (const auto& agent : pImpl->agents) {
        float distance = std::sqrt(std::pow(agent->getBody().x - x, 2) +
                                 std::pow(agent->getBody().y - y, 2));
        if (distance < 2.0f) {
            return true;
        }
    }
    return false;
}

void MultiAgentEnvironment::setEnvironmentType(const std::string& type) {
    pImpl->environmentType = type;
}

void MultiAgentEnvironment::enableCommunication(bool enable) {
    pImpl->communicationEnabled = enable;
}

float MultiAgentEnvironment::getCommunicationStrength(size_t agentId1, size_t agentId2) const {
    if (agentId1 < pImpl->communicationStrengths.size() &&
        agentId2 < pImpl->communicationStrengths[agentId1].size()) {
        return pImpl->communicationStrengths[agentId1][agentId2];
    }
    return 0.0f;
}

void MultiAgentEnvironment::setCommunicationStrength(size_t agentId1, size_t agentId2, float strength) {
    if (agentId1 < pImpl->communicationStrengths.size() &&
        agentId2 < pImpl->communicationStrengths[agentId1].size()) {
        pImpl->communicationStrengths[agentId1][agentId2] = std::clamp(strength, 0.0f, 1.0f);
    }
}

void MultiAgentEnvironment::enableKnowledgeTransfer(bool enable) {
    pImpl->knowledgeTransferEnabled = enable;
}

std::vector<std::vector<float>> MultiAgentEnvironment::getKnowledgeTransferMatrix() const {
    return pImpl->knowledgeTransferMatrix;
}

void MultiAgentEnvironment::setKnowledgeTransfer(size_t fromAgent, size_t toAgent, float strength) {
    if (fromAgent < pImpl->knowledgeTransferMatrix.size() &&
        toAgent < pImpl->knowledgeTransferMatrix[fromAgent].size()) {
        pImpl->knowledgeTransferMatrix[fromAgent][toAgent] = std::clamp(strength, 0.0f, 1.0f);
    }
}

void MultiAgentEnvironment::reset() {
    // Reset agent positions
    for (auto& agent : pImpl->agents) {
        agent->reset();
    }
    
    // Regenerate resources
    for (auto& resource : pImpl->resources) {
        resource.x = pImpl->posDist(pImpl->rng);
        resource.y = pImpl->posDist(pImpl->rng);
        resource.value = pImpl->valueDist(pImpl->rng);
        resource.ownerId = static_cast<size_t>(-1);
        resource.isDynamic = true;
    }
    
    NLM_LOG_INFO("Multi-agent environment reset");
}

bool MultiAgentEnvironment::save(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for saving: " + filepath);
        return false;
    }
    
    // Save environment state
    file << "MultiAgentEnvironment State" << std::endl;
    file << "Width: " << pImpl->width << std::endl;
    file << "Height: " << pImpl->height << std::endl;
    file << "Environment Type: " << pImpl->environmentType << std::endl;
    file << "Communication Enabled: " << (pImpl->communicationEnabled ? "true" : "false") << std::endl;
    file << "Knowledge Transfer Enabled: " << (pImpl->knowledgeTransferEnabled ? "true" : "false") << std::endl;
    file << "Agent Count: " << pImpl->agents.size() << std::endl;
    
    // Save resource positions
    file << "Resources:" << std::endl;
    for (const auto& resource : pImpl->resources) {
        file << resource.x << " " << resource.y << " " << resource.value << " "
             << resource.ownerId << " " << resource.isDynamic << " "
             << resource.decayRate << std::endl;
    }
    
    // Save communication strengths
    file << "Communication Strengths:" << std::endl;
    for (const auto& row : pImpl->communicationStrengths) {
        for (float strength : row) {
            file << strength << " ";
        }
        file << std::endl;
    }
    
    // Save knowledge transfer matrix
    file << "Knowledge Transfer Matrix:" << std::endl;
    for (const auto& row : pImpl->knowledgeTransferMatrix) {
        for (float strength : row) {
            file << strength << " ";
        }
        file << std::endl;
    }
    
    file.close();
    NLM_LOG_INFO("Environment saved to: " + filepath);
    return true;
}

bool MultiAgentEnvironment::load(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open file for loading: " + filepath);
        return false;
    }
    
    std::string line;
    std::getline(file, line); // Skip header
    
    while (std::getline(file, line)) {
        if (line.find("Width: ") == 0) {
            pImpl->width = std::stof(line.substr(7));
        } else if (line.find("Height: ") == 0) {
            pImpl->height = std::stof(line.substr(8));
        } else if (line.find("Environment Type: ") == 0) {
            pImpl->environmentType = line.substr(19);
        } else if (line.find("Communication Enabled: ") == 0) {
            pImpl->communicationEnabled = (line.substr(25) == "true");
        } else if (line.find("Knowledge Transfer Enabled: ") == 0) {
            pImpl->knowledgeTransferEnabled = (line.substr(28) == "true");
        } else if (line.find("Resources:") == 0) {
            // Parse resources
            pImpl->resources.clear();
            std::string resourceLine;
            while (std::getline(file, resourceLine) && !resourceLine.empty()) {
                std::istringstream iss(resourceLine);
                Resource resource;
                iss >> resource.x >> resource.y >> resource.value >> resource.ownerId >> resource.isDynamic >> resource.decayRate;
                pImpl->resources.push_back(resource);
            }
        } else if (line.find("Communication Strengths:") == 0) {
            // Parse communication strengths
            pImpl->communicationStrengths.clear();
            size_t agentCount = pImpl->agents.size();
            pImpl->communicationStrengths.assign(agentCount, std::vector<float>(agentCount, 0.0f));
            
            std::string strengthLine;
            size_t row = 0;
            while (row < agentCount && std::getline(file, strengthLine) && !strengthLine.empty()) {
                std::istringstream iss(strengthLine);
                for (size_t col = 0; col < agentCount && iss >> pImpl->communicationStrengths[row][col]; ++col) {
                }
                ++row;
            }
        } else if (line.find("Knowledge Transfer Matrix:") == 0) {
            // Parse knowledge transfer matrix
            pImpl->knowledgeTransferMatrix.clear();
            size_t agentCount = pImpl->agents.size();
            pImpl->knowledgeTransferMatrix.assign(agentCount, std::vector<float>(agentCount, 0.0f));
            
            std::string matrixLine;
            size_t row = 0;
            while (row < agentCount && std::getline(file, matrixLine) && !matrixLine.empty()) {
                std::istringstream iss(matrixLine);
                for (size_t col = 0; col < agentCount && iss >> pImpl->knowledgeTransferMatrix[row][col]; ++col) {
                }
                ++row;
            }
        }
    }
    
    file.close();
    NLM_LOG_INFO("Environment loaded from: " + filepath);
    return true;
}

} // namespace nlm