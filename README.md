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

The camera module provides a view matrix and a projection matrix as uniform buffer. The view matrix and projection matrix encode frustum-shaped part of the world to be seen through the camera. On interaction with the WASD keys it translates the view. Currently the camera supports no more complex interactions than moving around stepwise. The camera can shoot a world. The resulting frame is either directly rendered to the camera's viewport or sent through the camera's post-processor. The post-processor edits the image of the world seen through the camera. For post-processing a special shader is used that reads from an input-image with the size of the viewport and writes to an output-image with the same size.

### Light module

The light module provides a list of light sources as shader storage block as well as the current number of light sources. The number and properties of light sources can change dynamically at runtime. Aside from its position, a single light source is made up of 3 properties: 1st, the dominant wavelength, which is the one color that is most intense; 2nd, monochromaticity, which is how dominant the given wavelength is (zero means all wavelengths are present which results in white light); 3rd, amplitude, which is the overall intensity of the light source. When a world is illuminated by light, the light interacts with the models in the world. Models have a material. The final color depends on light interacting with material.

Materials in the framework are instance attributes. This means, individual model instances can have different material appearances. A material has the 4 properties absorption, reflection, transmission and shininess. They define which colors of light are how much absorped, reflected and transmitted. The shininess defines how sleek the material is.

The light module furthermore provides emitters. An emitter is an instance attribute that can make an instance of a model emit light. It works by providing an index per instance in shaders, that points into the list of light sources. The emitting model instance appears in the color of the pointed light source and illuminates the world around it. The light source is positioned in the center of the model instance. Currently light sources covering an area are not supported.

### Shadow mapping module

This module can render shadows to the world. Shadows are espescially difficult because they contain global information about the world. Far away models can cast shadows on other on many other models. However when rendering on graphics cards there is no global information. Shadow mapping determines if a point is in shadow, by looking at it from the light source's point of view. If the point isn't visible, the point is in shadow. The module provides a view matrix for a light source and a shadow map for looking up visibility. The shadow map is an image of the world from the light's point of view. The shadow mapping module defines one preceeding render pass where the shadow map is generated.

The module currently implements a basic form of shadow mapping. There are several limiations. First, it generates a shadow map only for one light source. Second, the shadow map is captures only one frustum and therefore is not fully suited for omnidirectional light sources. Third, no techniques are applied to account for cases in which the resolution of the shadow map is not sufficient; espescially no cascaded shadow maps and no sophisticated filtering is currently implemented.

### Volumetric light scattering modules

The framework contains 3 techniques for rendering volumetric light scattering or god rays which were used in the [thesis](../master/thesis.pdf). The techniques were taken from [Mitchell](https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch13.html), [Sousa](http://www.gdcvault.com/play/247/CRYSIS-Next-Gen) and [Tóth and Umenhoffer](http://sirkan.iit.bme.hu/~szirmay/lightshaft_link.htm) and implemented as modules of the framework.



## Screenshots

## Dependencies and build
