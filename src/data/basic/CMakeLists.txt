set(INCLUDE_DIRECTORIES
  ${INCLUDE_DIRECTORIES}
  ${CMAKE_CURRENT_SOURCE_DIR}
  PARENT_SCOPE
)

set(SOURCE
  ${SOURCE}
  ${CMAKE_CURRENT_SOURCE_DIR}/Id.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/SNP.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Person.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/EnvironmentFactor.cpp
  PARENT_SCOPE
)
