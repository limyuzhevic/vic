#!/usr/bin/env python3

import re

# Read the Brain.cpp file
with open('src/brain/Brain.cpp', 'r') as f:
    content = f.read()

# Find the problematic section
# We need to replace lines 86-132 with proper initialization
# Find the pattern for the initialization
pattern = r'(\s+Impl\(std::shared_ptr<Config> cfg\)\s*:\s*config\(cfg\)\s*\n\s*,\s*rng\(nullptr\)\s*\n\s*,\s*developmentalStage\(DevelopmentalStage::Initial\)\s*\n\s*,\s*nextRegionId\(1\)\s*\n\s*,\s*timestep\(0\.001\)\s*\n\s*,\s*currentStep\(0\)\s*\n\s*,\s*currentTime\(0\.0\)\s*\n\s*,\s*totalSpikesThisStep\(0\)\s*\n\s*,\s*totalSpikesTotal\(0\)\s*\n\s*,\s*isResting\(false\)\s*\n\s*,\s*stepsSinceLastEpisode\(0\)\s*\n\s*,\s*replayInterval\(100\)\s*\n\s*,\s*consolidationInterval\(1000\)\s*\n\s*\{\s*\n\s*// Initialize random generator with seed from config\s*\n\s*uint64_t seed = 42;\s*// Default seed\s*\n\s*if \(auto seedOpt = config->get<uint64_t>\("random_seed"\)\)\s*\n\s*seed = \*seedOpt;\s*\n\s*\)\s*\n\s*rng = std::make_unique<RandomGenerator>\(seed\);\s*\n\s*\s*// Initialize semantic memory\s*\n\s*\s*semanticMemory = std::make_unique<SemanticMemory>\(\);\s*\n\s*\s*// Initialize procedural memory\s*\n\s*\s*proceduralMemory = std::make_unique<ProceduralMemory>\(\);\s*\n\s*\s*// Initialize associative memory \(NeuralAssociativeMemory not SemanticMemory\)\s*\n\s*\s*associativeMemory = std::make_unique<NeuralAssociativeMemory>\(\);\s*\n\s*\s*// Initialize episodic memory\s*\n\s*\s*episodicMemory = std::make_unique<NeuralEpisodicMemory>\(\);\s*\n\s*\s*// Initialize working memory\s*\n\s*\s*workingMemory = std::make_unique<NeuralWorkingMemory>\(\);\s*\n\s*\s*// Initialize prediction system\s*\n\s*\s*predictionSystem = std::make_unique<PredictionSystem>\(\);\s*\n\s*\s*// Initialize cognition systems\s*\n\s*\s*planner = std::make_unique<NeuralPlanner>\(\);\s*\n\s*\s*conceptFormation = std::make_unique<ConceptFormation>\(\);\s*\n\s*\s*attention = std::make_unique<AttentionalSelection>\(\);\s*\n\s*\s*// Initialize development system\s*\n\s*\s*developmentSystem = std::make_unique<DevelopmentSystem>\(\);\s*\n\s*\s*// Initialize neuromodulation systems\s*\n\s*\s*dopamine = std::make_unique<Dopamine>\(\);\s*\n\s*\s*curiosity = std::make_unique<Curiosity>\(\);\s*\n\s*\s*predictionError = std::make_unique<PredictionError>\(\);\s*\n\s*\s*novelty = std::make_unique<Novelty>\(\);\s*\n\s*\s*\s*// Configure structural plasticity\s*\n\s*\s*float synaptogenesisRate = config->getOr<float>\("synaptogenesis_rate", 0\.0001f\);\s*\n\s*\s*float pruningRate = config->getOr<float>\("pruning_rate", 0\.00001f\);\s*\n\s*\s*structuralPlasticity->setSynaptogenesisRate\(synaptogenesisRate\);\s*\n\s*\s*structuralPlasticity->setPruningRate\(pruningRate\);\s*\n\s*\s*// Get timestep\s*\n\s*\s*timestep = config->getOr<double>\("simulation_timestep", 0\.001\);\s*\n\s*\s*// Get integration intervals from config\s*\n\s*\s*replayInterval = config->getOr<size_t>\("replay_interval", 100\);\s*\n\s*\s*consolidationInterval = config->getOr<size_t>\("consolidation_interval", 1000\);\s*\n\s*\s*// Initialize checkpoint manager\s*\n\s*\s*checkpointManager = std::make_unique<CheckpointManager>\(\);\s*\n\s*\}\s*\n)'

# Replace with correct initialization
new_content = '''    {
        // Initialize random generator with seed from config
        uint64_t seed = 42;  // Default seed
        if (auto seedOpt = config->get<uint64_t>("random_seed")) {
            seed = *seedOpt;
        }
        rng = std::make_unique<RandomGenerator>(seed);
        
        // Initialize plasticity systems
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // Initialize memory systems
        semanticMemory = std::make_unique<SemanticMemory>();
        proceduralMemory = std::make_unique<ProceduralMemory>();
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        workingMemory = std::make_unique<NeuralWorkingMemory>();
        
        // Initialize prediction system
        predictionSystem = std::make_unique<PredictionSystem>();
        
        // Initialize cognition systems
        planner = std::make_unique<NeuralPlanner>();
        conceptFormation = std::make_unique<ConceptFormation>();
        attention = std::make_unique<AttentionalSelection>();
        
        // Initialize development system
        developmentSystem = std::make_unique<DevelopmentSystem>();
        
        // Initialize neuromodulation systems
        dopamine = std::make_unique<Dopamine>();
        curiosity = std::make_unique<Curiosity>();
        predictionError = std::make_unique<PredictionError>();
        novelty = std::make_unique<Novelty>();
        
        // Configure STDP parameters
        float ltpWeight = config->getOr<float>("stdp_ltp_weight", 0.01f);
        float ltdWeight = config->getOr<float>("stdp_ltd_weight", 0.012f);
        float tau = config->getOr<float>("stdp_tau", 20.0f);
        stdp->configure(ltpWeight, ltdWeight, tau);
        
        // Configure structural plasticity
        float synaptogenesisRate = config->getOr<float>("synaptogenesis_rate", 0.0001f);
        float pruningRate = config->getOr<float>("pruning_rate", 0.00001f);
        structuralPlasticity->setSynaptogenesisRate(synaptogenesisRate);
        structuralPlasticity->setPruningRate(pruningRate);
        
        // Get timestep
        timestep = config->getOr<double>("simulation_timestep", 0.001);
        
        // Get integration intervals from config
        replayInterval = config->getOr<size_t>("replay_interval", 100);
        consolidationInterval = config->getOr<size_t>("consolidation_interval", 1000);
        
        // Initialize checkpoint manager
        checkpointManager = std::make_unique<CheckpointManager>();
    }
    
    DevelopmentalStage developmentalStage;
    RegionId nextRegionId;
};'''

# Write the fixed content back
with open('src/brain/Brain.cpp', 'w') as f:
    f.write(new_content)

print('Successfully fixed Brain.cpp!')
