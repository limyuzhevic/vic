# Missing C++ Classes and Methods for Python Bindings

This document provides a detailed analysis of all C++ classes and methods that are NOT currently exposed to Python but should be for a complete API.

## Class-by-Class Analysis

### 1. Logger System

#### ILogger (Abstract Base Class)
**Methods to expose:**
- `log(const LogEntry& entry)` - Log an entry
- `setLevel(LogLevel level)` - Set logging level
- `getLevel() const` - Get logging level

#### ConsoleLogger
**Methods to expose:**
- `ConsoleLogger()` - Default constructor
- `ConsoleLogger(LogLevel level)` - Constructor with level
- `setUseColors(bool useColors)` - Enable/disable colors
- `getUseColors() const` - Check if colors enabled
- `setOutputFile(const std::string& filepath)` - Set output file
- `closeOutputFile()` - Close output file

#### Logger
**Methods to expose:**
- `addLogger(std::shared_ptr<ILogger> logger)` - Add logger sink
- `clearLoggers()` - Remove all loggers
- `setLevel(LogLevel level)` - Set minimum level
- `log(LogLevel level, const std::string& message, ...)` - Log with context
- `debug/info/warning/error/critical()` - Convenience methods
- `isEnabled(LogLevel level) const` - Check if level enabled
- `getGlobal()` - Get global instance
- `setGlobal(std::shared_ptr<Logger> logger)` - Set global logger

### 2. SimulationClock

**Methods to expose:**
- `getStep() const` - Get simulation step
- `setStep(SimulationStep step)` - Set step
- `getTime() const` - Get current time
- `setTime(Timestamp time)` - Set time
- `getTimestep() const` - Get timestep
- `setTimestep(TimestepDuration dt)` - Set timestep
- `isFirstStep() const` - Check if first step
- `isLastStep(SimulationStep totalSteps) const` - Check if last step
- `stepToTime(SimulationStep step) const` - Convert step to time
- `timeToStep(Timestamp time) const` - Convert time to step
- `advanceBy(SimulationStep steps)` - Advance by steps
- `timeAtFutureStep(SimulationStep stepsAhead) const` - Get time at future step
- `getRealTimeElapsed() const` - Get real-time elapsed
- `setRealTimeStart()` - Start real-time tracking

### 3. RandomGenerator

**Methods to expose:**
- `seed(uint64_t seed)` - Set seed
- `getSeed() const` - Get seed
- `uniformInt(uint32_t min, uint32_t max)` - Uniform integer
- `uniformInt(uint64_t min, uint64_t max)` - Uniform integer (64-bit)
- `uniformReal(double min, double max)` - Uniform real (double)
- `uniformReal(float min, float max)` - Uniform real (float)
- `normal(double mean, double stddev)` - Normal distribution (double)
- `normal(float mean, float stddev)` - Normal distribution (float)
- `bernoulli(double p = 0.5)` - Bernoulli distribution
- `poisson(double lambda)` - Poisson distribution
- `exponential(double lambda)` - Exponential distribution
- `randomBool()` - Random boolean
- `randomBit()` - Random bit
- `shuffle(Container& container)` - Shuffle container
- `pickRandom(const Container& container)` - Pick random element
- `pickRandomIndex(uint64_t n)` - Pick random index
- `fillBytes(void* data, size_t size)` - Fill with random bytes
- `engine()` - Get underlying engine (advanced use)

### 4. Neuron

