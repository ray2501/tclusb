tclusb
=====

## About

It is an experimental project.
The Tclusb extension provides an tcl interface to the libusb library.
Now only have list devices function.

If you want to test this package, you could use examples/listdevs.tcl
to test.


## Implement commands

::usb::init  
::usb::exit  
::usb::has_capability  
::usb::get_device_list  
::usb::free_device_list  
::usb::open  
::usb::close  
::usb::get_device_descriptor  
::usb::get_bus_number  
::usb::get_device_address  
::usb::get_string_descriptor_ascii  
::usb::get_device_list_element  
::usb::free_device_list_element  


UNIX BUILD
=====

Linux build need install libusb-1.0-devel first.

The following minimal example will install the extension in the /opt/tcl
directory.

    $ cd tclusb
    $ ./configure --prefix=/opt/tcl
    $ make
    $ make install

If you need setup directory containing tcl configuration (tclConfig.sh),
below is an example:

    $ cd tclusb
    $ ./configure --with-tcl=/opt/activetcl/lib
    $ make
    $ make install


WINDOWS BUILD
=====

The recommended method to build extensions under windows is to use the
MSYS2 + MinGW-w64 build process. This provides a Unix-style build while
generating native Windows binaries. Using the MSYS2 + MinGW-w64 build tools
means that you can use the same configure script as per the Unix build
to create a Makefile.

User needs to install MinGW-w64 libusb package before to build this
extension.


