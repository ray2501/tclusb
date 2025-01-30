#!/usr/bin/env tclsh

package require tclusb


::usb::init

set dev_list [::usb::get_device_list]
set elements [::usb::get_device_list_element $dev_list]

foreach usb_dev $elements {
  set result [::usb::get_device_descriptor $usb_dev]
  
  set idVendor [lindex $result 7]
  set idProduct [lindex $result 8]
  set iManufacturer [lindex $result 10]
  set iProduct [lindex $result 11]
  set bus [::usb::get_bus_number $usb_dev]
  set address [::usb::get_device_address $usb_dev]

  #use::open and usb::close need root power  
  set handle [::usb::open $usb_dev]
  set manufacturer [::usb::get_string_descriptor_ascii $handle $iManufacturer]
  set product [::usb::get_string_descriptor_ascii $handle $iProduct]
  ::usb::close $handle
    
  #use ::usb::open and ::usb::close need root power
  set for_string [format "%04x:%04x (bus %d, device %d) - %s %s" \
                  $idVendor $idProduct $bus $address $manufacturer $product]
  
  puts $for_string
}

::usb::free_device_list_element $elements
::usb::free_device_list $dev_list

::usb::exit
