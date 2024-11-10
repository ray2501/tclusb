#include <tcl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tclusb.h"

/*
 *----------------------------------------------------------------------
 *
 * Tclusb_Init --
 *
 *	Initialize the new package.  The string "Tclusb" in the
 *	function name must match the PACKAGE declaration at the top of
 *	configure.in.
 *
 * Results:
 *	A standard Tcl result
 *
 *----------------------------------------------------------------------
 */

int
Tclusb_Init(Tcl_Interp *interp)
{
    if (Tcl_InitStubs(interp, TCL_VERSION, 0) == NULL) {
	return TCL_ERROR;
    }

    if (Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION) != TCL_OK) {
	return TCL_ERROR;
    }

    Tclusb_InitHashTable();

    Tcl_CreateObjCommand(interp, "::usb::init", USB_INIT, (ClientData) NULL, NULL);
    Tcl_CreateObjCommand(interp, "::usb::exit", USB_EXIT, (ClientData) NULL, NULL);

    Tcl_CreateObjCommand(interp, "::usb::has_capability", USB_HAS_CAPABILITY,
        (ClientData) NULL, NULL);

    Tcl_CreateObjCommand(interp, "::usb::get_device_list", USB_GET_DEVICE_LIST, 
        (ClientData) NULL, NULL);

    Tcl_CreateObjCommand(interp, "::usb::free_device_list", USB_FREE_DEVICE_LIST, 
        (ClientData) NULL, NULL);
        
    Tcl_CreateObjCommand(interp, "::usb::open", USB_OPEN, (ClientData) NULL, NULL);

    Tcl_CreateObjCommand(interp, "::usb::close", USB_CLOSE, (ClientData) NULL, NULL);

    Tcl_CreateObjCommand(interp, "::usb::get_device_descriptor", USB_GET_DEVICE_DESCRIPTOR, 
        (ClientData) NULL, NULL);

    Tcl_CreateObjCommand(interp, "::usb::get_bus_number", USB_GET_BUS_NUMBER, 
        (ClientData) NULL, NULL);

    Tcl_CreateObjCommand(interp, "::usb::get_device_address", USB_GET_DEVICE_ADDRESS, 
        (ClientData) NULL, NULL);
        
    Tcl_CreateObjCommand(interp, "::usb::get_string_descriptor_ascii", 
        USB_GET_STRING_DESCRIPTOR_ASCII, (ClientData) NULL, NULL);

    Tcl_CreateObjCommand(interp, "::usb::get_device_list_element", USB_GET_DEVICE_ELEMENT, 
        (ClientData) NULL, NULL);

    Tcl_CreateObjCommand(interp, "::usb::free_device_list_element", USB_FREE_DEVICE_ELEMENT, 
        (ClientData) NULL, NULL);   

    return TCL_OK;
}