**Methods to expose:**
- `getId() const` - Get neuron ID
- `getType() const` - Get neuron type
- `setType(NeuronType type)` - Set neuron type
- `getState() const` - Get neuron state (const)
- `getState()` - Get neuron state (non-const)
- `getMembranePotential() const` - Get membrane potential
- `setMembranePotential(MembranePotential potential)` - Set potential
- `addToMembranePotential(MembranePotential delta)` - Add to potential
- `getThreshold() const` - Get threshold
- `setThreshold(MembranePotential threshold)` - Set threshold
- `isFiring() const` - Check if firing
- `isRefractory() const` - Check if refractory
- `setFiringState(FiringState state)` - Set firing state
- `setRefractoryPeriod(uint32_t steps)` - Set refractory period
- `decrementRefractory()` - Decrement refractory counter
- `getFiringRate() const` - Get firing rate
- `setFiringRate(FiringRate rate)` - Set firing rate
- `setLeakConductance(MembranePotential conductance)` - Set leak conductance
- `getLeakConductance() const` - Get leak conductance
- `getRefractoryPeriod() const` - Get refractory period
- `setRestingPotential(MembranePotential potential)` - Set resting potential
- `getRestingPotential() const` - Get resting potential
- `setResetPotential(MembranePotential potential)` - Set reset potential
- `getResetPotential() const` - Get reset potential
- `checkThreshold() const` - Check threshold
- `getLastSpikeTime() const` - Get last spike time
- `stepLIF(Timestamp currentTime, TimestepDuration dt)` - LIF step
- `receiveExcitatoryInput(MembranePotential amplitude)` - Receive excitatory input
- `receiveInhibitoryInput(MembranePotential amplitude)` - Receive inhibitory input
- `receiveModulatoryInput(MembranePotential amplitude)` - Receive modulatory input
- `injectCurrent(MembranePotential current)` - Inject external current
- `getTotalCurrent() const` - Get total current
- `clearTotalCurrent()` - Clear total current
- `recordSpike(Timestamp timestamp)` - Record spike
- `getSpikeHistory() const` - Get spike history
- `clearSpikeHistory()` - Clear spike history
- `addIncomingSynapse(SynapseHandle handle)` - Add incoming synapse
- `addOutgoingSynapse(SynapseHandle handle)` - Add outgoing synapse
- `getIncomingSynapses() const` - Get incoming synapses
- `getOutgoingSynapses() const` - Get outgoing synapses
- `getPlasticityFlags() const` - Get plasticity flags (const)
- `getPlasticityFlags()` - Get plasticity flags (non-const)
- `enablePlasticity(bool hebbian, bool stdp, bool rewardModulated)` - Enable plasticity
- `setRegionId(RegionId region)` - Set region ID
- `getRegionId() const` - Get region ID
- `setPopulationId(PopulationId population)` - Set population ID
- `getPopulationId() const` - Get population ID
- `step(Timestamp currentTime)` - Update neuron (placeholder)
- `reset()` - Reset to initial state
- `initializeRandom(RandomGenerator& rng)` - Initialize with random parameters

### 5. Synapse

**Methods to expose:**
- `getId() const` - Get synapse ID
- `getSourceNeuron() const` - Get source neuron
- `getDestinationNeuron() const` - Get destination neuron
- `getWeight() const` - Get weight
- `setWeight(SynapticWeight weight)` - Set weight
- `addToWeight(SynapticWeight delta)` - Add to weight
- `getDelay() const` - Get delay
- `setDelay(Delay delay)` - Set delay
- `getType() const` - Get synapse type
- `setType(SynapseType type)` - Set synapse type
- `isExcitatory() const` - Check if excitatory
- `isInhibitory() const` - Check if inhibitory
- `recordPreSpike(Timestamp timestamp)` - Record pre-synaptic spike
- `recordPostSpike(Timestamp timestamp)` - Record post-synaptic spike
- `getPreSpikeHistory() const` - Get pre-synaptic spike history
- `getPostSpikeHistory() const` - Get post-synaptic spike history
- `clearHistory()` - Clear spike history
- `getPlasticityFlags() const` - Get plasticity flags (const)
- `getPlasticityFlags()` - Get plasticity flags (non-const)
- `enablePlasticity(bool hebbian, bool stdp, bool rewardModulated)` - Enable plasticity
- `getEligibilityTrace() const` - Get eligibility trace
- `setEligibilityTrace(float trace)` - Set eligibility trace
- `decayEligibilityTrace(float decayRate)` - Decay eligibility trace
- `getEfficacy() const` - Get efficacy
- `setEfficacy(float efficacy)` - Set efficacy
- `step(Timestamp currentTime)` - Update synapse (placeholder)
- `reset()` - Reset to initial state
- `initializeRandom(RandomGenerator& rng)` - Initialize with random parameters

### 6. NeuralRegion

