enumerations:
  Lav_ERRORS:
    doc_description: |
      All functions in this library return one of the following enum values, indicating their error condition.
    members:
      Lav_ERROR_NONE: No error occured.
      Lav_ERROR_UNKNOWN: Something went wrong.  This error indicates that we couldn't figure out what.
      Lav_ERROR_TYPE_MISMATCH: Indicates an attempt to manipulate a property through a function that does not work with that property's type.
      Lav_ERROR_INVALID_PROPERTY: An attempt to access a property which does not exist on the specified node.
      Lav_ERROR_NULL_POINTER: You passed a null pointer into Libaudioverse in a context where null pointers are not allowed.
      Lav_ERROR_MEMORY: Libaudioverse triedd to allocate a pointer, but could not.
      Lav_ERROR_INVALID_HANDLE: A value passed in as a handle is not currently a handle which is valid.
      Lav_ERROR_RANGE: A function parameter is not within a valid range.  This could be setting property values outside their range, accessing inputs and outputs that do not exist, or any of a variety of other range error conditions.
      Lav_ERROR_CANNOT_INIT_AUDIO:  The audio subsystem could not be initialized.
      Lav_ERROR_FILE: Represents a miscelaneous file error.
      Lav_ERROR_FILE_NOT_FOUND: Libaudioverse could not find a specified file.
      Lav_ERROR_HRTF_INVALID: An attempt to use an invalid HRTF database.
      Lav_ERROR_CANNOT_CROSS_SIMULATIONS: An attempt was made to relate two objects from different simulations. This could be assigning to buffer properties, connecting nodes, or any other such condition.
      Lav_ERROR_CAUSES_CYCLE: The requested operation would cause a cycle in the graph of nodes that need processing.
      Lav_ERROR_PROPERTY_IS_READ_ONLY: Attempt to set a read-only property.
      Lav_ERROR_OVERLAPPING_AUTOMATORS: An attempt to schedule an automator within the duration of another.
      Lav_ERROR_CANNOT_CONNECT_TO_PROPERTY: Attempt to connect a node to a property which cannot be automated.
      Lav_ERROR_INTERNAL: If you see this error, it's a bug.
  Lav_PROPERTY_TYPES:
    doc_description: |
      Indicates the type of a property.
    members:
      Lav_PROPERTYTYPE_INT: Property holds a 32-bit integer.
      Lav_PROPERTYTYPE_FLOAT: Property holds a 32-bit floating point value.
      Lav_PROPERTYTYPE_DOUBLE: Property holds a 64-bit double.
      Lav_PROPERTYTYPE_STRING: Property holds a string.
      Lav_PROPERTYTYPE_FLOAT3: Property holds a float3, a vector of 3 floats.
      Lav_PROPERTYTYPE_FLOAT6: Property holds a float6, a vector of 6 floats.
      Lav_PROPERTYTYPE_FLOAT_ARRAY: Property is an array of floats.
      Lav_PROPERTYTYPE_INT_ARRAY: Property is an array of ints.
      Lav_PROPERTYTYPE_BUFFER: Property holds a handle to a buffer.
  Lav_NODE_STATES:
    doc_description: |
      used to indicate the state of a node.
      This is the value of the node's state property and determins how the node is processed.
    members:
      Lav_NODESTATE_PAUSED: This node is paused.
      Lav_NODESTATE_PLAYING: This node advances if other nodes need audio from it.
      Lav_NODESTATE_ALWAYS_PLAYING: This node advances always.
  Lav_LOGGING_LEVELS:
    doc_description: |
      Possible levels for logging.
    members:
      Lav_LOG_LEVEL_OFF: No log messages will be generated.
      Lav_LOG_LEVEL_CRITICAL: Logs critical messages such as failures to initialize and error conditions.
      Lav_LOG_LEVEL_INFO: Logs informative messages.
      Lav_LOG_LEVEL_DEBUG: Logs everything possible.