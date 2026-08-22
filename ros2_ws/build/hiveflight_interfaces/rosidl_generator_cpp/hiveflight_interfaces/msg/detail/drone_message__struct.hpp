// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from hiveflight_interfaces:msg/DroneMessage.idl
// generated code does not contain a copyright notice

#ifndef HIVEFLIGHT_INTERFACES__MSG__DETAIL__DRONE_MESSAGE__STRUCT_HPP_
#define HIVEFLIGHT_INTERFACES__MSG__DETAIL__DRONE_MESSAGE__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'position'
// Member 'velocity'
#include "geometry_msgs/msg/detail/vector3__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__hiveflight_interfaces__msg__DroneMessage __attribute__((deprecated))
#else
# define DEPRECATED__hiveflight_interfaces__msg__DroneMessage __declspec(deprecated)
#endif

namespace hiveflight_interfaces
{

namespace msg
{

// message struct
template<class ContainerAllocator>
struct DroneMessage_
{
  using Type = DroneMessage_<ContainerAllocator>;

  explicit DroneMessage_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : position(_init),
    velocity(_init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->sender_id = 0l;
      this->target_id = 0l;
      this->timestamp = 0.0f;
    }
  }

  explicit DroneMessage_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : position(_alloc, _init),
    velocity(_alloc, _init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->sender_id = 0l;
      this->target_id = 0l;
      this->timestamp = 0.0f;
    }
  }

  // field types and members
  using _sender_id_type =
    int32_t;
  _sender_id_type sender_id;
  using _position_type =
    geometry_msgs::msg::Vector3_<ContainerAllocator>;
  _position_type position;
  using _velocity_type =
    geometry_msgs::msg::Vector3_<ContainerAllocator>;
  _velocity_type velocity;
  using _target_id_type =
    int32_t;
  _target_id_type target_id;
  using _timestamp_type =
    float;
  _timestamp_type timestamp;

  // setters for named parameter idiom
  Type & set__sender_id(
    const int32_t & _arg)
  {
    this->sender_id = _arg;
    return *this;
  }
  Type & set__position(
    const geometry_msgs::msg::Vector3_<ContainerAllocator> & _arg)
  {
    this->position = _arg;
    return *this;
  }
  Type & set__velocity(
    const geometry_msgs::msg::Vector3_<ContainerAllocator> & _arg)
  {
    this->velocity = _arg;
    return *this;
  }
  Type & set__target_id(
    const int32_t & _arg)
  {
    this->target_id = _arg;
    return *this;
  }
  Type & set__timestamp(
    const float & _arg)
  {
    this->timestamp = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    hiveflight_interfaces::msg::DroneMessage_<ContainerAllocator> *;
  using ConstRawPtr =
    const hiveflight_interfaces::msg::DroneMessage_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<hiveflight_interfaces::msg::DroneMessage_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<hiveflight_interfaces::msg::DroneMessage_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      hiveflight_interfaces::msg::DroneMessage_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<hiveflight_interfaces::msg::DroneMessage_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      hiveflight_interfaces::msg::DroneMessage_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<hiveflight_interfaces::msg::DroneMessage_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<hiveflight_interfaces::msg::DroneMessage_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<hiveflight_interfaces::msg::DroneMessage_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__hiveflight_interfaces__msg__DroneMessage
    std::shared_ptr<hiveflight_interfaces::msg::DroneMessage_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__hiveflight_interfaces__msg__DroneMessage
    std::shared_ptr<hiveflight_interfaces::msg::DroneMessage_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const DroneMessage_ & other) const
  {
    if (this->sender_id != other.sender_id) {
      return false;
    }
    if (this->position != other.position) {
      return false;
    }
    if (this->velocity != other.velocity) {
      return false;
    }
    if (this->target_id != other.target_id) {
      return false;
    }
    if (this->timestamp != other.timestamp) {
      return false;
    }
    return true;
  }
  bool operator!=(const DroneMessage_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct DroneMessage_

// alias to use template instance with default allocator
using DroneMessage =
  hiveflight_interfaces::msg::DroneMessage_<std::allocator<void>>;

// constant definitions

}  // namespace msg

}  // namespace hiveflight_interfaces

#endif  // HIVEFLIGHT_INTERFACES__MSG__DETAIL__DRONE_MESSAGE__STRUCT_HPP_
