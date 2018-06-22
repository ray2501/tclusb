#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "tclusb.h"


Tcl_HashTable *usb_hashtblPtr;
int devs_count = 0;
int dev_count = 0;
int handle_count = 0;
int hot_count = 0;

TCL_DECLARE_MUTEX(myMutex);


/*!
 * This program use a static Hash table to maintain pointer and Tcl handle
 * mapping. Initial our Hash table here
 */
void Tclusb_InitHashTable()
{
    usb_hashtblPtr = (Tcl_HashTable *) malloc (sizeof(Tcl_HashTable));

    Tcl_InitHashTable(usb_hashtblPtr, TCL_STRING_KEYS);
}


int USB_INIT(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST obj[])
{
    int result;

    result = libusb_init(NULL);
    if (result < 0) {
        return TCL_ERROR;
    }

    return TCL_OK;
}


int USB_EXIT(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *CONST obj[])
{
    libusb_exit(NULL);

    return TCL_OK;
}


int USB_HAS_CAPABILITY(ClientData clientData, Tcl_Interp *interp, 
        int objc, Tcl_Obj *CONST obj[])
{
    Tcl_Obj *cap_result;
    int capability;
    int result;

    if(objc != 2)
    {
        Tcl_WrongNumArgs(interp, 1, obj, "capability");
        return TCL_ERROR;
    }

    //LIBUSB_CAP_HAS_CAPABILITY = 0x0000,
    //LIBUSB_CAP_HAS_HOTPLUG = 0x0001,
    //LIBUSB_CAP_HAS_HID_ACCESS = 0x0100,
    //LIBUSB_CAP_SUPPORTS_DETACH_KERNEL_DRIVER = 0x0101
    if(Tcl_GetIntFromObj(interp, obj[1], &capability) != TCL_OK) {
        return TCL_ERROR;
    }

    result = libusb_has_capability(capability);
    if(result > 0) {
        cap_result = Tcl_NewBooleanObj( 1 );
    } else {
        cap_result = Tcl_NewBooleanObj( 0 );
    }


    Tcl_SetObjResult(interp, cap_result);

    return TCL_OK;
}


int USB_GET_DEVICE_LIST(ClientData clientData, Tcl_Interp *interp, 
	int objc, Tcl_Obj *CONST obj[])
{
    Tcl_Obj *returnvalue = NULL;
    Tcl_HashEntry *hashEntryPtr;
    char handleName[16 + TCL_INTEGER_SPACE];
    int newvalue;    

    // For libusb
    libusb_device **devs;
    int result;

    result = libusb_get_device_list(NULL, &devs);
    if (result < 0) {
        return TCL_ERROR;
    }
    
    Tcl_MutexLock(&myMutex);
    sprintf( handleName, "usb%d", devs_count++ );
    returnvalue = Tcl_NewStringObj( handleName, -1 );
    hashEntryPtr = Tcl_CreateHashEntry(usb_hashtblPtr, handleName, &newvalue);
    Tcl_SetHashValue(hashEntryPtr, devs);
    Tcl_MutexUnlock(&myMutex);

    Tcl_SetObjResult(interp, returnvalue);
    return TCL_OK;
}


int USB_FREE_DEVICE_LIST(ClientData clientData, Tcl_Interp *interp, 
	int objc, Tcl_Obj *CONST obj[])
{
    Tcl_HashEntry *hashEntryPtr;
    char *handle;
    int len;

    libusb_device **devs;

    if(objc != 2)
    {
        Tcl_WrongNumArgs(interp, 1, obj, "usbHandle");
        return TCL_ERROR;
    }

    handle = Tcl_GetStringFromObj(obj[1], &len);
    if(!handle || len < 1) {
        return TCL_ERROR;
    }
    
    hashEntryPtr = Tcl_FindHashEntry( usb_hashtblPtr, handle );
    if( !hashEntryPtr ) {
        if( interp ) {
            Tcl_Obj *resultObj = Tcl_GetObjResult( interp );
            Tcl_AppendStringsToObj( resultObj, "invalid usb handle ", handle, (char *)NULL );
        }

        return TCL_ERROR;
    }

    // Got the libusb_device pointer from hash table
    devs = Tcl_GetHashValue( hashEntryPtr );

    //call libusb free function
    libusb_free_device_list(devs, 1);
    
    // Remove from hash table
    Tcl_MutexLock(&myMutex);
    Tcl_DeleteHashEntry(hashEntryPtr);
    Tcl_MutexUnlock(&myMutex);

    return TCL_OK;
}