**Methods to expose:**
- `getId() const` - Get region ID
- `getName() const` - Get region name
- `setName(const std::string& name)` - Set region name
- `addPopulation(size_t size, NeuronType type = NeuronType::Internal)` - Add population
- `getPopulation(PopulationId id)` - Get population (non-const)
- `getPopulation(PopulationId id) const` - Get population (const)
- `getPopulationCount() const` - Get population count
- `getPopulations() const` - Get all populations
- `getAllPopulations()` - Get all populations (non-const)
- `addSynapse(NeuronId source, NeuronId destination, SynapticWeight weight = 0.0f, Delay delay = 1)` - Add synapse
- `getSynapse(SynapseId id)` - Get synapse (non-const)
- `getSynapse(SynapseId id) const` - Get synapse (const)
- `getSynapseCount() const` - Get synapse count
- `getSynapses() const` - Get all synapses
- `getSynapsesFrom(NeuronId neuron)` - Get synapses from neuron
- `getSynapsesTo(NeuronId neuron)` - Get synapses to neuron
- `getTotalNeuronCount() const` - Get total neuron count
- `getActiveNeuronCount() const` - Get active neuron count
- `getFiringNeuronCount() const` - Get firing neuron count
- `getAverageFiringRate() const` - Get average firing rate
- `getAverageSynapticWeight() const` - Get average synaptic weight
- `getSynapticDensity() const` - Get synaptic density
- `step(Timestamp currentTime)` - Step all populations and synapses
- `reset()` - Reset all neurons and synapses
- `initializeRandomConnectivity(RandomGenerator& rng, float connectionProbability, float meanWeight, float weightVariance)` - Initialize random connectivity
- `getAllNeurons()` - Get all neurons (non-const)
- `getAllNeurons() const` - Get all neurons (const)

### 7. NeuralPopulation

**Methods to expose:**
- `getId() const` - Get population ID
- `getSize() const` - Get population size
- `isEmpty() const` - Check if empty
- `getNeuron(NeuronIndex index)` - Get neuron (non-const)
- `getNeuron(NeuronIndex index) const` - Get neuron (const)
- `addNeuron(Neuron* neuron)` - Add neuron
- `getNeurons() const` - Get all neurons
- `getNeuronType() const` - Get neuron type
- `setNeuronType(NeuronType type)` - Set neuron type
- `getActiveCount() const` - Get active count
- `getFiringCount() const` - Get firing count
- `getAverageFiringRate() const` - Get average firing rate
- `getActivityLevel() const` - Get activity level
- `initializeRandom(RandomGenerator& rng)` - Initialize all neurons randomly
- `step(Timestamp currentTime)` - Step all neurons
- `reset()` - Reset all neurons
- `getNeuronIds() const` - Get neuron IDs
- `recordActivity()` - Record current state
- `getMeanMembranePotential() const` - Get mean membrane potential
- `getVarianceMembranePotential() const` - Get variance of membrane potential

### 8. NeuralWorkingMemory

**Methods to expose:**
- `initialize(Brain* brain)` - Initialize with brain reference
- `store(const std::vector<float>& pattern, float strength = 1.0f)` - Store pattern
- `storeToNeuron(NeuronId neuron, float activation)` - Store to neuron
- `retrieve() const` - Retrieve working memory content
- `contains(NeuronId neuron) const` - Check if neuron in memory
- `getNeuronActivation(NeuronId neuron) const` - Get neuron activation
- `update(TimestepDuration dt)` - Update working memory
- `clear()` - Clear working memory
- `getActiveTraces() const` - Get active trace count
- `getCapacity() const` - Get capacity
- `setCapacity(size_t cap)` - Set capacity
- `getDecayRate() const` - Get decay rate
- `setDecayRate(float rate)` - Set decay rate
- `getMemoryNeurons() const` - Get memory neurons
- `strengthenMemory(float factor)` - Strengthen memory
- `runCompetition()` - Run competition
- `isWinning(NeuronId neuron) const` - Check if winning
- `getMemoryActivity() const` - Get memory activity

### 9. AttentionalSelection

