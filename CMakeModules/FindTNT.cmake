# - Find TNT
#
#  TNT_INCLUDE_DIRS - where to find tnt.h, etc.
#  TNT_LIBRARIES    - empty : THERE IS NO TNT LIBS!!
#  TNT_FOUND        - True if TNT found.

# Look for the header file.
FIND_PATH(TNT_INCLUDE_DIR tnt.h
	"$ENV{TNT_ROOT}/include/TNT"
	"$ENV{TNT_ROOT}"
	"$ENV{NR_ROOT}/include/TNT"
	"$ENV{TNT_ROOT}/include"
	"$ENV{NR_ROOT}/include"
	/usr/local/include/TNT
	/usr/local/include
	/usr/include/TNT
	/usr/include/tnt
	/usr/include
	"C:/Program Files/NRandTNT/include/TNT"
	"$ENV{ProgramFiles}/NRandTNT/include/TNT")
	
MARK_AS_ADVANCED(TNT_INCLUDE_DIR)
  
# handle the QUIETLY and REQUIRED arguments and set TNT_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TNT DEFAULT_MSG TNT_INCLUDE_DIR)

IF(TNT_FOUND)
  SET(TNT_INCLUDE_DIRS ${TNT_INCLUDE_DIR})
ELSE(TNT_FOUND)
  SET(TNT_INCLUDE_DIRS)
ENDIF(TNT_FOUND)