int USB_OPEN(ClientData clientData, Tcl_Interp *interp, 
	int objc, Tcl_Obj *CONST obj[])
{
    Tcl_Obj *returnvalue = NULL;
    Tcl_HashEntry *hashEntryPtr;
    char *handle;
    int len;
    char handleName[16 + TCL_INTEGER_SPACE];
    int newvalue;    

    libusb_device *dev;
    libusb_device_handle *dev_handle;
    int result;

    if(objc != 2)
    {
        Tcl_WrongNumArgs(interp, 1, obj, "usb_devHandle");
        return TCL_ERROR;
    }

    handle = Tcl_GetStringFromObj(obj[1], &len);
    if(!handle || len < 1) {
        return TCL_ERROR;
    }
    hashEntryPtr = Tcl_FindHashEntry( usb_hashtblPtr, handle );
    if( !hashEntryPtr ) {
        if( interp ) {
            Tcl_Obj *resultObj = Tcl_GetObjResult( interp );
            Tcl_AppendStringsToObj( resultObj, "invalid usb_dev handle ", handle, (char *)NULL );
        }

        return TCL_ERROR;
    }

    // Got the libusb_device pointer from hash table
    dev = Tcl_GetHashValue( hashEntryPtr );
    
    result = libusb_open(dev, &dev_handle);    
    if(result < 0) {
        return TCL_ERROR;
    }

    // OK, save the libusb_device_handle pointer to hash table
    Tcl_MutexLock(&myMutex);
    sprintf( handleName, "usb_dev_h%d", handle_count++ );
    returnvalue = Tcl_NewStringObj( handleName, -1 );
    hashEntryPtr = Tcl_CreateHashEntry(usb_hashtblPtr, handleName, &newvalue);
    Tcl_SetHashValue(hashEntryPtr, dev_handle);
    Tcl_MutexUnlock(&myMutex);    

    Tcl_SetObjResult(interp, returnvalue);
    return TCL_OK;
}


int USB_CLOSE(ClientData clientData, Tcl_Interp *interp, 
	int objc, Tcl_Obj *CONST obj[])
{
    Tcl_HashEntry *hashEntryPtr;
    char *handle;
    int len;
    
    libusb_device_handle *dev_handle;
    
    if(objc != 2)
    {
        Tcl_WrongNumArgs(interp, 1, obj, "usb_dev_hHandle");
        return TCL_ERROR;
    }

    handle = Tcl_GetStringFromObj(obj[1], &len);
    if(!handle || len < 1) {
        return TCL_ERROR;
    }
    hashEntryPtr = Tcl_FindHashEntry( usb_hashtblPtr, handle );

    if( !hashEntryPtr ) {
        if( interp ) {
          Tcl_Obj *resultObj = Tcl_GetObjResult( interp );
          Tcl_AppendStringsToObj( resultObj, "invalid usb_dev_h handle ", handle, (char *)NULL );
        }

        return TCL_ERROR;
    }

    // Got the libusb_device pointer from hash table
    dev_handle = Tcl_GetHashValue( hashEntryPtr );            
    
    // Now close this handle
    libusb_close(dev_handle);
    
    // Remove from hash table
    Tcl_MutexLock(&myMutex);
    Tcl_DeleteHashEntry(hashEntryPtr);
    Tcl_MutexUnlock(&myMutex);
    
    return TCL_OK;
}


