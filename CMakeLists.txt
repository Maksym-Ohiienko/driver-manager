cmake_minimum_required(VERSION 3.16)

project(DriverManager VERSION 1.0 LANGUAGES CXX)

# Встановлення автоматичної обробки UI, MOC і ресурсів у Qt
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Встановлення стандарту C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Пошук Qt (підтримка Qt5 і Qt6)
set(CMAKE_PREFIX_PATH "C:/Qt/6.6.1/msvc2019_64")
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Quick AxContainer WebEngineCore Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Quick AxContainer WebEngineCore Concurrent)

# Точка входу проєкту
set(PROJECT_SOURCES
    main.cpp
)

# Створення файлу, що виконується, залежить від версії Qt
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)

    qt_add_executable(
        DriverManager
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}

        driver.h driver.cpp
        driverlistmodel.h driverlistmodel.cpp
        drivermaster.h drivermaster.cpp
        downloadhelper.h downloadhelper.cpp
        threadmanager.h threadmanager.cpp
        resources.qrc
    )
else()
    if(ANDROID)
        add_library(DriverManager SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(DriverManager
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Лінкування з бібліотеками
target_link_libraries(DriverManager PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::AxContainer
    Qt${QT_VERSION_MAJOR}::WebEngineCore
    Qt${QT_VERSION_MAJOR}::Concurrent)

# Налаштування для Windows
if(WIN32)
    # Встановлення властивості WIN32_EXECUTABLE для Windows
    set_target_properties(DriverManager PROPERTIES WIN32_EXECUTABLE TRUE)

endif()

# Встановлення шляху
include(GNUInstallDirs)
install(TARGETS DriverManager
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Фіналізація для Qt6
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(DriverManager)
endif()

