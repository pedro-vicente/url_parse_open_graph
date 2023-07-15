cmake_minimum_required(VERSION 3.15)
project (url_parse_open_graph)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#//////////////////////////
#asio
#//////////////////////////

set(CMAKE_CXX_STANDARD 17)
add_definitions(-DASIO_STANDALONE)
add_definitions(-DASIO_HAS_STD_ADDRESSOF)
add_definitions(-DASIO_HAS_STD_ARRAY)
add_definitions(-DASIO_HAS_CSTDINT)
add_definitions(-DASIO_HAS_STD_SHARED_PTR)
add_definitions(-DASIO_HAS_STD_TYPE_TRAITS)
add_definitions(-DASIO_HAS_VARIADIC_TEMPLATES)
add_definitions(-DASIO_HAS_STD_FUNCTION)
add_definitions(-DASIO_HAS_STD_CHRONO)
add_definitions(-DBOOST_ALL_NO_LIB)
if (MSVC)
  add_definitions(-D_WIN32_WINNT=0x0501)
  add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
endif()

include_directories(ext/asio-1.22.2/include)

#//////////////////////////
#link with libraries
#lib_dep contains a cascade definition of all the libraries needed to link
#//////////////////////////

set(lib_dep ${lib_dep})

#//////////////////////////
#openssl
#//////////////////////////

option(USE_OPENSSL "set OFF to build without OpenSSL" ON)

#//////////////////////////
#OpenSSL
#//////////////////////////

if(USE_OPENSSL)
  if (APPLE)
    set(OPENSSL_ROOT_DIR ${OPENSSL_ROOT_DIR} /usr/local/Cellar/openssl@1.1/1.1.1h/)
    set(OPENSSL_CRYPTO_LIBRARY ${OPENSSL_ROOT_DIR}/lib/libcrypto.dylib CACHE FILEPATH "" FORCE)
    set(OPENSSL_SSL_LIBRARY ${OPENSSL_ROOT_DIR}/lib/libssl.dylib CACHE FILEPATH "" FORCE)
  endif()
  if (MSVC)
    set(OPENSSL_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext/openssl-3.0.5)
    set(OPENSSL_INCLUDE_DIR ${OPENSSL_ROOT_DIR}/include)
    set(OPENSSL_CRYPTO_LIBRARY ${OPENSSL_ROOT_DIR}/libcrypto.lib)
    set(OPENSSL_SSL_LIBRARY ${OPENSSL_ROOT_DIR}/libssl.lib)
  endif()
  message(STATUS "OpenSSL root: ${OPENSSL_ROOT_DIR}")
  message(STATUS "OpenSSL include: ${OPENSSL_INCLUDE_DIR}")
  message(STATUS "OpenSSL libs: ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY}")
  find_package(OpenSSL REQUIRED)
  if(UNIX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
  endif()
  include_directories(${OPENSSL_INCLUDE_DIR})
  set(lib_dep ${lib_dep} ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY})
  if (MSVC)
    set(lib_dep ${lib_dep} crypt32.lib)
  endif()
  add_definitions(-DHAVE_OPENSSL)
endif()

#//////////////////////////
# http lib
#//////////////////////////

set(src ${src})
set(src ${src} src/ssl_read.hh)
set(src ${src} src/ssl_read.cc)
set(src ${src} src/graph.hh)

add_library(http ${src})

#//////////////////////////
# htmlcxx lib
#//////////////////////////

set(htm ${htm})
set(htm ${htm} src/htmlcxx/ParserSax.h )
set(htm ${htm} src/htmlcxx/ParserSax.tcc)
set(htm ${htm} src/htmlcxx/ParserSax.cc)
set(htm ${htm} src/htmlcxx/ParserDom.h)
set(htm ${htm} src/htmlcxx/ParserDom.cc)
set(htm ${htm} src/htmlcxx/Extensions.h)
set(htm ${htm} src/htmlcxx/Extensions.cc)
set(htm ${htm} src/htmlcxx/Uri.h)
set(htm ${htm} src/htmlcxx/Uri.cc)
set(htm ${htm} src/htmlcxx/Node.h)
set(htm ${htm} src/htmlcxx/Node.cc)
set(htm ${htm} src/htmlcxx/utils.h)
set(htm ${htm} src/htmlcxx/utils.cc)
set(htm ${htm} src/htmlcxx/ci_string.h)
set(htm ${htm} src/htmlcxx/tree.h)
set(htm ${htm} src/htmlcxx/debug.h)

add_library(htmlcxx ${htm})

set(lib_dep ${lib_dep} http htmlcxx)

add_executable(url_parse_open_graph src/url_parse_open_graph.cc)
target_link_libraries (url_parse_open_graph ${lib_dep})

#//////////////////////////
# IDE project virtual folders
#//////////////////////////

set_target_properties(htmlcxx PROPERTIES FOLDER "htmlcxx")

#//////////////////////////
# copy sample files
#//////////////////////////

message(STATUS "Copying files to: ${CMAKE_BINARY_DIR}")
file(COPY "${CMAKE_SOURCE_DIR}/samples/image-url.html" DESTINATION ${CMAKE_BINARY_DIR})

