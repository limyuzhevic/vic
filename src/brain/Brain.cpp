    NLM_LOG_INFO("  Region " + std::to_string(region->getId().index()) + 
                    " (" + region->getName() + "): " +
                    std::to_string(region->getTotalNeuronCount()) + " neurons, " +
                    std::to_string(region->getSynapseCount()) + " synapses, " +
                    "avg weight: " + std::to_string(region->getAverageSynapticWeight()));
    }
    
    NLM_LOG_INFO("");
}

// Static utility functions for safe configuration loading
bool Brain::safeLoadConfigFromFile(Config& config, const std::string& filepath) {
    return SafeConfigLoader::safeLoadFromFile(config, filepath);
}

bool Brain::safeLoadConfigFromArgs(Config& config, int argc, char** argv) {
    return SafeConfigLoader::safeLoadFromArgs(config, argc, argv);
}

// Execute step with safe error handling
bool Brain::executeStepSafely(SimulationStep currentStep, Timestamp currentTime) {
    if (!pImpl->safeStepExecutor) {
        NLM_LOG_ERROR("Safe step executor not initialized");
        return false;
    }
    
    return pImpl->safeStepExecutor->executeStep(currentStep, currentTime);
}

} // namespace nlm