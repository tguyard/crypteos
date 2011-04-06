# - Find ELS
#
#  ELS_INCLUDE_DIRS - where to find digiplant/Utils/FromString.h, etc.
#  ELS_LIBRARIES    - List of libraries when using ELS.
#  ELS_FOUND        - True if ELS found.

# Look for the header file.
FIND_PATH(ELS_INCLUDE_DIR ELS/Log/TrivialLogger.h
	/usr/local/include
	/usr/include
	"$ENV{ELS_ROOT}/include"
	"C:/Program Files/ELS/include"
)
MARK_AS_ADVANCED(ELS_INCLUDE_DIR)

# handle the QUIETLY and REQUIRED arguments and set ELS_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ELS DEFAULT_MSG ELS_INCLUDE_DIR)

IF(ELS_FOUND)
  SET(ELS_INCLUDE_DIRS ${ELS_INCLUDE_DIR})
ELSE(ELS_FOUND)
  SET(ELS_INCLUDE_DIRS)
ENDIF(ELS_FOUND)
