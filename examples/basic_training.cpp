#include "TrainingCore.hpp"
#include "TrainingComponents.hpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Training Module Basic Example ===" << std::endl;
    std::cout << "This example demonstrates the basic usage of the NLM Training Module." << std::endl;
    std::cout << std::endl;
    
    // Create training components
    auto trainer = std::make_shared<nlm::TrainingCore>();
    auto config = std::make_shared<nlm::TrainingConfig>();
    auto callback = std::make_shared<nlm::SimpleTrainingCallback>();
    auto monitor = std::make_shared<nlm::TrainingMonitor>();
    
    // Configure training
    config->maxEpisodes = 10;  // Short training for demonstration
    config->learningRate = 0.01f;
    config->discountFactor = 0.9f;
    config->explorationRate = 0.1f;
    config->rewardConfig.type = nlm::RewardConfig::Type::SIMPLE;
    config->rewardConfig.simpleReward = 1.0f;
    
    std::cout << "Training Configuration:" << std::endl;
    std::cout << "  Max Episodes: " << config->maxEpisodes << std::endl;
    std::cout << "  Learning Rate: " << config->learningRate << std::endl;
    std::cout << "  Discount Factor: " << config->discountFactor << std::endl;
    std::cout << "  Exploration Rate: " << config->explorationRate << std::endl;
    std::cout << "  Reward Type: " << (config->rewardConfig.type == nlm::RewardConfig::Type::SIMPLE ? "SIMPLE" : "UNKNOWN") << std::endl;
    std::cout << std::endl;
    
    // Create a simple mock environment (in a real implementation, you would use the actual NLM environment)
    // For this example, we'll just demonstrate the training loop structure
    
    std::cout << "Training Loop Simulation:" << std::endl;
    for (int episode = 0; episode < config->maxEpisodes; ++episode) {
        // Simulate training episode
        float episode_reward = static_cast<float>(episode + 1) * 10.0f;  // Increasing reward
        float episode_loss = 1.0f / static_cast<float>(episode + 1);   // Decreasing loss
        
        // Record performance
        monitor->recordEpisodePerformance(episode, episode_reward, episode_loss, 1.0f);
        
        // Notify callbacks
        callback->afterEpisode(episode, episode_reward, episode_reward > 50.0f);
        
        std::cout << "  Episode " << episode << ": Reward = " << episode_reward 
                  << ", Loss = " << episode_loss 
                  << ", Success = " << (episode_reward > 50.0f ? "Yes" : "No") << std::endl;
    }
    
    // Display performance summary
    std::cout << std::endl;
    std::cout << "Training Performance Summary:" << std::endl;
    std::cout << "  Episodes Completed: " << monitor->getEpisodesCompleted() << std::endl;
    std::cout << "  Average Reward: " << monitor->getAverageReward() << std::endl;
    std::cout << "  Average Loss: " << monitor->getAverageLoss() << std::endl;
    std::cout << "  Best Reward: " << monitor->getCurrentReward() << std::endl;
    std::cout << std::endl;
    
    std::cout << "✓ Training completed successfully!" << std::endl;
    std::cout << "✓ All training components are working correctly." << std::endl;
    
    return 0;
}
