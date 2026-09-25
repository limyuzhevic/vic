# NLM Documentation Templates

## Overview

This document provides templates for consistent documentation across the NLM codebase. These templates ensure comprehensive and standardized documentation following Doxygen standards for C++ and Google/NumPy style for Python.

## C++ Documentation Templates

### File-Level Documentation Template

```cpp
/**
 * @file <filename>
 * 
 * <Purpose and scope of the file>
 * 
 * Key features and components implemented in this file:
 * - Feature 1
 * - Feature 2
 * - Feature 3
 * 
 * Dependencies and relationships:
 * - Depends on: <list of dependent files>
 * - Provides: <list of interfaces/classes provided>
 * 
 * @note <Additional important notes about this file>
 * @see <Related files or documentation>
 */
```

### Class Documentation Template

```cpp
/**
 * @class <ClassName>
 * 
 * <Description of the class purpose and scope>
 * 
 * <Detailed explanation of the class functionality, including>
 * <key features, design rationale, and usage patterns.>
 * 
 * <Class is part of:><br>
 * - System 1<br>
 * - Sub-system 2<br>
 * 
 * <Inheritance hierarchy:><br>
 * - <BaseClass1><br>
 * - <BaseClass2><br>
 * 
 * <Key responsibilities:>
 * 1. Responsibility 1
 * 2. Responsibility 2
 * 3. Responsibility 3
 * 
 * <Typical usage:><br>
 * @code
 * <Usage example>
 * @endcode
 * 
 * <Performance characteristics:>
 * - Time complexity: O(n) or constant
 * - Memory usage: Low/Medium/High
 * - Thread safety: Yes/No
 * 
 * <Dependencies:><br>
 * - <Dependency1><br>
 * - <Dependency2><br>
 * 
 * @see <Related classes or documentation>
 */
```

### Method Documentation Template

```cpp
/**
 * @brief <Brief description of the method functionality>
 * 
 * <Detailed explanation of what the method does, including>
 * <algorithm steps, preconditions, and postconditions.>
 * 
 * <Purpose:>
 * <Explain the method's role in the system.>
 * 
 * <Parameters:>
 * @param <param1> Description of parameter 1, including type and constraints
 * @param <param2> Description of parameter 2, units if applicable
 * @param ... Additional parameters
 * 
 * <Returns:>
 * @return <ReturnType> Description of what the method returns
 * 
 * <Throws:>
 * @throw <ExceptionType> Description of error conditions
 * 
 * <Preconditions:>
 * - <Condition 1>
 * - <Condition 2>
 * 
 * <Postconditions:>
 * - <Result 1>
 * - <Result 2>
 * 
 * <Performance:>
 * - Time complexity: O(n log n) or constant
 * - Space complexity: O(1) or O(n)
 * 
 * <Example:><br>
 * @code
 * <Example code>
 * @endcode
 * 
 * <See also:>
 * @see <Related methods>
 * @see <Related documentation>
 * 
 * @note <Important notes, warnings, or special considerations>
 */
```

### Algorithm Documentation Template

```cpp
/**
 * @brief <Algorithm name> - <Short description>
 * 
 * <Detailed explanation of the algorithm, including:
 * - Problem being solved
 * - High-level approach
 * - Key steps and their purpose
 * - Time and space complexity
 * - Convergence properties (for iterative algorithms)
 * 
 * <Mathematical formulation (if applicable)>:>
 * <Mathematical model and equations>
 * 
 * <Parameters:>
 * @param <param1> Description
 * @param ... Additional parameters
 * 
 * <Returns:>
 * @return <Description of output>
 * 
 * <References:>
 * @cite <Reference 1>
 * @cite <Reference 2>
 * 
 * @note <Important implementation notes>
 */
```

## Python Documentation Templates

### Function Documentation Template