int USB_GET_DEVICE_DESCRIPTOR(ClientData clientData, Tcl_Interp *interp, 
	int objc, Tcl_Obj *CONST obj[])
{
    Tcl_HashEntry *hashEntryPtr;
    Tcl_Obj *listobj = NULL;
    Tcl_Obj *bLength, *bDescriptorType, *bcdUSB;
    Tcl_Obj *bDeviceClass, *bDeviceSubClass, *bDeviceProtocol;
    Tcl_Obj *bMaxPacketSize0;
    Tcl_Obj *idVendor, *idProduct;
    Tcl_Obj *bcdDevice;
    Tcl_Obj *iManufacturer, *iProduct, *iSerialNumber;
    Tcl_Obj *bNumConfigurations;
    char *handle;
    int len;

    libusb_device *dev;
    struct libusb_device_descriptor desc;
    int result = 0;

    if(objc != 2)
    {
        Tcl_WrongNumArgs(interp, 1, obj, "usb_devHandle");
        return TCL_ERROR;
    }

    handle = Tcl_GetStringFromObj(obj[1], &len);
    if(!handle || len < 1) {
        return TCL_ERROR;
    }
    hashEntryPtr = Tcl_FindHashEntry( usb_hashtblPtr, handle );
    if( !hashEntryPtr ) {
        if( interp ) {
          Tcl_Obj *resultObj = Tcl_GetObjResult( interp );
          Tcl_AppendStringsToObj( resultObj, "invalid usb_dev handle ", handle, (char *)NULL );
        }

        return TCL_ERROR;
    }

    // Got the libusb_device pointer from hash table
    dev = Tcl_GetHashValue( hashEntryPtr );    
    result = libusb_get_device_descriptor(dev, &desc);
    if (result < 0) {	
        return TCL_ERROR;
    }
    
    listobj = Tcl_NewListObj(0, NULL);
    bLength = Tcl_NewIntObj(desc.bLength);
    Tcl_ListObjAppendElement(interp, listobj, bLength);
    
    bDescriptorType = Tcl_NewIntObj(desc.bDescriptorType);
    Tcl_ListObjAppendElement(interp, listobj, bDescriptorType);    
        
    bcdUSB = Tcl_NewIntObj(desc.bcdUSB);
    Tcl_ListObjAppendElement(interp, listobj, bcdUSB);     
        
    bDeviceClass = Tcl_NewIntObj(desc.bDeviceClass);
    Tcl_ListObjAppendElement(interp, listobj, bDeviceClass);    
    
    bDeviceSubClass = Tcl_NewIntObj(desc.bDeviceSubClass);
    Tcl_ListObjAppendElement(interp, listobj, bDeviceSubClass);
        
    bDeviceProtocol = Tcl_NewIntObj(desc.bDeviceProtocol);
    Tcl_ListObjAppendElement(interp, listobj, bDeviceProtocol);    
        
    bMaxPacketSize0 = Tcl_NewIntObj(desc.bMaxPacketSize0);
    Tcl_ListObjAppendElement(interp, listobj, bMaxPacketSize0);     
    
    idVendor = Tcl_NewIntObj(desc.idVendor);
    Tcl_ListObjAppendElement(interp, listobj, idVendor);
        
    idProduct = Tcl_NewIntObj(desc.idProduct);
    Tcl_ListObjAppendElement(interp, listobj, idProduct);    
        
    bcdDevice = Tcl_NewIntObj(desc.bcdDevice);
    Tcl_ListObjAppendElement(interp, listobj, bcdDevice);
        
    iManufacturer = Tcl_NewIntObj(desc.iManufacturer);
    Tcl_ListObjAppendElement(interp, listobj, iManufacturer);    
    
    iProduct = Tcl_NewIntObj(desc.iProduct);
    Tcl_ListObjAppendElement(interp, listobj, iProduct);     
    
    iSerialNumber = Tcl_NewIntObj(desc.iSerialNumber);
    Tcl_ListObjAppendElement(interp, listobj, iSerialNumber);
        
    bNumConfigurations = Tcl_NewIntObj(desc.bNumConfigurations);
    Tcl_ListObjAppendElement(interp, listobj, bNumConfigurations);    
    
    Tcl_SetObjResult(interp, listobj);
    return TCL_OK;
}


