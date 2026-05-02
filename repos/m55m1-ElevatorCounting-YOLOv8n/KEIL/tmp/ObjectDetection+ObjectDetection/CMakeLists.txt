cmake_minimum_required(VERSION 3.27)

# Roots
include("../roots.cmake")

set(CONTEXT ObjectDetection+ObjectDetection)
set(TARGET ${CONTEXT})
set(DNAME M55M1H2LJAE)
set(DPACK Nuvoton::NuMicroM55_DFP@3.1.4-rc.3)
set(DPACK_DIR "${CMSIS_PACK_ROOT}/Nuvoton/NuMicroM55_DFP/3.1.4-rc.3")
set(OUT_DIR "${SOLUTION_ROOT}/out/ObjectDetection/ObjectDetection")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(LD_SCRIPT "${SOLUTION_ROOT}/M55M1.scatter")
set(LD_SCRIPT_PP ${LD_SCRIPT})

# Processor Options
set(CPU Cortex-M55)
set(FPU DP_FPU)
set(DSP DSP)
set(MVE FP_FVE)
set(BYTE_ORDER Little-endian)

# Toolchain config map
set(COMPILER AC6)
include("toolchain.cmake")

# Setup project
project(${CONTEXT} LANGUAGES C CXX)

# Enable color diagnostics
set(CMAKE_COLOR_DIAGNOSTICS ON)

# Compilation database
add_custom_target(database
  COMMAND ${CMAKE_COMMAND} -E make_directory "${OUT_DIR}"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json" "${OUT_DIR}"
)
set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES})
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})

# Setup context
add_executable(${CONTEXT})
set_target_properties(${CONTEXT} PROPERTIES PREFIX "" SUFFIX ".axf" OUTPUT_NAME "ObjectDetection")
set_target_properties(${CONTEXT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUT_DIR})
add_library(${CONTEXT}_GLOBAL INTERFACE)

# Includes
target_include_directories(${CONTEXT} PUBLIC
  $<$<COMPILE_LANGUAGE:C,CXX>:
    "${SOLUTION_ROOT}/../../../../Library/CMSIS/Core/Include"
    "${SOLUTION_ROOT}/../../../../Library/Device/Nuvoton/M55M1/Include"
    "${SOLUTION_ROOT}/../../../../Library/CMSIS/DSP/Include"
    "${SOLUTION_ROOT}/../../../../Library/StdDriver/inc"
    "${SOLUTION_ROOT}/../../../../Library/StdDriver/inc/npu"
    "${SOLUTION_ROOT}/../../../../ThirdParty/openmv/omv/imlib"
    "${SOLUTION_ROOT}/../../../../ThirdParty/openmv/omv/alloc"
    "${SOLUTION_ROOT}/../../../../ThirdParty/openmv/omv/common"
    "${SOLUTION_ROOT}/../../../../ThirdParty/openmv/omv/Lib"
    "${SOLUTION_ROOT}/../../../../ThirdParty/tflite_micro"
    "${SOLUTION_ROOT}/../../../../ThirdParty/tflite_micro/_deps/tensorflow-flatbuffers-src/include"
    "${SOLUTION_ROOT}/../../../../ThirdParty/ml-embedded-evaluation-kit/source/application/api/common/include"
    "${SOLUTION_ROOT}/../../../../ThirdParty/ml-embedded-evaluation-kit/source/log/include"
    "${SOLUTION_ROOT}/../../../../ThirdParty/ml-embedded-evaluation-kit/source/math/include"
    "${SOLUTION_ROOT}/../../../../ThirdParty/ml-embedded-evaluation-kit/source/application/main/include"
    "${SOLUTION_ROOT}/../../../../ThirdParty/ml-embedded-evaluation-kit/source/profiler/include"
    "${SOLUTION_ROOT}/../Device/include"
    "${SOLUTION_ROOT}/../Model/include"
    "${SOLUTION_ROOT}/../NPU/include"
    "${SOLUTION_ROOT}/../Pattern/include"
    "${SOLUTION_ROOT}/../ProfilerCounter/include"
    "${SOLUTION_ROOT}/../../../../ThirdParty/tflite_micro/_deps/tensorflow-gemlowp-src"
    "${SOLUTION_ROOT}/../../../../ThirdParty/FatFs/source"
  >
  "${CMSIS_PACK_ROOT}/Nuvoton/NuMicroM55_DFP/3.1.4-rc.3/Library/Device/Nuvoton/M55M1/Include"
  "${SOLUTION_ROOT}/.."
)

# Defines
target_compile_definitions(${CONTEXT} PUBLIC
  $<$<COMPILE_LANGUAGE:C,CXX>:
    NVT_VECTOR_ON_FLASH
    TF_LITE_STATIC_MEMORY
    ARM_MATH_DSP
    ARM_NPU
    ACTIVATION_BUF_SZ=1048576
  >
)

