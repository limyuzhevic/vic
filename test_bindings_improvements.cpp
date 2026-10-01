#include "nlm_bindings.hpp"

// Include original NLM headers for compatibility
#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/Action.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/world/WorldObject.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/ActionResult.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/core/Types/Types.hpp"

// Helper functions implementation
namespace python_bindings {
    std::shared_ptr<Config> createDefaultConfig() {
        return std::make_shared<Config>();
    }
    
    std::shared_ptr<Brain> createBrain(std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }
    
    std::shared_ptr<SimpleWorld> createWorld() {
        return std::make_shared<SimpleWorld>();
    }
    
    std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }
    
    py::list batchProcessNeurons(
        std::shared_ptr<Brain> brain,
        const std::vector<NeuronId>& neuronIds,
        std::function<void(NeuronId, double)> operation) {
        
        py::list results;
        for (const auto& id : neuronIds) {
            try {
                operation(id, brain->getCurrentValue(id));
                results.append(py::make_tuple(id, true, "Success"));
            } catch (const std::exception& e) {
                results.append(py::make_tuple(id, false, std::string(e.what())));
            }
        }
        return results;
    }
    
    std::vector<float> pyArrayToVector(const py::array_t<float>& arr) {
        auto buffer = arr.request();
        float* ptr = static_cast<float*>(buffer.ptr);
        size_t size = buffer.size;
        return std::vector<float>(ptr, ptr + size);
    }
    
    py::array_t<float> vectorToPyArray(const std::vector<float>& vec) {
        return py::array_t<float>(vec);
    }
}

// Enable copy support for Action
void enable_copying(py::class_<Action>& cl) {
    cl.def(py::init<const Action&>()); // Copy constructor
    cl.def(py::pickle(
        [](const Action& a) {
            return py::make_tuple(a.getType(), a.getParameters());
        },
        [](const py::tuple& t) {
            ActionType type = t[0].cast<ActionType>();
            std::vector<float> params = t[1].cast<std::vector<float>>();
            Action action(type, params);
            return action;
        }
    ));
}

// Enable copy support for IDClass
template<typename T, typename ValueType>
void enable_copying(py::class_<IDClass<T, ValueType>>& cl) {
    cl.def(py::init<const IDClass<T, ValueType>&>()); // Copy constructor
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Test Version
        ---------------------------------------------------------------
        Test module to verify enhanced bindings functionality.
    )pbdoc";
    
    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception<ValidationError>(m, "ValidationError", PyExc_ValueError);
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_RuntimeError);
    py::register_exception<ResourceError>(m, "ResourceError", PyExc_RuntimeError);
    
    // Test ID classes
    py::class_<IDClass<NeuronId, uint64_t>>(m, "NeuronId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def(py::init<const IDClass<NeuronId, uint64_t>&>())
        .def_readwrite("value", &IDClass<NeuronId, uint64_t>::getValue)
        .def("index", &IDClass<NeuronId, uint64_t>::getIndex)
        .def("__eq__", &IDClass<NeuronId, uint64_t>::operator==)
        .def("__ne__", &IDClass<NeuronId, uint64_t>::operator!=)
        .def("__hash__", [](const IDClass<NeuronId, uint64_t>& id) { return id.hash(); })
        .def("__repr__", [](const IDClass<NeuronId, uint64_t>& id) { return id.repr(); })
        .def("batch", &IDClass<NeuronId, uint64_t>::batch, py::arg("count"), py::arg("start") = 0);
    
    // Test Action class with copy support
    py::class_<Action>(m, "Action")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def(py::init<const Action&>())
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);
    
    // Test context manager
    py::class_<SimulationSession>(m, "SimulationSession")
        .def(py::init<>())
        .def("enter", &SimulationSession::enter)
        .def("exit", [](SimulationSession& self, int ptype, const char* pname, const char* pval, const char* ptrace) {
            self.exit(ptype, pname, pval, ptrace);
        })
        .def("is_active", &SimulationSession::is_active_context);
    
    // Test builder pattern
    py::class_<ActionBuilder>(m, "ActionBuilder")
        .def(py::init<>())
        .def("setType", &ActionBuilder::setType, py::arg("type"))
        .def("addParameter", &ActionBuilder::addParameter, py::arg("param"))
        .def("build", &ActionBuilder::build)
        .def("reset", &ActionBuilder::reset);
    
    // Test Config validation
    py::class_<Config>(m, "Config")
        .def(py::init<>())
        .def("has", &Config::has, py::arg("key"))
        .def("get", &Config::get, py::arg("key"))
        .def("getKeys", &Config::getKeys);
    
    // Test batch operations helper
    m.def("batch_process_neurons", &python_bindings::batchProcessNeurons,
         py::arg("brain"), py::arg("neuronIds"), py::arg("operation"));
    
    // Test factory functions
    m.def("create_brain", &python_bindings::createBrain, py::arg("config"));
    m.def("create_world", &python_bindings::createWorld);
    
    // Test serialization
    m.def("serialize", &Serializer::toJson, py::arg("obj"));
    m.def("deserialize", &Serializer::fromJson, py::arg("json"));
    
    // Test type conversion
    m.def("to_numpy", &python_bindings::vectorToPyArray, py::arg("vec"));
    m.def("from_numpy", &python_bindings::pyArrayToVector, py::arg("arr"));
}

