/*
 * apptypes.h
 *
 *  Created on: 17.07.2011
 *      Author: klen
 */

#ifndef __APPTYPES_H__
#define __APPTYPES_H__


#include <errno.h>

typedef enum  { cdc,  msc  } usb_connect_type_t ;

extern usb_connect_type_t usb_connect_type ;

#endif /* APPTYPES_H_ */
