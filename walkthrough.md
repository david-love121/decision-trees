# Visualizer Walkthrough

I have created a new Qt6-based visualizer for the Binary Decision Tree.

## Features
- **Visualizes the Tree Structure**: Draws nodes and edges using `QGraphicsScene`.
- **Node Information**: Displays Feature Index, Classifier Value, and Impurity for each node.
- **Interactive Simulation**: "Run Random Example" button picks a random sample from the Iris dataset.
- **Animation**: Animates the traversal of the sample through the tree, highlighting the current node in red.

## How to Run
You can run the visualizer using Bazel:

```bash
bazel run //visualizer:visualizer
```

## Implementation Details
- **Module**: `visualizer`
- **Main Classes**:
    - `MainWindow`: Main application window with the scene view and controls.
    - `TreeScene`: Handles the drawing of the tree and the animation logic.
- **Integration**:
    - Modified `Node` class to expose child nodes (`getLeftChild`, `getRightChild`).
    - Modified `DecisionTree` to expose the head node.
    - Updated `BUILD` files to export necessary headers and data.

## Dependencies
- Qt6 (Widgets, Gui, Core)
- OpenGL
