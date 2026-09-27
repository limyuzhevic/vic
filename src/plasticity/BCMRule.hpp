// Empty implementation - stub for BCM rule
// Placeholder for future BCM implementation

class BCMRule : public PlasticityRule {
public:
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override {}
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {}
    
    const char* getName() const override { return "BCM"; }
};