**Methods to expose:**
- `initialize(Brain* brain)` - Initialize with brain reference
- `processCompetition(const std::vector<NeuronId>& competitors, float globalInhibition = 0.5f)` - Process competition
- `focusOnRegion(RegionId region)` - Focus on region
- `releaseAttention()` - Release attention
- `getAttendedRegions() const` - Get attended regions
- `setInhibitionStrength(float strength)` - Set inhibition strength
- `setExcitationStrength(float strength)` - Set excitation strength
- `setCompetitionThreshold(float threshold)` - Set competition threshold
- `getInhibitionFor(NeuronId neuron) const` - Get inhibition for neuron
- `getExcitationFor(NeuronId neuron) const` - Get excitation for neuron
- `update(TimestepDuration dt)` - Update attention
- `isAttended(NeuronId neuron) const` - Check if neuron attended
- `getWinners() const` - Get winners
- `applyTopDownBias(NeuronId neuron, float biasStrength)` - Apply top-down bias
- `applyBottomUpSalience(NeuronId neuron, float salienceStrength)` - Apply bottom-up salience
- `reset()` - Reset attention

### 10. PredictionSystem

**Methods to expose:**
- `getSensoryPrediction(const SensoryInput& input) const` - Get sensory prediction
- `computePredictionError(const SensoryInput& input) const` - Compute prediction error
- `getForwardModel() const` - Get forward model
- `updateForwardModel(const SensoryInput& input)` - Update forward model
- `getPredictionAccuracy() const` - Get prediction accuracy
- `getTemporalHorizon() const` - Get temporal horizon
- `setTemporalHorizon(double horizon)` - Set temporal horizon

### 11. NeuralPlanner

**Methods to expose:**
- `planAction(const std::vector<float>& state) const` - Plan action
- `getActionPolicy(const std::vector<float>& state) const` - Get action policy
- `updateValueFunction(const std::vector<float>& state, float reward)` - Update value function
- `getActionValue(const std::vector<float>& state, MotorCommand action) const` - Get action value
- `setPolicyLearningRate(float rate)` - Set policy learning rate
- `getPolicyEntropy() const` - Get policy entropy
- `planSequence(const std::vector<float>& initialState, size_t steps) const` - Plan action sequence

### 12. ConceptFormation

**Methods to expose:**
- `learnConcept(const std::vector<float>& pattern, const std::string& conceptName)` - Learn concept
- `getConceptVector(const std::string& conceptName) const` - Get concept vector
- `clusterPatterns(const std::vector<std::vector<float>>& patterns)` - Cluster patterns
- `getConceptHierarchy() const` - Get concept hierarchy
- `getConceptAssociations(const std::string& concept) const` - Get concept associations
- `updateConceptVector(const std::string& concept, const std::vector<float>& newVector)` - Update concept vector
- `detectNovelConcepts(const std::vector<std::vector<float>>& patterns) const` - Detect novel concepts

### 13. Neuromodulator System

#### Neuromodulator (Base)
**Methods to expose:**
- `getName() const` - Get modulator name
- `getLevel() const` - Get current level
- `setLevel(float level)` - Set current level
- `getPlasticityFactor() const` - Get plasticity factor
- `update(TimestepDuration dt)` - Update state

#### Dopamine
**Methods to expose:**
- `signalReward(float reward)` - Signal reward
- `signalRewardPredictionError(float error)` - Signal prediction error
- `getRewardHistory() const` - Get reward history
- `getPredictionErrorHistory() const` - Get prediction error history

#### Curiosity
**Methods to expose:**
- `getExplorationBonus() const` - Get exploration bonus
- `setExplorationRate(float rate)` - Set exploration rate
- `getIntrinsicMotivation() const` - Get intrinsic motivation

#### Novelty
**Methods to expose:**
- `detectNovelty(const std::vector<float>& input) const` - Detect novelty
- `getNoveltyThreshold() const` - Get novelty threshold
- `setNoveltyThreshold(float threshold)` - Set novelty threshold
- `getNoveltyHistory() const` - Get novelty history

#### PredictionError
**Methods to expose:**
- `computeError(float predicted, float actual) const` - Compute error
- `getErrorHistory() const` - Get error history
- `getErrorMagnitude() const` - Get error magnitude

### 14. Plasticity Rules

