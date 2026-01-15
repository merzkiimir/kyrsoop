cmake_minimum_required(VERSION 3.19)
project(qttrip2 LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Sql)

qt_standard_project_setup()

qt_add_executable(qttrip2
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    src/contact.cpp
    src/phonebook.cpp
    src/contactdialog.h src/contactdialog.cpp
    src/searchdialog.h src/searchdialog.cpp
)

target_include_directories(qttrip2 PRIVATE src)

target_link_libraries(qttrip2
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::Sql
)

include(GNUInstallDirs)

install(TARGETS qttrip2
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET qttrip2
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
