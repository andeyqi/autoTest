#ifndef __SERVICE_MANAGER_COMMUNICATION_IPCWRAPPER_TOYOTA_DBUS_COMMON_H__
#define __SERVICE_MANAGER_COMMUNICATION_IPCWRAPPER_TOYOTA_DBUS_COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus-glib.h>
#include <dbus/dbus.h>
#include <unistd.h>
#include <pthread.h>

//#include "serviceManagerLog/LitesTraceDrc.h"

#define TOYOTA_DBUS_SESSION  					DBUS_BUS_SESSION
#define TOYOTA_DBUS_NAME_FLAG_REPLACE_EXISTING 	DBUS_NAME_FLAG_REPLACE_EXISTING
#define TOYOTA_DBUS_NAME_FLAG_ALLOW_REPLACEMENT 	DBUS_NAME_FLAG_ALLOW_REPLACEMENT
#define TOYOTA_DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER   DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER

#define TOYOTA_DBUS_BUS_NAME_NAVIGATION 		"com.fujitsuten.navigation"
#define TOYOTA_DBUS_OBJECT_PATH_NAVIGATION		"/naviinterface/signal/ObjectNavi"
#define TOYOTA_DBUS_INTERFACE_NAVIGATION		"fujitsuten.signal.navigation"
#define TOYOTA_DBUS_SIGNAL_NAME_NAVIGATION		"signal_navi"

#define TOYOTA_DBUS_BUS_NAME_WRAPPER 			"com.fujitsuten.wrapper"
#define TOYOTA_DBUS_OBJECT_PATH_WRAPPER			"/naviinterface/signal/ObjectWrapper"
#define TOYOTA_DBUS_INTERFACE_WRAPPER			"fujitsuten.signal.wrapper"
#define TOYOTA_DBUS_SIGNAL_NAME_WRAPPER			"signal_wrapper"


#endif /* __SERVICE_MANAGER_COMMUNICATION_IPCWRAPPER_TOYOTA_DBUS_COMMON_H__ */