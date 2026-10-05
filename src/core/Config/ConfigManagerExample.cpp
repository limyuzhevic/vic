// Example usage of ConfigManager in Brain.cpp (before and after comparison)

// BEFORE: Repetitive config->getOr<...>() patterns:
// size_t neuronCount = pImpl->config->getOr<size_t>("neuron_count", 1000);
// size_t regionCount = pImpl->config->getOr<size_t>("region_count", 1);
// float connectionProbability = pImpl->config->getOr<float>("connection_probability", 0.1f);
// float ltpWeight = pImpl->config->getOr<float>("stdp_ltp_weight", 0.01f);
// float ltdWeight = pImpl->config->getOr<float>("stdp_ltd_weight", 0.012f);
// float tau = pImpl->config->getOr<float>("stdp_tau", 20.0f);
// float synaptogenesisRate = pImpl->config->getOr<float>("synaptogenesis_rate", 0.0001f);
// float pruningRate = pImpl->config->getOr<float>("pruning_rate", 0.00001f);
// double timestep = pImpl->config->getOr<double>("simulation_timestep", 0.001);
// size_t replayInterval = pImpl->config->getOr<size_t>("replay_interval", 100);
// size_t consolidationInterval = pImpl->config->getOr<size_t>("consolidation_interval", 1000);
// std::string checkpointDir = pImpl->config->getOr<std::string>("checkpoint_dir", "./checkpoints");

// AFTER: Simplified ConfigManager usage:
// size_t neuronCount = pImpl->configManager->getConfigValue<size_t>("neuron_count", 1000);
// size_t regionCount = pImpl->configManager->getConfigValue<size_t>("region_count", 1);
// float connectionProbability = pImpl->configManager->getConfigValue<float>("connection_probability", 0.1f);
// float ltpWeight = pImpl->configManager->getConfigValue<float>("stdp_ltp_weight", 0.01f);
// float ltdWeight = pImpl->configManager->getConfigValue<float>("stdp_ltd_weight", 0.012f);
// float tau = pImpl->configManager->getConfigValue<float>("stdp_tau", 20.0f);
// float synaptogenesisRate = pImpl->configManager->getConfigValue<float>("synaptogenesis_rate", 0.0001f);
// float pruningRate = pImpl->configManager->getConfigValue<float>("pruning_rate", 0.00001f);
// double timestep = pImpl->configManager->getConfigValue<double>("simulation_timestep", 0.001);
// size_t replayInterval = pImpl->configManager->getConfigValue<size_t>("replay_interval", 100);
// size_t consolidationInterval = pImpl->configManager->getConfigValue<size_t>("consolidation_interval", 1000);
// std::string checkpointDir = pImpl->configManager->getConfigValue<std::string>("checkpoint_dir", "./checkpoints");

// Benefits of ConfigManager:
// 1. Type-safe access with default values
// 2. Caching for performance
// 3. Error handling
// 4. Validation support
// 5. Reduced code duplication
// 6. Simpler, cleaner interface

// Example of validation:
// float validatedRate = pImpl->configManager->getValidatedConfigValue<float>(
//     "synaptogenesis_rate", 
//     0.0001f,
//     [](float rate) { return rate >= 0.0f && rate <= 1.0f; }
// );