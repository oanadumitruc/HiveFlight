// generated from rosidl_typesupport_fastrtps_c/resource/idl__type_support_c.cpp.em
// with input from hiveflight_interfaces:msg/DroneMessage.idl
// generated code does not contain a copyright notice
#include "hiveflight_interfaces/msg/detail/drone_message__rosidl_typesupport_fastrtps_c.h"


#include <cassert>
#include <limits>
#include <string>
#include "rosidl_typesupport_fastrtps_c/identifier.h"
#include "rosidl_typesupport_fastrtps_c/wstring_conversion.hpp"
#include "rosidl_typesupport_fastrtps_cpp/message_type_support.h"
#include "hiveflight_interfaces/msg/rosidl_typesupport_fastrtps_c__visibility_control.h"
#include "hiveflight_interfaces/msg/detail/drone_message__struct.h"
#include "hiveflight_interfaces/msg/detail/drone_message__functions.h"
#include "fastcdr/Cdr.h"

#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-parameter"
# ifdef __clang__
#  pragma clang diagnostic ignored "-Wdeprecated-register"
#  pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
# endif
#endif
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif

// includes and forward declarations of message dependencies and their conversion functions

#if defined(__cplusplus)
extern "C"
{
#endif

#include "geometry_msgs/msg/detail/vector3__functions.h"  // position, velocity

// forward declare type support functions
ROSIDL_TYPESUPPORT_FASTRTPS_C_IMPORT_hiveflight_interfaces
size_t get_serialized_size_geometry_msgs__msg__Vector3(
  const void * untyped_ros_message,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_IMPORT_hiveflight_interfaces
size_t max_serialized_size_geometry_msgs__msg__Vector3(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment);

ROSIDL_TYPESUPPORT_FASTRTPS_C_IMPORT_hiveflight_interfaces
const rosidl_message_type_support_t *
  ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, geometry_msgs, msg, Vector3)();


using _DroneMessage__ros_msg_type = hiveflight_interfaces__msg__DroneMessage;

static bool _DroneMessage__cdr_serialize(
  const void * untyped_ros_message,
  eprosima::fastcdr::Cdr & cdr)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  const _DroneMessage__ros_msg_type * ros_message = static_cast<const _DroneMessage__ros_msg_type *>(untyped_ros_message);
  // Field name: sender_id
  {
    cdr << ros_message->sender_id;
  }

  // Field name: position
  {
    const message_type_support_callbacks_t * callbacks =
      static_cast<const message_type_support_callbacks_t *>(
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(
        rosidl_typesupport_fastrtps_c, geometry_msgs, msg, Vector3
      )()->data);
    if (!callbacks->cdr_serialize(
        &ros_message->position, cdr))
    {
      return false;
    }
  }

  // Field name: velocity
  {
    const message_type_support_callbacks_t * callbacks =
      static_cast<const message_type_support_callbacks_t *>(
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(
        rosidl_typesupport_fastrtps_c, geometry_msgs, msg, Vector3
      )()->data);
    if (!callbacks->cdr_serialize(
        &ros_message->velocity, cdr))
    {
      return false;
    }
  }

  // Field name: target_id
  {
    cdr << ros_message->target_id;
  }

  // Field name: timestamp
  {
    cdr << ros_message->timestamp;
  }

  return true;
}