# Compile options
target_compile_options(${CONTEXT} PUBLIC
  $<$<COMPILE_LANGUAGE:C>:
    "SHELL:${CC_CPU}"
    "SHELL:${CC_FLAGS}"
    "SHELL:${CC_SECURE}"
    "SHELL:${CC_BYTE_ORDER}"
    "SHELL:-std=c11"
    "SHELL:-fno-rtti"
    "SHELL:-funsigned-char"
    "SHELL:-fshort-enums"
    "SHELL:-fshort-wchar"
    "SHELL:-ffunction-sections"
    "SHELL:-Wno-packed"
    "SHELL:-Wno-missing-variable-declarations"
    "SHELL:-Wno-missing-prototypes"
    "SHELL:-Wno-missing-noreturn"
    "SHELL:-Wno-sign-conversion"
    "SHELL:-Wno-nonportable-include-path"
    "SHELL:-Wno-reserved-id-macro"
    "SHELL:-Wno-unused-macros"
    "SHELL:-Wno-documentation-unknown-command"
    "SHELL:-Wno-documentation"
    "SHELL:-Wno-license-management"
    "SHELL:-Wno-parentheses-equality"
  >
  $<$<COMPILE_LANGUAGE:CXX>:
    "SHELL:${CXX_CPU}"
    "SHELL:${CXX_FLAGS}"
    "SHELL:${CXX_SECURE}"
    "SHELL:${CXX_BYTE_ORDER}"
    "SHELL:-xc++"
    "SHELL:-std=c++17"
    "SHELL:-fno-exceptions"
    "SHELL:-fno-rtti"
    "SHELL:-funsigned-char"
    "SHELL:-fshort-enums"
    "SHELL:-fshort-wchar"
    "SHELL:-ffunction-sections"
    "SHELL:-Wno-packed"
    "SHELL:-Wno-missing-variable-declarations"
    "SHELL:-Wno-missing-prototypes"
    "SHELL:-Wno-missing-noreturn"
    "SHELL:-Wno-sign-conversion"
    "SHELL:-Wno-nonportable-include-path"
    "SHELL:-Wno-reserved-id-macro"
    "SHELL:-Wno-unused-macros"
    "SHELL:-Wno-documentation-unknown-command"
    "SHELL:-Wno-documentation"
    "SHELL:-Wno-license-management"
    "SHELL:-Wno-parentheses-equality"
  >
)

# Compile Options Abstractions
add_library(${CONTEXT}_ABSTRACTIONS INTERFACE)
cbuild_set_options_flags(CC "debug" "on" "" "" CC_OPTIONS_FLAGS_${CONTEXT})
cbuild_set_options_flags(CXX "debug" "on" "" "" CXX_OPTIONS_FLAGS_${CONTEXT})
target_compile_options(${CONTEXT}_ABSTRACTIONS INTERFACE
  $<$<COMPILE_LANGUAGE:C>:
    "SHELL:${CC_OPTIONS_FLAGS_${CONTEXT}}"
  >
  $<$<COMPILE_LANGUAGE:CXX>:
    "SHELL:${CXX_OPTIONS_FLAGS_${CONTEXT}}"
  >
)

# Add groups and components
include("groups.cmake")
include("components.cmake")

target_link_libraries(${CONTEXT} PUBLIC
  Group_CMSIS
  Group_Driver
  Group_Application
  Group_Lib
  Group_Model
  Group_ArmMLApi
  Group_NPU
  Group_ProfilerCounter
  Group_Device
  Group_FatFS
  Group_Pattern
  ${SOLUTION_ROOT}/../../../../Library/CMSIS/Lib/KEIL/cmsis_dsp.lib
  ${SOLUTION_ROOT}/../../../../ThirdParty/tflite_micro/Lib/tflu.lib
  ${SOLUTION_ROOT}/../../../../Library/CMSIS/Lib/KEIL/cmsis_nn.lib
  ${SOLUTION_ROOT}/../../../../ThirdParty/openmv/omv/Lib/omv.lib
)

# Linker options
target_link_options(${CONTEXT} PUBLIC
  "SHELL:${LD_CPU}"
  "SHELL:${_LS}\"${LD_SCRIPT_PP}\""
  "SHELL:${LD_SECURE}"
  "SHELL:--map"
  "SHELL:--load_addr_map_info"
  "SHELL:--xref"
  "SHELL:--callgraph"
  "SHELL:--symbols"
  "SHELL:--info sizes"
  "SHELL:--info totals"
  "SHELL:--info unused"
  "SHELL:--info veneers"
  "SHELL:--strict"
  "SHELL:--diag_suppress=6314"
  "SHELL:--summary_stderr"
  "SHELL:--info summarysizes"
)
set(LD_DEPS ${LD_SCRIPT})
set_target_properties(${CONTEXT} PROPERTIES LINK_DEPENDS "${LD_DEPS}")
