// generated from rosidl_generator_cpp/resource/idl__traits.hpp.em
// with input from hiveflight_interfaces:msg/DroneMessage.idl
// generated code does not contain a copyright notice

#ifndef HIVEFLIGHT_INTERFACES__MSG__DETAIL__DRONE_MESSAGE__TRAITS_HPP_
#define HIVEFLIGHT_INTERFACES__MSG__DETAIL__DRONE_MESSAGE__TRAITS_HPP_

#include <stdint.h>

#include <sstream>
#include <string>
#include <type_traits>

#include "hiveflight_interfaces/msg/detail/drone_message__struct.hpp"
#include "rosidl_runtime_cpp/traits.hpp"

// Include directives for member types
// Member 'position'
// Member 'velocity'
#include "geometry_msgs/msg/detail/vector3__traits.hpp"

namespace hiveflight_interfaces
{

namespace msg
{

inline void to_flow_style_yaml(
  const DroneMessage & msg,
  std::ostream & out)
{
  out << "{";
  // member: sender_id
  {
    out << "sender_id: ";
    rosidl_generator_traits::value_to_yaml(msg.sender_id, out);
    out << ", ";
  }

  // member: position
  {
    out << "position: ";
    to_flow_style_yaml(msg.position, out);
    out << ", ";
  }

  // member: velocity
  {
    out << "velocity: ";
    to_flow_style_yaml(msg.velocity, out);
    out << ", ";
  }

  // member: target_id
  {
    out << "target_id: ";
    rosidl_generator_traits::value_to_yaml(msg.target_id, out);
    out << ", ";
  }

  // member: timestamp
  {
    out << "timestamp: ";
    rosidl_generator_traits::value_to_yaml(msg.timestamp, out);
  }
  out << "}";
}  // NOLINT(readability/fn_size)

inline void to_block_style_yaml(
  const DroneMessage & msg,
  std::ostream & out, size_t indentation = 0)
{
  // member: sender_id
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "sender_id: ";
    rosidl_generator_traits::value_to_yaml(msg.sender_id, out);
    out << "\n";
  }

  // member: position
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "position:\n";
    to_block_style_yaml(msg.position, out, indentation + 2);
  }

  // member: velocity
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "velocity:\n";
    to_block_style_yaml(msg.velocity, out, indentation + 2);
  }

  // member: target_id
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "target_id: ";
    rosidl_generator_traits::value_to_yaml(msg.target_id, out);
    out << "\n";
  }

  // member: timestamp
  {
    if (indentation > 0) {
      out << std::string(indentation, ' ');
    }
    out << "timestamp: ";
    rosidl_generator_traits::value_to_yaml(msg.timestamp, out);
    out << "\n";
  }
}  // NOLINT(readability/fn_size)

inline std::string to_yaml(const DroneMessage & msg, bool use_flow_style = false)
{
  std::ostringstream out;
  if (use_flow_style) {
    to_flow_style_yaml(msg, out);
  } else {
    to_block_style_yaml(msg, out);
  }
  return out.str();
}

}  // namespace msg

}  // namespace hiveflight_interfaces

namespace rosidl_generator_traits
{

[[deprecated("use hiveflight_interfaces::msg::to_block_style_yaml() instead")]]
inline void to_yaml(
  const hiveflight_interfaces::msg::DroneMessage & msg,
  std::ostream & out, size_t indentation = 0)
{
  hiveflight_interfaces::msg::to_block_style_yaml(msg, out, indentation);
}

[[deprecated("use hiveflight_interfaces::msg::to_yaml() instead")]]
inline std::string to_yaml(const hiveflight_interfaces::msg::DroneMessage & msg)
{
  return hiveflight_interfaces::msg::to_yaml(msg);
}

template<>
inline const char * data_type<hiveflight_interfaces::msg::DroneMessage>()
{
  return "hiveflight_interfaces::msg::DroneMessage";
}

template<>
inline const char * name<hiveflight_interfaces::msg::DroneMessage>()
{
  return "hiveflight_interfaces/msg/DroneMessage";
}

template<>
struct has_fixed_size<hiveflight_interfaces::msg::DroneMessage>
  : std::integral_constant<bool, has_fixed_size<geometry_msgs::msg::Vector3>::value> {};

template<>
struct has_bounded_size<hiveflight_interfaces::msg::DroneMessage>
  : std::integral_constant<bool, has_bounded_size<geometry_msgs::msg::Vector3>::value> {};

template<>
struct is_message<hiveflight_interfaces::msg::DroneMessage>
  : std::true_type {};

}  // namespace rosidl_generator_traits

#endif  // HIVEFLIGHT_INTERFACES__MSG__DETAIL__DRONE_MESSAGE__TRAITS_HPP_
