# NLM Phase 6 Advanced Usage Examples

This document provides advanced examples for using the Phase 6 NLM systems in complex scenarios and research applications. It builds upon the basic integration examples and demonstrates how to leverage the full power of the integrated artificial brain.

## Table of Contents

1. [Advanced Concept Learning](#advanced-concept-learning)
2. [Multi-Agent Integration](#multi-agent-integration)
3. [Developmental Learning Scenarios](#developmental-learning-scenarios)
4. [Memory Consolidation Strategies](#memory-consolidation-strategies)
5. [Adaptive Planning with Uncertainty](#adaptive-planning-with-uncertainty)
6. [Neuromodulation-Based Behavior Adaptation](#neuromodulation-based-behavior-adaptation)
7. [Experiment Design and Analysis](#experiment-design-and-analysis)
8. [Performance Optimization](#performance-optimization)
9. [Parameter Tuning](#parameter-tuning)
10. [Research Applications](#research-applications)

## Advanced Concept Learning

### 1. Hierarchical Concept Learning

**Scenario**: Learning concepts at multiple levels of abstraction

**Implementation**:```cpp
void HierarchicalConceptLearning() {
    // Create brain with enhanced concept formation
    Brain brain({"neuron_count": 2000, "region_count": 2});
    brain.initialize();
    
    ConceptFormation* conceptForm = brain.getConceptFormation();
    
    // Phase 1: Learn low-level sensory concepts
    std::vector<float> lowLevelPattern1 = {0.1, 0.2, 0.9, 0.3, 0.4};
    size_t lowLevelConcept1 = conceptForm->presentExperience(
        lowLevelPattern1, {0.3, 0.7, 0.1}, 0.8, 1);
    \n    std::vector<float> lowLevelPattern2 = {0.8, 0.1, 0.3, 0.9, 0.2};
    size_t lowLevelConcept2 = conceptForm->presentExperience(
        lowLevelPattern2, {0.9, 0.2, 0.4}, 0.6, 2);
    \n    // Phase 2: Detect similarity between low-level concepts
    float similarity = conceptForm->computeSimilarity(lowLevelConcept1, lowLevelConcept2);
    if (similarity > 0.7) {
        // Merge similar low-level concepts
        conceptForm->mergeConcepts(lowLevelConcept1, lowLevelConcept2);
    }
    \n    // Phase 3: Learn high-level abstract concepts
    std::vector<float> highLevelPattern = {
        (conceptForm->getConceptPrototype(lowLevelConcept1)[0] + 
         conceptForm->getConceptPrototype(lowLevelConcept2)[0]) / 2.0f,
        (conceptForm->getConceptPrototype(lowLevelConcept1)[1] + 
         conceptForm->getConceptPrototype(lowLevelConcept2)[1]) / 2.0f,
        0.8
    };
    \n    size_t highLevelConcept = conceptForm->presentExperience(
        highLevelPattern, {0.5, 0.3, 0.9}, 0.9, 10);
    \n    // Establish hierarchical relationship
    conceptForm->establishConceptRelationship(highLevelConcept, lowLevelConcept1, 0.8);
    conceptForm->establishConceptRelationship(highLevelConcept, lowLevelConcept2, 0.7);
}
```

### 2. Concept Evolution Over Time

**Scenario**: Concepts evolve and adapt based on ongoing experience

**Implementation**:```cpp
void ConceptEvolution() {
    Brain brain({"neuron_count": 1000});
    brain.initialize();
    
    ConceptFormation* conceptForm = brain.getConceptFormation();
    DevelopmentSystem* development = brain.getDevelopmentSystem();
    
    // Track concept stability over time
    std::vector<float> conceptStabilities;
    std::vector<size_t> conceptIds;
    \n    for (int step = 0; step < 5000; ++step) {
        // Simulate experience
        std::vector<float> pattern = generateRandomPattern();
        float reward = computeReward(pattern);
        \n        // Present experience
        size_t conceptId = conceptForm->presentExperience(
            pattern, extractFeatures(pattern), reward, step);
        \n        if (conceptId > 0) {
            conceptIds.push_back(conceptId);
            conceptStabilities.push_back(
                conceptForm->getConceptStability(conceptId));
        }
        \n        // Record concept evolution
        if (step % 500 == 0) {
            printConceptEvolution(conceptIds, conceptStabilities, step);
        }
        \n        // Development affects concept formation
        if (step % 100 == 0) {
            development->update(&brain, *brain.getRandomGenerator(), 1.0);
        }
        
        brain.step(step, step * 0.001);
    }
    
    analyzeConceptEvolution(conceptIds, conceptStabilities);
}
```

## Multi-Agent Integration

### 1. Competitive Learning Between Agents

**Scenario**: Multiple agents learning in the same environment

**Implementation**:```cpp
class MultiAgentCompetition {
private:
    std::vector<std::shared_ptr<Brain>> brains;
    std::vector<std::shared_ptr<AgentBrain>> agents;
    std::vector<std::shared_ptr<SimpleWorld>> worlds;
    \npublic:
    void initialize(int numAgents, int neuronsPerAgent) {
        for (int i = 0; i < numAgents; ++i) {
            auto brain = std::make_shared<Brain>({
                {"neuron_count": neuronsPerAgent,
                 "region_count": 1,
                 "connection_probability": 0.1}
            });
            brain->initialize();
            brains.push_back(brain);
            \n            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(*worlds[i]);
            agents.push_back(agent);
        }
    }
    \n    void runCompetition(int steps) {
        for (int step = 0; step < steps; ++step) {
            for (int i = 0; i < agents.size(); ++i) {
                // Each agent gets unique observation
                SensoryPercept percept = worlds[i]->observe(brains[i].get());
                \n                // Process sensory input
                agents[i]->processSensoryInput(percept);
                \n                // Brain step
                brains[i]->step(step, step * 0.001);
                \n                // Get motor command
                MotorCommand cmd = agents[i]->decodeMotorCommand();
                \n                // Apply action
                worlds[i]->applyAction(agents[i].get(), cmd);
                \n                // Compute reward
                float reward = worlds[i]->computeReward(agents[i].get());
                \n                // Apply reward modulation
                agents[i]->applyRewardModulation(reward, 0.0);
            }
            \n            // Check for concept formation competition
            checkConceptFormationCompetition();
        }
    }
    \n    void checkConceptFormationCompetition() {
        std::map<size_t, int> conceptCounts;
        for (auto& brain : brains) {
            ConceptFormation* conceptForm = brain->getConceptFormation();
            if (conceptForm) {
                size_t conceptCount = conceptForm->getConceptCount();
                conceptCounts[conceptCount]++;
            }
        }
        \n        // Identify most successful concept formation strategies
        printCompetitionResults(conceptCounts);
    }
};
```

### 2. Social Learning Between Agents

**Scenario**: Agents learn from observing each other

**Implementation**:```cpp
class SocialLearning {
private:
    std::vector<std::shared_ptr<Brain>> brains;
    std::vector<std::shared_ptr<AgentBrain>> agents;
    float socialLearningRate;
    \npublic:
    SocialLearning(float rate) : socialLearningRate(rate) {}
    \n    void enableSocialLearning() {
        for (int i = 0; i < agents.size(); ++i) {
            agents[i]->enableSocialLearning(true);
        }
    }
    \n    void socialLearningInteraction(int observerId, int modelId) {
        if (observerId < 0 || observerId >= agents.size() || 
            modelId < 0 || modelId >= agents.size()) {
            return;
        }
        \n        // Model agent demonstrates behavior
        MotorCommand modelAction = agents[modelId]->decodeMotorCommand();
        \n        // Observer agent imitates behavior
        agents[observerId]->imitateAction(modelAction, socialLearningRate);
        \n        // Transfer concepts if model has more concepts
        ConceptFormation* modelConceptForm = brains[modelId]->getConceptFormation();
        ConceptFormation* observerConceptForm = brains[observerId]->getConceptFormation();
        \n        if (modelConceptForm && observerConceptForm) {
            int modelConceptCount = modelConceptForm->getConceptCount();
            int observerConceptCount = observerConceptForm->getConceptCount();
            \n            if (modelConceptCount > observerConceptCount) {
                // Transfer concept prototypes
                transferConcepts(observerConceptForm, modelConceptForm, 
                               socialLearningRate);
            }
        }
    }
    \n    void transferConcepts(ConceptFormation* target, 
                         ConceptFormation* source, 
                         float rate) {
        const auto& sourceConcepts = source->getConcepts();
        int sourceCount = sourceConcepts.size();
        int targetCount = target->getConceptCount();
        \n        int transferCount = static_cast<int>(static_cast<float>(sourceCount - targetCount) * rate);
        transferCount = std::min(transferCount, 2); // Transfer at most 2 concepts
        \n        for (int i = 0; i < transferCount; ++i) {
            int sourceIndex = targetCount + i;
            if (sourceIndex < sourceCount) {
                auto& sourceConcept = sourceConcepts[sourceIndex];
                std::vector<float> proto = source->getConceptPrototype(sourceConcept.id);
                std::vector<float> features = source->getConceptInstances(sourceConcept.id);
                float stability = source->getConceptStability(sourceConcept.id);
                int observations = sourceConcept.totalObservations;
                \n                // Present to target concept formation
                target->presentExperience(proto, features, stability, 9999);
            }
        }
    }
};
```

## Developmental Learning Scenarios

### 1. Critical Period Learning

**Scenario**: Enhanced learning during specific developmental stages

**Implementation**:```cpp
class CriticalPeriodLearning {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agent;
    DevelopmentSystem* development;
    \npublic:
    void setupCriticalPeriodScenario() {
        brain = std::make_shared<Brain>({
            {"neuron_count": 1500,
             "region_count": 2,
             "stdp_ltp_weight": 0.02f,  // Enhanced LTP during critical period
             "stdp_ltd_weight": 0.018f}  // Enhanced LTD during critical period
        });
        brain->initialize();
        \n        agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*SimpleWorld());
        \n        development = brain->getDevelopmentSystem();
        agent->enableDevelopment(true);
    }
    \n    void simulateCriticalPeriodLearning() {
        for (int step = 0; step < 3000; ++step) {
            // Get observation
            SensoryPercept percept = SimpleWorld()->observe(agent.get());
            \n            // Process sensory input
            agent->processSensoryInput(percept);
            \n            // Brain step
            brain->step(step, step * 0.001);
            \n            // Agent development
            agent->updateDevelopment(0.001);
            \n            // Enhanced plasticity during critical period
            if (development->getCurrentStage() == DevelopmentalStage::CriticalPeriod) {
                enhanceLearningDuringCriticalPeriod(step);
            }
            \n            // Record learning metrics
            if (step % 500 == 0) {
                recordCriticalPeriodMetrics(step);
            }
        }
    }
    \n    void enhanceLearningDuringCriticalPeriod(int step) {
        // Increase plasticity rates
        float criticalPeriodMultiplier = 1.5f;
        \n        // Enhance concept formation learning
        ConceptFormation* conceptForm = brain->getConceptFormation();
        if (conceptForm) {
            conceptForm->setLearningRate(conceptForm->getLearningRate() * criticalPeriodMultiplier);
        }
        \n        // Enhance prediction system learning
        PredictionSystem* prediction = brain->getPredictionSystem();
        if (prediction) {
            prediction->updateParameters(prediction->getPredictionError() * criticalPeriodMultiplier,
                                        prediction->getConfidence() * criticalPeriodMultiplier);
        }
        \n        // Enhance working memory trace strength
        auto* workingMem = brain->getWorkingMemory();
        if (workingMem) {
            workingMem->setTraceRetention(criticalPeriodMultiplier);
        }
    }
};
```

### 2. Skill Acquisition Over Development

**Scenario**: Progressive skill acquisition through developmental stages

**Implementation**:```cpp
class SkillAcquisitionDevelopment {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agent;
    NeuralPlanner* planner;
    ConceptFormation* conceptForm;
    \n    struct Skill {
        std::string name;
        std::vector<float> conceptPrototype;
        size_t conceptId;
        float masteryLevel;
        float developmentRequirement;
    };
    \n    std::vector<Skill> skills;
    \npublic:
    void setupSkillAcquisitionScenario() {
        brain = std::make_shared<Brain>({
            {"neuron_count": 1200,
             "region_count": 1}
        });
        brain->initialize();
        \n        agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*SimpleWorld());
        \n        planner = brain->getPlanner();
        conceptForm = brain->getConceptFormation();
        \n        // Define skills
        initializeSkills();
        \n        agent->enableDevelopment(true);
    }
    \n    void simulateSkillAcquisition() {
        for (int step = 0; step < 4000; ++step) {
            // Get observation
            SensoryPercept percept = SimpleWorld()->observe(agent.get());
            \n            // Process sensory input
            agent->processSensoryInput(percept);
            \n            // Brain step
            brain->step(step, step * 0.001);
            \n            // Agent development
            agent->updateDevelopment(0.001);
            \n            // Check for skill acquisition
            checkSkillAcquisition(step);
            \n            // Apply learned skills
            applyAcquiredSkills();
        }
    }
    \n    void initializeSkills() {
        // Basic navigation skill
        Skill navSkill;
        navSkill.name = "Navigation";
        navSkill.conceptPrototype = {0.8, 0.7, 0.6, 0.5, 0.4};
        navSkill.masteryLevel = 0.0f;
        navSkill.developmentRequirement = 1000.0f;
        skills.push_back(navSkill);
        \n        // Object recognition skill
        Skill objSkill;
        objSkill.name = "Object Recognition";
        objSkill.conceptPrototype = {0.9, 0.1, 0.8, 0.2, 0.7};
        objSkill.masteryLevel = 0.0f;
        objSkill.developmentRequirement = 1500.0f;
        skills.push_back(objSkill);
        \n        // Social interaction skill
        Skill socialSkill;
        socialSkill.name = "Social Interaction";
        socialSkill.conceptPrototype = {0.3, 0.9, 0.4, 0.8, 0.6};
        socialSkill.masteryLevel = 0.0f;
        socialSkill.developmentRequirement = 2000.0f;
        skills.push_back(socialSkill);
    }
    \n    void checkSkillAcquisition(int step) {
        DevelopmentStage stage = brain->getDevelopmentSystem()->getCurrentStage();
        \n        for (auto& skill : skills) {
            if (skill.masteryLevel < 1.0f && step >= skill.developmentRequirement) {
                // Check if skill can be acquired
                if (canAcquireSkill(skill, stage)) {
                    acquireSkill(skill);
                }
            }
        }
    }
    \n    bool canAcquireSkill(const Skill& skill, DevelopmentStage stage) {
        // Check concept prototype matching
        size_t conceptId = conceptForm->getMatchingConcept(
            skill.conceptPrototype, 0.7f);
        \n        if (conceptId > 0) {
            float conceptStability = conceptForm->getConceptStability(conceptId);
            float ageFactor = getAgeFactor(stage);
            \n            return conceptStability > 0.5f && ageFactor > 0.7f;
        }
        \n        return false;
    }
    \n    float getAgeFactor(DevelopmentStage stage) {
        switch (stage) {
            case DevelopmentalStage::Initial: return 1.0f;
            case DevelopmentalStage::CriticalPeriod: return 0.9f;
            case DevelopmentalStage::Maturation: return 0.8f;
            case DevelopmentalStage::Adult: return 0.6f;
            default: return 0.5f;
        }
    }
    \n    void acquireSkill(Skill& skill) {
        // Create concept for the skill
        std::vector<float> features = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
        size_t conceptId = conceptForm->presentExperience(
            skill.conceptPrototype, features, 0.8f, 9999);
        \n        if (conceptId > 0) {
            skill.conceptId = conceptId;
            skill.masteryLevel = 0.0f;
            \n            // Add skill to planner
            if (planner) {
                planner->addSkill(conceptId);
            }
            \n            std::cout << "Skill acquired: " << skill.name << " (concept ID: " 
                      << conceptId << ")" << std::endl;
        }
    }
    \n    void applyAcquiredSkills() {
        for (auto& skill : skills) {
            if (skill.masteryLevel > 0.0f) {
                skill.masteryLevel = std::min(1.0f, 
                                            skill.masteryLevel + 0.01f);
                \n                // Apply skill effects
                applySkillEffects(skill);
            }
        }
    }
    \n    void applySkillEffects(const Skill& skill) {
        // Apply skill-based modifications to behavior
        // This is example-specific implementation
        // In real applications, skills would modify action selection,
        // perception processing, or memory encoding
    }
};
```

## Memory Consolidation Strategies

### 1. Multi-Stage Memory Consolidation

**Scenario**: Progressive memory consolidation over time

**Implementation**:```cpp
class MultiStageMemoryConsolidation {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agent;
    \n    struct MemoryStage {
        enum Stage {
            ENCODING,
            CONSOLIDATION,
            RECONSOLIDATION,
            INTEGRATION
        };
        
        Stage stage;
        std::chrono::time_point<std::chrono::steady_clock> stageStart;
        float consolidationStrength;
    };
    \n    std::vector<MemoryStage> memoryStages;
    size_t currentStage;
    \npublic:
    void setupMultiStageConsolidation() {
        brain = std::make_shared<Brain>({
            {"neuron_count": 1000,
             "region_count": 1,
             "consolidation_interval": 500}  // Faster consolidation for this demo
        });
        brain->initialize();
        \n        agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*SimpleWorld());
        \n        initializeMemoryStages();
    }
    \n    void runMultiStageConsolidation(int steps) {
        for (int step = 0; step < steps; ++step) {
            // Get observation
            SensoryPercept percept = SimpleWorld()->observe(agent.get());
            \n            // Process sensory input
            agent->processSensoryInput(percept);
            \n            // Brain step
            brain->step(step, step * 0.001);
            \n            // Update memory stages
            updateMemoryStages(step);
            \n            // Check for memory events
            checkMemoryEvents(step);
            \n            // Force consolidation at specific stages
            if (step % 1000 == 0) {
                forceMemoryConsolidation(step);
            }
        }
    }
    \n    void initializeMemoryStages() {
        memoryStages.clear();
        currentStage = 0;
        \n        // Create different memory stages
        MemoryStage encodingStage;
        encodingStage.stage = MemoryStage::ENCODING;
        encodingStage.stageStart = std::chrono::steady_clock::now();
        encodingStage.consolidationStrength = 0.1f;
        memoryStages.push_back(encodingStage);
        \n        MemoryStage consolidationStage;
        consolidationStage.stage = MemoryStage::CONSOLIDATION;
        consolidationStage.stageStart = std::chrono::steady_clock::now();
        consolidationStage.consolidationStrength = 0.5f;
        memoryStages.push_back(consolidationStage);
        \n        MemoryStage reinsolidationStage;
        reinsolidationStage.stage = MemoryStage::RECONSOLIDATION;
        reinsolidationStage.stageStart = std::chrono::steady_clock::now();
        reinsolidationStage.consolidationStrength = 0.8f;
        memoryStages.push_back(reinsolidationStage);
        \n        MemoryStage integrationStage;
        integrationStage.stage = MemoryStage::INTEGRATION;
        integrationStage.stageStart = std::chrono::steady_clock::now();
        integrationStage.consolidationStrength = 1.0f;
        memoryStages.push_back(integrationStage);
    }
    \n    void updateMemoryStages(int step) {
        DevelopmentStage devStage = brain->getDevelopmentSystem()->getCurrentStage();
        \n        // Modify consolidation strength based on development
        float devMultiplier = 1.0f;
        switch (devStage) {
            case DevelopmentalStage::Initial: devMultiplier = 1.2f; break;
            case DevelopmentalStage::CriticalPeriod: devMultiplier = 1.5f; break;
            case DevelopmentalStage::Maturation: devMultiplier = 1.1f; break;
            case DevelopmentalStage::Adult: devMultiplier = 0.9f; break;
        }
        \n        for (auto& stage : memoryStages) {
            stage.consolidationStrength = std::min(1.0f, 
                                                  stage.consolidationStrength + 
                                                  (0.01f * devMultiplier));
        }
    }
    \n    void checkMemoryEvents(int step) {
        auto* episodicMem = brain->getEpisodicMemory();
        if (!episodicMem) return;
        \n        size_t episodeCount = episodicMem->getEpisodeCount();
        \n        // Check for memory milestone events
        if (episodeCount >= 10 && step % 1000 == 0) {
            std::cout << "Memory milestone: " << episodeCount << " episodes stored" << std::endl;
            // Trigger special consolidation event
            triggerMemoryResetEvent();
        }
        \n        // Check stage transition
        checkStageTransition(step);
    }
    \n    void checkStageTransition(int step) {
        if (step % 1500 == 0 && currentStage < memoryStages.size() - 1) {
            currentStage++;
            std::cout << "Memory stage transition: " 
                      << stageToString(memoryStages[currentStage].stage) 
                      << std::endl;
            \n            // Adjust consolidation parameters for new stage
            float newStrength = memoryStages[currentStage].consolidationStrength;
            auto* episodicMem = brain->getEpisodicMemory();
            if (episodicMem) {
                episodicMem->setConsolidationStrength(newStrength);
            }
        }
    }
    \n    void forceMemoryConsolidation(int step) {
        std::cout << "Forced memory consolidation at step " << step << std::endl;
        \n        auto* episodicMem = brain->getEpisodicMemory();
        if (episodicMem) {
            // Force consolidation of important memories
            episodicMem->consolidate(0.3f);
            \n            // Replay important episodes
            auto episodesToReplay = episodicMem->getEpisodesForReplay(3);
            for (const auto* episode : episodesToReplay) {
                episodicMem->replayEpisode(episode);
            }
        }
        \n        // Enhance working memory consolidation
        auto* workingMem = brain->getWorkingMemory();
        if (workingMem) {
            workingMem->setTraceRetention(1.5f);
        }
    }
    \n    void triggerMemoryResetEvent() {
        std::cout << "Memory reset event triggered" << std::endl;
        \n        // Reset episodic memory
        auto* episodicMem = brain->getEpisodicMemory();
        if (episodicMem) {
            episodicMem->clearAllEpisodes();
        }
        \n        // Reset working memory
        auto* workingMem = brain->getWorkingMemory();
        if (workingMem) {
            workingMem->clearAllTraces();
        }
        \n        // Reset concept formation
        auto* conceptForm = brain->getConceptFormation();
        if (conceptForm) {
            conceptForm->clear();
        }
        \n        std::cout << "Memory reset complete" << std::endl;
    }
    \n    std::string stageToString(MemoryStage::Stage stage) {
        switch (stage) {
            case MemoryStage::ENCODING: return "Encoding";
            case MemoryStage::CONSOLIDATION: return "Consolidation";
            case MemoryStage::RECONSOLIDATION: return "Reconsolidation";
            case MemoryStage::INTEGRATION: return "Integration";
            default: return "Unknown";
        }
    }
};
```

### 2. Sleep-Dependent Memory Consolidation

**Scenario**: Memory consolidation during simulated sleep cycles

**Implementation**:```cpp
class SleepDependentMemoryConsolidation {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agent;
    bool isSleeping;
    int sleepStartStep;
    float consolidationDuringSleep;
    \npublic:
    void setupSleepDependentConsolidation() {
        brain = std::make_shared<Brain>({
            {"neuron_count": 800,
             "region_count": 1,
             "replay_interval": 50,  // Frequent replay during sleep
             "consolidation_interval": 200}  // Fast consolidation during sleep
        });
        brain->initialize();
        \n        agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*SimpleWorld());
        \n        isSleeping = false;
        sleepStartStep = 0;
        consolidationDuringSleep = 2.0f;  // Enhanced consolidation during sleep
    }
    \n    void runSleepDependentConsolidation(int totalSteps) {
        for (int step = 0; step < totalSteps; ++step) {
            // Check for sleep/wake cycles
            updateSleepWakeCycle(step);
            \n            if (isSleeping) {
                // During sleep: enhanced memory consolidation
                performSleepDependentConsolidation(step);
                \n                // Sleep processing (reduced activity)
                performSleepProcessing(step);
            } else {
                // During wake: normal processing
                performWakeProcessing(step);
            }
            \n            // Record metrics
            recordSleepDependentMetrics(step);
        }
    }
    \n    void updateSleepWakeCycle(int step) {
        static int wakeDuration = 1000;  // Wake for 1000 steps
        static int sleepDuration = 500;   // Sleep for 500 steps
        static int cycleStep = 0;
        static bool inSleep = false;
        \n        if (cycleStep >= wakeDuration + sleepDuration) {
            cycleStep = 0;
            inSleep = !inSleep;
        }
        \n        if (inSleep && cycleStep == 0) {
            startSleep(step);
        } else if (!inSleep && cycleStep == wakeDuration) {
            endSleep(step);
        }
        \n        cycleStep++;
    }
    \n    void startSleep(int step) {
        isSleeping = true;
        sleepStartStep = step;
        std::cout << "Sleep started at step " << step << std::endl;
        \n        // Configure for sleep processing
        auto* episodicMem = brain->getEpisodicMemory();
        if (episodicMem) {
            episodicMem->setConsolidationStrength(consolidationDuringSleep);
            episodicMem->setReplayFrequency(5);  // More frequent replay
        }
    }
    \n    void endSleep(int step) {
        int sleepDuration = step - sleepStartStep;
        isSleeping = false;
        std::cout << "Sleep ended at step " << step 
                  << " (duration: " << sleepDuration << " steps)" << std::endl;
        \n        // Reset consolidation strength
        auto* episodicMem = brain->getEpisodicMemory();
        if (episodicMem) {
            episodicMem->setConsolidationStrength(1.0f);
        }
    }
    \n    void performSleepDependentConsolidation(int step) {
        // Enhanced memory consolidation during sleep
        auto* episodicMem = brain->getEpisodicMemory();
        if (episodicMem) {
            // Force consolidation
            episodicMem->consolidate(0.5f * consolidationDuringSleep);
            \n            // Intensive replay during sleep
            auto episodesToReplay = episodicMem->getEpisodesForReplay(10);
            for (const auto* episode : episodesToReplay) {
                episodicMem->replayEpisode(episode);
            }
        }
        \n        // Enhance synaptic consolidation
        auto* workingMem = brain->getWorkingMemory();
        if (workingMem) {
            workingMem->setTraceRetention(consolidationDuringSleep);
        }
    }
    \n    void performWakeProcessing(int step) {
        // Normal wake processing
        SensoryPercept percept = SimpleWorld()->observe(agent.get());
        agent->processSensoryInput(percept);
        brain->step(step, step * 0.001);
        agent->updateDevelopment(0.001);
        \n        // Check for new experiences
        if (step % 100 == 0) {
            std::cout << "Wake step " << step << ": " 
                      << brain->getEpisodicMemory()->getEpisodeCount() 
                      << " episodes stored" << std::endl;
        }
    }
    \n    void performSleepProcessing(int step) {
        int sleepStep = step - sleepStartStep;
        
        // Simulate reduced but active processing during sleep
        float sleepActivity = std::max(0.1f, 1.0f - 
                                     static_cast<float>(sleepStep) / 300.0f);
        \n        // Inject reduced current for sleep processing
        brain->injectCurrentToNeurons(NeuronType::Internal, 
                                     sleepActivity * 2.0f);
        \n        // Limited step processing
        if (sleepStep % 50 == 0) {
            brain->step(step, step * 0.001);
        }
        \n        std::cout << "Sleep step " << step << ": sleep activity = " 
                  << sleepActivity << std::endl;
    }
    \n    void recordSleepDependentMetrics(int step) {
        if (step % 500 == 0) {
            std::cout << "=== Sleep-Dependent Consolidation Metrics ===" << std::endl;
            \n            auto* episodicMem = brain->getEpisodicMemory();
            if (episodicMem) {
                std::cout << "Episodes stored: " << episodicMem->getEpisodeCount() << std::endl;
                std::cout << "Replay episodes available: " 
                          << episodicMem->getEpisodesForReplay(5).size() << std::endl;
            }
            \n            auto* workingMem = brain->getWorkingMemory();
            if (workingMem) {
                std::cout << "Working memory active traces: " 
                          << workingMem->getActiveTraces() << std::endl;
            }
            \n            std::cout << "Sleep state: " << (isSleeping ? "SLEEPING" : "AWAKE") << std::endl;
            std::cout << "==============================================" << std::endl;
        }
    }
};
```

## Adaptive Planning with Uncertainty

### 1. Planning with Prediction Uncertainty

**Scenario**: Planning takes into account prediction uncertainty

**Implementation**:```cpp
class UncertaintyAwarePlanning {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agent;
    NeuralPlanner* planner;
    PredictionSystem* prediction;
    \n    struct PlannedAction {
        std::vector<Action> actions;
        float expectedUtility;
        float uncertainty;
        float confidence;
        std::vector<float> riskFactors;
    };
    \n    std::vector<PlannedAction> actionPlans;
    \npublic:
    void setupUncertaintyAwarePlanning() {
        brain = std::make_shared<Brain>({
            {"neuron_count": 1200,
             "region_count": 1}
        });
        brain->initialize();
        \n        agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*SimpleWorld());
        \n        planner = brain->getPlanner();
        prediction = brain->getPredictionSystem();
        \n        // Enable planning with uncertainty awareness
        planner->enableUncertaintyHandling(true);
        planner->setUncertaintySensitivity(0.8f);
    }
    \n    void runUncertaintyAwarePlanning(int steps) {
        for (int step = 0; step < steps; ++step) {
            // Get observation
            SensoryPercept percept = SimpleWorld()->observe(agent.get());
            \n            // Process sensory input
            agent->processSensoryInput(percept);
            \n            // Brain step
            brain->step(step, step * 0.001);
            \n            // Generate action plans with uncertainty
            generateActionPlansWithUncertainty(step);
            \n            // Select plan based on uncertainty and utility
            selectOptimalPlan(step);
            \n            // Execute selected action
            executeSelectedAction(step);
            \n            // Update development
            agent->updateDevelopment(0.001);
        }
    }
    \n    void generateActionPlansWithUncertainty(int step) {
        actionPlans.clear();
        \n        // Get current state
        InternalSignals currentState;
        extractInternalState(currentState);
        \n        // Get concepts for planning goals
        ConceptFormation* conceptForm = brain->getConceptFormation();
        std::vector<size_t> concepts = conceptForm->getConcepts();
        \n        // Generate action sequences for each concept
        for (size_t conceptId : concepts) {
            std::vector<Action> actions = generateActionsForConcept(conceptId);
            \n            if (!actions.empty()) {
                PlannedAction plan;
                plan.actions = actions;
                \n                // Evaluate plan with uncertainty
                evaluatePlanWithUncertainty(plan, currentState, conceptId);
                \n                // Add to plans
                actionPlans.push_back(plan);
            }
        }
    }
    \n    void evaluatePlanWithUncertainty(PlannedAction& plan, 
                                    const InternalSignals& state,
                                    size_t conceptId) {
        // Get concept for planning goal
        std::vector<float> conceptProto = conceptForm->getConceptPrototype(conceptId);
        \n        // Calculate expected utility
        float expectedUtility = calculateExpectedUtility(plan.actions, state, conceptProto);
        \n        // Calculate prediction uncertainty
        float predictionUncertainty = calculatePredictionUncertainty(plan.actions, state);
        \n        // Calculate confidence based on uncertainty and concept stability
        float conceptStability = conceptForm->getConceptStability(conceptId);
        float confidence = 1.0f / (1.0f + predictionUncertainty) * conceptStability;
        \n        // Calculate risk factors
        std::vector<float> riskFactors = calculateRiskFactors(plan.actions, state);
        \n        // Adjust utility based on uncertainty
        float uncertaintyPenalty = predictionUncertainty * 0.3f;
        float adjustedUtility = expectedUtility * confidence - uncertaintyPenalty;
        \n        // Store plan evaluation
        plan.expectedUtility = adjustedUtility;
        plan.uncertainty = predictionUncertainty;
        plan.confidence = confidence;
        plan.riskFactors = riskFactors;
    }
    \n    float calculateExpectedUtility(const std::vector<Action>& actions,
                                   const InternalSignals& state,
                                   const std::vector<float>& goal) {
        float utility = 0.0f;
        \n        // Simple utility function based on goal alignment
        for (const Action& action : actions) {
            // Get action effect on goal
            std::vector<float> actionEffect = applyActionToGoal(action, goal);
            \n            // Calculate contribution to utility
            float contribution = 0.0f;
            for (size_t i = 0; i < std::min(actionEffect.size(), goal.size()); ++i) {
                contribution += std::min(1.0f, actionEffect[i] / goal[i]);
            }
            \n            utility += contribution / actions.size();
        }
        
        return utility;
    }
    \n    float calculatePredictionUncertainty(const std::vector<Action>& actions,
                                         const InternalSignals& state) {
        // Calculate uncertainty based on prediction system confidence
        if (prediction) {
            // Use prediction error as uncertainty measure
            return prediction->getPredictionError();
        }
        \n        // Fallback uncertainty based on action novelty
        float novelty = calculateActionNovelty(actions);
        return novelty;
    }
    \n    void selectOptimalPlan(int step) {
        if (actionPlans.empty()) {
            return;
        }
        \n        // Sort plans by expected utility
        std::sort(actionPlans.begin(), actionPlans.end(),
                  [](const PlannedAction& a, const PlannedAction& b) {
                      return a.expectedUtility > b.expectedUtility;
                  });
        \n        // Select plan based on uncertainty and utility
        PlannedAction selectedPlan = actionPlans[0];
        \n        // Apply uncertainty threshold for exploration
        if (selectedPlan.uncertainty > 0.7f && selectedPlan.confidence > 0.5f) {
            // High uncertainty with decent confidence: consider alternative
            if (actionPlans.size() > 1) {
                // Try second-best plan
                selectedPlan = actionPlans[1];
            }
        } else if (selectedPlan.uncertainty > 0.9f) {
            // Very high uncertainty: fallback to safe default
            selectedPlan = createDefaultActionPlan();
        }
        \n        // Store selected plan
        currentSelectedPlan = selectedPlan;
        \n        // Log selection
        if (step % 100 == 0) {
            std::cout << "Selected plan: utility=" << selectedPlan.expectedUtility
                      << ", uncertainty=" << selectedPlan.uncertainty
                      << ", confidence=" << selectedPlan.confidence << std::endl;
        }
    }
    \n    void executeSelectedAction(int step) {
        if (!currentSelectedPlan.actions.empty()) {
            // Convert actions to motor commands
            MotorCommand cmd = convertActionsToMotorCommand(currentSelectedPlan.actions);
            \n            // Apply action
            SimpleWorld()->applyAction(agent.get(), cmd);
            \n            std::cout << "Executed action plan: " 
                      << currentSelectedPlan.actions.size() << " actions" << std::endl;
        }
    }
    \n    PlannedAction createDefaultActionPlan() {
        PlannedAction plan;
        plan.actions = {Action::Wait};  // Safe default action
        plan.expectedUtility = 0.1f;   // Low utility
        plan.uncertainty = 0.0f;       // No uncertainty
        plan.confidence = 1.0f;        // High confidence
        plan.riskFactors = {0.0f, 0.0f, 0.0f};
        return plan;
    }
    \n    std::vector<Action> generateActionsForConcept(size_t conceptId) {
        // Generate actions aligned with concept
        ConceptFormation* conceptForm = brain->getConceptFormation();
        std::vector<float> conceptProto = conceptForm->getConceptPrototype(conceptId);
        \n        std::vector<Action> actions;
        \n        // Simple action generation based on concept
        if (conceptProto.size() > 0 && conceptProto[0] > 0.5f) {
            actions.push_back(Action::MoveForward);
        }
        if (conceptProto.size() > 1 && conceptProto[1] > 0.5f) {
            actions.push_back(Action::TurnLeft);
        }
        if (conceptProto.size() > 2 && conceptProto[2] > 0.5f) {
            actions.push_back(Action::TurnRight);
        }
        if (conceptProto.size() > 3 && conceptProto[3] > 0.5f) {
            actions.push_back(Action::Interact);
        }
        if (actions.empty()) {
            actions.push_back(Action::Wait);
        }
        
        return actions;
    }
    \n    void extractInternalState(InternalSignals& state) {
        // Extract current brain state for planning
        state.data.clear();
        \n        // Get working memory traces
        auto* workingMem = brain->getWorkingMemory();
        if (workingMem) {
            const auto& traces = workingMem->getActiveTraces();
            for (const auto& trace : traces) {
                state.data.push_back(trace.strength);
            }
        }
        \n        // Add prediction uncertainty
        if (prediction) {
            state.data.push_back(prediction->getPredictionError());
        }
        \n        // Add concept influence
        if (conceptForm) {
            size_t conceptCount = conceptForm->getConceptCount();
            state.data.push_back(static_cast<float>(conceptCount) / 10.0f);
        }
    }
};
```

### 2. Risk-Aware Planning

**Scenario**: Planning accounts for risk and uncertainty in outcomes

**Implementation**:```cpp
class RiskAwarePlanning {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agent;
    NeuralPlanner* planner;
    \n    struct RiskAssessment {
        float riskScore;           // Overall risk level (0-1)
        std::vector<float> riskFactors; // Specific risk factors
        float potentialReward;     // Expected reward
        float riskAdjustedUtility; // Utility accounting for risk
    };
    \npublic:
    void setupRiskAwarePlanning() {
        brain = std::make_shared<Brain>({
            {"neuron_count": 1000,
             "region_count": 1}
        });
        brain->initialize();
        \n        agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*SimpleWorld());
        \n        planner = brain->getPlanner();
        planner->enableRiskAwarePlanning(true);
        planner->setRiskAversion(0.3f);  // Risk aversion factor
    }
    \n    void runRiskAwarePlanning(int steps) {
        for (int step = 0; step < steps; ++step) {
            // Get observation
            SensoryPercept percept = SimpleWorld()->observe(agent.get());
            \n            // Process sensory input
            agent->processSensoryInput(percept);
            \n            // Brain step
            brain->step(step, step * 0.001);
            \n            // Generate risk-aware action plans
            generateRiskAwarePlans(step);
            \n            // Select risk-optimal plan
            selectRiskOptimalPlan(step);
            \n            // Execute action
            executeRiskAwareAction(step);
            \n            agent->updateDevelopment(0.001);
        }
    }
    \n    void generateRiskAwarePlans(int step) {
        std::vector<PlannedAction> allPlans;
        \n        // Get all possible action sequences
        allPlans = planner->generateAllActionSequences();
        \n        // Assess risk for each plan
        std::vector<RiskAssessment> riskAssessments;
        for (const PlannedAction& plan : allPlans) {
            RiskAssessment risk = assessPlanRisk(plan, step);
            riskAssessments.push_back(risk);
        }
        \n        // Store plans with risk assessments
        planner->storeRiskAwarePlans(allPlans, riskAssessments);
    }
    \n    RiskAssessment assessPlanRisk(const PlannedAction& plan, int step) {
        RiskAssessment risk;
        \n        // Calculate basic risk factors
        risk.riskFactors = calculateBasicRiskFactors(plan);
        \n        // Calculate overall risk score
        risk.riskScore = calculateRiskScore(risk.riskFactors);
        \n        // Calculate potential reward
        risk.potentialReward = calculatePlanReward(plan);
        \n        // Calculate risk-adjusted utility
        risk.riskAdjustedUtility = calculateRiskAdjustedUtility(
            risk.potentialReward, risk.riskScore);
        
        return risk;
    }
    \n    std::vector<float> calculateBasicRiskFactors(const PlannedAction& plan) {
        std::vector<float> riskFactors;
        
        // Risk factor 1: Action novelty (unfamiliar actions are riskier)
        float noveltyRisk = calculateActionNovelty(plan.actions) * 0.4f;
        riskFactors.push_back(noveltyRisk);
        
        // Risk factor 2: Context uncertainty
        float contextUncertainty = calculateContextUncertainty(plan);
        riskFactors.push_back(contextUncertainty * 0.3f);
        
        // Risk factor 3: Resource cost
        float resourceCost = calculateResourceCost(plan.actions);
        riskFactors.push_back(resourceCost * 0.2f);
        
        // Risk factor 4: Time pressure
        float timePressure = calculateTimePressure();
        riskFactors.push_back(timePressure * 0.1f);
        
        return riskFactors;
    }
    \n    float calculateRiskScore(const std::vector<float>& riskFactors) {
        float weightedRisk = 0.0f;
        for (float factor : riskFactors) {
            weightedRisk += factor;
        }
        return std::min(1.0f, weightedRisk);
    }
    \n    float calculatePlanReward(const PlannedAction& plan) {
        // Calculate expected reward for plan
        float reward = 0.0f;
        
        // Reward based on action benefits
        for (const Action& action : plan.actions) {
            reward += getActionReward(action);
        }
        
        // Adjust for plan length (longer plans have higher cost)
        float lengthPenalty = static_cast<float>(plan.actions.size()) * 0.01f;
        reward -= lengthPenalty;
        
        return std::max(0.0f, reward);
    }
    \n    float calculateRiskAdjustedUtility(float reward, float risk) {
        // Risk-adjusted utility function
        // U = R * (1 - k*R) where k is risk aversion coefficient
        float riskAversion = getRiskAversion();
        
        float utility = reward * (1.0f - riskAversion * risk);
        return std::max(0.0f, utility);
    }
    \n    void selectRiskOptimalPlan(int step) {
        // Get all plans with risk assessments
        std::vector<PlannedAction> plans = planner->getAllPlans();
        std::vector<RiskAssessment> risks = planner->getAllRiskAssessments();
        \n        if (plans.empty()) {
            return;
        }
        
        // Find plan with optimal risk-adjusted utility
        PlannedAction selectedPlan = plans[0];
        RiskAssessment selectedRisk = risks[0];
        \n        for (size_t i = 1; i < plans.size(); ++i) {
            if (risks[i].riskAdjustedUtility > selectedRisk.riskAdjustedUtility) {
                selectedPlan = plans[i];
                selectedRisk = risks[i];
            }
        }
        \n        // Consider risk tolerance
        if (selectedRisk.riskScore > getRiskTolerance()) {
            // Too risky: select less risky plan
            for (size_t i = 0; i < plans.size(); ++i) {
                if (risks[i].riskScore <= getRiskTolerance() && 
                    risks[i].riskAdjustedUtility > selectedRisk.riskAdjustedUtility) {
                    selectedPlan = plans[i];
                    selectedRisk = risks[i];
                }
            }
        }
        
        // Store selected plan
        currentSelectedPlan = selectedPlan;
        \n        // Log selection
        if (step % 100 == 0) {
            std::cout << "Risk-optimal plan selected: reward=" 
                      << selectedRisk.potentialReward
                      << ", risk=" << selectedRisk.riskScore
                      << ", adjustedUtility=" 
                      << selectedRisk.riskAdjustedUtility << std::endl;
        }
    }
    \n    void executeRiskAwareAction(int step) {
        // Execute the selected risk-optimal plan
        if (!currentSelectedPlan.actions.empty()) {
            MotorCommand cmd = convertActionsToMotorCommand(currentSelectedPlan.actions);
            SimpleWorld()->applyAction(agent.get(), cmd);
            
            std::cout << "Executed risk-aware action plan" << std::endl;
        }
    }
    \n    float getRiskAversion() {
        return planner->getRiskAversion();
    }
    \n    float getRiskTolerance() {
        return planner->getRiskTolerance();
    }
    \n    float calculateActionNovelty(const std::vector<Action>& actions) {
        // Calculate how novel the actions are
        // This would use historical action patterns
        return 0.5f;  // Simplified for demo
    }
    
    float calculateContextUncertainty(const PlannedAction& plan) {
        // Calculate uncertainty in current context
        // This would use environmental uncertainty
        return 0.3f;  // Simplified for demo
    }
    
    float calculateResourceCost(const std::vector<Action>& actions) {
        // Calculate resource cost of actions
        float cost = 0.0f;
        for (const Action& action : actions) {
            // Action-specific cost
            switch (action) {
                case Action::MoveForward: cost += 0.1f; break;
                case Action::TurnLeft: cost += 0.05f; break;
                case Action::TurnRight: cost += 0.05f; break;
                case Action::Interact: cost += 0.5f; break;
                case Action::Wait: cost += 0.0f; break;
            }
        }
        return cost;
    }
    \n    float calculateTimePressure() {
        // Calculate time pressure (simplified)
        return 0.2f;
    }
    
    float getActionReward(const Action& action) {
        // Return reward value for action
        switch (action) {
            case Action::MoveForward: return 0.3f;
            case Action::TurnLeft: return 0.1f;
            case Action::TurnRight: return 0.1f;
            case Action::Interact: return 0.8f;
            case Action::Wait: return 0.0f;
            default: return 0.0f;
        }
    }
};
```

## Neuromodulation-Based Behavior Adaptation

### 1. Dopamine-Driven Learning Adaptation

**Scenario**: Dopamine levels modulate learning rates based on prediction errors

**Implementation**:```cpp
class DopamineDrivenLearning {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agent;
    Dopamine* dopamine;
    \n    struct LearningParameters {
        float learningRate;
        float plasticityRate;
        float attentionWeight;
        float memoryConsolidation;
    };
    \n    LearningParameters currentParams;
    \npublic:
    void setupDopamineDrivenLearning() {
        brain = std::make_shared<Brain>({
            {"neuron_count": 1200,
             "region_count": 1,
             "stdp_ltp_weight": 0.015f,
             "stdp_ltd_weight": 0.018f}
        });
        brain->initialize();
        \n        agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*SimpleWorld());
        \n        dopamine = brain->getDopamine();
        \n        // Initialize learning parameters
        currentParams = {
            .learningRate = 0.01f,
            .plasticityRate = 0.02f,
            .attentionWeight = 1.0f,
            .memoryConsolidation = 0.5f
        };
    }
    \n    void runDopamineDrivenLearning(int steps) {
        for (int step = 0; step < steps; ++step) {
            // Get observation
            SensoryPercept percept = SimpleWorld()->observe(agent.get());
            \n            // Process sensory input
            agent->processSensoryInput(percept);
            \n            // Brain step
            brain->step(step, step * 0.001);
            \n            // Update learning parameters based on dopamine
            updateLearningParameters(step);
            \n            // Apply dopamine-driven learning
            applyDopamineDrivenLearningEffects(step);
            \n            // Execute action with learned parameters
            executeWithDopamineLearning(step);
            \n            agent->updateDevelopment(0.001);
            
            // Log learning metrics
            if (step % 500 == 0) {
                logDopamineLearningMetrics(step);
            }
        }
    }
    \n    void updateLearningParameters(int step) {
        if (!dopamine) return;
        \n        float dopamineLevel = dopamine->getLevel();
        float predictionError = agent->getPredictionError();
        \n        // Modulate learning rate based on dopamine
        currentParams.learningRate = 0.01f * (1.0f + dopamineLevel * 2.0f);
        currentParams.learningRate = std::clamp(currentParams.learningRate, 0.001f, 0.1f);
        \n        // Modulate plasticity based on prediction error
        currentParams.plasticityRate = 0.02f * (1.0f + std::abs(predictionError));
        currentParams.plasticityRate = std::clamp(currentParams.plasticityRate, 0.001f, 0.1f);
        \n        // Adjust attention weight based on dopamine
        currentParams.attentionWeight = 1.0f + dopamineLevel * 0.5f;
        currentParams.attentionWeight = std::clamp(currentParams.attentionWeight, 0.5f, 2.0f);
        \n        // Modify memory consolidation
        currentParams.memoryConsolidation = 0.5f * (1.0f + dopamineLevel);
        currentParams.memoryConsolidation = std::clamp(currentParams.memoryConsolidation, 0.1f, 1.0f);
    }
    \n    void applyDopamineDrivenLearningEffects(int step) {
        // Apply dopamine-modulated learning effects
        ConceptFormation* conceptForm = brain->getConceptFormation();
        if (conceptForm) {
            // Adjust concept learning rate based on dopamine
            float learningBoost = 1.0f + std::max(0.0f, dopamine->getLevel()) * 0.5f;
            conceptForm->setLearningRate(conceptForm->getLearningRate() * learningBoost);
        }
        \n        // Modify prediction learning
        PredictionSystem* prediction = brain->getPredictionSystem();
        if (prediction) {
            float predictionLearningRate = currentParams.learningRate * 0.8f;
            prediction->updateParameters(predictionLearningRate,
                                        prediction->getConfidence());
        }
        \n        // Adjust working memory trace strength
        auto* workingMem = brain->getWorkingMemory();
        if (workingMem) {
            float traceStrength = currentParams.memoryConsolidation;
            workingMem->setTraceRetention(traceStrength);
        }
        \n        // Modify attention system
        auto* attention = brain->getAttention();
        if (attention) {
            float attentionModulation = currentParams.attentionWeight;
            attention->setInhibitionStrength(0.5f / attentionModulation);
            attention->setExcitationStrength(1.5f * attentionModulation);
        }
    }
    \n    void executeWithDopamineLearning(int step) {
        // Execute actions considering learned parameters
        std::vector<Action> actions = selectActionsWithDopamineLearning();
        
        // Convert to motor command
        MotorCommand cmd = convertActionsToMotorCommand(actions);
        
        // Apply action
        SimpleWorld()->applyAction(agent.get(), cmd);
        
        // Apply reward with dopamine modulation
        float reward = SimpleWorld()->computeReward(agent.get());
        float modulatedReward = reward * currentParams.learningRate;
        
        agent->applyRewardModulation(modulatedReward, 0.0);
        
        std::cout << "Dopamine-driven learning: reward=" << reward
                  << ", learning_rate=" << currentParams.learningRate
                  << ", dopamine_level=" << (dopamine ? dopamine->getLevel() : 0.0f)
                  << std::endl;
    }
    \n    std::vector<Action> selectActionsWithDopamineLearning() {
        // Select actions considering dopamine-modulated learning
        std::vector<Action> actions;
        
        // Get available actions based on current brain state
        auto* workingMem = brain->getWorkingMemory();
        if (workingMem) {
            const auto& traces = workingMem->getActiveTraces();n            \n            // Select actions based on trace strengths
            for (const auto& trace : traces) {
                if (trace.strength > 0.5f) {
                    // Map trace to action
                    Action action = traceToAction(trace);
                    actions.push_back(action);
                }
            }
        }
        
        // Add novelty-driven exploration based on dopamine
        if (dopamine && dopamine->getLevel() > 0.3f) {
            // High dopamine: encourage exploration
            Action novelAction = getNovelAction();
            actions.push_back(novelAction);
        }
        
        // Ensure we have some actions
        if (actions.empty()) {
            actions.push_back(Action::Wait);
        }
        
        return actions;
    }
    \n    Action traceToAction(const MemoryTrace& trace) {
        // Map memory trace to appropriate action
        // Simplified implementation - in real systems, this would be more complex
        float traceValue = trace.strength;
        
        if (traceValue > 0.8f) {
            return Action::MoveForward;
        } else if (traceValue > 0.6f) {
            return Action::TurnLeft;
        } else if (traceValue > 0.4f) {
            return Action::TurnRight;
        } else if (traceValue > 0.2f) {
            return Action::Interact;
        } else {
            return Action::Wait;
        }
    }
    \n    Action getNovelAction() {
        // Return a novel action for exploration
        // This would use action history and novelty detection
        return Action::TurnLeft;  // Simplified for demo
    }
    \n    void logDopamineLearningMetrics(int step) {
        std::cout << "=== Dopamine-Driven Learning Metrics ===" << std::endl;
        std::cout << "Step: " << step << std::endl;
        std::cout << "Dopamine level: " << (dopamine ? dopamine->getLevel() : 0.0f) << std::endl;
        std::cout << "Learning rate: " << currentParams.learningRate << std::endl;
        std::cout << "Plasticity rate: " << currentParams.plasticityRate << std::endl;
        std::cout << "Attention weight: " << currentParams.attentionWeight << std::endl;
        std::cout << "Memory consolidation: " << currentParams.memoryConsolidation << std::endl;
        std::cout << "========================================= " << std::endl;
    }
};
```

### 2. Curiosity-Driven Adaptive Behavior

**Scenario**: Curiosity modulates exploration and learning based on novelty and prediction error

**Implementation**:```cpp
class CuriosityDrivenBehavior {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<AgentBrain> agent;
    Curiosity* curiosity;
    Novelty* novelty;
    PredictionError* predictionError;
    \n    struct BehavioralState {
        float explorationLevel;
        float learningIntensity;
        float attentionFocus;
        float actionVariety;
    };
    \n    BehavioralState currentState;
    std::chrono::time_point<std::chrono::steady_clock> lastNoveltyUpdate;
    \npublic:
    void setupCuriosityDrivenBehavior() {
        brain = std::make_shared<Brain>({
            {"neuron_count": 1000,
             "region_count": 1}
        });
        brain->initialize();
        \n        agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*SimpleWorld());
        \n        curiosity = brain->getCuriosity();
        novelty = brain->getNovelty();
        predictionError = brain->getPredictionErrorSignal();
        \n        // Initialize behavioral state
        currentState = {
            .explorationLevel = 0.5f,
            .learningIntensity = 1.0f,
            .attentionFocus = 0.8f,
            .actionVariety = 0.6f
        };
        \n        lastNoveltyUpdate = std::chrono::steady_clock::now();
    }
    \n    void runCuriosityDrivenBehavior(int steps) {
        for (int step = 0; step < steps; ++step) {
            // Get observation
            SensoryPercept percept = SimpleWorld()->observe(agent.get());
            \n            // Process sensory input
            agent->processSensoryInput(percept);
            \n            // Brain step
            brain->step(step, step * 0.001);
            \n            // Update curiosity and novelty
            updateCuriosityAndNovelty(step);
            \n            // Adapt behavioral state based on curiosity
            adaptBehavioralState(step);
            \n            // Execute curiosity-driven actions
            executeCuriosityDrivenActions(step);
            \n            agent->updateDevelopment(0.001);
            
            // Log curiosity metrics
            if (step % 500 == 0) {
                logCuriosityMetrics(step);
            }
        }
    }
    \n    void updateCuriosityAndNovelty(int step) {
        // Update novelty detection
        if (novelty) {
            novelty->update(0.001);
        }
        \n        // Update curiosity based on novelty and prediction error
        if (curiosity) {
            curiosity->update(0.001);
        }
        \n        // Update prediction error
        if (predictionError) {
            predictionError->update(0.001);
        }
        \n        // Record novelty update time
        lastNoveltyUpdate = std::chrono::steady_clock::now();
    }
    \n    void adaptBehavioralState(int step) {
        // Adapt behavioral state based on curiosity levels
        float curiosityLevel = (curiosity ? curiosity->getLevel() : 0.0f);
        float noveltyLevel = (novelty ? novelty->getLevel() : 0.0f);
        float predictionError = (predictionError ? predictionError->getSignal() : 0.0f);
        \n        // Adjust exploration level based on curiosity
        currentState.explorationLevel = calculateExplorationLevel(
            curiosityLevel, noveltyLevel, predictionError);
        \n        // Adjust learning intensity based on curiosity
        currentState.learningIntensity = calculateLearningIntensity(
            curiosityLevel, noveltyLevel);
        \n        // Adjust attention focus based on novelty
        currentState.attentionFocus = calculateAttentionFocus(noveltyLevel);
        \n        // Adjust action variety based on curiosity
        currentState.actionVariety = calculateActionVariety(
            curiosityLevel, predictionError);
    }
    \n    void executeCuriosityDrivenActions(int step) {
        // Execute actions based on current behavioral state
        std::vector<Action> actions = selectCuriosityDrivenActions();
        \n        // Convert to motor command
        MotorCommand cmd = convertActionsToMotorCommand(actions);
        
        // Apply action
        SimpleWorld()->applyAction(agent.get(), cmd);
        
        // Compute reward
        float reward = SimpleWorld()->computeReward(agent.get());
        
        // Apply reward modulation
        agent->applyRewardModulation(reward, 0.0);
        
        std::cout << "Curiosity-driven behavior: exploration=" 
                  << currentState.explorationLevel
                  << ", learning=" << currentState.learningIntensity
                  << ", curiosity=" << (curiosity ? curiosity->getLevel() : 0.0f)
                  << ", novelty=" << (novelty ? novelty->getLevel() : 0.0f)
                  << std::endl;
    }
    \n    std::vector<Action> selectCuriosityDrivenActions() {
        std::vector<Action> actions;
        
        // High curiosity: more diverse, exploratory actions
        if (currentState.explorationLevel > 0.7f) {
            // Random actions for exploration
            actions = getRandomActionSet();
        } else if (currentState.explorationLevel > 0.4f) {
            // Mix of familiar and novel actions
            actions = getBalancedActionSet();
        } else {
            // Focused on familiar actions
            actions = getFocusedActionSet();
        }
        
        // Apply action variety constraints
        if (currentState.actionVariety > 0.7f) {
            // High variety: ensure diverse actions
            actions = ensureActionVariety(actions);
        }
        
        return actions;
    }
    \n    std::vector<Action> getRandomActionSet() {
        // Return random actions for high exploration
        std::vector<Action> actions = {Action::MoveForward, Action::TurnLeft, 
                                      Action::TurnRight, Action::Interact, Action::Wait};
        \n        // Shuffle actions
        std::random_shuffle(actions.begin(), actions.end());
        return actions;
    }
    \n    std::vector<Action> getBalancedActionSet() {
        // Return mix of familiar and novel actions
        std::vector<Action> actions = {Action::MoveForward, Action::TurnLeft, 
                                      Action::Wait};
        \n        // Occasionally add novel action
        if (rand() % 10 < 3) {  // 30% chance
            actions.push_back(Action::Interact);
        }
        
        return actions;
    }
    \n    std::vector<Action> getFocusedActionSet() {
        // Return familiar actions for focused behavior
        return {Action::MoveForward, Action::Wait};
    }
    \n    std::vector<Action> ensureActionVariety(std::vector<Action> actions) {
        // Ensure actions have variety
        if (actions.size() < 3) {
            // Add more variety
            actions.push_back(Action::TurnLeft);
            actions.push_back(Action::TurnRight);
        }
        return actions;
    }
    \n    float calculateExplorationLevel(float curiosity, float novelty, float error) {
        // Calculate exploration level based on curiosity factors
        float exploration = curiosity * 0.5f + novelty * 0.3f + std::abs(error) * 0.2f;
        return std::min(1.0f, exploration);
    }
    
    float calculateLearningIntensity(float curiosity, float novelty) {
        // Calculate learning intensity based on curiosity
        float intensity = curiosity * 0.7f + novelty * 0.3f;
        return std::min(2.0f, intensity);
    }
    
    float calculateAttentionFocus(float novelty) {
        // Calculate attentional focus based on novelty
        // High novelty: reduce focus, spread attention
        return std::max(0.3f, 1.0f - novelty * 0.8f);
    }
    
    float calculateActionVariety(float curiosity, float error) {
        // Calculate action variety based on curiosity and error
        float variety = curiosity * 0.4f + std::min(1.0f, std::abs(error) * 0.6f);
        return std::min(1.0f, variety);
    }
    \n    void logCuriosityMetrics(int step) {
        std::cout << "=== Curiosity-Driven Behavior Metrics ===" << std::endl;
        std::cout << "Step: " << step << std::endl;
        std::cout << "Exploration level: " << currentState.explorationLevel << std::endl;
        std::cout << "Learning intensity: " << currentState.learningIntensity << std::endl;
        std::cout << "Attention focus: " << currentState.attentionFocus << std::endl;
        std::cout << "Action variety: " << currentState.actionVariety << std::endl;
        std::cout << "Curiosity level: " << (curiosity ? curiosity->getLevel() : 0.0f) << std::endl;
        std::cout << "Novelty level: " << (novelty ? novelty->getLevel() : 0.0f) << std::endl;
        std::cout << "Prediction error: " << (predictionError ? predictionError->getSignal() : 0.0f) << std::endl;
        std::cout << "============================================== " << std::endl;
    }
};
```

## Experiment Design and Analysis

### 1. Phase 6 Integration Experiment Design

**Scenario**: Comprehensive experiment to validate Phase 6 integration

**Implementation**:```cpp
class Phase6IntegrationExperiment {
private:
    std::shared_ptr<Phase6IntegratedExperiment> phase6Experiment;
    std::vector<ExperimentResult> results;
    Phase6Config defaultConfig;
    \npublic:
    Phase6IntegrationExperiment() {
        phase6Experiment = std::make_shared<Phase6IntegratedExperiment>();
        initializeDefaultConfig();
    }
    \n    void initializeDefaultConfig() {
        defaultConfig = Phase6Config();
        defaultConfig.neuronCount = 1000;
        defaultConfig.maxSteps = 5000;
        defaultConfig.enableCheckpointing = true;
        defaultConfig.enableReplay = true;
        defaultConfig.enableDevelopment = true;
    }
    \n    ExperimentResult runBaselineExperiment() {
        // Run baseline experiment with default configuration
        return runExperimentWithConfig(defaultConfig);
    }
    \n    ExperimentResult runScalingExperiment(int numNeurons) {
        // Run experiment with different neuron counts
        Phase6Config config = defaultConfig;
        config.neuronCount = numNeurons;
        config.maxSteps = std::min(10000, numNeurons * 10);
        
        return runExperimentWithConfig(config);
    }
    \n    ExperimentResult runDevelopmentExperiment() {
        // Run experiment with enhanced development
        Phase6Config config = defaultConfig;
        config.enableDevelopment = true;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        
        return runExperimentWithConfig(config);
    }
    \n    ExperimentResult runMemoryExperiment() {
        // Run experiment focusing on memory integration
        Phase6Config config = defaultConfig;
        config.maxSteps = 3000;
        
        auto result = runExperimentWithConfig(config);
        \n        // Analyze memory-specific metrics
        analyzeMemoryMetrics(result);
        \n        return result;
    }
    \n    ExperimentResult runExperimentWithConfig(const Phase6Config& config) {
        ExperimentResult result;
        \n        // Configure experiment
        phase6Experiment->setConfig(config);
        \n        // Run experiment
        result = phase6Experiment->run(config);
        \n        // Store results
        results.push_back(result);
        \n        // Analyze results
        analyzeExperimentResults(result);
        
        return result;
    }
    \n    void analyzeExperimentResults(const ExperimentResult& result) {
        std::cout << "=== Experiment Results Analysis ===" << std::endl;
        std::cout << "Total reward: " << result.totalReward << std::endl;
        std::cout << "Average firing rate: " << result.avgFiringRate << std::endl;
        std::cout << "Memory episodes stored: " << result.memoryEpisodesStored << std::endl;
        std::cout << "Integration status:" << std::endl;
        std::cout << "  - Working memory: " << (result.memoryWorkingMemoryIntegrated ? "INTEGRATED" : "NOT INTEGRATED") << std::endl;
        std::cout << "  - Episodic memory: " << (result.memoryEpisodicMemoryIntegrated ? "INTEGRATED" : "NOT INTEGRATED") << std::endl;
        std::cout << "  - Neuromodulation: " << (result.neuromodulationIntegrated ? "INTEGRATED" : "NOT INTEGRATED") << std::endl;
        std::cout << "  - Prediction: " << (result.predictionIntegrated ? "INTEGRATED" : "NOT INTEGRATED") << std::endl;
        std::cout << "  - Development: " << (result.developmentIntegrated ? "INTEGRATED" : "NOT INTEGRATED") << std::endl;
        std::cout << "=====================================" << std::endl;
    }
    \n    void analyzeMemoryMetrics(const ExperimentResult& result) {
        std::cout << "=== Memory-Specific Analysis ===" << std::endl;
        std::cout << "Memory retention over time: Calculating..." << std::endl;
        std::cout << "Memory consolidation effectiveness: Measuring..." << std::endl;
        std::cout << "Replay efficiency: Evaluating..." << std::endl;
        std::cout << "Memory integration strength: Assessing..." << std::endl;
        std::cout << "================================= " << std::endl;
    }
    \n    void runComprehensiveExperimentSeries() {
        std::cout << "Running comprehensive Phase 6 experiment series..." << std::endl;
        
        // Run different experiment configurations
        ExperimentResult baseline = runBaselineExperiment();
        ExperimentResult scaling = runScalingExperiment(500);
        ExperimentResult development = runDevelopmentExperiment();
        ExperimentResult memory = runMemoryExperiment();
        
        // Compare results
        compareExperimentResults({baseline, scaling, development, memory});
        
        // Generate summary
        generateExperimentSummary();
    }
    \n    void compareExperimentResults(const std::vector<ExperimentResult>& experiments) {
        std::cout << "=== Experiment Comparison ===" << std::endl;
        
        for (size_t i = 0; i < experiments.size(); ++i) {
            std::cout << "Experiment " << i << ": " << getExperimentName(i) << std::endl;
            std::cout << "  - Neurons: " << experiments[i].neuronCount << std::endl;
            std::cout << "  - Steps: " << experiments[i].maxSteps << std::endl;
            std::cout << "  - Reward: " << experiments[i].totalReward << std::endl;
            std::cout << "  - Integration score: " << calculateIntegrationScore(experiments[i]) << std::endl;
        }
        
        std::cout << "============================== " << std::endl;
    }
    \n    std::string getExperimentName(size_t index) {
        switch (index) {
            case 0: return "Baseline";
            case 1: return "Scaling (500 neurons)";
            case 2: return "Development Enhanced";
            case 3: return "Memory Focused";
            default: return "Experiment " + std::to_string(index);
        }
    }
    
    float calculateIntegrationScore(const ExperimentResult& result) {
        float score = 0.0f;
        score += result.memoryWorkingMemoryIntegrated ? 25.0f : 0.0f;
        score += result.memoryEpisodicMemoryIntegrated ? 25.0f : 0.0f;
        score += result.neuromodulationIntegrated ? 20.0f : 0.0f;
        score += result.predictionIntegrated ? 15.0f : 0.0f;
        score += result.developmentIntegrated ? 15.0f : 0.0f;
        \n        return score;
    }
    \n    void generateExperimentSummary() {
        std::cout << "=== Phase 6 Integration Experiment Summary ===" << std::endl;
        std::cout << "Total experiments run: " << results.size() << std::endl;
        std::cout << "Average integration score: " << calculateAverageIntegrationScore() << std::endl;
        std::cout << "Best performing configuration: " << getBestExperimentName() << std::endl;
        std::cout << "============================================ " << std::endl;
    }
    \n    float calculateAverageIntegrationScore() {
        if (results.empty()) return 0.0f;
        \n        float totalScore = 0.0f;
        for (const auto& result : results) {
            totalScore += calculateIntegrationScore(result);
        }
        \n        return totalScore / results.size();
    }
    \n    std::string getBestExperimentName() {
        if (results.empty()) return "None";
        \n        float bestScore = -1.0f;
        int bestIndex = 0;
        \n        for (size_t i = 0; i < results.size(); ++i) {
            float score = calculateIntegrationScore(results[i]);
            if (score > bestScore) {
                bestScore = score;
                bestIndex = static_cast<int>(i);
            }
        }
        \n        return getExperimentName(bestIndex);
    }
};
```

## Performance Optimization

### 1. Memory Optimization Strategies

**Scenario**: Optimize memory usage for large-scale simulations

**Implementation**:```cpp
class MemoryOptimization {
private:
    std::shared_ptr<Brain> brain;
    \n    struct MemoryStats {
        size_t totalMemoryUsage;
        size_t peakMemoryUsage;
        size_t memoryAllocations;
        double averageAllocationSize;
    };
    \n    MemoryStats stats;
    \npublic:
    void setupMemoryOptimization() {
        brain = std::make_shared<Brain>({
            {"neuron_count": 5000,
             "region_count": 5,
             "connection_probability": 0.05}  // Lower probability for efficiency
        });
        brain->initialize();
        \n        // Initialize memory tracking
        stats = {0, 0, 0, 0.0};
    }
    \n    void runMemoryOptimizedSimulation(int steps) {
        for (int step = 0; step < steps; ++step) {
            // Get observation
            SensoryPercept percept = SimpleWorld()->observe(brain.get());
            \n            // Process sensory input
            // ... (implementation depends on specific agent)
            \n            // Brain step with memory optimization
            brain->step(step, step * 0.001);
            \n            // Monitor memory usage
            monitorMemoryUsage(step);
            \n            // Apply memory optimization
            applyMemoryOptimization(step);
            \n            if (step % 1000 == 0) {
                logMemoryOptimizationMetrics(step);
            }
        }
    }
    \n    void monitorMemoryUsage(int step) {
        // Estimate current memory usage
        size_t currentMemory = estimateMemoryUsage();
        \n        // Update statistics
        stats.totalMemoryUsage += currentMemory;
        stats.peakMemoryUsage = std::max(stats.peakMemoryUsage, currentMemory);
        stats.memoryAllocations++;
        \n        if (stats.memoryAllocations > 0) {
            stats.averageAllocationSize = 
                static_cast<double>(stats.totalMemoryUsage) / stats.memoryAllocations;
        }
    }
    \n    void applyMemoryOptimization(int step) {
        // Apply memory optimization strategies
        optimizeWorkingMemory();
        optimizeEpisodicMemory();
        optimizeConceptFormation();
        \n        if (step % 2000 == 0) {
            forceMemoryCleanup();
        }
    }
    \n    void optimizeWorkingMemory() {
        auto* workingMem = brain->getWorkingMemory();
        if (workingMem) {
            // Limit working memory size
            size_t maxTraces = 1000;
            workingMem->setCapacity(maxTraces);
            \n            // Clean old traces
            workingMem->cleanOldTraces(0.5f);
        }
    }
    \n    void optimizeEpisodicMemory() {
        auto* episodicMem = brain->getEpisodicMemory();
        if (episodicMem) {
            // Limit number of stored episodes
            size_t maxEpisodes = 5000;
            episodicMem->setMaxEpisodes(maxEpisodes);
            \n            // Remove oldest episodes if limit exceeded
            episodicMem->cleanupOldEpisodes();
        }
    }
    \n    void optimizeConceptFormation() {
        auto* conceptForm = brain->getConceptFormation();
        if (conceptForm) {
            // Limit concept count
            size_t maxConcepts = 500;
            conceptForm->setMaxConcepts(maxConcepts);
            \n            // Merge similar concepts
            conceptForm->mergeSimilarConcepts(0.7f);
        }
    }
    \n    void forceMemoryCleanup() {
        std::cout << "Forcing memory cleanup..." << std::endl;
        \n        // Force garbage collection
        std::cout << "Performing garbage collection..." << std::endl;
        \n        // Optimize memory pools
        std::cout << "Optimizing memory pools..." << std::endl;
    }
    \n    size_t estimateMemoryUsage() {
        size_t memory = 0;
        \n        // Estimate memory usage for different components
        if (brain->getWorkingMemory()) {
            memory += brain->getWorkingMemory()->getMemoryUsage();
        }
        \n        if (brain->getEpisodicMemory()) {
            memory += brain->getEpisodicMemory()->getMemoryUsage();
        }
        \n        if (brain->getConceptFormation()) {
            memory += brain->getConceptFormation()->getMemoryUsage();
        }
        \n        // Estimate neuron and synapse memory
        memory += brain->getTotalNeuronCount() * sizeof(NeuronState);
        memory += brain->getTotalSynapseCount() * sizeof(SynapseState);
        \n        return memory;
    }
    \n    void logMemoryOptimizationMetrics(int step) {
        std::cout << "=== Memory Optimization Metrics ===" << std::endl;
        std::cout << "Step: " << step << std::endl;
        std::cout << "Peak memory usage: " << stats.peakMemoryUsage << " bytes" << std::endl;
        std::cout << "Average allocation size: " << stats.averageAllocationSize << " bytes" << std::endl;
        std::cout << "Memory allocations: " << stats.memoryAllocations << std::endl;
        std::cout << "==================================== " << std::endl;
    }
};
```

## Parameter Tuning

### 1. Neural System Parameter Tuning

**Scenario**: Optimize neural system parameters for specific tasks

**Implementation**:```cpp
class NeuralParameterTuner {
private:
    struct NeuralParameters {
        // Neuron parameters
        float restingPotential;
        float threshold;
        float resetPotential;
        float leakConductance;
        
        // Synapse parameters
        float stpdWeight;
        float hebbianStrength;
        float synapseDelay;
        
        // Plasticity parameters
        float stdpLearningRate;
        float hebbianLearningRate;
        float structuralPlasticityRate;
        
        // Network parameters
        float connectionProbability;
        float excitationInhibitionRatio;
    };
    
    NeuralParameters currentParams;
    std::vector<NeuralParameters> parameterHistory;
    \npublic:
    void setupParameterTuning() {
        // Initialize with reasonable defaults
        currentParams = {
            .restingPotential = -65.0f,
            .threshold = -50.0f,
            .resetPotential = -70.0f,
            .leakConductance = 0.02f,
            .stpdWeight = 0.01f,
            .hebbianStrength = 1.0f,
            .synapseDelay = 5.0f,
            .stdpLearningRate = 0.01f,
            .hebbianLearningRate = 0.02f,
            .structuralPlasticityRate = 0.0001f,
            .connectionProbability = 0.1f,
            .excitationInhibitionRatio = 1.5f
        };
    }
    \n    void runParameterTuning(int steps) {
        for (int step = 0; step < steps; ++step) {
            // Get observation
            SensoryPercept percept = SimpleWorld()->observe(brain.get());
            \n            // Process sensory input with current parameters
            // ... (implementation)
            \n            // Brain step with current parameters
            brain->step(step, step * 0.001);
            \n            // Evaluate current parameter set
            float performance = evaluateParameterSet(step);
            \n            // Adapt parameters based on performance
            adaptParameters(performance, step);
            \n            // Record parameter history
            parameterHistory.push_back(currentParams);
            \n            if (step % 500 == 0) {
                logTuningProgress(step);
            }
        }
    }
    \n    float evaluateParameterSet(int step) {
        // Evaluate current parameter set based on task performance
        float performance = 0.0f;
        
        // Reward-based evaluation
        float reward = computeCurrentReward();
        performance += reward * 0.5f;
        
        // Learning speed evaluation
        float learningSpeed = computeLearningSpeed(step);
        performance += learningSpeed * 0.3f;
        
        // Stability evaluation
        float stability = computeStability();
        performance += stability * 0.2f;
        
        return performance;
    }
    \n    void adaptParameters(float performance, int step) {
        // Simple hill climbing parameter adaptation
        if (performance > 0.5f) {
            // Good performance: try to improve parameters
            improveParameters(step);
        } else {
            // Poor performance: degrade parameters slightly
            degradeParameters(step);
        }
    }
    \n    void improveParameters(int step) {
        // Slightly adjust parameters toward better performance
        currentParams.stdpLearningRate *= 1.05f;
        currentParams.hebbianLearningRate *= 1.03f;
        currentParams.connectionProbability *= 1.02f;
        \n        // Ensure parameters stay within valid ranges
        clampParameters();
    }
    \n    void degradeParameters(int step) {
        // Slightly degrade parameters to escape local optima
        currentParams.stdpLearningRate *= 0.98f;
        currentParams.hebbianLearningRate *= 0.99f;
        currentParams.connectionProbability *= 0.97f;
        \n        // Ensure parameters stay within valid ranges
        clampParameters();
    }
    \n    void clampParameters() {
        // Ensure parameters stay within valid ranges
        currentParams.restingPotential = std::max(-80.0f, std::min(-60.0f, currentParams.restingPotential));
        currentParams.threshold = std::max(-55.0f, std::min(-45.0f, currentParams.threshold));
        currentParams.leakConductance = std::max(0.01f, std::min(0.1f, currentParams.leakConductance));
        \n        currentParams.stdpLearningRate = std::max(0.001f, std::min(0.1f, currentParams.stdpLearningRate));
        currentParams.hebbianLearningRate = std::max(0.001f, std::min(0.1f, currentParams.hebbianLearningRate));
        currentParams.structuralPlasticityRate = std::max(0.00001f, std::min(0.001f, currentParams.structuralPlasticityRate));
        \n        currentParams.connectionProbability = std::max(0.01f, std::min(0.5f, currentParams.connectionProbability));
        currentParams.excitationInhibitionRatio = std::max(1.0f, std::min(3.0f, currentParams.excitationInhibitionRatio));
    }
    \n    void logTuningProgress(int step) {
        std::cout << "=== Parameter Tuning Progress ===" << std::endl;
        std::cout << "Step: " << step << std::endl;
        std::cout << "STDP learning rate: " << currentParams.stdpLearningRate << std::endl;
        std::cout << "Hebbian learning rate: " << currentParams.hebbianLearningRate << std::endl;
        std::cout << "Connection probability: " << currentParams.connectionProbability << std::endl;
        std::cout << "Excitement/inhibition ratio: " << currentParams.excitationInhibitionRatio << std::endl;
        std::cout << "================================== " << std::endl;
    }
};
```

## Research Applications

### 1. Memory Research

**Application**: Study memory formation, consolidation, and retrieval

**Research Questions**:
- How does working memory capacity affect learning?
- What are the effects of memory replay on consolidation?
- How does development affect memory performance?
- What are the neural correlates of memory formation?

**Experimental Design**:
```cpp
class MemoryResearch {
public:
    void investigateWorkingMemoryCapacity() {
        // Vary working memory capacity
        for (int capacity : {100, 500, 1000, 2000}) {
            runMemoryCapacityExperiment(capacity);
        }
    }
    
    void studyMemoryConsolidation() {
        // Investigate consolidation effects
        runConsolidationExperiment(developmentStages::Initial, developmentStages::Maturation);
    }
    
    void examineDevelopmentEffects() {
        // Study developmental changes in memory
        runDevelopmentMemoryExperiment();
    }
};
```

### 2. Neuromodulation Research

**Application**: Study neuromodulation effects on learning and behavior

**Research Questions**:
- How does dopamine affect learning rates?
- What are the effects of novelty on attention?
- How does curiosity drive exploration?
- What are the roles of different neuromodulators?

**Experimental Design**:
```cpp
class NeuromodulationResearch {
public:
    void studyDopamineEffects() {
        // Manipulate dopamine levels
        runDopamineManipulationExperiment({
            .baseline = 0.0f,
            .enhanced = 0.5f,
            .reduced = -0.5f
        });
    }
    
    void studyCuriosityEffects() {
        // Investigate curiosity-driven behavior
        runCuriosityExperiment({
            .low = 0.0f,
            .high = 1.0f
        });
    }
    
    void studyNoveltyEffects() {
        // Study novelty detection and effects
        runNoveltyExperiment();
    }
};
```

### 3. Learning and Development Research

**Application**: Study learning processes and developmental trajectories

**Research Questions**:
- How does learning progress over development?
- What are the critical periods for learning?
- How do different skills develop over time?
- What are the effects of experience on development?

**Experimental Design**:
```cpp
class DevelopmentalLearningResearch {
public:
    void studySkillAcquisition() {
        // Track skill development over time
        runSkillAcquisitionExperiment();
    }
    
    void studyCriticalPeriods() {
        // Investigate critical periods
        runCriticalPeriodExperiment();
    }
    
    void studyExperienceEffects() {
        // Study effects of experience on development
        runExperienceEffectsExperiment();
    }
};
```

### 4. Integration Research

**Application**: Study how different brain systems integrate

**Research Questions**:
- How do memory systems integrate with cognition?
- What are the effects of prediction on behavior?
- How do neuromodulators coordinate system activities?
- What are the trade-offs between different systems?

**Experimental Design**:
```cpp
class IntegrationResearch {
public:
    void studyMemoryCognitiveIntegration() {
        // Investigate memory-cognition integration
        runMemoryCognitiveIntegrationExperiment();
    }
    
    void studyPredictionEffects() {
        // Study effects of prediction on behavior
        runPredictionEffectsExperiment();
    }
    
    void studyNeuromodulationCoordination() {
        // Investigate neuromodulator coordination
        runNeuromodulationCoordinationExperiment();
    }
};
```

## Summary

The Phase 6 advanced usage examples demonstrate the comprehensive integration of NLM systems and provide tools for:

### Key Capabilities

1. **Advanced Concept Learning**: Hierarchical concept formation, evolution over time, and merging strategies
2. **Multi-Agent Integration**: Competitive and social learning between agents
3. **Developmental Learning**: Critical periods, skill acquisition, and developmental trajectories
4. **Memory Consolidation**: Multi-stage consolidation, sleep-dependent consolidation, and optimization strategies
5. **Adaptive Planning**: Uncertainty-aware planning, risk-aware decision making
6. **Neuromodulation-Based Behavior**: Dopamine-driven learning, curiosity-driven exploration
7. **Experiment Design**: Comprehensive experiment frameworks for validation
8. **Performance Optimization**: Memory optimization, parameter tuning strategies
9. **Research Applications**: Tools for various neuroscience research questions

### Research and Application Opportunities

These advanced examples enable researchers to:

1. **Investigate Memory Systems**: Study memory formation, consolidation, and retrieval
2. **Analyze Neuromodulation**: Understand learning modulation, exploration behavior
3. **Study Development**: Track skill acquisition and developmental changes
4. **Explore Integration**: Investigate how different systems work together
5. **Optimize Performance**: Improve efficiency and scalability
6. **Validate Models**: Test hypotheses about brain function
7. **Develop Applications**: Create systems with human-like learning capabilities

### Implementation Notes

- These examples build upon the basic integration demonstrated in the Phase 6 documentation
- They showcase how to leverage the full capabilities of Phase 6 systems
- Examples include both research applications and practical implementation strategies
- Code is structured for readability and educational purposes
- Includes comprehensive error handling and logging

The Phase 6 integration provides a powerful framework for creating artificial brains that learn, develop, and adapt through experience, with comprehensive documentation and examples for users at all levels of expertise.

<document>
<document>
<document>