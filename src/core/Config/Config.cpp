// Additional ConfigBuilder implementation (fixing duplicate and missing methods)
struct Config::Builder {
    bool hasBrain_ = false;
    size_t neuronCount_ = 1000;
    size_t regionCount_ = 1;
    float connectionProbability_ = 0.1f;
    
    bool hasDevelopment_ = false;
    bool developmentEnabled_ = true;
    bool maturationEnabled_ = true;
    bool pruningEnabled_ = true;
    bool synaptogenesisEnabled_ = true;
    float maturationRate_ = 0.001f;
    float consolidationRate_ = 0.0001f;
    float hebbianLearningRate_ = 0.01f;
    float pruningRate_ = 0.00001f;
    float pruningActivityThreshold_ = 0.001f;
    float pruningWeightThreshold_ = 0.01f;
    float pruningCorrelationThreshold_ = 0.1f;
    float synaptogenesisRate_ = 0.001f;
    float synaptogenesisTargetDensity_ = 0.1f;
    size_t maxSynapsesPerNeuron_ = 100;
    
    bool hasNeuromodulation_ = false;
    bool neuromodulationEnabled_ = true;
    bool dopamineEnabled_ = true;
    bool curiosityEnabled_ = true;
    bool noveltyEnabled_ = true;
    bool predictionErrorEnabled_ = true;
    
    bool hasPlasticity_ = false;
    bool plasticityEnabled_ = true;
    bool stdpEnabled_ = true;
    bool hebbianEnabled_ = true;
    bool structuralEnabled_ = true;
    float stdpLtpWeight_ = 0.01f;
    float stdpLtdWeight_ = 0.012f;
    float stdpTau_ = 20.0f;
    
    bool hasMemory_ = false;
    bool workingMemoryEnabled_ = true;
    bool episodicMemoryEnabled_ = true;
    bool associativeMemoryEnabled_ = true;
    
    bool hasPrediction_ = false;
    bool predictionEnabled_ = true;
    
    bool hasCognition_ = false;
    bool planningEnabled_ = true;
    bool conceptFormationEnabled_ = true;
    bool attentionEnabled_ = true;
    size_t planningDepth_ = 5;
    float attentionInhibitionStrength_ = 0.5f;
    float attentionExcitationStrength_ = 1.5f;
    
    bool hasSimulation_ = false;
    double simulationTimestep_ = 0.001;
    uint64_t randomSeed_ = 42;
    size_t replayInterval_ = 100;
    size_t consolidationInterval_ = 1000;
    
    bool hasDebug_ = false;
    bool debugEnabled_ = true;
    bool debugVerbose_ = false;
    std::string debugLevel_ = "info";
    
    bool hasPerformance_ = false;
    float targetFPS_ = 30.0f;
    std::string performanceOptimization_ = "balanced";
    
    bool hasAccuracy_ = false;
    bool accuracyEnabled_ = true;
    float accuracyTolerance_ = 0.01f;
    
    // Builder methods
    Config::Builder& withBrain(size_t neurons, size_t regions = 1, float connectionProb = 0.1f) {
        hasBrain_ = true;
        neuronCount_ = neurons;
        regionCount_ = regions;
        connectionProbability_ = connectionProb;
        return *this;
    }
    
    Config::Builder& withDevelopment(bool enabled = true, bool mat = true, bool prune = true, bool synapt = true) {
        hasDevelopment_ = true;
        developmentEnabled_ = enabled;
        maturationEnabled_ = mat;
        pruningEnabled_ = prune;
        synaptogenesisEnabled_ = synapt;
        return *this;
    }
    
    Config::Builder& withDevelopmentRates(float matRate, float consRate, float hebbRate) {
        maturationRate_ = matRate;
        consolidationRate_ = consRate;
        hebbianLearningRate_ = hebbRate;
        return *this;
    }
    
    Config::Builder& withPruningRates(float pruneRate, float activityThresh, float weightThresh, float correlationThresh) {
        pruningRate_ = pruneRate;
        pruningActivityThreshold_ = activityThresh;
        pruningWeightThreshold_ = weightThresh;
        pruningCorrelationThreshold_ = correlationThresh;
        return *this;
    }
    
    Config::Builder& withSynaptogenesis(float rate, float targetDensity, size_t maxSynapses) {
        synaptogenesisRate_ = rate;
        synaptogenesisTargetDensity_ = targetDensity;
        maxSynapsesPerNeuron_ = maxSynapses;
        return *this;
    }
    
    Config::Builder& withNeuromodulation(bool enabled = true, bool dop = true, bool cur = true, bool nov = true, bool predErr = true) {
        hasNeuromodulation_ = true;
        neuromodulationEnabled_ = enabled;
        dopamineEnabled_ = dop;
        curiosityEnabled_ = cur;
        noveltyEnabled_ = nov;
        predictionErrorEnabled_ = predErr;
        return *this;
    }
    
