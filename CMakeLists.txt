cmake_minimum_required(VERSION 3.24 FATAL_ERROR)
project(uefi-hello VERSION 0.0.2)

option(WITH_GOP "Enable GOP code." OFF)

set(HELLO hello)
set(HELLO_EFI ${HELLO}.efi)
set(HELLO_ISO ${HELLO}.iso)
set(HELLO_SO ${HELLO}.so)

set(POSSIBLE_GNUEFI_LIBDIRS
    /lib
    /lib/gnuefi
    /lib64
    /lib64/gnuefi
    /usr/lib
    /usr/lib/gnuefi
    /usr/lib64
    /usr/lib64/gnuefi)
find_file(EFI_CRT_OBJS crt0-efi-${CMAKE_HOST_SYSTEM_PROCESSOR}.o PATHS ${POSSIBLE_GNUEFI_LIBDIRS}
                                                                       REQUIRED)
find_file(EFI_LDS elf_${CMAKE_HOST_SYSTEM_PROCESSOR}_efi.lds PATHS ${POSSIBLE_GNUEFI_LIBDIRS}
                                                                   REQUIRED)

add_library(${HELLO} main.c)
if(WITH_GOP)
  target_compile_definitions(${HELLO} PRIVATE GOP)
endif()
target_compile_options(${HELLO} PRIVATE -fno-stack-protector -fpic -fshort-wchar -mno-red-zone
                                        -Wall)
target_include_directories(${HELLO} PRIVATE /usr/include/efi /usr/include/efi/protocol)
if(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86_64")
  target_compile_definitions(${HELLO} PRIVATE EFI_FUNCTION_WRAPPER)
  target_include_directories(${HELLO} PRIVATE /usr/include/efi/${CMAKE_HOST_SYSTEM_PROCESSOR})
endif()

add_custom_target(
  ${HELLO_SO} ALL
  COMMAND ld -nostdlib -znocombreloc -T ${EFI_LDS} -shared -Bsymbolic -L /usr/lib64 -L /usr/lib
          ${EFI_CRT_OBJS} ${CMAKE_CURRENT_BINARY_DIR}/lib${HELLO}.a -o ${HELLO_SO} -lefi -lgnuefi
  DEPENDS ${HELLO})

add_custom_target(
  ${HELLO_EFI} ALL
  COMMAND objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc
          --target=efi-app-${CMAKE_HOST_SYSTEM_PROCESSOR} ${HELLO_SO} ${HELLO_EFI}
  DEPENDS ${HELLO_SO})

find_program(MKISOFS mkisofs)
if(NOT MKISOFS-NOTFOUND)
  add_custom_target(
    ${HELLO_ISO} ALL
    COMMAND mkisofs -J -R -o ${HELLO_ISO} ${HELLO}.efi
    DEPENDS ${HELLO_EFI})
endif()

if(WITH_TESTS AND CMAKE_COMPILER_IS_GNUCXX)
  enable_testing()
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
  find_package(cmocka REQUIRED)
  set(mocked_funcs "InitializeLib;Print;efi_call2;efi_call3;efi_call4")
  foreach(func ${mocked_funcs})
    list(APPEND test_flags "-Wl,--wrap,${func}")
  endforeach()
  add_executable(general_tests test.c)
  if(WITH_GOP)
    target_compile_definitions(general_tests PRIVATE GOP)
  endif()
  target_compile_definitions(general_tests PRIVATE EFI_FUNCTION_WRAPPER)
  target_compile_options(
    general_tests
    PRIVATE --coverage
            -fno-stack-protector
            -fpic
            -fshort-wchar
            -mno-red-zone
            -Wall
            -ggdb)
  target_include_directories(general_tests PRIVATE ${cmocka_INCLUDE_DIRS} /usr/include/efi
                                                   /usr/include/efi/protocol)
  target_link_options(general_tests PRIVATE ${test_flags})
  target_link_libraries(general_tests cmocka gcov)
  add_test(NAME general_tests COMMAND general_tests)
endif()
