tclusb
=====

## About

It is an experimental project.
The Tclusb extension provides an tcl interface to the libusb library.
Now only have list devices function.

If you want to test this package, you could use examples/listdevs.tcl
to test.


## Implement commands

usb::init  
usb::exit  
usb::has_capability  
usb::get_device_list  
usb::free_device_list  
usb::open  
usb::close  
usb::get_device_descriptor  
usb::get_bus_number  
usb::get_device_address  
usb::get_string_descriptor_ascii  
usb::get_device_list_element  
usb::free_device_list_element  


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

Please put the file libusb-1.0.lib (MSVC x86 static build) to win/libusb/lib.
If you need build a X64 version, you need download windows binary files and
put libusb-1.0.lib (MSVC X64 static build) to win/libusb/lib.

The install dir in rules.vc setup _INSTALLDIR to C:\Tcl, if your Tcl folder
is not in this path, please update or setup _INSTALLDIR value.

Install steps (for x86):
nmake -f makefile.vc MACHINE=IX86
nmake -f makefile.vc install

* NOTE: If you want to use usb::open command on Windows platform,
        I think you need WinUSB driver (not test on Windows, sorry).

