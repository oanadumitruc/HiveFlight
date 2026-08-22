// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from hiveflight_interfaces:msg/DroneMessage.idl
// generated code does not contain a copyright notice
#include "hiveflight_interfaces/msg/detail/drone_message__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `position`
// Member `velocity`
#include "geometry_msgs/msg/detail/vector3__functions.h"

bool
hiveflight_interfaces__msg__DroneMessage__init(hiveflight_interfaces__msg__DroneMessage * msg)
{
  if (!msg) {
    return false;
  }
  // sender_id
  // position
  if (!geometry_msgs__msg__Vector3__init(&msg->position)) {
    hiveflight_interfaces__msg__DroneMessage__fini(msg);
    return false;
  }
  // velocity
  if (!geometry_msgs__msg__Vector3__init(&msg->velocity)) {
    hiveflight_interfaces__msg__DroneMessage__fini(msg);
    return false;
  }
  // target_id
  // timestamp
  return true;
}

void
hiveflight_interfaces__msg__DroneMessage__fini(hiveflight_interfaces__msg__DroneMessage * msg)
{
  if (!msg) {
    return;
  }
  // sender_id
  // position
  geometry_msgs__msg__Vector3__fini(&msg->position);
  // velocity
  geometry_msgs__msg__Vector3__fini(&msg->velocity);
  // target_id
  // timestamp
}

bool
hiveflight_interfaces__msg__DroneMessage__are_equal(const hiveflight_interfaces__msg__DroneMessage * lhs, const hiveflight_interfaces__msg__DroneMessage * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // sender_id
  if (lhs->sender_id != rhs->sender_id) {
    return false;
  }
  // position
  if (!geometry_msgs__msg__Vector3__are_equal(
      &(lhs->position), &(rhs->position)))
  {
    return false;
  }
  // velocity
  if (!geometry_msgs__msg__Vector3__are_equal(
      &(lhs->velocity), &(rhs->velocity)))
  {
    return false;
  }
  // target_id
  if (lhs->target_id != rhs->target_id) {
    return false;
  }
  // timestamp
  if (lhs->timestamp != rhs->timestamp) {
    return false;
  }
  return true;
}

bool
hiveflight_interfaces__msg__DroneMessage__copy(
  const hiveflight_interfaces__msg__DroneMessage * input,
  hiveflight_interfaces__msg__DroneMessage * output)
{
  if (!input || !output) {
    return false;
  }
  // sender_id
  output->sender_id = input->sender_id;
  // position
  if (!geometry_msgs__msg__Vector3__copy(
      &(input->position), &(output->position)))
  {
    return false;
  }
  // velocity
  if (!geometry_msgs__msg__Vector3__copy(
      &(input->velocity), &(output->velocity)))
  {
    return false;
  }
  // target_id
  output->target_id = input->target_id;
  // timestamp
  output->timestamp = input->timestamp;
  return true;
}

hiveflight_interfaces__msg__DroneMessage *
hiveflight_interfaces__msg__DroneMessage__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  hiveflight_interfaces__msg__DroneMessage * msg = (hiveflight_interfaces__msg__DroneMessage *)allocator.allocate(sizeof(hiveflight_interfaces__msg__DroneMessage), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(hiveflight_interfaces__msg__DroneMessage));
  bool success = hiveflight_interfaces__msg__DroneMessage__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
hiveflight_interfaces__msg__DroneMessage__destroy(hiveflight_interfaces__msg__DroneMessage * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    hiveflight_interfaces__msg__DroneMessage__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
hiveflight_interfaces__msg__DroneMessage__Sequence__init(hiveflight_interfaces__msg__DroneMessage__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  hiveflight_interfaces__msg__DroneMessage * data = NULL;

  if (size) {
    data = (hiveflight_interfaces__msg__DroneMessage *)allocator.zero_allocate(size, sizeof(hiveflight_interfaces__msg__DroneMessage), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = hiveflight_interfaces__msg__DroneMessage__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        hiveflight_interfaces__msg__DroneMessage__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
hiveflight_interfaces__msg__DroneMessage__Sequence__fini(hiveflight_interfaces__msg__DroneMessage__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      hiveflight_interfaces__msg__DroneMessage__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

hiveflight_interfaces__msg__DroneMessage__Sequence *
hiveflight_interfaces__msg__DroneMessage__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  hiveflight_interfaces__msg__DroneMessage__Sequence * array = (hiveflight_interfaces__msg__DroneMessage__Sequence *)allocator.allocate(sizeof(hiveflight_interfaces__msg__DroneMessage__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = hiveflight_interfaces__msg__DroneMessage__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
hiveflight_interfaces__msg__DroneMessage__Sequence__destroy(hiveflight_interfaces__msg__DroneMessage__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    hiveflight_interfaces__msg__DroneMessage__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
hiveflight_interfaces__msg__DroneMessage__Sequence__are_equal(const hiveflight_interfaces__msg__DroneMessage__Sequence * lhs, const hiveflight_interfaces__msg__DroneMessage__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!hiveflight_interfaces__msg__DroneMessage__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
hiveflight_interfaces__msg__DroneMessage__Sequence__copy(
  const hiveflight_interfaces__msg__DroneMessage__Sequence * input,
  hiveflight_interfaces__msg__DroneMessage__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(hiveflight_interfaces__msg__DroneMessage);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    hiveflight_interfaces__msg__DroneMessage * data =
      (hiveflight_interfaces__msg__DroneMessage *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!hiveflight_interfaces__msg__DroneMessage__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          hiveflight_interfaces__msg__DroneMessage__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!hiveflight_interfaces__msg__DroneMessage__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
