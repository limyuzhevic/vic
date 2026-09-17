#include "src/plasticity/STDP.hpp"
#include "src/plasticity/Hebbian.hpp"
#include "src/neuromodulation/Neuromodulator.hpp"
#include <iostream>

int main() {
    nlm::STDP stdp;
    nlm::Hebbian hebbian;
    nlm::Dopamine dopamine;
    
    std::cout << "STDP name: " << stdp.getName() << std::endl;
    std::cout << "Hebbian name: " << hebbian.getName() << std::endl;
    std::cout << "Dopamine name: " << dopamine.getName() << std::endl;
    
    return 0;
}
