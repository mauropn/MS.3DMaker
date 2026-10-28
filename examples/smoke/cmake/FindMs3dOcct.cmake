# Resolve OpenCASCADE for MS3DMaker smoke tests (Windows + Linux).
if(NOT DEFINED ENV{CASROOT})
  if(EXISTS "C:/Dev/OCCT/install")
    set(ENV{CASROOT} "C:/Dev/OCCT/install")
  elseif(EXISTS "$ENV{HOME}/Dev/OCCT/install")
    set(ENV{CASROOT} "$ENV{HOME}/Dev/OCCT/install")
  elseif(EXISTS "F:/Dev/OCCT/install")
    set(ENV{CASROOT} "F:/Dev/OCCT/install")
  endif()
endif()

if(DEFINED ENV{CASROOT} AND NOT OpenCASCADE_DIR)
  set(_ms3d_occt_candidates
    "$ENV{CASROOT}/cmake"
    "$ENV{CASROOT}/lib/cmake/opencascade"
    "$ENV{CASROOT}/lib/cmake/OpenCASCADE"
  )
  foreach(_d IN LISTS _ms3d_occt_candidates)
    if(EXISTS "${_d}/OpenCASCADEConfig.cmake")
      set(OpenCASCADE_DIR "${_d}" CACHE PATH "OCCT CMake package dir" FORCE)
      break()
    endif()
  endforeach()
endif()

find_package(OpenCASCADE REQUIRED)
