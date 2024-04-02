add_compiler_rt_component(mince)

set(MINCE_SOURCES
  rtl.cc
  )

include_directories(..)

set(MINCE_CFLAGS
  ${SANITIZER_COMMON_CFLAGS})
append_rtti_flag(OFF MINCE_CFLAGS)

foreach(arch ${MINCE_SUPPORTED_ARCH})
  add_compiler_rt_runtime(clang_rt.mince
    STATIC
    ARCHS ${arch}
    SOURCES ${MINCE_SOURCES}
    $<TARGET_OBJECTS:RTInterception.${arch}>
    $<TARGET_OBJECTS:RTSanitizerCommon.${arch}>
    $<TARGET_OBJECTS:RTSanitizerCommonNoLibc.${arch}>
    CFLAGS ${MINCE_CFLAGS}
    PARENT_TARGET mince)
  add_sanitizer_rt_symbols(clang_rt.mince
    ARCHS ${arch}
    EXTRA mince.syms.extra)
endforeach()
