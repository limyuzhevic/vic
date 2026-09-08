// Command interface base class
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
};

// Command factory interface
class CommandFactory {
public:
    virtual ~CommandFactory() = default;
    virtual std::unique_ptr<Command> createCommand() = 0;
};

// Command registry for dynamic command registration
class CommandRegistry {
private:
    std::map<std::string, std::unique_ptr<CommandFactory>> factories;
    
public:
    template<typename TCommand, typename... Args>
    void registerCommand(const std::string& name, const std::string& description, Args&&... args) {
        factories[name] = std::make_unique<CommandFactoryImpl<TCommand, Args...>>(
            description, std::forward<Args>(args)...);
    }
    
    bool hasCommand(const std::string& name) const {
        return factories.find(name) != factories.end();
    }
    
    std::unique_ptr<Command> createCommand(const std::string& name) {
        auto it = factories.find(name);
        if (it != factories.end()) {
            return it->second->createCommand();
        }
        return nullptr;
    }
    
    std::vector<std::string> getCommandNames() const {
        std::vector<std::string> names;
        for (const auto& pair : factories) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    std::map<std::string, std::string> getCommandDescriptions() const {
        std::map<std::string, std::string> descriptions;
        for (const auto& pair : factories) {
            descriptions[pair.first] = "TODO: get description";
        }
        return descriptions;
    }
};

// Template implementation of CommandFactory
template<typename TCommand, typename... Args>
class CommandFactoryImpl : public CommandFactory {
private:
    std::string description_;
    std::tuple<Args...> args_;
    
public:
    CommandFactoryImpl(const std::string& description, Args&&... args)
        : description_(description), args_(std::forward<Args>(args)...) {}
    
    std::unique_ptr<Command> createCommand() override {
        return std::apply([](auto&&... a) {
            return std::make_unique<TCommand>(a...);
        }, args_);
    }
};