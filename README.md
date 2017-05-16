# glhello

This framework was written for fun and to explore computer graphics and was later used in a bachelor thesis.

## Description

The framework supports basic 3D models consisting of a set of vertices. A vertex has a number of vertex attributes. At least, each vertex must have a position in 3-space. Each model is automatically an instance of itself. More instances that use the same set of vertices can be created from a model. An instance, just as a vertex, also has a number of attributes. At least, each instance has a model matrix. The model matrix encodes the translation, rotation and scale of the model instance in the world. All this data, vertex attributes and instance attributes, can be loaded to the graphics card and with only one draw command many instances of a model (with varying positions, scales, rotations and arbitrary other instance attributes) are rendered to the world.

Vertex attributes and instance attributes are processed by shader programs running on the graphics card. A shader can run on all models of the same type, where the modeltype means the vertex attributes and instance attributes that the model provides. The framework supports grouping models into modeltypes for which a specific shader is used to render them. When rendering a set of vertices the graphics card also has to know how these vertices are connected. Mostly the vertices are meant to form triangular faces, but they can also be rendered as points or lines. In the framework, the drawing primitive is part of the modeltype.

Models belong to a world and the world draws its models. All models of one modeltype are loaded to one big buffer on the graphics card. A draw command is made for each chunk of the buffer, where a model with all its vertex attributes and instance attributes resides. A new buffer is only used for each modeltype. Another shader is only used when modeltypes have different shaders. When the world has drawn all its models, a new frame is finished.

For purposes like simulating a camera or lighting more data is required in shaders. The framework supports the management of additional shader data through uniform variables, uniform buffers and shader storage blocks. The word uniform stands for read-only in shaders, but shader storage blocks can also be written. Uniform variables represent a single value (integer, floating point number, vector or matrix) and are updated on each draw call. Uniform buffers and shader storage blocks represent arbitrary data and are only updated on use of another shader or, if only one shader is used, on each frame. Uniform buffers have a fixed size whereas shader storage blocks can change their size in each frame and are only limited by graphics card memory. A simulated camera can use the framework to load its matrices into a uniform buffer. A lighting simulation can use the framework to dynamically add light sources into a shader storage block.

A module in the framework is an extension of the world. A module provides a set of shader data. The world makes sure that this data is delivered to all contained shaders that make use of this data in their program code. Furthermore a module can define preceding renderpasses. Through this, the framework can be extended with rendering techniques using additional images of the world to provide more information to the final rendering (such a technique is shadow mapping). Finally a module can be a receiver of interactions from a user.

A user in the framework represents someone who interacts with the world. He sends interactions made with input devices to modules. A module has its corresponding interaction type. The interaction type defines what some specific interaction (like a key press or mouse move) means to the module. When a module receives an interaction it updates the shader data provided by it.

### Camera module

The camera module provides a view matrix and a projection matrix as uniform buffer. The view matrix and projection matrix encode frustum-shaped part of the world to be seen through the camera. The camera defines no additional renderpasses. On interaction with the WASD keys it translates the view. Currently the camera supports no more complex interactions than moving around stepwise. The camera can shoot a world. The resulting frame is either directly rendered to the camera's viewport or sent through the camera's post-processor. The post-processor edits the image of the world seen through the camera. For post-processing a special shader is used that reads from an input-image with the size of the viewport and writes to an output-image with the same size.

### Light module

### Shadow mapping module

### Volumetric light scattering modules

## Screenshots

## Dependencies and build
