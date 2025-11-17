# Squirrel API

## Classes

### String {#string}
A wrapper for `std::string` so that C++ can easily manipulate with strings.
#### Functions
- `String(string value)` - constructor
- `c_str()` - returns Squirrels string

### Vec3 {#vec3}
A wrapper for `glm::vec3`.
#### Functions
- `Vec3(float x, float y, float z)` - constructor
- `x()` - get or set the X value
- `y()` - get or set the Y value
- `z()` - get or set the Z value

### Vec4
A wrapper for `glm::vec4`/`glm::quat`.
#### Functions
- `Vec4(float x, float y, float z, float w)` - constructor
- `x()` - get or set the X value
- `y()` - get or set the Y value
- `z()` - get or set the Z value
- `w()` - get or set the W value

### Parameters {#parameters}
A wrapper for `hl::radix_tree<hl::hson::parameter>`.
#### Functions
- `GetParameterFloat(string name)` - get the value of a float parameter based on name
- `GetParameterInt(string name)` - get the value of a int parameter based on name
- `GetParameterUInt(string name)` - get the value of a uint parameter based on name
- `GetParameterString(string name)` - get the value of a string parameter based on name (returns the custom [String](#string) class)
- `GetParameterArray(string name)` - get an array of values of a parameter based on name
- `GetParameterObject(string name)` - get the value of an object parameter based on name (returns the custom [Parameters](#parameters) class)

### Object {#object}
A wrapper for `ulvl::app::ObjectService::Object`.
#### Functions
- `position()` - get the world position of the object (returns the custom [Vec3](#vec3) class)
- `localPosition()` - get the local position of the object (returns the custom [Vec3](#vec3) class)
- `name()` - get the name of the object (returns the custom [String](#string) class)
- `parameters()` - get the parameters of the object (returns the custom [Parameters](#parameters) class)
- `parent()` - get the parent object of the object (returns the custom [Object](#object) class)
- `id()` - get the object's guid of the object (returns string)
- `HasParent()` - returns true or false whether the object has a parent

### ModelData {#modeldata}
A class used for setting the model of an object.
Indices are always u16.
#### Functions
- `GetVertexCount()` - get the set vertex count
- `SetVertexCount(int count)` - set the vertex count
- `GetVertexStride()` - get the set vertex stride
- `SetVertexStride(int stride)` - set the vertex stride
- `SetVertices(void* vertices)` - set the vertices
- `GetIndexCount()` - get the set index count
- `SetIndexCount(int count)` - set the index count
- `SetIndices(unsigned short* indices)` - set the indices

### DebugVisual {#debugvisual}
A class used for adding debug visuals to the scene.
#### Functions
- `DrawCube(Vec4 color, Vec3 position, Vec4 rotation, Vec3 scale, Object obj)` - adds a cube debug visual

## Global Functions
- `GetObjByID(string guid)` - get an object by its guid (returns the custom [Object](#object) class)
