class BrainCommands {
public:
    // Base class for brain commands
    class BrainCommand : public Command {
    public:
        virtual ~BrainCommand() = default;
        
        // Configure command prefix
        static std::string getCommandPrefix() { return "brain"; }
    };
};