int USB_GET_BUS_NUMBER(ClientData clientData, Tcl_Interp *interp, 
	int objc, Tcl_Obj *CONST obj[])
{
    Tcl_HashEntry *hashEntryPtr;
    Tcl_Obj *bus_number;
    char *handle;
    int len;

    libusb_device *dev;
    int bus = 0;

    if(objc != 2) {
        Tcl_WrongNumArgs(interp, 1, obj, "usb_devHandle");
        return TCL_ERROR;
    }

    handle = Tcl_GetStringFromObj(obj[1], &len);
    if(!handle || len < 1) {
        return TCL_ERROR;
    }
    hashEntryPtr = Tcl_FindHashEntry( usb_hashtblPtr, handle );

    if( !hashEntryPtr ) {
        if( interp ) {
          Tcl_Obj *resultObj = Tcl_GetObjResult( interp );
          Tcl_AppendStringsToObj( resultObj, "invalid usb_dev handle ", handle, (char *)NULL );
        }

        return TCL_ERROR;
    }

    // Got the libusb_device pointer from hash table
    dev = Tcl_GetHashValue( hashEntryPtr );    	
    bus = libusb_get_bus_number(dev);       
    bus_number = Tcl_NewIntObj(bus);    
    
    Tcl_SetObjResult(interp, bus_number);
    return TCL_OK;
}

int USB_GET_DEVICE_ADDRESS(ClientData clientData, Tcl_Interp *interp,
        int objc, Tcl_Obj *CONST obj[])
{
    Tcl_HashEntry *hashEntryPtr;
    Tcl_Obj *dev_address;
    char *handle;
    int len;

    libusb_device *dev;
    int address = 0;

    if(objc != 2) {
        Tcl_WrongNumArgs(interp, 1, obj, "usb_devHandle");
        return TCL_ERROR;
    }

    handle = Tcl_GetStringFromObj(obj[1], &len);
    if(!handle || len < 1) {
        return TCL_ERROR;
    }
    hashEntryPtr = Tcl_FindHashEntry( usb_hashtblPtr, handle );

    if( !hashEntryPtr ) {
        if( interp ) {
          Tcl_Obj *resultObj = Tcl_GetObjResult( interp );
          Tcl_AppendStringsToObj( resultObj, "invalid usb_dev handle ", handle, (char *)NULL );
        }

        return TCL_ERROR;
    }

    // Got the libusb_device pointer from hash table
    dev = Tcl_GetHashValue( hashEntryPtr );
    address = libusb_get_device_address(dev);    
    dev_address = Tcl_NewIntObj(address);

    Tcl_SetObjResult(interp, dev_address);
    return TCL_OK;
}



int USB_GET_STRING_DESCRIPTOR_ASCII(ClientData clientData, Tcl_Interp *interp, 
	int objc, Tcl_Obj *CONST obj[])
{
    Tcl_HashEntry *hashEntryPtr;
    Tcl_Obj *descriptor;
    int index;
    char *handle;
    int len;

    libusb_device_handle *dev_handle;
    uint8_t desc_index;
    char str[256];
    char *default_Str = "Unknown";
    int result;

    if(objc != 3) {
        Tcl_WrongNumArgs(interp, 1, obj, "usb_devHandle desc_index");
        return TCL_ERROR;
    }

    handle = Tcl_GetStringFromObj(obj[1], &len);
    if(!handle || len < 1) {
        return TCL_ERROR;
    }
    hashEntryPtr = Tcl_FindHashEntry( usb_hashtblPtr, handle );
    if( !hashEntryPtr ) {
        if( interp ) {
            Tcl_Obj *resultObj = Tcl_GetObjResult( interp );
            Tcl_AppendStringsToObj( resultObj, "invalid usb_dev handle ", handle, (char *)NULL );
        }

        return TCL_ERROR;
    }    

    // Got the libusb_device pointer from hash table
    dev_handle = Tcl_GetHashValue( hashEntryPtr );    
    
    // Got the desc_index
    Tcl_GetIntFromObj(interp, obj[2], &index);
    if (index < 255 && index > 0)
        desc_index = (uint8_t) index;
    else
        desc_index = 0;
    
    str[0] = '\0';
    if(index > 0) { //if desc_index equals zero is invalid value
        result = libusb_get_string_descriptor_ascii(dev_handle, desc_index, 
            (unsigned char *) str, sizeof(str));
            
        if (result < 0) {
            str[0] = '\0';
            strcpy(str, default_Str);
        }        
    } else {
        strcpy(str, default_Str);
    }                                
    
    descriptor = Tcl_NewStringObj( str, -1 );
    
    Tcl_SetObjResult(interp, descriptor);
    return TCL_OK;
}   


