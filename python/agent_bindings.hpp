#ifndef AGENT_BINDINGS_HPP
#define AGENT_BINDINGS_HPP

#include <pybind11/pybind11.h>

namespace nlm {
    // Forward declarations
    class Action;
    class AgentBody;
    class SensoryPercept;
    class AgentBrain;
    class SimpleWorld;
    class Brain;
    enum class ActionType;
    enum class MotorCommand;
    enum class WorldObjectType;
}

// Forward declare the module creation function
void bindAgentModule(pybind11::module_ &m);

#endif // AGENT_BINDINGS_HPP
