#ifndef _USB
#define _USB

#include <tcl.h>
#include <errno.h>
#include <libusb-1.0/libusb.h>

/*
 * Windows needs to know which symbols to export.
 */

#ifdef BUILD_usb
#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLEXPORT
#endif /* BUILD_usb */


/*
 * Only the _Init function is exported.
 */

EXTERN int	Tclusb_Init(Tcl_Interp * interp);

int USB_INIT (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[]);
int USB_EXIT (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[]);

int USB_HAS_CAPABILITY (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[]);

int USB_GET_DEVICE_LIST (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[]);
int USB_FREE_DEVICE_LIST (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[]);

int USB_OPEN (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[]);
int USB_CLOSE (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[]);

int USB_GET_DEVICE_DESCRIPTOR (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[]);
int USB_GET_BUS_NUMBER (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[]);
int USB_GET_DEVICE_ADDRESS (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[]);

int USB_GET_STRING_DESCRIPTOR_ASCII (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[]);

int USB_GET_DEVICE_ELEMENT (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[]);
int USB_FREE_DEVICE_ELEMENT (ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const obj[]);

void Tclusb_InitHashTable ();

#endif /* _USB */
