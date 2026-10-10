// Minimal test to check C++ syntax
#include <iostream>
#include <memory>

namespace nlm {
    class Config {};
    class Brain {};
    class SemanticMemory {
    public:
        void initialize(Brain* brain) {}
        Brain* getBrain() const { return nullptr; }
    };
    class ProceduralMemory {
    public:
        void initialize(Brain* brain) {}
        Brain* getBrain() const { return nullptr; }
    };
    class PredictionSystem {
    public:
        void initialize(Brain* brain) {}
        Brain* getBrain() const { return nullptr; }
    };
}

int main() {
    std::cout << "Test compilation successful!" << std::endl;
    return 0;
}
