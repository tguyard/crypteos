# - Find NR
#
# Components can be :
# nr
# nrexample
#
#  NR_INCLUDE_DIRS - where to find the numerical recipes headers.
#  NR_LIBRARIES    - List of libraries when using NR.
#  NR_FOUND        - True if NR found.

# Look for the header file.
FIND_PATH(NR_INCLUDE_DIR nr.h
	"$ENV{NR_ROOT}/include/NR"
	"$ENV{NR_ROOT}/include"
	/usr/local/include
	/usr/local/include/NR
	/usr/include
	/usr/include/NR
	"C:/Program Files/NRandTNT/include/NR"
	"$ENV{ProgramFiles}/NRandTNT/include/NR")
	
MARK_AS_ADVANCED(NR_INCLUDE_DIR)

# Look for the library.
SET(SEARCH_PATH 
	"$ENV{NR_ROOT}/lib/NR"
	"$ENV{NR_ROOT}/lib"
	/usr/local/lib
	/usr/local/lib/NR
	/usr/lib
	/usr/lib/NR
	"C:/Program Files/NRandTNT/lib"
	"$ENV{ProgramFiles}/NRandTNT/lib")
  
FOREACH(COMPONENT ${NR_FIND_COMPONENTS})

    IF(CMAKE_BUILD_TYPE MATCHES "Debug")
        SET(COMPONENT ${COMPONENT}d)
    ENDIF(CMAKE_BUILD_TYPE MATCHES "Debug")

    FIND_LIBRARY(${COMPONENT}_LIBRARY
		NAMES ${COMPONENT}
		PATHS ${SEARCH_PATH})
		
	MARK_AS_ADVANCED(${COMPONENT}_LIBRARY)
	SET(NR_LIBRARIES ${NR_LIBRARIES} ${${COMPONENT}_LIBRARY})    
ENDFOREACH(COMPONENT)

# handle the QUIETLY and REQUIRED arguments and set NR_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(NR DEFAULT_MSG NR_INCLUDE_DIR NR_LIBRARIES)

IF(NR_FOUND)
  SET(NR_INCLUDE_DIRS ${NR_INCLUDE_DIR})
ELSE(NR_FOUND)
  SET(NR_INCLUDE_DIRS)
ENDIF(NR_FOUND)
