// python/bindings_action.cpp

void bindAction(py::module_& m) {
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)
    
    Represents a motor command that the brain can produce.
    An action includes a type (e.g., "move forward") and optional parameters
    (e.g., speed, duration, target position).
    
    Common Uses:
        - Motor output from brain
        - Agent-environment interaction
        - Movement control
        
    Example:
        >>> action = pynlm.Action(pynlm.ActionType.MoveForward)
        >>> action.setParameter("speed", 0.5)
        >>> action.getParameter("speed")
        0.5
    
    Attributes:
        action_type: Type of action (see ActionType enum)
        parameters: Dictionary of action parameters
        name: Human-readable name of the action
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             R"pbdoc(Create action with just a type)
             
             Args:
                 type: Action type (from ActionType enum)
                 
             Example:
                 >>> action = pynlm.Action(pynlm.ActionType.LookLeft)
                 >>> action.getType()
                 'look_left'
             ")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             R"pbdoc(Create action with type and parameters)
             
             Args:
                 type: Action type (from ActionType enum)
                 parameters: List of float parameters
                 
             Example:
                 >>> action = pynlm.Action(pynlm.ActionType.Interact, [0.5, 0.8])
                 >>> action.getParameters()
                 [0.5, 0.8]
             ")
        .def("getType", &Action::getType,
             R"pbdoc(Get action type)
             
             Returns:
                 str: Action type as string (from ActionType enum)
                 
             Example:
                 >>> action = pynlm.Action(pynlm.ActionType.Eat)
                 >>> action.getType()
                 'eat'
             ")
        .def("setType", &Action::setType, py::arg("type"),
             R"pbdoc(Set action type)
             
             Args:
                 type: New action type
                 
             Example:
                 >>> action = pynlm.Action(pynlm.ActionType.MoveForward)
                 >>> action.setType(pynlm.ActionType.TurnLeft)
                 >>> action.getType()
                 'turn_left'
             ")
        .def("getParameters", &Action::getParameters,
             R"pbdoc(Get action parameters)
             
             Returns:
                 list: List of action parameters
                 
             Example:
                 >>> action = pynlm.Action(pynlm.ActionType.Look, [0.3, 0.7])
                 >>> params = action.getParameters()
                 >>> len(params)
                 2
             ")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             R"pbdoc(Set action parameters)
             
             Args:
                 params: New list of action parameters
                 
             Example:
                 >>> action = pynlm.Action(pynlm.ActionType.Interact)
                 >>> action.setParameters([1.0, 0.5, 0.8])
                 >>> action.getParameters()
                 [1.0, 0.5, 0.8]
             ")
        .def("getName", &Action::getName,
             R"pbdoc(Get human-readable action name)
             
             Returns:
                 str: Human-readable action name
                 
             Example:
                 >>> action = pynlm.Action(pynlm.ActionType.MoveForward)
                 >>> action.getName()
                 'Move forward'
             ")
        .def("getName", &Action::getName,
             R"pbdoc(Get human-readable action name)
             
             Returns:
                 str: Human-readable action name
                 
             Example:
                 >>> action = pynlm.Action(pynlm.ActionType.MoveForward)
                 >>> action.getName()
                 'Move forward'
             ")
        .def("clone", &Action::clone,
             R"pbdoc(Clone this action)
             
             Returns:
                 Action: New action with same type and parameters
                 
             Example:
                 >>> action1 = pynlm.Action(pynlm.ActionType.MoveForward, [0.5])
                 >>> action2 = action1.clone()
                 >>> action1 is action2
                 False
                 >>> action1.getType() == action2.getType()
                 True
             ")
        .def("addParameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"), R"pbdoc(Add parameter to action)
        
        Args:
            value: Parameter value to add
            
        Example:
            >>> action = pynlm.Action(pynlm.ActionType.Custom)
            >>> action.addParameter(0.5)
            >>> action.addParameter(1.0)
            >>> action.getParameters()
            [0.5, 1.0]
        ")
        .def("getParameter", [](const Action& self, size_t index) {
            auto params = self.getParameters();
            if (index >= params.size()) {
                throw nlm::InvalidParameterError(
                    "parameter index",
                    "index must be between 0 and " + std::to_string(params.size() - 1)
                );
            }
            return params[index];
        }, py::arg("index"), R"pbdoc(Get parameter by index)
        
        Args:
            index: Parameter index (0-based)
            
        Returns:
            float: Parameter value
            
        Raises:
            InvalidParameterError: If index out of range
            
        Example:
            >>> action = pynlm.Action(pynlm.ActionType.Custom, [0.1, 0.5, 0.9])
            >>> action.getParameter(1)
            0.5
        ")
        .def("getParameterCount", [](const Action& self) {
            return self.getParameters().size();
        }, R"pbdoc(Get number of parameters)
        
        Returns:
            int: Number of parameters
            
        Example:
            >>> action = pynlm.Action(pynlm.ActionType.Custom, [1.0, 2.0, 3.0])
            >>> action.getParameterCount()
            3
        ")
        .def("getParameterSum", [](Action& self) {
            auto params = self.getParameters();
            float sum = std::accumulate(params.begin(), params.end(), 0.0f);
            return sum;
        }, R"pbdoc(Get sum of all parameters)
        
        Returns:
            float: Sum of all parameter values
            
        Example:
            >>> action = pynlm.Action(pynlm.ActionType.Custom, [0.1, 0.2, 0.3])
            >>> action.getParameterSum()
            0.6
        ")
        .def("isValid", [](const Action& self) {
            auto params = self.getParameters();
            return !params.empty() && std::all_of(params.begin(), params.end(),
                [](float p) { return !std::isnan(p) && !std::isinf(p); });
        }, R"pbdoc(Check if action parameters are valid)
        
        Returns:
            bool: True if action has valid parameters
            
        Example:
            >>> valid_action = pynlm.Action(pynlm.ActionType.Custom, [0.5, 0.8])
            >>> valid_action.isValid()
            True
            >>> invalid_action = pynlm.Action(pynlm.ActionType.Custom, [1.0, float('inf')])
            >>> invalid_action.isValid()
            False
        ")
        .def("__repr__", [](const Action& a) {
            return "<Action: " + a.getName() + " (type=" + a.getType() + ">";
        });
}