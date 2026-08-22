// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from hiveflight_interfaces:msg/DroneMessage.idl
// generated code does not contain a copyright notice

#ifndef HIVEFLIGHT_INTERFACES__MSG__DETAIL__DRONE_MESSAGE__STRUCT_H_
#define HIVEFLIGHT_INTERFACES__MSG__DETAIL__DRONE_MESSAGE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

// Include directives for member types
// Member 'position'
// Member 'velocity'
#include "geometry_msgs/msg/detail/vector3__struct.h"

/// Struct defined in msg/DroneMessage in the package hiveflight_interfaces.
typedef struct hiveflight_interfaces__msg__DroneMessage
{
  int32_t sender_id;
  geometry_msgs__msg__Vector3 position;
  geometry_msgs__msg__Vector3 velocity;
  int32_t target_id;
  float timestamp;
} hiveflight_interfaces__msg__DroneMessage;

// Struct for a sequence of hiveflight_interfaces__msg__DroneMessage.
typedef struct hiveflight_interfaces__msg__DroneMessage__Sequence
{
  hiveflight_interfaces__msg__DroneMessage * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} hiveflight_interfaces__msg__DroneMessage__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // HIVEFLIGHT_INTERFACES__MSG__DETAIL__DRONE_MESSAGE__STRUCT_H_
