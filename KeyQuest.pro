# Basic Qt configuration
QT       += core gui multimedia widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++17

# Compilation optimization settings
CONFIG += optimize_full parallel_mode precompile_header unity_build
PRECOMPILED_HEADER = stable.h
UNITY_BUILD_BATCH_SIZE = 4

# Optimize for lower memory usage and compilation speed
unix:!macx {
    QMAKE_CXXFLAGS += -MP -Os -ffunction-sections -fdata-sections
    QMAKE_LFLAGS += -Wl,--gc-sections
}

macx {
    QMAKE_CXXFLAGS += -MP -Os
    QMAKE_LFLAGS += -Wl,-dead_strip
}

# Increase make timeout and set memory-aware parallel jobs
unix:!macx {
    QMAKE_MAKEFLAGS += -j4
    # Enable ccache if available
    CCACHE = $$system(which ccache)
    !isEmpty(CCACHE) {
        QMAKE_CXX = ccache $$QMAKE_CXX
    }
    # Linux-specific includes and libs
    INCLUDEPATH += /usr/include
    LIBS += -L/usr/lib -lfluidsynth
}

macx {
    QMAKE_MAKEFLAGS += -j4
    # Enable ccache if available
    CCACHE = $$system(which ccache)
    !isEmpty(CCACHE) {
        QMAKE_CXX = ccache $$QMAKE_CXX
    }
    # macOS-specific includes and libs
    INCLUDEPATH += /opt/homebrew/Cellar/qt/6.7.3/include
    INCLUDEPATH += /opt/homebrew/Cellar/qt/6.7.3/lib/QtCore.framework/Headers
    INCLUDEPATH += /opt/homebrew/Cellar/qt/6.7.3/lib/QtGui.framework/Headers
    INCLUDEPATH += /opt/homebrew/Cellar/qt/6.7.3/lib/QtWidgets.framework/Headers
    INCLUDEPATH += /opt/homebrew/Cellar/qt/6.7.3/lib/QtMultimedia.framework/Headers
    INCLUDEPATH += /opt/homebrew/include
    LIBS += -L/opt/homebrew/Cellar/qt/6.7.3/lib
    LIBS += -F/opt/homebrew/Cellar/qt/6.7.3/lib
    LIBS += -L/opt/homebrew/lib -lfluidsynth
}

win32 {
    QMAKE_MAKEFLAGS += -j4
}

# Release mode optimizations
CONFIG(release, debug|release) {
    QMAKE_CXXFLAGS_RELEASE -= -g -O2
    QMAKE_CXXFLAGS_RELEASE += -O1
    QMAKE_CFLAGS_RELEASE -= -g
    QMAKE_LFLAGS_RELEASE = -Wl,-O1
}

SOURCES += \
    adaptivequiz.cpp \
    backgroundpage.cpp \
    datamanager.cpp \
    keyboard.cpp \
    lessonsbackgroundpage.cpp \
    lessonsgame.cpp \
    lessonspage.cpp \
    lessonswidget.cpp \
    loaddatamanager.cpp \
    main.cpp \
    mainpage.cpp \
    mainwindow.cpp \
    mathutils.cpp \
    multiplayergame.cpp \
    multiplayergamewidget.cpp \
    navigationmanager.cpp \
    pianowidget.cpp \
    question.cpp \
    questionloader.cpp \
    quizwidget.cpp \
    soundmanager.cpp \
    statisticswidget.cpp

HEADERS += \
    adaptivequiz.h \
    backgroundpage.h \
    datamanager.h \
    keyboard.h \
    lessonsbackgroundpage.h \
    lessonsgame.h \
    lessonspage.h \
    lessonswidget.h \
    loaddatamanager.h \
    mainpage.h \
    mainwindow.h \
    mathutils.h \
    multiplayergame.h \
    multiplayergamewidget.h \
    navigationmanager.h \
    pianowidget.h \
    question.h \
    questionloader.h \
    quizreport.h \
    quizwidget.h \
    soundmanager.h \
    stable.h \
    state.h \
    statisticswidget.h

FORMS += \
    mainwindow.ui 

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += resources.qrc \
             soundFiles.qrc \
             lessonsImages.qrc \
             pianoImages.qrc \
             multiplayerImages.qrc