#### PlasticityRule (Base)
**Methods to expose:**
- `getEligibilityTrace() const` - Get eligibility trace
- `setEligibilityTrace(float trace)` - Set eligibility trace
- `decayEligibilityTrace(float rate)` - Decay eligibility trace
- `getLearningRate() const` - Get learning rate
- `setLearningRate(float rate)` - Set learning rate

#### Hebbian
**Methods to expose:**
- `setSynapticStrength(SynapseId synapse, float strength)` - Set synaptic strength
- `getSynapticStrength(SynapseId synapse) const` - Get synaptic strength

#### STDP
**Methods to expose:**
- `getTimeWindow() const` - Get time window
- `setTimeWindow(float window)` - Set time window
- `getLearningRates() const` - Get learning rates
- `setLearningRates(float prePost, float postPre)` - Set learning rates

#### StructuralPlasticity
**Methods to expose:**
- `enableStructuralPlasticity(bool enable)` - Enable structural plasticity
- `isStructuralPlasticityEnabled() const` - Check if enabled
- `createNewSynapse(NeuronId pre, NeuronId post)` - Create new synapse
- `pruneSynapse(SynapseId synapse)` - Prune synapse

### 15. DevelopmentSystem

**Methods to expose:**
- `getDevelopmentalStage() const` - Get developmental stage
- `setDevelopmentalStage(DevelopmentalStage stage)` - Set developmental stage
- `getMaturationProgress() const` - Get maturation progress
- `getSynaptogenesisRate() const` - Get synaptogenesis rate
- `getPruningRate() const` - Get pruning rate
- `enableDevelopment(bool enable)` - Enable development
- `isDevelopmentEnabled() const` - Check if development enabled
- `updateDevelopment(TimestepDuration dt)` - Update development
- `getBrain() const` - Get brain reference

### 16. Synaptogenesis

**Methods to expose:**
- `getFormationRate() const` - Get formation rate
- `setFormationRate(float rate)` - Set formation rate
- `getPatternDependency() const` - Get pattern dependency
- `setPatternDependency(bool enable)` - Set pattern dependency
- `createSynapse(const std::pair<NeuronId, NeuronId>& neurons)` - Create synapse
- `getFormationHistory() const` - Get formation history

### 17. Maturation

**Methods to expose:**
- `getMaturationLevel() const` - Get maturation level
- `setMaturationRate(float rate)` - Set maturation rate
- `getThresholdShift() const` - Get threshold shift
- `setThresholdShift(float shift)` - Set threshold shift
- `getRefractoryPeriodShift() const` - Get refractory period shift
- `setRefractoryPeriodShift(float shift)` - Set refractory period shift
- `updateMaturation(TimestepDuration dt)` - Update maturation

### 18. Pruning

**Methods to expose:**
- `getPruningRate() const` - Get pruning rate
- `setPruningRate(float rate)` - Set pruning rate
- `getActivityThreshold() const` - Get activity threshold
- `setActivityThreshold(float threshold)` - Set activity threshold
- `pruneInactiveSynapses(float threshold)` - Prune inactive synapses
- `getPruningHistory() const` - Get pruning history

### 19. MotorSystem

**Methods to expose:**
- `getMotorCommands(const std::vector<float>& neuralActivity) const` - Get motor commands
- `getInverseKinematics(const std::vector<float>& target) const` - Get inverse kinematics
- `setMotorConstraints(const std::vector<float>& constraints)` - Set motor constraints
- `getMotorErrors() const` - Get motor errors
- `updateMotorControl(TimestepDuration dt)` - Update motor control

### 20. VisionProcessor

**Methods to expose:**
- `process(const Vision& input)` - Process vision input
- `getFeatures() const` - Get feature representation
- `getFeatureDimensions() const` - Get feature dimensionality
- `setFeatureExtractor(std::function<std::vector<float>(const Vision&)>)` - Set feature extractor
- `getFeatureHistory(size_t n) const` - Get feature history

### 21. Memory Systems

#### NeuralEpisodicMemory
**Methods to expose:**
- `storeEpisode(const std::vector<float>& state, const std::vector<float>& action, float reward)` - Store episode
- `recallEpisode(size_t index) const` - Recall episode
- `getEpisodeCount() const` - Get episode count
- `getEpisodeSequence() const` - Get episode sequence
- `clearEpisodeMemory()` - Clear episode memory
- `setReplayRate(float rate)` - Set replay rate
- `getReplayStrength(size_t index) const` - Get replay strength

