cmake_minimum_required(VERSION 3.22)
# Not "html2pdf": the deploy scripts name their server folder after it, and sc-html2pdf has that one.
set(SC_MODULE html2pdf-app)
project(html2pdf HOMEPAGE_URL "https://github.com/roelofrossouw/html2pdf" LANGUAGES CXX)
include(cmake/sc_bootstrap.cmake)
include(GNUInstallDirs)
include(CTest)
set(CPACK_PACKAGE_NAME html2pdf) # before sc_program_info(): its licence files are under this name

get_sc_version()
message(STATUS "html2pdf Version: ${SC_VERSION}")

find_or_install_package(sc-core simply-cpp-dev simply-cpp 1.20.0)
find_or_install_package(sc-html2pdf simply-cpp-html2pdf-dev simply-cpp-html2pdf 1.3.0)

add_executable(${PROJECT_NAME} html2pdf.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE sc::sc-core sc::sc-html2pdf-shared)
target_compile_definitions(${PROJECT_NAME} PRIVATE HTML2PDF_VERSION=\"${SC_VERSION}\")
sc_program_info(${PROJECT_NAME}) # --license and --license-text

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES LICENSE DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/${PROJECT_NAME} RENAME copyright)
install(FILES NOTICE DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/${PROJECT_NAME}) # the Apache License asks that it go along

if (BUILD_TESTING)
    # Converts a page with a header, footer and table of contents, and checks the PDF it writes.
    add_test(NAME test-html2pdf
            COMMAND ${CMAKE_COMMAND} -DHTML2PDF=$<TARGET_FILE:${PROJECT_NAME}> -DSOURCE=${CMAKE_CURRENT_SOURCE_DIR}/tests
            -DWORK=${CMAKE_CURRENT_BINARY_DIR}/test-html2pdf -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/convert.cmake)
endif ()

if (APPLE)
    set(HTML2PDF_CODENAME apple)
    set(CPACK_GENERATOR TGZ)
elseif (UNIX)
    execute_process(COMMAND lsb_release -sc OUTPUT_VARIABLE HTML2PDF_CODENAME OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(CPACK_GENERATOR DEB)
endif ()
set(CPACK_PACKAGE_VERSION "${SC_VERSION}~${HTML2PDF_CODENAME}")
set(CPACK_PACKAGE_CONTACT "Roelof Rossouw")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "HTML files as PDFs")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Roelof Rossouw <roelof@roelof.co.za>")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "HTML files as PDFs, with headers, footers, page numbers and a table of contents, for Ubuntu ${HTML2PDF_CODENAME}")
set(CPACK_DEBIAN_PACKAGE_SECTION utils)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
# Our versions end in ~<codename>, which sorts below the bare version: a minimum ends in ~.
set(CPACK_DEBIAN_PACKAGE_DEPENDS "simply-cpp-html2pdf (>= 1.3.0~)")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_SET_DESTDIR ON)
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "${PROJECT_HOMEPAGE_URL}")
include(CPack)
