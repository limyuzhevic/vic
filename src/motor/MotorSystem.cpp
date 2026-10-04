#include "MotorSystem.hpp"
#include "../command/Command.hpp"
#include "../command/CommandManager.hpp"
#include "../core/ErrorHandling.hpp"
#include <memory>
#include <random>
#include <algorithm>

namespace nlm {

struct MotorSystem::Impl {
    std::vector<ActionType> availableActions;
    std::vector<const Action*> actionHistory;
    std::unique_ptr<CommandManager> commandManager;
    std::mt19937 rng;
    float explorationRate;
    float curiosityThreshold;
    
    Impl() : rng(std::random_device{}()), explorationRate(0.1f), curiosityThreshold(0.5f) {
        availableActions = {
            ActionType::MoveForward,
            ActionType::MoveBackward,
            ActionType::MoveLeft,
            ActionType::MoveRight,
            ActionType::TurnLeft,
            ActionType::TurnRight,
            ActionType::Look,
            ActionType::LookUp,
            ActionType::LookDown,
            ActionType::Interact,
            ActionType::Eat,
            ActionType::Drink,
            ActionType::Rest,
            ActionType::Wait
        };
        
        // Initialize command manager
        commandManager = std::make_unique<CommandManager>();
        
        // Add default exploration commands
        addExplorationCommands();
    }
    
    void addExplorationCommands() {
        try {
            // Add curiosity-driven exploration command
            auto curiosityCommand = std::make_unique<SimpleCommand>(
                std::make_unique<Action>(ActionType::MoveForward),
                1.5f  // Higher priority for exploration
            );
            curiosityCommand->setEnabled(false);  // Initially disabled
            commandManager->addCommand(std::move(curiosityCommand));
            
            // Add reward-based command
            auto rewardCommand = std::make_unique<SimpleCommand>(
                std::make_unique<Action>(ActionType::Interact),
                1.2f
            );
            rewardCommand->setEnabled(true);  // Enabled by default
            commandManager->addCommand(std::move(rewardCommand));
            
            // Add exploration command
            auto explorationCommand = std::make_unique<SimpleCommand>(
                std::make_unique<Action>(ActionType::MoveLeft),
                0.8f
            );
            explorationCommand->setEnabled(true);
            commandManager->addCommand(std::move(explorationCommand));
            
            NLM_LOG_INFO("MotorSystem: Added " + std::to_string(commandManager->getCommandCount()) + " default commands");
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error adding exploration commands: " + std::string(e.what()));
        }
    }
};

MotorSystem::MotorSystem() : pImpl(new Impl) {}

MotorSystem::~MotorSystem() = default;

std::unique_ptr<Action> MotorSystem::selectAction(const NeuralRegion& motorRegion) {
    try {
        // First try command manager for sophisticated action selection
        auto action = pImpl->commandManager->executeCommands(nullptr);
        if (action && action->getType() != ActionType::Wait) {
            // Record action in history
            pImpl->actionHistory.push_back(action.get());
            return action;
        }
        
        // Fallback to simple random selection
        if (!pImpl->availableActions.empty() &&
            (pImpl->rng() % 100 < (pImpl->explorationRate * 100))) {
            
            std::uniform_int_distribution<size_t> dist(0, pImpl->availableActions.size() - 1);
            size_t index = dist(pImpl->rng);
            auto action = std::make_unique<Action>(pImpl->availableActions[index]);
            pImpl->actionHistory.push_back(action.get());
            return action;
        }
        
        // Default to wait
        return std::make_unique<Action>(ActionType::Wait);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error selecting action: " + std::string(e.what()));
        return std::make_unique<Action>(ActionType::Wait);
    }
}

const std::vector<ActionType>& MotorSystem::getAvailableActions() const {
    return pImpl->availableActions;
}

void MotorSystem::setActionPreference(ActionType action, float preference) {
    try {
        // TODO: Implement action preferences for learned behavior
        NLM_LOG_DEBUG("Action preference set for " + std::to_string(static_cast<int>(action)) + ": " + std::to_string(preference));
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error setting action preference: " + std::string(e.what()));
    }
}

const std::vector<const Action*>& MotorSystem::getActionHistory() const {
    return pImpl->actionHistory;
}

void MotorSystem::clearHistory() {
    try {
        for (auto* action : pImpl->actionHistory) {
            delete action;
        }
        pImpl->actionHistory.clear();
        NLM_LOG_INFO("MotorSystem action history cleared");
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error clearing action history: " + std::string(e.what()));
    }
}

} // namespace nlm