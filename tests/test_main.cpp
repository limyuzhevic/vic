#include <iostream>
#include "tests/experiment_system_tests.cpp"

int main() {
    std::cout << "Running NLM Experiment System Tests..." << std::endl;
    runExperimentSystemTests();
    std::cout << "All tests completed successfully!" << std::endl;
    return 0;
}