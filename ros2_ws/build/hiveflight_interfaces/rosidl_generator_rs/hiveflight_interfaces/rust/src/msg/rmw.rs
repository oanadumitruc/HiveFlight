#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};


#[link(name = "hiveflight_interfaces__rosidl_typesupport_c")]
extern "C" {
    fn rosidl_typesupport_c__get_message_type_support_handle__hiveflight_interfaces__msg__DroneMessage() -> *const std::ffi::c_void;
}

#[link(name = "hiveflight_interfaces__rosidl_generator_c")]
extern "C" {
    fn hiveflight_interfaces__msg__DroneMessage__init(msg: *mut DroneMessage) -> bool;
    fn hiveflight_interfaces__msg__DroneMessage__Sequence__init(seq: *mut rosidl_runtime_rs::Sequence<DroneMessage>, size: usize) -> bool;
    fn hiveflight_interfaces__msg__DroneMessage__Sequence__fini(seq: *mut rosidl_runtime_rs::Sequence<DroneMessage>);
    fn hiveflight_interfaces__msg__DroneMessage__Sequence__copy(in_seq: &rosidl_runtime_rs::Sequence<DroneMessage>, out_seq: *mut rosidl_runtime_rs::Sequence<DroneMessage>) -> bool;
}

// Corresponds to hiveflight_interfaces__msg__DroneMessage
#[cfg_attr(feature = "serde", derive(Deserialize, Serialize))]


// This struct is not documented.
#[allow(missing_docs)]

#[repr(C)]
#[derive(Clone, Debug, PartialEq, PartialOrd)]
pub struct DroneMessage {

    // This member is not documented.
    #[allow(missing_docs)]
    pub sender_id: i32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub position: geometry_msgs::msg::rmw::Vector3,


    // This member is not documented.
    #[allow(missing_docs)]
    pub velocity: geometry_msgs::msg::rmw::Vector3,


    // This member is not documented.
    #[allow(missing_docs)]
    pub target_id: i32,


    // This member is not documented.
    #[allow(missing_docs)]
    pub timestamp: f32,

}



impl Default for DroneMessage {
  fn default() -> Self {
    unsafe {
      let mut msg = std::mem::zeroed();
      if !hiveflight_interfaces__msg__DroneMessage__init(&mut msg as *mut _) {
        panic!("Call to hiveflight_interfaces__msg__DroneMessage__init() failed");
      }
      msg
    }
  }
}

impl rosidl_runtime_rs::SequenceAlloc for DroneMessage {
  fn sequence_init(seq: &mut rosidl_runtime_rs::Sequence<Self>, size: usize) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { hiveflight_interfaces__msg__DroneMessage__Sequence__init(seq as *mut _, size) }
  }
  fn sequence_fini(seq: &mut rosidl_runtime_rs::Sequence<Self>) {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { hiveflight_interfaces__msg__DroneMessage__Sequence__fini(seq as *mut _) }
  }
  fn sequence_copy(in_seq: &rosidl_runtime_rs::Sequence<Self>, out_seq: &mut rosidl_runtime_rs::Sequence<Self>) -> bool {
    // SAFETY: This is safe since the pointer is guaranteed to be valid/initialized.
    unsafe { hiveflight_interfaces__msg__DroneMessage__Sequence__copy(in_seq, out_seq as *mut _) }
  }
}

impl rosidl_runtime_rs::Message for DroneMessage {
  type RmwMsg = Self;
  fn into_rmw_message(msg_cow: std::borrow::Cow<'_, Self>) -> std::borrow::Cow<'_, Self::RmwMsg> { msg_cow }
  fn from_rmw_message(msg: Self::RmwMsg) -> Self { msg }
}

impl rosidl_runtime_rs::RmwMessage for DroneMessage where Self: Sized {
  const TYPE_NAME: &'static str = "hiveflight_interfaces/msg/DroneMessage";
  fn get_type_support() -> *const std::ffi::c_void {
    // SAFETY: No preconditions for this function.
    unsafe { rosidl_typesupport_c__get_message_type_support_handle__hiveflight_interfaces__msg__DroneMessage() }
  }
}


