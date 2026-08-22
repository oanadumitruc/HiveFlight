// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from hiveflight_interfaces:msg/DroneMessage.idl
// generated code does not contain a copyright notice

#ifndef HIVEFLIGHT_INTERFACES__MSG__DETAIL__DRONE_MESSAGE__BUILDER_HPP_
#define HIVEFLIGHT_INTERFACES__MSG__DETAIL__DRONE_MESSAGE__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "hiveflight_interfaces/msg/detail/drone_message__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace hiveflight_interfaces
{

namespace msg
{

namespace builder
{

class Init_DroneMessage_timestamp
{
public:
  explicit Init_DroneMessage_timestamp(::hiveflight_interfaces::msg::DroneMessage & msg)
  : msg_(msg)
  {}
  ::hiveflight_interfaces::msg::DroneMessage timestamp(::hiveflight_interfaces::msg::DroneMessage::_timestamp_type arg)
  {
    msg_.timestamp = std::move(arg);
    return std::move(msg_);
  }

private:
  ::hiveflight_interfaces::msg::DroneMessage msg_;
};

class Init_DroneMessage_target_id
{
public:
  explicit Init_DroneMessage_target_id(::hiveflight_interfaces::msg::DroneMessage & msg)
  : msg_(msg)
  {}
  Init_DroneMessage_timestamp target_id(::hiveflight_interfaces::msg::DroneMessage::_target_id_type arg)
  {
    msg_.target_id = std::move(arg);
    return Init_DroneMessage_timestamp(msg_);
  }

private:
  ::hiveflight_interfaces::msg::DroneMessage msg_;
};

class Init_DroneMessage_velocity
{
public:
  explicit Init_DroneMessage_velocity(::hiveflight_interfaces::msg::DroneMessage & msg)
  : msg_(msg)
  {}
  Init_DroneMessage_target_id velocity(::hiveflight_interfaces::msg::DroneMessage::_velocity_type arg)
  {
    msg_.velocity = std::move(arg);
    return Init_DroneMessage_target_id(msg_);
  }

private:
  ::hiveflight_interfaces::msg::DroneMessage msg_;
};

class Init_DroneMessage_position
{
public:
  explicit Init_DroneMessage_position(::hiveflight_interfaces::msg::DroneMessage & msg)
  : msg_(msg)
  {}
  Init_DroneMessage_velocity position(::hiveflight_interfaces::msg::DroneMessage::_position_type arg)
  {
    msg_.position = std::move(arg);
    return Init_DroneMessage_velocity(msg_);
  }

private:
  ::hiveflight_interfaces::msg::DroneMessage msg_;
};

class Init_DroneMessage_sender_id
{
public:
  Init_DroneMessage_sender_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_DroneMessage_position sender_id(::hiveflight_interfaces::msg::DroneMessage::_sender_id_type arg)
  {
    msg_.sender_id = std::move(arg);
    return Init_DroneMessage_position(msg_);
  }

private:
  ::hiveflight_interfaces::msg::DroneMessage msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::hiveflight_interfaces::msg::DroneMessage>()
{
  return hiveflight_interfaces::msg::builder::Init_DroneMessage_sender_id();
}

}  // namespace hiveflight_interfaces

#endif  // HIVEFLIGHT_INTERFACES__MSG__DETAIL__DRONE_MESSAGE__BUILDER_HPP_
