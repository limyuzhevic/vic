import pynlm

def test_agent_body():
    """Test AgentBody class functionality."""
    
    print("Testing AgentBody class...")
    
    body = pynlm.AgentBody()
    
    # Test default values
    assert body.x == 0.0, "Default x should be 0.0"
    assert body.y == 0.0, "Default y should be 0.0"
    assert body.orientation == 0.0, "Default orientation should be 0.0"
    assert body.velocityX == 0.0, "Default velocityX should be 0.0"
    assert body.velocityY == 0.0, "Default velocityY should be 0.0"
    assert body.energy == 1.0, "Default energy should be 1.0"
    assert body.health == 1.0, "Default health should be 1.0"
    assert body.age == 0.0, "Default age should be 0.0"
    
    # Test reset method
    body.x = 10.0
    body.y = 10.0
    body.energy = 0.5
    body.reset()
    assert body.x == 0.0, "Reset x should be 0.0"
    assert body.y == 0.0, "Reset y should be 0.0"
    assert body.energy == 1.0, "Reset energy should be 1.0"
    
    print("✓ AgentBody tests passed")
    return body

if __name__ == "__main__":
    test_agent_body()