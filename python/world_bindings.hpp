#ifndef WORLD_BINDINGS_HPP
#define WORLD_BINDINGS_HPP

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace nlm {
    // Forward declarations
    class WorldObject;
    class SensoryInput;
    class Vision;
    class Audio;
    class InternalSignals;
    class Action;
    class AgentBody;
    class SensoryPercept;
    class SimpleWorld;
    class ActionResult;
    enum class WorldObjectType;
    enum class ActionType;
    enum class MotorCommand;
}

// Forward declare the module creation function
void bindWorldModule(pybind11::module_ &m);

#endif // WORLD_BINDINGS_HPP