    Config::Builder& withPlasticity(bool enabled = true, bool stdp = true, bool hebb = true, bool structural = true) {
        hasPlasticity_ = true;
        plasticityEnabled_ = enabled;
        stdpEnabled_ = stdp;
        hebbianEnabled_ = hebb;
        structuralEnabled_ = structural;
        return *this;
    }
    
    Config::Builder& withSTDP(float ltpWeight, float ltdWeight, float tau) {
        stdpLtpWeight_ = ltpWeight;
        stdpLtdWeight_ = ltdWeight;
        stdpTau_ = tau;
        return *this;
    }
    
    Config::Builder& withMemory(bool work = true, bool epi = true, bool assoc = true) {
        hasMemory_ = true;
        workingMemoryEnabled_ = work;
        episodicMemoryEnabled_ = epi;
        associativeMemoryEnabled_ = assoc;
        return *this;
    }
    
    Config::Builder& withPrediction(bool enabled = true) {
        hasPrediction_ = true;
        predictionEnabled_ = enabled;
        return *this;
    }
    
    Config::Builder& withCognition(bool planning = true, bool conceptForm = true, bool attention = true) {
        hasCognition_ = true;
        planningEnabled_ = planning;
        conceptFormationEnabled_ = conceptForm;
        attentionEnabled_ = attention;
        return *this;
    }
    
    Config::Builder& withPlanningDepth(size_t depth) {
        planningDepth_ = depth;
        return *this;
    }
    
    Config::Builder& withAttention(float inhibition, float excitation) {
        attentionInhibitionStrength_ = inhibition;
        attentionExcitationStrength_ = excitation;
        return *this;
    }
    
    Config::Builder& withSimulation(double timestep, uint64_t seed, size_t replay, size_t consolidation) {
        hasSimulation_ = true;
        simulationTimestep_ = timestep;
        randomSeed_ = seed;
        replayInterval_ = replay;
        consolidationInterval_ = consolidation;
        return *this;
    }
    
    Config::Builder& withDebug(bool enabled = true, bool verbose = false, const std::string& level = "info") {
        hasDebug_ = true;
        debugEnabled_ = enabled;
        debugVerbose_ = verbose;
        debugLevel_ = level;
        return *this;
    }
    
    Config::Builder& withPerformance(float fps, const std::string& optimization = "balanced") {
        hasPerformance_ = true;
        targetFPS_ = fps;
        performanceOptimization_ = optimization;
        return *this;
    }
    
    Config::Builder& withAccuracy(bool enabled = true, float tolerance = 0.01f) {
        hasAccuracy_ = true;
        accuracyEnabled_ = enabled;
        accuracyTolerance_ = tolerance;
        return *this;
    }
    