#### NeuralAssociativeMemory
**Methods to expose:**
- `storeAssociation(const std::vector<float>& pattern1, const std::vector<float>& pattern2, float strength = 1.0f)` - Store association
- `retrieveAssociation(const std::vector<float>& pattern) const` - Retrieve association
- `getAssociationStrength(const std::vector<float>& pattern1, const std::vector<float>& pattern2) const` - Get association strength
- `getAllPatterns() const` - Get all patterns
- `removeAssociation(const std::vector<float>& pattern1, const std::vector<float>& pattern2)` - Remove association
- `updateAssociation(const std::vector<float>& pattern1, const std::vector<float>& pattern2, float newStrength)` - Update association
- `getAssociationMatrix() const` - Get association matrix

### 22. Experimentation Systems

#### Experiment
**Methods to expose:**
- `addCondition(const std::string& name, const std::map<std::string, double>& parameters)` - Add condition
- `runExperiment()` - Run experiment
- `getResults() const` - Get results
- `exportResults(const std::string& filepath) const` - Export results
- `getConditionResults(const std::string& conditionName) const` - Get condition results
- `setRandomSeed(uint64_t seed)` - Set random seed
- `getExperimentStatus() const` - Get experiment status

#### ExperimentRunner
**Methods to expose:**
- `addExperiment(std::shared_ptr<Experiment> experiment)` - Add experiment
- `runAllExperiments()` - Run all experiments
- `getExperimentProgress() const` - Get experiment progress
- `getExperimentResults() const` - Get experiment results
- `pauseExperiment(size_t index)` - Pause experiment
- `resumeExperiment(size_t index)` - Resume experiment
- `stopExperiment(size_t index)` - Stop experiment
- `setParallelExecution(bool enable)` - Set parallel execution

#### Metrics
**Methods to expose:**
- `recordMetric(const std::string& name, double value)` - Record metric
- `getMetricAverage(const std::string& name) const` - Get metric average
- `getMetricVariance(const std::string& name) const` - Get metric variance
- `getMetricHistory(const std::string& name, size_t n) const` - Get metric history
- `compareMetrics(const std::string& metric1, const std::string& metric2) const` - Compare metrics
- `generateReport() const` - Generate report
- `exportMetrics(const std::string& filepath) const` - Export metrics

#### AblationSystem
**Methods to expose:**
- `ablateComponent(const std::string& componentName)` - Ablate component
- `restoreComponent(const std::string& componentName)` - Restore component
- `getAblationStatus() const` - Get ablation status
- `runControlExperiment()` - Run control experiment
- `getAblationResults() const` - Get ablation results
- `setAblationMode(AblationMode mode)` - Set ablation mode
- `getComponentDependencies(const std::string& componentName) const` - Get component dependencies

### 23. Performance Systems

#### CheckpointSystem
**Methods to expose:**
- `saveCheckpoint(const std::string& name, const std::string& filepath) const` - Save checkpoint
- `loadCheckpoint(const std::string& name)` - Load checkpoint
- `getCheckpointList() const` - Get checkpoint list
- `deleteCheckpoint(const std::string& name)` - Delete checkpoint
- `restoreFromCheckpoint(const std::string& name)` - Restore from checkpoint
- `setAutoSaveInterval(double interval)` - Set auto-save interval

#### SIMDVectorization
**Methods to expose:**
- `enableSIMD(bool enable)` - Enable SIMD
- `isSIMDEnabled() const` - Check if SIMD enabled
- `getSIMDInfo() const` - Get SIMD information
- `optimizeForOperation(const std::string& operation)` - Optimize for operation
- `getVectorizationLevel() const` - Get vectorization level

#### MemoryPool
**Methods to expose:**
- `allocate(size_t size)` - Allocate memory
- `deallocate(void* ptr)` - Deallocate memory
- `getTotalAllocated() const` - Get total allocated
- `getAvailableMemory() const` - Get available memory
- `optimize() const` - Optimize memory
- `getFragmentationLevel() const` - Get fragmentation level

