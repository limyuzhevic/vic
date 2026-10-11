// NLM Unit Testing Framework
// Motor Tests

#include "framework/TestFramework.hpp"
#include "brain/MotorSystem.hpp"
#include "brain/MotorCommand.hpp"
#include <cassert>
#include <cmath>

class TestMotor : public TestCase {
public:
    TestMotor() : TestCase("Motor") {}
    
    void testMotorCommandGeneration() {
        MotorSystem motor;
        
        // Generate motor command for reaching movement
        std::vector<double> target_position = {1.0, 0.5, 0.2}; // XYZ
        std::vector<double> current_position = {0.0, 0.0, 0.0};
        
        MotorCommand command = motor.generateCommand(
            target_position, current_position, MotorType::REACHING);
        
        assert(command.magnitude > 0.0);
        assert(command.direction.size() == 3);
        assert(std::abs(command.magnitude - 0.87) < 0.1); // Distance to target
    }
    
    void testMotorLearning() {
        MotorSystem motor;
        
        // Test motor skill learning through reinforcement
        MotorCommand initial_command = motor.generateCommand({1.0}, {0.0});
        double initial_error = motor.calculateMovementError(initial_command, {1.0});
        
        // Learn from error
        motor.learnFromError(initial_command, initial_error, 0.1);
        
        // Generate new command after learning
        MotorCommand learned_command = motor.generateCommand({1.0}, {0.0});
        double improved_error = motor.calculateMovementError(learned_command, {1.0});
        
        assert(improved_error < initial_error); // Should improve
    }
    
    void testMotorControl() {
        MotorSystem motor;
        
        // Test feedforward control
        FeedForwardController& ff_controller = motor.getFeedForwardController();
        std::vector<double> desired_velocity = {0.5, 0.3, 0.2};
        
        MotorCommand command = ff_controller.generateCommand(desired_velocity);
        assert(command.magnitude > 0.0);
        
        // Test feedback control
        std::vector<double> actual_velocity = {0.4, 0.25, 0.18};
        std::vector<double> error = motor.calculateError(desired_velocity, actual_velocity);
        
        assert(error[0] < desired_velocity[0]); // Error should be small
        assert(std::all_of(error.begin(), error.end(), 
                          [](double e) { return std::abs(e) < 0.2; }));
    }
    
    void testMotorCoordination() {
        MotorSystem motor;
        
        // Test multi-joint coordination
        std::vector<double> joint_positions = {0.0, 0.5, 1.0, 0.8, 0.3};
        std::vector<double> target_positions = {0.2, 0.6, 1.2, 0.9, 0.4};
        
        std::vector<MotorCommand> commands = motor.coordinateMovement(joint_positions, target_positions);
        
        assert(commands.size() == joint_positions.size());
        
        for (size_t i = 0; i < commands.size(); ++i) {
            assert(commands[i].magnitude > 0.0);
            assert(commands[i].joint_id == i); // Each command should target correct joint
        }
    }
    
    void testMotorFatigue() {
        MotorSystem motor;
        
        // Test motor fatigue accumulation
        double initial_fatigue = motor.getFatigueLevel();
        
        // Perform intensive movement
        for (int i = 0; i < 100; ++i) {
            MotorCommand cmd = motor.generateCommand({0.1}, {0.0});
            motor.applyCommand(cmd);
        }
        
        double fatigue_after = motor.getFatigueLevel();
        assert(fatigue_after > initial_fatigue);
        
        // Test fatigue recovery
        motor.recover(0.5); // 50% recovery
        double fatigue_recovered = motor.getFatigueLevel();
        assert(fatigue_recovered < fatigue_after);
        assert(fatigue_recovered > initial_fatigue);
    }
};