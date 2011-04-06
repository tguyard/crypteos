# - Find MPI
#
#  MPI_INCLUDE_DIRS - where to find digiplant/MPI/FromString.h, etc.
#  MPI_LIBRARIES    - List of libraries when using MPI.
#  MPI_FOUND        - True if MPI found.

INCLUDE(FindMPI)

SET(MPI_INCLUDE_DIR ${MPI_INCLUDE_PATH})

# Look for the library path.
SET(SEARCH_PATH /usr/lib
	/usr/local/lib 
	/opt/local/lib
	"$ENV{MPI_DIR}/lib"
	"$ENV{MPI_ROOT}/lib"
	"C:/Program Files/MPICH/SDK/Lib" 
	"$ENV{SystemDrive}/Program Files/MPICH/SDK/Lib"
	"$ENV{SystemDrive}/Program Files/Microsoft Compute Cluster Pack/Lib/${MS_MPI_ARCH_DIR}"
	"$ENV{SystemDrive}/Program Files/MPICH2/Lib"
	"C:/Program Files/MPICH/SDK/Lib" 
)

IF(NOT MPI_LIBRARY)
	find_library(MPI_LIBRARY 
		NAMES mpi mpich msmpi
		PATHS ${SEARCH_PATH}
	)
	IF(NOT MPI_LIBRARY)
		set (MPI_LIBRARIES ${MPI_LIBRARIES} ${MPI_LIBRARY})
	ENDIF(NOT MPI_LIBRARY)
ENDIF(NOT MPI_LIBRARY)

find_library(MPI_CXX_LIBRARY 
    NAMES cxx cxxd
    PATHS ${SEARCH_PATH}
)
	
FIND_PATH( MPI_LIB_DIR mpi.lib
	PATH
	${SEARCH_PATH}
)

IF(MPI_CXX_LIBRARY)
	SET(MPI_LIBRARIES ${MPI_LIBRARIES} ${MPI_CXX_LIBRARY})
ENDIF(MPI_CXX_LIBRARY)

if (MPI_INCLUDE_PATH AND MPI_LIBRARY)
  set(MPI_FOUND TRUE)
else (MPI_INCLUDE_PATH AND MPI_LIBRARY)
  set(MPI_FOUND FALSE)
endif (MPI_INCLUDE_PATH AND MPI_LIBRARY)

MARK_AS_ADVANCED(MPI_LIBRARY)