# - Find EXX
# Componants can be :
# exxXmlUtils
# exxXslUtils
# exxXsdTree
# exxXmlTree
# exxHumanTree
# exxWidgets
#
#  REMEMBER : The order of the component is important.
#
#  EXX_INCLUDE_DIRS - where to find digiplant/Utils/FromString.h, etc.
#  EXX_LIBRARIES    - List of libraries when using EXX.
#  EXX_FOUND        - True if EXX found.

# Look for the header file.
FIND_PATH(EXX_INCLUDE_DIR exxGui/exxWidgets/Document.h
	/usr/local/include/libexx
	/usr/include/libexx
	"$ENV{DIGIPLANT_DIR}/include/libexx"
	"$ENV{DIGIPLANT_ROOT}/include/libexx"
	"$ENV{APPDATA}/digiplant/include/libexx"
	"C:/Program Files/Digiplant/include/libexx"
	/usr/local/include
	/usr/include
	"$ENV{EXX_ROOT}/include"
	"$ENV{EXX_ROOT}/include/libexx"
	"$ENV{DIGIPLANT_DIR}/include"
	"$ENV{DIGIPLANT_ROOT}/include"
	"$ENV{APPDATA}/digiplant/include"
	"C:/Program Files/Digiplant/include")
	
MARK_AS_ADVANCED(EXX_INCLUDE_DIR)

# Look for the library.
SET(SEARCH_PATH /usr/lib
  /usr/local/lib 
  /opt/local/lib
  "$ENV{DIGIPLANT_DIR}/lib"
  "$ENV{DIGIPLANT_ROOT}/lib"
  "$ENV{EXX_ROOT}/lib"
  "$ENV{APPDATA}/digiplant/lib"
  "C:/Program Files/Digiplant/lib")
  

FOREACH(COMPONENT ${Exx_FIND_COMPONENTS})

    IF(CMAKE_BUILD_TYPE MATCHES "Debug")
        SET(COMPONENT ${COMPONENT}d)
    ENDIF(CMAKE_BUILD_TYPE MATCHES "Debug")

    FIND_LIBRARY(${COMPONENT}_LIBRARY
		NAMES ${COMPONENT} 
		PATHS ${SEARCH_PATH})
		
		MARK_AS_ADVANCED(${COMPONENT}_LIBRARY)
		SET(EXX_LIBRARIES ${EXX_LIBRARIES} ${${COMPONENT}_LIBRARY})    
	
ENDFOREACH(COMPONENT)

# handle the QUIETLY and REQUIRED arguments and set EXX_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Exx DEFAULT_MSG EXX_INCLUDE_DIR EXX_LIBRARIES)

IF(EXX_FOUND)
  SET(EXX_INCLUDE_DIRS ${EXX_INCLUDE_DIR})
ELSE(EXX_FOUND)
  SET(EXX_INCLUDE_DIRS)
ENDIF(EXX_FOUND)
