#include "src/neuromodulation/Reward.hpp"
#include "src/world/Observation.hpp"
#include <iostream>

int main() {
    std::cout << "Testing Reward class compilation..." << std::endl;
    
    nlm::Reward reward;
    auto config = std::make_shared<nlm::Config>();
    config->set("reward_sparse", true);
    reward.initialize(config);
    
    std::cout << "Reward initialized successfully." << std::endl;
    std::cout << "Goal reward: " << reward.getGoalReward() << std::endl;
    std::cout << "Current value: " << reward.getValue() << std::endl;
    
    return 0;
}
