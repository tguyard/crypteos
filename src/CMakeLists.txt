
SET(EXE_NAME crypto)
INCLUDE(Utils)

#FIND_PACKAGE(Boost 1.42.0 COMPONENTS system filesystem math_tr1 REQUIRED)
#INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

ADD_SUBDIRECTORY(gcryptWrapper)

FILE(GLOB_RECURSE SRC *.cpp *.h)
ADD_EXECUTABLE(${EXE_NAME} ${SRC})

TARGET_LINK_LIBRARIES(${EXE_NAME} gcryptWrapper)

# To make folder in visual.
GROUP_FILES()

###Install
INSTALL(DIRECTORY "./"  
        DESTINATION "include/CryptPasswords" 
        COMPONENT headers 
        FILES_MATCHING PATTERN "*.h"
		PATTERN ".svn" EXCLUDE)

INSTALL( TARGETS ${EXE_NAME} RUNTIME DESTINATION bin COMPONENT applications)

