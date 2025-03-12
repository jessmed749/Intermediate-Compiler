add_library(utils utils/debug_assert.cpp ${CMAKE_SOURCE_DIR}/include/debug_assert.h)
target_include_directories(utils PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_options(utils PUBLIC -fsanitize=address,undefined -fsanitize-address-use-after-scope)
target_link_options(utils PUBLIC -fsanitize=address,undefined)

add_library(cool-support)
target_sources(cool-support
PRIVATE
    cool-support/ast_lex.cpp
    cool-support/ast_parse.cpp
    cool-support/cool_tree.cpp
    cool-support/dumptype.cpp
    cool-support/stringtab.cpp
    cool-support/tree.cpp
    cool-support/utils.cpp
PUBLIC
    "${CMAKE_SOURCE_DIR}/include/cool-support/ast_parse.h"
    "${CMAKE_SOURCE_DIR}/include/cool-support/cool_tree.h"
    "${CMAKE_SOURCE_DIR}/include/cool-support/copyright.h"
    "${CMAKE_SOURCE_DIR}/include/cool-support/stringtab.h"
    "${CMAKE_SOURCE_DIR}/include/cool-support/symtab.h"
    "${CMAKE_SOURCE_DIR}/include/cool-support/tree.h"
    "${CMAKE_SOURCE_DIR}/include/cool-support/utils.h"
    "${CMAKE_SOURCE_DIR}/include/cool-support/symbol.h"
    "${CMAKE_SOURCE_DIR}/include/cool_tree.handcode.h"
    "${CMAKE_SOURCE_DIR}/include/stringtab.handcode.h"
)
target_include_directories(cool-support PUBLIC "${CMAKE_SOURCE_DIR}/include/cool-support" "${CMAKE_SOURCE_DIR}/include")
target_compile_options(cool-support PRIVATE -O3 -UNDEBUG -Wno-write-strings -Wno-register -fsanitize=address,undefined -fsanitize-address-use-after-scope)
target_link_libraries(cool-support PUBLIC utils)
target_link_options(cool-support PRIVATE -fsanitize=address,undefined)

add_library(lab2-support)
target_sources(lab2-support
PRIVATE
        CgenClassTable.cpp
        CgenEnvironment.cpp
        CgenNode.cpp
PUBLIC
        "${CMAKE_SOURCE_DIR}/include/CgenClassTable.h"
        "${CMAKE_SOURCE_DIR}/include/CgenEnvironment.h"
        "${CMAKE_SOURCE_DIR}/include/CgenNode.h"
)
llvm_map_components_to_libnames(llvm_libs support core irreader transformutils Passes)
target_include_directories(lab2-support PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(lab2-support PUBLIC cool-support ${llvm_libs})
target_compile_options(lab2-support PRIVATE -fsanitize=address,undefined -fsanitize-address-use-after-scope)
target_link_options(lab2-support PRIVATE -fsanitize=address,undefined)

add_library(lab2)
target_sources(lab2
PRIVATE
    cgen.cpp
)
target_include_directories(lab2 PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_link_libraries(lab2 PUBLIC lab2-support)
target_compile_options(lab2 PRIVATE -fsanitize=address,undefined -fsanitize-address-use-after-scope)
target_link_options(lab2 PRIVATE -fsanitize=address,undefined)
target_precompile_headers(lab2 PRIVATE
    <llvm/Support/FileSystem.h>
    <sstream>
    <iostream>
    <string>
    "${CMAKE_SOURCE_DIR}/include/CgenClassTable.h"
    "${CMAKE_SOURCE_DIR}/include/CgenEnvironment.h"
    "${CMAKE_SOURCE_DIR}/include/CgenNode.h"
)


add_executable(debugir debugir/Main.cpp debugir/DebugIR.cpp)
target_compile_options(debugir PRIVATE -O3)
target_link_libraries(debugir PRIVATE ${llvm_libs})
target_include_directories(debugir PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_link_options(debugir PRIVATE -fsanitize=address,undefined)