```python
def function_name(param1: <type>, param2: <type> = <default>) -> <return_type>:
    """<Brief one-line description>
    
    <Detailed description of what the function does, including>
    <parameters, return values, and important notes.>
    
    Parameters
    ----------
    param1 : <type>
        Description of param1, including units and constraints
    param2 : <type>, optional
        Description of param2, default value if applicable
    ... Additional parameters
    
    Returns
    -------
    <return_type>
        Description of what the function returns
    
    Raises
    ------
    <ExceptionType>
        Description of error conditions
    
    Examples
    --------
    >>> # Example usage
    >>> result = function_name(value1, value2)
    >>> print(result)
    
    Notes
    -----
    <Important notes, warnings, or special considerations>
    
    See Also
    --------
    <Related functions>
    """
```

## System Architecture Documentation Template

```markdown
# System Architecture Documentation

## Overview

<High-level description of the system architecture>

## Components

### <Component Name>
- **Purpose**: <What the component does>
- **Key Responsibilities**: <List of main functions>
- **Interfaces**: <External interfaces/methods>
- **Dependencies**: <What it depends on>
- **Performance**: <Key performance characteristics>

### <Another Component>
[...]

## Data Flow

<Diagram or textual description of data flow between components>

## Configuration

<Configuration parameters and their purposes>

## Deployment

<How to deploy and run the system>
```

## Algorithm Documentation Template

```markdown
# Algorithm Name

## Description
<High-level problem statement and solution approach>

## Mathematical Model
<Mathematical formulation, equations, and parameters>

## Algorithm Steps
1. <Step 1 description>
2. <Step 2 description>
... Continue with numbered steps

## Complexity Analysis
- **Time Complexity**: O(n log n)
- **Space Complexity**: O(n)
- **Convergence**: Guaranteed/Diverges under certain conditions

## Parameters
| Parameter | Type | Default | Range | Description |
|-----------|------|---------|-------|-------------|
| param1 | float | 0.0 | [0,1] | Description |

## Implementation Notes
<Important implementation details and optimizations>

## References
- <Reference 1>
- <Reference 2>
```

## Usage Examples Template

```markdown
# Usage Examples

## <Example Name>

### Purpose
<What this example demonstrates>

### Prerequisites
- <List of prerequisites>

### Code Example
```<language>
<code example>
```

### Expected Output
<Description of expected output>

### Notes
<Important notes or considerations>
```

## Design Patterns Documentation Template

```markdown
# Design Patterns Used

## <Pattern Name>
### Purpose
<What problem this pattern solves>

### When to Use
<When this pattern is appropriate>

### Implementation in NLM
<How this pattern is implemented in the NLM codebase>

### Examples
<Code examples showing the pattern usage>

### Benefits
<Advantages of using this pattern>

### Drawbacks
<Potential disadvantages>
```

## Common Documentation Patterns

### Error Handling Documentation
```cpp
/**
 * @throw <ExceptionType>
 * @exception <ExceptionType> Description of when this exception is thrown
 * @warning <Warning text about error conditions>
 */
```

### Performance Documentation
```cpp
/**
 * @complexity <Time complexity> time, <Space complexity> space
 * @performance <Performance characteristics and optimizations>
 * @note <Performance considerations and limitations>
 */
```

### Thread Safety Documentation
```cpp
/**
 * @threadsafe <Thread-safe or not thread-safe>
 * @concurrent <Thread-safe implementation details>
 */
```

## Template Usage Guidelines

1. **Consistency**: Use the same template structure across all files
2. **Completeness**: Include all required sections, even if brief
3. **Accuracy**: Ensure documentation matches the actual implementation
4. **Updates**: Update documentation when implementation changes
5. **Reviews**: Have peer reviews for documentation accuracy

## Template Maintenance

- Review and update templates regularly
- Add templates for new patterns or documentation needs
- Create documentation style guide based on these templates
- Use templates as starting point for documentation creation

This template system provides a foundation for creating comprehensive, consistent documentation across the entire NLM codebase.