// Actions bindings implementation
#include "actions_bindings.h"

namespace nlm {

void bindAction(py::module_& m) {
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        // Add convenience methods for Action class (static factory methods)
        .def("createMove", [](const std::string& distance) {
            // Parse distance from string (simplified for now)
            float dist = std::stof(distance);
            return std::make_shared<Action>(ActionType::MoveForward, std::vector<float>{dist});
        }, py::arg("distance"), "Create a move action with distance")
        .def("createLook", [](const std::string& x, const std::string& y) {
            // Parse coordinates from strings (simplified for now)
            float coordX = std::stof(x);
            float coordY = std::stof(y);
            return std::make_shared<Action>(ActionType::Look, std::vector<float>{coordX, coordY});
        }, py::arg("x"), py::arg("y"), "Create a look action with target coordinates")
        .def("createInteract", [](const std::string& targetId) {
            return std::make_shared<Action>(ActionType::Interact, std::vector<float>{});
        }, py::arg("targetId"), "Create an interact action")
        .def("createEat", [](const std::string& foodId) {
            return std::make_shared<Action>(ActionType::Eat, std::vector<float>{});
        }, py::arg("foodId"), "Create an eat action")
        .def("createDrink", [](const std::string& drinkId) {
            return std::make_shared<Action>(ActionType::Drink, std::vector<float>{});
        }, py::arg("drinkId"), "Create a drink action")
        .def("createRest", [](const std::string& duration) {
            float dur = std::stof(duration);
            return std::make_shared<Action>(ActionType::Rest, std::vector<float>{dur});
        }, py::arg("duration"), "Create a rest action with duration")
        .def("createWait", [](const std::string& duration) {
            float dur = std::stof(duration);
            return std::make_shared<Action>(ActionType::Wait, std::vector<float>{dur});
        }, py::arg("duration"), "Create a wait action with duration")
        // Alternative static factory methods
        .def_static("moveAction", [](float distance) {
            return std::make_shared<Action>(ActionType::MoveForward, std::vector<float>{distance});
        }, py::arg("distance"), "Create a move action (alternative factory method)")
        .def_static("lookAction", [](float x, float y) {
            return std::make_shared<Action>(ActionType::Look, std::vector<float>{x, y});
        }, py::arg("x"), py::arg("y"), "Create a look action (alternative factory method)")
        .def_static("interactAction", [](const std::string& targetId) {
            return std::make_shared<Action>(ActionType::Interact, std::vector<float>{});
        }, py::arg("targetId"), "Create an interact action (alternative factory method)")
        .def_static("eatAction", [](const std::string& foodId) {
            return std::make_shared<Action>(ActionType::Eat, std::vector<float>{});
        }, py::arg("foodId"), "Create an eat action (alternative factory method)")
        .def_static("drinkAction", [](const std::string& drinkId) {
            return std::make_shared<Action>(ActionType::Drink, std::vector<float>{});
        }, py::arg("drinkId"), "Create a drink action (alternative factory method)")
        .def_static("restAction", [](float duration) {
            return std::make_shared<Action>(ActionType::Rest, std::vector<float>{duration});
        }, py::arg("duration"), "Create a rest action (alternative factory method)")
        .def_static("waitAction", [](float duration) {
            return std::make_shared<Action>(ActionType::Wait, std::vector<float>{duration});
        }, py::arg("duration"), "Create a wait action (alternative factory method)")
}

void bindAgentBody(py::module_& m) {
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset)
        // Add utility methods
        .def("distanceTo", [](const AgentBody& self, float x, float y) {
            float dx = x - self.x;
            float dy = y - self.y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        .def("angleTo", [](const AgentBody& self, float x, float y) {
            float dx = x - self.x;
            float dy = y - self.y;
            return std::atan2(dy, dx);
        }, py::arg("x"), py::arg("y"), "Calculate angle to position (radians)")
        .def("moveTowards", [](AgentBody& self, float x, float y, float speed) {
            float dx = x - self.x;
            float dy = y - self.y;
            float dist = std::sqrt(dx*dx + dy*dy);
            if (dist > 0) {
                self.x += dx / dist * speed;
                self.y += dy / dist * speed;
            }
            self.isMoving = true;
        }, py::arg("x"), py::arg("y"), py::arg("speed"), "Move towards position");
}

void bindActionResult(py::module_& m) {
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        // Add convenience properties
        .def("is_success", &ActionResult::is_success, "Check if action was successful")
        .def("is_failure", &ActionResult::is_failure, "Check if action failed")
        .def("to_dict", &ActionResult::to_dict, "Convert result to dictionary");
}

void bindSensoryPercept(py::module_& m) {
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        // Add convenience methods
        .def("addVision", [](SensoryPercept& self, const Vision& v) {
            self.setVision(v);
        }, py::arg("vision"), "Add vision data")
        .def("addTouch", [](SensoryPercept& self, const std::vector<float>& touch) {
            self.setTouch(touch);
        }, py::arg("touch"), "Add touch data")
        .def("addInternal", [](SensoryPercept& self, const std::vector<float>& internal) {
            self.setInternal(internal);
        }, py::arg("internal"), "Add internal signal data")
        .def("addAudio", [](SensoryPercept& self, const Audio& a) {
            self.setAudio(a);
        }, py::arg("audio"), "Add audio data")
        .def("getAllData", &SensoryPercept::getAllSignals, "Get all sensory data as dictionary");
}

void bindAgentBrain(py::module_& m) {
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Add high-level convenience methods
        .def("processPercept", [](AgentBrain& self, const py::dict& perceptData) {
            // Convert dict to SensoryPercept
            auto percept = std::make_shared<SensoryPercept>();
            // Implementation would convert dict to actual percept
            self.processSensoryInput(percept);
        }, py::arg("perceptData"),
             "Process sensory input from dictionary (convenience method)")
        .def("getAction", [](AgentBrain& self, float timestep) {
            // Get action based on current brain state
            return self.decodeMotorCommand();
        }, py::arg("timestep"), "Get action for current timestep")
        .def("update", &AgentBrain::updateDevelopment, py::arg("timestep"),
             "Update agent brain state (alias for updateDevelopment)");
}

} // namespace nlm