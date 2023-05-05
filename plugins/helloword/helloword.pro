include($$top_srcdir/library.pri)
include($$top_srcdir/extensionsystem.pri)
QT                      +=              core gui widgets
CONFIG                  +=              plugin
TEMPLATE                 =              lib
DESTDIR                  =              $$top_builddir/bin/plugins
MOC_DIR                 +=              $$top_builddir/plugins/helloword/moc
UI_DIR                  +=              $$top_builddir/plugins/helloword/uic
RCC_DIR                 +=              $$top_builddir/plugins/helloword/rcc
OBJECTS_DIR             +=              $$top_builddir/plugins/helloword/obj
CONFIG                  -=              debug_and_release
DEFINES                 +=              HELLOWORD_LIBRARY
DISTFILES               +=              Helloword.json
QMAKE_CXXFLAGS_RELEASE   =              $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE     =              $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

win32:QMAKE_CXXFLAGS    +=              /MP
SRC_DIR = $$top_srcdir/plugins/helloword

for(var, SRC_DIR){
    SOURCES         += $$files($$join(var, , , /*.cpp) , true)
    SOURCES         += $$files($$join(var, , , /*.c)   , true)
    HEADERS         += $$files($$join(var, , , /*.h)   , true)
    HEADERS         += $$files($$join(var, , , /*.hpp) , true)
    FORMS           += $$files($$join(var, , , /*.ui)  , true)
    RESOURCES       += $$files($$join(var, , , /*.qrc) , true)
    TRANSLATIONS    += $$files($$join(var, , , /*.ts)  , true)
}