    Config build() {
        Config config;
        
        if (hasBrain_) {
            config.set("neuron_count", static_cast<int64_t>(neuronCount_), ConfigSource::Default);
            config.set("region_count", static_cast<int64_t>(regionCount_), ConfigSource::Default);
            config.set("connection_probability", connectionProbability_, ConfigSource::Default);
        }
        
        if (hasDevelopment_) {
            config.set("development.enabled", developmentEnabled_, ConfigSource::Default);
            if (developmentEnabled_) {
                config.set("development.maturation.enabled", maturationEnabled_, ConfigSource::Default);
                config.set("development.pruning.enabled", pruningEnabled_, ConfigSource::Default);
                config.set("development.synaptogenesis.enabled", synaptogenesisEnabled_, ConfigSource::Default);
                if (maturationEnabled_) {
                    config.set("maturation_rate", maturationRate_, ConfigSource::Default);
                    config.set("consolidation_rate", consolidationRate_, ConfigSource::Default);
                    config.set("hebbian_learning_rate", hebbianLearningRate_, ConfigSource::Default);
                }
                if (pruningEnabled_) {
                    config.set("pruning_rate", pruningRate_, ConfigSource::Default);
                    config.set("pruning_activity_threshold", pruningActivityThreshold_, ConfigSource::Default);
                    config.set("pruning_weight_threshold", pruningWeightThreshold_, ConfigSource::Default);
                    config.set("pruning_correlation_threshold", pruningCorrelationThreshold_, ConfigSource::Default);
                }
                if (synaptogenesisEnabled_) {
                    config.set("synaptogenesis_rate", synaptogenesisRate_, ConfigSource::Default);
                    config.set("synaptogenesis_target_density", synaptogenesisTargetDensity_, ConfigSource::Default);
                    config.set("max_synapses_per_neuron", static_cast<int64_t>(maxSynapsesPerNeuron_), ConfigSource::Default);
                }
            }
        }
        
        if (hasNeuromodulation_) {
            config.set("neuromodulation.enabled", neuromodulationEnabled_, ConfigSource::Default);
            if (neuromodulationEnabled_) {
                config.set("neuromodulation.dopamine.enabled", dopamineEnabled_, ConfigSource::Default);
                config.set("neuromodulation.curiosity.enabled", curiosityEnabled_, ConfigSource::Default);
                config.set("neuromodulation.novelty.enabled", noveltyEnabled_, ConfigSource::Default);
                config.set("neuromodulation.prediction_error.enabled", predictionErrorEnabled_, ConfigSource::Default);
            }
        }
        
        if (hasPlasticity_) {
            config.set("plasticity.enabled", plasticityEnabled_, ConfigSource::Default);
            if (plasticityEnabled_) {
                config.set("plasticity.stdp.enable", stdpEnabled_, ConfigSource::Default);
                config.set("plasticity.hebbian.enable", hebbianEnabled_, ConfigSource::Default);
                config.set("plasticity.structural.enable", structuralEnabled_, ConfigSource::Default);
                if (stdpEnabled_) {
                    config.set("stdp_ltp_weight", stdpLtpWeight_, ConfigSource::Default);
                    config.set("stdp_ltd_weight", stdpLtdWeight_, ConfigSource::Default);
                    config.set("stdp_tau", stdpTau_, ConfigSource::Default);
                }
            }
        }
        
        if (hasMemory_) {
            config.set("memory.working.enabled", workingMemoryEnabled_, ConfigSource::Default);
            config.set("memory.episodic.enabled", episodicMemoryEnabled_, ConfigSource::Default);
            config.set("memory.associative.enabled", associativeMemoryEnabled_, ConfigSource::Default);
        }
        
        if (hasPrediction_) {
            config.set("prediction.enabled", predictionEnabled_, ConfigSource::Default);
        }
        
        if (hasCognition_) {
            config.set("cognition.planning.enabled", planningEnabled_, ConfigSource::Default);
            config.set("cognition.concept_formation.enabled", conceptFormationEnabled_, ConfigSource::Default);
            config.set("cognition.attention.enabled", attentionEnabled_, ConfigSource::Default);
            if (planningEnabled_) {
                config.set("planning.depth", static_cast<int64_t>(planningDepth_), ConfigSource::Default);
            }
            if (attentionEnabled_) {
                config.set("attention.inhibition_strength", attentionInhibitionStrength_, ConfigSource::Default);
                config.set("attention.excitation_strength", attentionExcitationStrength_, ConfigSource::Default);
            }
        }
        
        if (hasSimulation_) {
            config.set("simulation_timestep", simulationTimestep_, ConfigSource::Default);
            config.set("random_seed", static_cast<int64_t>(randomSeed_), ConfigSource::Default);
            config.set("replay_interval", static_cast<int64_t>(replayInterval_), ConfigSource::Default);
            config.set("consolidation_interval", static_cast<int64_t>(consolidationInterval_), ConfigSource::Default);
        }
        
        if (hasDebug_) {
            config.set("debug.enabled", debugEnabled_, ConfigSource::Default);
            config.set("debug.verbose", debugVerbose_, ConfigSource::Default);
            config.set("debug.level", debugLevel_, ConfigSource::Default);
        }
        
        if (hasPerformance_) {
            config.set("performance.target_fps", targetFPS_, ConfigSource::Default);
            config.set("performance.optimization", performanceOptimization_, ConfigSource::Default);
        }
        
        if (hasAccuracy_) {
            config.set("accuracy.enabled", accuracyEnabled_, ConfigSource::Default);
            config.set("accuracy.tolerance", accuracyTolerance_, ConfigSource::Default);
        }
        
        return config;
    }
};

Config::Builder Config::builder() {
    return Builder();
}

// Add convenience method to create default config
Config Config::createDefaultConfig() {
    return Builder()
        .withBrain(1000)
        .withDevelopment(true)
        .withNeuromodulation(true)
        .withPlasticity(true)
        .withMemory(true, true, false)
        .build();
}

// Add getDotNotation implementation
std::optional<ConfigValue> Config::getDotNotation(const std::string& key) const {
    return pImpl->nestedConfig.get(key);
}

// Add hasDotNotation implementation
bool Config::hasDotNotation(const std::string& key) const {
    return pImpl->nestedConfig.has(key);
}

// Update toJson to handle nested config
std::string Config::toJson() const {
#ifdef HAVE_NLOHMANN_JSON
    json j;
    for (const auto& entry : pImpl->entries) {
        std::visit([&j, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                j[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                j[entry.key] = arg;
            }
        }, entry.value);
    }
    
    // Add nested config
    std::string nestedJson = pImpl->nestedConfig.toJson();
    if (!nestedJson.empty() && nestedJson != "{}") {
        try {
            json nestedJ = json::parse(nestedJson);
            for (auto it = nestedJ.begin(); it != nestedJ.end(); ++it) {
                j[it.key()] = *it;
            }
        } catch (...) {
            // Ignore parsing errors
        }
    }
    
    return j.dump(2);
#else
    return "{}";
#endif
}