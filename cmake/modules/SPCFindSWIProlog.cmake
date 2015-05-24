find_program(SWIPL_EXISTS swipl)

if(NOT SWIPL_EXISTS)
  return()
endif()

set(SPC_SWIPL_EXISTS true)

execute_process(
  COMMAND swipl --dump-runtime-variables
  OUTPUT_VARIABLE SWIPL_DUMP
)

foreach(S PLARCH;PLBASE;PLCFLAGS;PLLDFLAGS)
  string(REGEX MATCH "${S}=\"([^\n]*)\"" _ ${SWIPL_DUMP})
  set(${S} ${CMAKE_MATCH_1})

  if(NOT DEFINED ${S})
     message(SEND_ERROR "Could not find ${S} in SWI-Prolog runtime dump")
     return()
  endif()

endforeach()

set(SPC_SWIPL_INCLUDE_DIRS "${PLBASE}/include" "${swipl_cpp_SOURCE_DIR}/include")
set(SPC_SWIPL_DEFS "-D_REENTRANT -D__SWI_PROLOG__ -D__SWI_EMBEDDED__")
set(SPC_SWIPL_CXX_FLAGS "${PLCFLAGS}")
set(SPC_SWIPL_LDFLAGS "${PLLDFLAGS}")
set(SPC_SWIPL_LINK_DIR "${PLBASE}/lib/${PLARCH}")
set(SPC_SWIPL_LIB "lswipl")

