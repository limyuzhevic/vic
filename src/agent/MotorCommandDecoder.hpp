#pragma once

#include "../brain/Brain.hpp"
#include "../core/Types/Types.hpp"
#include <vector>

namespace nlm {

class MotorCommandDecoder {
public:
    MotorCommandDecoder(std::shared_ptr<Brain> brain);
    
    MotorCommand decode(const std::vector<Neuron*>& motorForward,
                        const std::vector<Neuron*>& motorBackward,
                        const std::vector<Neuron*>& motorTurnLeft,
                        const std::vector<Neuron*>& motorTurnRight,
                        const std::vector<Neuron*>& motorInteract,
                        const std::vector<Neuron*>& motorWait,
                        bool curiosityEnabled = false,
                        float curiosityLevel = 0.0f);
    
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd, bool curiosityEnabled,
                                     float curiosityLevel, std::shared_ptr<Brain> brain);
    
private:
    std::shared_ptr<Brain> brain_;
};

} // namespace nlm
