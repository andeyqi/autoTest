CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

SET(MODULE_NAME "service_manager")

IF (NOT DEFINED MASTER_PROJECT)
	IF (NOT DEFINED MBMAKE_BUILD_TEST)
		PROJECT ("${MODULE_NAME}")
	ENDIF()
ENDIF()

INCLUDE("../navicore/CMakeListsPub.txt")
INCLUDE_DIRECTORIES ("include/externalMbApi")
INCLUDE_DIRECTORIES ("include/service_manager")
INCLUDE_DIRECTORIES ("src")
INCLUDE_DIRECTORIES ("src/serviceManagerService/externalClient")
INCLUDE_DIRECTORIES ("src/serviceManagerService/engine")
INCLUDE_DIRECTORIES ("src/communication")
INCLUDE_DIRECTORIES ("src/communication/ipcwrapper_toyota")
INCLUDE_DIRECTORIES ("src/communication/ipcwrapper_win32")
INCLUDE_DIRECTORIES ("src/communication/ipcwrapper_desai")
INCLUDE_DIRECTORIES ("src/communication/threadManager")
INCLUDE_DIRECTORIES ("../mapbar-navicore-lib/include")
INCLUDE_DIRECTORIES ("../navicore-lib/include")

SET(lib_name "${LIB_PREFIX}${MODULE_NAME}${LIB_SUFFIX}")

IF (NOT DEFINED MBMAKE_BUILD_TEST)
	IF (${MAPBAR_PLATFORM} MATCHES "^(win32|x64|qnx660)$")
		FILE (GLOB LIB_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"  "${CMAKE_CURRENT_SOURCE_DIR}/src/communication/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/communication/ipcwrapper_win32/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/externalMbApi/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/serviceManagerCommon/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/serviceManagerService/engine/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/serviceManagerService/externalClient/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/communication/threadManager/*.cpp")
		SET (LIB_SRC_NOSVR ${LIB_SRC})
	ENDIF()
	IF (${MAPBAR_PLATFORM} MATCHES "^(fsl_toyota_19dop)$")
		remove_definitions(-DMAPBAR_WIN32)
		remove_definitions(-DMAPBAR_TOYOTA_262AVN)
		add_definitions(-DMAPBAR_TOYOTA)
		add_definitions(-DMAPBAR_TOYOTA_19DOP)
		FILE (GLOB LIB_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"  "${CMAKE_CURRENT_SOURCE_DIR}/src/communication/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/communication/ipcwrapper_toyota/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/externalMbApi/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/serviceManagerCommon/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/serviceManagerService/engine/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/serviceManagerService/externalClient/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/communication/threadManager/*.cpp")
		SET (LIB_SRC_NOSVR ${LIB_SRC})
	ENDIF()
	IF (${MAPBAR_PLATFORM} MATCHES "^(fsl_toyota_262avn)$")
		remove_definitions(-DMAPBAR_WIN32)
		remove_definitions(-DMAPBAR_TOYOTA_19DOP)
		add_definitions(-DMAPBAR_TOYOTA)
		add_definitions(-DMAPBAR_TOYOTA_262AVN)
		FILE (GLOB LIB_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"  "${CMAKE_CURRENT_SOURCE_DIR}/src/communication/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/communication/ipcwrapper_toyota/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/externalMbApi/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/serviceManagerCommon/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/serviceManagerService/engine/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/serviceManagerService/externalClient/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/communication/threadManager/*.cpp")
		SET (LIB_SRC_NOSVR ${LIB_SRC})
	ENDIF()
	IF (${MAPBAR_PLATFORM} MATCHES "^(linux_x64|fsl|fsl_hf|fsl_hf_desaysv_gwm)$")
		remove_definitions(-DMAPBAR_WIN32)
		add_definitions(-DMAPBAR_DESAY)
		FILE (GLOB LIB_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"  "${CMAKE_CURRENT_SOURCE_DIR}/src/communication/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/communication/ipcwrapper_desai/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/externalMbApi/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/serviceManagerCommon/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/serviceManagerService/engine/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/serviceManagerService/externalClient/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/communication/threadManager/*.cpp")
		SET (LIB_SRC_NOSVR ${LIB_SRC})
	ENDIF()

	USE_PCH("${CMAKE_CURRENT_SOURCE_DIR}/src/stdafx_serviceManager.h")
	ADD_NC_LIBRARY (${lib_name} STATIC ${LIB_SRC})	
ENDIF()