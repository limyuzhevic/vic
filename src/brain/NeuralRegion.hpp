// NeuralRegion: A brain region containing multiple populations and local connectivity
// PHASE 2 IMPLEMENTED - Now includes region-level dynamics

class NeuralRegion {
public:
    // Create region with ID
    explicit NeuralRegion(RegionId id);

    // Create region with ID and name
    NeuralRegion(RegionId id, const std::string& name);

    ~NeuralRegion();

    // Disable copying, enable moving
    NeuralRegion(const NeuralRegion&) = delete;
    NeuralRegion& operator=(const NeuralRegion&) = delete;
    NeuralRegion(NeuralRegion&&) noexcept;
    NeuralRegion& operator=(NeuralRegion&&) noexcept;

    // Identity
    RegionId getId() const;
    const std::string& getName() const;
    void setName(const std::string& name);

    // Population management
    PopulationId addPopulation(size_t size, NeuronType type = NeuronType::Internal);
    NeuralPopulation* getPopulation(PopulationId id);
    const NeuralPopulation* getPopulation(PopulationId id) const;
    size_t getPopulationCount() const;
    const std::vector<std::unique_ptr<NeuralPopulation>>& getPopulations() const;
    std::vector<NeuralPopulation*> getAllPopulations();

    // Synapse management (local connections within this region)
    SynapseId addSynapse(NeuronId source, NeuronId destination,
                         SynapticWeight weight = 0.0f, Delay delay = 1);
    Synapse* getSynapse(SynapseId id);
    const Synapse* getSynapse(SynapseId id) const;
    size_t getSynapseCount() const;
    const std::vector<std::unique_ptr<Synapse>>& getSynapses() const;

    // Connectivity queries
    std::vector<Synapse*> getSynapsesFrom(NeuronId neuron);
    std::vector<Synapse*> getSynapsesTo(NeuronId neuron);

    // Statistics
    size_t getTotalNeuronCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;

    // Local connectivity statistics
    float getAverageSynapticWeight() const;
    float getSynapticDensity() const;  // fraction of possible connections

    // Region-level dynamics (Phase 2 implementation)
    // Update region-wide dynamics based on neural activity
    void updateRegionDynamics(float timestep);

    // Modulate region excitability based on neuromodulation
    void applyNeuromodulation(float dopamine, float serotonin, float acetylcholine);

    // Calculate region contribution to global workspace
    float getRegionActivation() const;
    void setRegionActivation(float activation);

    // Integration with global brain state
    void integrateWithGlobalPrediction(const std::vector<float>& prediction);

    // Development effects on region
    void applyDevelopment(float plasticityModifier, DevelopmentalStage stage);

    // Step all populations and synapses (Phase 1 functionality)
    void step(Timestamp currentTime);

    // Reset all neurons and synapses
    void reset();

    // Initialize connectivity
    void initializeRandomConnectivity(class RandomGenerator& rng,
                                      float connectionProbability,
                                      float meanWeight,
                                      float weightVariance);

    // Get all neurons across all populations
    std::vector<Neuron*> getAllNeurons();
    std::vector<const Neuron*> getAllNeurons() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
