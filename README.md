# glhello

This framework was written for fun and to explore computer graphics and was later used in a bachelor thesis.

## Description

### Rendering

The framework supports basic 3D models consisting of a set of vertices. A vertex has a number of vertex attributes. At least, each vertex must have a position in 3-space. Each model is automatically an instance of itself. More instances that use the same set of vertices can be created from a model. An instance, just as a vertex, also has a number of attributes. At least, each instance has a model matrix. The model matrix encodes the translation, rotation and scale of the model instance in the world. All this data, vertex attributes and instance attributes, can be loaded to the graphics card and with only one draw command many instances of a model (with varying positions, scales, rotations and arbitrary other instance attributes) are rendered to the world.

Vertex attributes and instance attributes are processed by shader programs running on the graphics card. A shader can run on all models of the same type, where the modeltype means the vertex attributes and instance attributes that the model provides. The framework supports grouping models into modeltypes for which a specific shader is used to render them. When rendering a set of vertices the graphics card also has to know how these vertices are connected. Mostly the vertices are meant to form triangular faces, but they can also be rendered as points or lines. In the framework, the drawing primitive is part of the modeltype.

Models belong to a world and the world draws its models. All models of one modeltype are loaded to one big buffer on the graphics card. A draw command is made for each chunk of the buffer, where a model with all its vertex attributes and instance attributes resides. A new buffer is only used for each modeltype. Another shader is only used when modeltypes have different shaders. When the world has drawn all its models, a new frame is finished.

For purposes like simulating a camera or lighting more data is required in shaders. The framework supports the management of additional shader data through uniform variables, uniform buffers and shader storage blocks. The word uniform stands for read-only in shaders, but shader storage blocks can also be written. Uniform variables represent a single value (integer, floating point number, vector or matrix) and are updated on each draw call. Uniform buffers and shader storage blocks represent arbitrary data and are only updated on use of another shader or, if only one shader is used, on each frame. Uniform buffers have a fixed size whereas shader storage blocks can change their size in each frame and are only limited by graphics card memory. A simulated camera can use the framework to load its matrices into a uniform buffer. A lighting simulation can use the framework to dynamically add light sources into a shader storage block.

### User interaction

### Modules

## Screenshots

## Dependencies and build
