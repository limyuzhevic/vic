// Implementation of advanced agent features in AgentBrain.cpp

// Add these implementations to the AgentBrain.cpp file

// Process advanced command
bool AgentBrain::processAdvancedCommand(const std::string& commandLine) {
    return advancedCommandHandler_.executeCommand(*this, commandLine);
}

// Enable feature
void AgentBrain::enableFeature(const std::string& featureName) {
    if (featureName == "meta_learning") {
        featureFlags_.enableMetaLearning = true;
    } else if (featureName == "memory_integration") {
        featureFlags_.enableMemoryIntegration = true;
    } else if (featureName == "adaptive_curiosity") {
        featureFlags_.enableAdaptiveCuriosity = true;
    } else if (featureName == "social_learning") {
        featureFlags_.enableSocialLearning = true;
    } else if (featureName == "transfer_learning") {
        featureFlags_.enableTransferLearning = true;
    } else if (featureName == "self_reflection") {
        featureFlags_.enableSelfReflection = true;
    } else if (featureName == "emergent_behaviors") {
        featureFlags_.enableEmergentBehaviors = true;
    } else if (featureName == "goal_directed") {
        featureFlags_.enableGoalDirectedBehavior = true;
    }
}

// Disable feature
void AgentBrain::disableFeature(const std::string& featureName) {
    if (featureName == "meta_learning") {
        featureFlags_.enableMetaLearning = false;
    } else if (featureName == "memory_integration") {
        featureFlags_.enableMemoryIntegration = false;
    } else if (featureName == "adaptive_curiosity") {
        featureFlags_.enableAdaptiveCuriosity = false;
    } else if (featureName == "social_learning") {
        featureFlags_.enableSocialLearning = false;
    } else if (featureName == "transfer_learning") {
        featureFlags_.enableTransferLearning = false;
    } else if (featureName == "self_reflection") {
        featureFlags_.enableSelfReflection = false;
    } else if (featureName == "emergent_behaviors") {
        featureFlags_.enableEmergentBehaviors = false;
    } else if (featureName == "goal_directed") {
        featureFlags_.enableGoalDirectedBehavior = false;
    }
}

// Check if feature is enabled
bool AgentBrain::isFeatureEnabled(const std::string& featureName) const {
    if (featureName == "meta_learning") return featureFlags_.enableMetaLearning;
    if (featureName == "memory_integration") return featureFlags_.enableMemoryIntegration;
    if (featureName == "adaptive_curiosity") return featureFlags_.enableAdaptiveCuriosity;
    if (featureName == "social_learning") return featureFlags_.enableSocialLearning;
    if (featureName == "transfer_learning") return featureFlags_.enableTransferLearning;
    if (featureName == "self_reflection") return featureFlags_.enableSelfReflection;
    if (featureName == "emergent_behaviors") return featureFlags_.enableEmergentBehaviors;
    if (featureName == "goal_directed") return featureFlags_.enableGoalDirectedBehavior;
    return false;
}

// Register advanced command
void AgentBrain::registerAdvancedCommand(const std::string& name, std::shared_ptr<AgentCommand> command) {
    advancedCommandHandler_.registerCommand(name, command);
}

// Set feature flag value
void AgentBrain::setFeatureFlag(const std::string& featureName, float value) {
    if (featureName == "meta_learning_rate") {
        featureFlags_.metaLearningRate = value;
    } else if (featureName == "memory_integration_weight") {
        featureFlags_.memoryIntegrationWeight = value;
    } else if (featureName == "curiosity_adaptation_rate") {
        featureFlags_.curiosityAdaptationRate = value;
    } else if (featureName == "social_learning_rate") {
        featureFlags_.socialLearningRate = value;
    } else if (featureName == "transfer_learning_threshold") {
        featureFlags_.transferLearningThreshold = value;
    } else if (featureName == "self_reflection_threshold") {
        featureFlags_.selfReflectionThreshold = value;
    }
}

// Get feature flag value
float AgentBrain::getFeatureFlag(const std::string& featureName) const {
    if (featureName == "meta_learning_rate") return featureFlags_.metaLearningRate;
    if (featureName == "memory_integration_weight") return featureFlags_.memoryIntegrationWeight;
    if (featureName == "curiosity_adaptation_rate") return featureFlags_.curiosityAdaptationRate;
    if (featureName == "social_learning_rate") return featureFlags_.socialLearningRate;
    if (featureName == "transfer_learning_threshold") return featureFlags_.transferLearningThreshold;
    if (featureName == "self_reflection_threshold") return featureFlags_.selfReflectionThreshold;
    return 0.0f;
}