static bool _DroneMessage__cdr_deserialize(
  eprosima::fastcdr::Cdr & cdr,
  void * untyped_ros_message)
{
  if (!untyped_ros_message) {
    fprintf(stderr, "ros message handle is null\n");
    return false;
  }
  _DroneMessage__ros_msg_type * ros_message = static_cast<_DroneMessage__ros_msg_type *>(untyped_ros_message);
  // Field name: sender_id
  {
    cdr >> ros_message->sender_id;
  }

  // Field name: position
  {
    const message_type_support_callbacks_t * callbacks =
      static_cast<const message_type_support_callbacks_t *>(
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(
        rosidl_typesupport_fastrtps_c, geometry_msgs, msg, Vector3
      )()->data);
    if (!callbacks->cdr_deserialize(
        cdr, &ros_message->position))
    {
      return false;
    }
  }

  // Field name: velocity
  {
    const message_type_support_callbacks_t * callbacks =
      static_cast<const message_type_support_callbacks_t *>(
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(
        rosidl_typesupport_fastrtps_c, geometry_msgs, msg, Vector3
      )()->data);
    if (!callbacks->cdr_deserialize(
        cdr, &ros_message->velocity))
    {
      return false;
    }
  }

  // Field name: target_id
  {
    cdr >> ros_message->target_id;
  }

  // Field name: timestamp
  {
    cdr >> ros_message->timestamp;
  }

  return true;
}  // NOLINT(readability/fn_size)

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_hiveflight_interfaces
size_t get_serialized_size_hiveflight_interfaces__msg__DroneMessage(
  const void * untyped_ros_message,
  size_t current_alignment)
{
  const _DroneMessage__ros_msg_type * ros_message = static_cast<const _DroneMessage__ros_msg_type *>(untyped_ros_message);
  (void)ros_message;
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  (void)padding;
  (void)wchar_size;

  // field.name sender_id
  {
    size_t item_size = sizeof(ros_message->sender_id);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // field.name position

  current_alignment += get_serialized_size_geometry_msgs__msg__Vector3(
    &(ros_message->position), current_alignment);
  // field.name velocity

  current_alignment += get_serialized_size_geometry_msgs__msg__Vector3(
    &(ros_message->velocity), current_alignment);
  // field.name target_id
  {
    size_t item_size = sizeof(ros_message->target_id);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }
  // field.name timestamp
  {
    size_t item_size = sizeof(ros_message->timestamp);
    current_alignment += item_size +
      eprosima::fastcdr::Cdr::alignment(current_alignment, item_size);
  }

  return current_alignment - initial_alignment;
}

static uint32_t _DroneMessage__get_serialized_size(const void * untyped_ros_message)
{
  return static_cast<uint32_t>(
    get_serialized_size_hiveflight_interfaces__msg__DroneMessage(
      untyped_ros_message, 0));
}

ROSIDL_TYPESUPPORT_FASTRTPS_C_PUBLIC_hiveflight_interfaces
size_t max_serialized_size_hiveflight_interfaces__msg__DroneMessage(
  bool & full_bounded,
  bool & is_plain,
  size_t current_alignment)
{
  size_t initial_alignment = current_alignment;

  const size_t padding = 4;
  const size_t wchar_size = 4;
  size_t last_member_size = 0;
  (void)last_member_size;
  (void)padding;
  (void)wchar_size;

  full_bounded = true;
  is_plain = true;

  // member: sender_id
  {
    size_t array_size = 1;

    last_member_size = array_size * sizeof(uint32_t);
    current_alignment += array_size * sizeof(uint32_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint32_t));
  }
  // member: position
  {
    size_t array_size = 1;


    last_member_size = 0;
    for (size_t index = 0; index < array_size; ++index) {
      bool inner_full_bounded;
      bool inner_is_plain;
      size_t inner_size;
      inner_size =
        max_serialized_size_geometry_msgs__msg__Vector3(
        inner_full_bounded, inner_is_plain, current_alignment);
      last_member_size += inner_size;
      current_alignment += inner_size;
      full_bounded &= inner_full_bounded;
      is_plain &= inner_is_plain;
    }
  }
  // member: velocity
  {
    size_t array_size = 1;


    last_member_size = 0;
    for (size_t index = 0; index < array_size; ++index) {
      bool inner_full_bounded;
      bool inner_is_plain;
      size_t inner_size;
      inner_size =
        max_serialized_size_geometry_msgs__msg__Vector3(
        inner_full_bounded, inner_is_plain, current_alignment);
      last_member_size += inner_size;
      current_alignment += inner_size;
      full_bounded &= inner_full_bounded;
      is_plain &= inner_is_plain;
    }
  }
  // member: target_id
  {
    size_t array_size = 1;

    last_member_size = array_size * sizeof(uint32_t);
    current_alignment += array_size * sizeof(uint32_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint32_t));
  }
  // member: timestamp
  {
    size_t array_size = 1;

    last_member_size = array_size * sizeof(uint32_t);
    current_alignment += array_size * sizeof(uint32_t) +
      eprosima::fastcdr::Cdr::alignment(current_alignment, sizeof(uint32_t));
  }

  size_t ret_val = current_alignment - initial_alignment;
  if (is_plain) {
    // All members are plain, and type is not empty.
    // We still need to check that the in-memory alignment
    // is the same as the CDR mandated alignment.
    using DataType = hiveflight_interfaces__msg__DroneMessage;
    is_plain =
      (
      offsetof(DataType, timestamp) +
      last_member_size
      ) == ret_val;
  }

  return ret_val;
}

static size_t _DroneMessage__max_serialized_size(char & bounds_info)
{
  bool full_bounded;
  bool is_plain;
  size_t ret_val;

  ret_val = max_serialized_size_hiveflight_interfaces__msg__DroneMessage(
    full_bounded, is_plain, 0);

  bounds_info =
    is_plain ? ROSIDL_TYPESUPPORT_FASTRTPS_PLAIN_TYPE :
    full_bounded ? ROSIDL_TYPESUPPORT_FASTRTPS_BOUNDED_TYPE : ROSIDL_TYPESUPPORT_FASTRTPS_UNBOUNDED_TYPE;
  return ret_val;
}


static message_type_support_callbacks_t __callbacks_DroneMessage = {
  "hiveflight_interfaces::msg",
  "DroneMessage",
  _DroneMessage__cdr_serialize,
  _DroneMessage__cdr_deserialize,
  _DroneMessage__get_serialized_size,
  _DroneMessage__max_serialized_size
};

static rosidl_message_type_support_t _DroneMessage__type_support = {
  rosidl_typesupport_fastrtps_c__identifier,
  &__callbacks_DroneMessage,
  get_message_typesupport_handle_function,
};

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_c, hiveflight_interfaces, msg, DroneMessage)() {
  return &_DroneMessage__type_support;
}

#if defined(__cplusplus)
}
#endif
