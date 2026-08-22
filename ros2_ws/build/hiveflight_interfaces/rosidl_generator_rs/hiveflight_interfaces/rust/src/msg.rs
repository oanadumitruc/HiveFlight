#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};



// Corresponds to hiveflight_interfaces__msg__DroneMessage

// This struct is not documented.
#[allow(missing_docs)]

#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct DroneMessage {

    // This member is not documented.
    #[allow(missing_docs)]
    pub sender_id: i32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub position: geometry_msgs::msg::Vector3,


    // This member is not documented.
    #[allow(missing_docs)]
    pub velocity: geometry_msgs::msg::Vector3,


    // This member is not documented.
    #[allow(missing_docs)]
    pub target_id: i32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub timestamp: f32,

}



impl Default for DroneMessage {
  fn default() -> Self {
    <Self as rosidl_runtime_rs::Message>::from_rmw_message(super::msg::rmw::DroneMessage::default())
  }
}

impl rosidl_runtime_rs::Message for DroneMessage {
  type RmwMsg = super::msg::rmw::DroneMessage;

  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> {
    match msg_cow {
      std::borrow::Cow::Owned(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
        sender_id: msg.sender_id,
        position: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Owned(msg.position)).into_owned(),
        velocity: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Owned(msg.velocity)).into_owned(),
        target_id: msg.target_id,
        timestamp: msg.timestamp,
      }),
      std::borrow::Cow::Borrowed(msg) => std::borrow::Cow::Owned(Self::RmwMsg {
      sender_id: msg.sender_id,
        position: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Borrowed(&msg.position)).into_owned(),
        velocity: geometry_msgs::msg::Vector3::into_rmw_message(std::borrow::Cow::Borrowed(&msg.velocity)).into_owned(),
      target_id: msg.target_id,
      timestamp: msg.timestamp,
      })
    }
  }

  fn from_rmw_message(msg: Self::RmwMsg) -> Self {
    Self {
      sender_id: msg.sender_id,
      position: geometry_msgs::msg::Vector3::from_rmw_message(msg.position),
      velocity: geometry_msgs::msg::Vector3::from_rmw_message(msg.velocity),
      target_id: msg.target_id,
      timestamp: msg.timestamp,
    }
  }
}


