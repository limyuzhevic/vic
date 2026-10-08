#pragma once

namespace nlm {

class SemanticMemory {
public:
    SemanticMemory() {}
    ~SemanticMemory() = default;
    
    void initialize() {}
    void update(double timestep) {}
};

class ProceduralMemory {
public:
    ProceduralMemory() {}
    ~ProceduralMemory() = default;
    
    void initialize() {}
    void update(double timestep) {}
};

class AssociativeMemory {
public:
    AssociativeMemory() {}
    ~AssociativeMemory() = default;
    
    void initialize() {}
    void update(double timestep) {}
};

} // namespace nlm