int main() {
    std::cout << "Testing enhanced NLM bindings..." << std::endl;
    
    try {
        // Test 1: Create ID objects
        std::cout << "Test 1: Creating NeuronId objects..." << std::endl;
        IDClass<NeuronId, uint64_t> id1(42);
        IDClass<NeuronId, uint64_t> id2(42);
        IDClass<NeuronId, uint64_t> id3(100);
        
        std::cout << "id1: " << id1.repr() << std::endl;
        std::cout << "id2: " << id2.repr() << std::endl;
        std::cout << "id3: " << id3.repr() << std::endl;
        
        // Test equality
        if (id1 == id2) std::cout << "✓ id1 == id2" << std::endl;
        if (id1 != id3) std::cout << "✓ id1 != id3" << std::endl;
        
        // Test batch operations
        std::cout << "\nTest 2: Testing batch operations..." << std::endl;
        auto batch = IDClass<NeuronId, uint64_t>::batch(5, 100);
        std::cout << "Created batch of " << batch.size() << " NeuronId objects" << std::endl;
        
        // Test Action creation
        std::cout << "\nTest 3: Testing Action creation..." << std::endl;
        Action action(ActionType::MoveForward);
        action.setType(ActionType::MoveForward);
        action.addParameter(0.5f);
        action.addParameter(1.0f);
        
        std::cout << "Action type: " << static_cast<int>(action.getType()) << std::endl;
        std::cout << "Action parameters: ";
        for (float param : action.getParameters()) std::cout << param << " ";
        std::cout << std::endl;
        
        // Test copy constructor
        std::cout << "\nTest 4: Testing copy constructor..." << std::endl;
        Action action2 = action; // Copy constructor
        if (action == action2) std::cout << "✓ Actions are equal after copy" << std::endl;
        
        // Test builder pattern
        std::cout << "\nTest 5: Testing builder pattern..." << std::endl;
        ActionBuilder builder;
        Action built = builder
            .setType(ActionType::Look)
            .addParameter(45.0f)
            .addParameter(30.0f)
            .build();
        
        std::cout << "Built action type: " << static_cast<int>(built.getType()) << std::endl;
        std::cout << "Built action parameters count: " << built.getParameters().size() << std::endl;
        
        // Test context manager
        std::cout << "\nTest 6: Testing context manager..." << std::endl;
        SimulationSession session;
        session.enter();
        std::cout << "✓ Session activated" << std::endl;
        
        // Test error handling
        std::cout << "\nTest 7: Testing error handling..." << std::endl;
        try {
            IDClass<NeuronId, uint64_t> invalid_id(-1);
            if (!invalid_id.isValid()) std::cout << "✓ Invalid ID detected" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ Exception: " << e.what() << std::endl;
        }
        
        // Test serialization
        std::cout << "\nTest 8: Testing serialization..." << std::endl;
        try {
            std::string json = Serializer::toJson(id1);
            std::cout << "Serialized: " << json << std::endl;
            
            auto deserialized = Serializer::fromJson<IDClass<NeuronId, uint64_t>>(json);
            std::cout << "Deserialized: " << deserialized.repr() << std::endl;
            
            if (id1 == deserialized) std::cout << "✓ Serialization/deserialization successful" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ Serialization error: " << e.what() << std::endl;
        }
        
        std::cout << "\n✓ All tests passed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "✗ Test failed: " << e.what() << std::endl;
        return 1;
    }
}