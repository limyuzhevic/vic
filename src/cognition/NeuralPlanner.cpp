namespace nlm {

class NeuralPlanner {
public:
    NeuralPlanner(std::shared_ptr<Config> config) {}
    ~NeuralPlanner() = default;
    
    bool initialize() { return true; }
    void update(double timestep) {}
};

class ConceptFormation {
public:
    ConceptFormation(std::shared_ptr<Config> config) {}
    ~ConceptFormation() = default;
    
    bool initialize() { return true; }
    void update(double timestep) {}
};

} // namespace nlm