#### ParallelProcessing
**Methods to expose:**
- `setThreadCount(size_t count)` - Set thread count
- `getThreadCount() const` - Get thread count
- `isParallelEnabled() const` - Check if parallel enabled
- `parallelFor(std::function<void(size_t)> func, size_t start, size_t end)` - Parallel for
- `parallelReduce(std::function<double(size_t)> func, size_t start, size_t end) const` - Parallel reduce
- `getLoadBalance() const` - Get load balance

#### SparseConnectivity
**Methods to expose:**
- `addConnection(NeuronId pre, NeuronId post, SynapticWeight weight)` - Add connection
- `removeConnection(NeuronId pre, NeuronId post)` - Remove connection
- `getConnectionsFrom(NeuronId neuron) const` - Get connections from neuron
- `getConnectionsTo(NeuronId neuron) const` - Get connections to neuron
- `getConnectionWeight(NeuronId pre, NeuronId post) const` - Get connection weight
- `setConnectionWeight(NeuronId pre, NeuronId post, SynapticWeight weight)` - Set connection weight
- `getConnectionCount() const` - Get connection count
- `compress()` - Compress connectivity
- `getMemoryUsage() const` - Get memory usage

#### OptimizedBrain
**Methods to expose:**
- `enableSIMD(bool enable)` - Enable SIMD
- `enableParallel(bool enable)` - Enable parallel
- `enableCaching(bool enable)` - Enable caching
- `getPerformanceStats() const` - Get performance stats
- `optimizeForWorkload(const std::string& workload)` - Optimize for workload
- `getCurrentConfiguration() const` - Get current configuration

#### EventQueue
**Methods to expose:**
- `enqueueEvent(const std::string& name, double priority, std::function<void()> callback)` - Enqueue event
- `processEvents(double timeStep)` - Process events
- `getEventCount() const` - Get event count
- `getEventQueueSize() const` - Get event queue size
- `clearEvents()` - Clear events
- `setMaxQueueSize(size_t size)` - Set max queue size

### 24. Development Tools

#### DevelopmentSystem
**Methods to expose:**
- `enableMaturation(bool enable)` - Enable maturation
- `isMaturationEnabled() const` - Check if maturation enabled
- `enableSynaptogenesis(bool enable)` - Enable synaptogenesis
- `isSynaptogenesisEnabled() const` - Check if synaptogenesis enabled
- `enablePruning(bool enable)` - Enable pruning
- `isPruningEnabled() const` - Check if pruning enabled
- `updateDevelopment(TimestepDuration dt)` - Update development
- `getDevelopmentProgress() const` - Get development progress

#### Synaptogenesis
**Methods to expose:**
- `enablePatternBased(bool enable)` - Enable pattern-based formation
- `isPatternBasedEnabled() const` - Check if pattern-based enabled
- `getFormationProbability() const` - Get formation probability
- `setFormationProbability(double prob)` - Set formation probability
- `createPatternBasedConnection(const std::vector<float>& pattern)` - Create pattern-based connection

#### Maturation
**Methods to expose:**
- `enableActivityDependent(bool enable)` - Enable activity-dependent maturation
- `isActivityDependentEnabled() const` - Check if activity-dependent enabled
- `getMaturationRate() const` - Get maturation rate
- `setMaturationRate(double rate)` - Set maturation rate
- `getThresholdEvolution() const` - Get threshold evolution
- `setThresholdEvolution(double evolution)` - Set threshold evolution

#### Pruning
**Methods to expose:**
- `enableActivityDependent(bool enable)` - Enable activity-dependent pruning
- `isActivityDependentEnabled() const` - Check if activity-dependent enabled
- `getPruningIntensity() const` - Get pruning intensity
- `setPruningIntensity(double intensity)` - Set pruning intensity
- `getSurvivalThreshold() const` - Get survival threshold
- `setSurvivalThreshold(double threshold)` - Set survival threshold

## Summary

This analysis documents 48 C++ classes and approximately 400+ methods that are NOT currently exposed to Python but should be for a complete NLM Python API. The implementation should be prioritized based on criticality and user needs.

The next step is to identify the most important functionality for immediate implementation and create a detailed implementation plan.