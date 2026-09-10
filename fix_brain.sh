cd /workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_2aacbc76-8a67-4084-b7ff-645ef4a196d0

# Backup original file
cp src/brain/Brain.cpp src/brain/Brain.cpp.bak

# Read current content and fix it
cat > /tmp/fix_brain.cpp << 'EOF'
#!/usr/bin/env python3
import re

with open('src/brain/Brain.cpp', 'r') as f:
    content = f.read()

# Find and replace the problematic initialization
# Replace lines 86-132 with proper initialization
pattern = r'// Initialize semantic memory.*?// Initialize checkpoint manager.*?\n'
replacement = '''        // Initialize plasticity systems
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // Initialize semantic memory
        semanticMemory = std::make_unique<SemanticMemory>();
        
        // Initialize procedural memory
        proceduralMemory = std::make_unique<ProceduralMemory>();
        
        // Initialize associative memory (NeuralAssociativeMemory not SemanticMemory)
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        
        // Initialize episodic memory
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        
        // Initialize working memory
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
'''

# Use a simpler approach - manually fix the file
# We'll use sed to delete lines 86-132 and insert the correct content
EOF

# Create a proper fix using sed
sed -i '86,132d' src/brain/Brain.cpp

# Now insert the correct initialization at line 86
cat > /tmp/insert_fix.cpp << 'EOF'
        // Initialize plasticity systems
        spikeSystem = std::make_unique<SpikeSystem>();
        stdp = std::make_unique<STDP>();
        hebbian = std::make_unique<Hebbian>();
        structuralPlasticity = std::make_unique<StructuralPlasticity>();
        
        // Initialize semantic memory
        semanticMemory = std::make_unique<SemanticMemory>();
        
        // Initialize procedural memory
        proceduralMemory = std::make_unique<ProceduralMemory>();
        
        // Initialize associative memory (NeuralAssociativeMemory not SemanticMemory)
        associativeMemory = std::make_unique<NeuralAssociativeMemory>();
        
        // Initialize episodic memory
        episodicMemory = std::make_unique<NeuralEpisodicMemory>();
        
        // Initialize working memory
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
EOF

# Use awk to insert the fix
sed -i '86r /tmp/insert_fix.cpp' src/brain/Brain.cpp
