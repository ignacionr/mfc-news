cmake_minimum_required(VERSION 3.20)
project(MfcNews LANGUAGES CXX RC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable MFC (1 = Static Library, 2 = Shared DLL)
set(CMAKE_MFC_FLAG 1)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Find TinyXML2
find_package(tinyxml2 CONFIG REQUIRED)

# Sources list
set(SRC_FILES
    src/framework.h
    src/targetver.h
    src/resource.h
    src/mfc_news.rc
    src/mfc_news.h
    src/mfc_news.cpp
    src/MainFrm.h
    src/MainFrm.cpp
    src/mfc_newsDoc.h
    src/mfc_newsDoc.cpp
    src/mfc_newsView.h
    src/mfc_newsView.cpp
    src/ArticleView.h
    src/ArticleView.cpp
    src/rss/html_media_extractor.hpp
    src/rss/rss_date_parser.hpp
    src/rss/rss_date_parser.cpp
    src/rss/feed_item.hpp
    src/rss/feed_item.cpp
    src/rss/feed.hpp
)

# Build a WIN32 GUI executable
add_executable(MfcNews WIN32 ${SRC_FILES})

# Compiler definitions
target_compile_definitions(MfcNews PRIVATE
    UNICODE
    _UNICODE
)

# Include directories
target_include_directories(MfcNews PRIVATE src)

# Link dependencies
target_link_libraries(MfcNews PRIVATE tinyxml2::tinyxml2)

# Enable precompiled headers if on MSVC for faster builds
if(MSVC)
    target_compile_options(MfcNews PRIVATE /W4)
    target_link_options(MfcNews PRIVATE /MAP)
endif()
