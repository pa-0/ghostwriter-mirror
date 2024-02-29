# SPDX-FileCopyrightText: 2023 Renaud Guezennec <renaud@rolisteam.org>
#
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.16)

project(asynctextwritertest VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 5.15 REQUIRED COMPONENTS Core Test Concurrent Widgets)

if (MSVC)
    set(CMARK_LIBS
        libcmark-gfm
        libcmark-gfm-extensions
    )
else()
    set(CMARK_LIBS
        libcmark-gfm_static
        libcmark-gfm-extensions_static
    )
endif()

#if(QT5_FOUND)
add_executable(markdownEditTest
    tst_markdownedit.cpp
    tst_markdownedit.h
    ../../src/editor/markdowneditor.h
    ../../src/editor/markdowneditor.cpp
    ../../src/editor/markdowndocument.h
    ../../src/editor/markdowndocument.cpp
    ../../src/editor/markdownhighlighter.h
    ../../src/editor/markdownhighlighter.cpp
    ../../src/markdown/markdownnode.h
    ../../src/markdown/markdownnode.cpp
    ../../src/editor/colorscheme.h
    ../../src/markdowneditor_p.h
    ../../src/markdown/cmarkgfmapi.h
    ../../src/markdown/cmarkgfmapi.cpp
    ../../src/markdown/markdownast.h
    ../../src/markdown/markdownast.cpp
)

target_compile_definitions(markdownEditTest PUBLIC BUILD_TEST=1)

add_test(markdownEditTest markdownEditTest)
enable_testing(true)

target_link_libraries(markdownEditTest PRIVATE Qt5::Core Qt5::Test Qt5::Concurrent Qt5::Widgets ${CMARK_LIBS})
#endif()