/*!
 * This function helps to get all element from libusb_device list, and
 * puts these elements to our hash table.
 */
int USB_GET_DEVICE_ELEMENT(ClientData clientData, Tcl_Interp *interp, 
	int objc, Tcl_Obj *CONST obj[])
{
    Tcl_HashEntry *hashEntryPtr;
    Tcl_Obj *listobj = NULL;
    Tcl_Obj *strobj = NULL;
    char handleName[16 + TCL_INTEGER_SPACE];
    char *handle;
    int len;

    libusb_device **devs;
    libusb_device *dev;
    int i = 0;

    if(objc != 2) {
        Tcl_WrongNumArgs(interp, 1, obj, "usbHandle");
        return TCL_ERROR;
    }

    handle = Tcl_GetStringFromObj(obj[1], &len);
    if(!handle || len < 1) {
        return TCL_ERROR;
    }
    hashEntryPtr = Tcl_FindHashEntry( usb_hashtblPtr, handle );
    if( !hashEntryPtr ) {
        if( interp ) {
            Tcl_Obj *resultObj = Tcl_GetObjResult( interp );
            Tcl_AppendStringsToObj( resultObj, "invalid usb handle ", handle, (char *)NULL );
        }

        return TCL_ERROR;
    }

    // Got the libusb_device pointer from hash table
    devs = Tcl_GetHashValue( hashEntryPtr );
    listobj = Tcl_NewListObj(0, NULL);
    while ((dev = devs[i++]) != NULL) {
        int newvalue;
	
        Tcl_MutexLock(&myMutex);
        sprintf( handleName, "usb_dev%d", i );
        strobj = Tcl_NewStringObj( handleName, -1 );
        hashEntryPtr = Tcl_CreateHashEntry(usb_hashtblPtr, handleName, &newvalue);
        Tcl_SetHashValue(hashEntryPtr, dev);

        Tcl_ListObjAppendElement(interp, listobj, strobj);
        Tcl_MutexUnlock(&myMutex);  
    }
    
    dev_count = dev_count + i;        

    Tcl_SetObjResult(interp, listobj);
    return TCL_OK;
}


/*!
 * This function helps to remove libusb_device elements in our hash table.
 */
int USB_FREE_DEVICE_ELEMENT(ClientData clientData, Tcl_Interp *interp, 
	int objc, Tcl_Obj *CONST obj[])
{
    Tcl_Obj *list_obj = NULL;
    Tcl_Obj *element = NULL;
    Tcl_HashEntry *hashEntryPtr;
    char *handle;
    int length, i;
    int str_len;

    if(objc != 2) {
        Tcl_WrongNumArgs(interp, 1, obj, "usbHandle");
        return TCL_ERROR;
    }

    list_obj = obj[1];
    Tcl_ListObjLength(interp, list_obj, &length);

    for(i = 0; i < length; i++) {
	Tcl_ListObjIndex(interp, list_obj, i,  &element);
        
        handle = Tcl_GetStringFromObj(element, &str_len);
        hashEntryPtr = Tcl_FindHashEntry( usb_hashtblPtr, handle );
        if( !hashEntryPtr ) {
            if( interp ) {
              Tcl_Obj *resultObj = Tcl_GetObjResult( interp );
              Tcl_AppendStringsToObj(resultObj, "invalid usb_dev handle ", 
                                     handle, (char *)NULL );
            }
        }
        Tcl_MutexLock(&myMutex);
        Tcl_DeleteHashEntry(hashEntryPtr);
        Tcl_MutexUnlock(&myMutex);        
    }

    return TCL_OK;
}
