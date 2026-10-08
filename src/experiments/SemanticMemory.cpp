namespace nlm {

class SemanticMemory {
public:
    SemanticMemory(std::shared_ptr<Config> config) {}
    ~SemanticMemory() = default;
    
    bool initialize() { return true; }
    void update(double timestep) {}
};

class ProceduralMemory {
public:
    ProceduralMemory(std::shared_ptr<Config> config) {}
    ~ProceduralMemory() = default;
    
    bool initialize() { return true; }
    void update(double timestep) {}
};

class AssociativeMemory {
public:
    AssociativeMemory(std::shared_ptr<Config> config) {}
    ~AssociativeMemory() = default;
    
    bool initialize() { return true; }
    void update(double timestep) {}
};

} // namespace nlm