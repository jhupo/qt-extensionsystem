include($$top_srcdir/library.pri)
QT                      +=              core
CONFIG                  +=              shared
TEMPLATE                 =              lib
DESTDIR                  =              $$top_builddir/bin
MOC_DIR                 +=              $$top_builddir/extensionsystem/moc
UI_DIR                  +=              $$top_builddir/extensionsystem/uic
RCC_DIR                 +=              $$top_builddir/extensionsystem/rcc
OBJECTS_DIR             +=              $$top_builddir/extensionsystem/obj
CONFIG                  -=              debug_and_release
DEFINES                 +=              EXTENSIONSYSTEM_LIBRARY
QMAKE_CXXFLAGS_RELEASE   =              $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE     =              $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

win32:QMAKE_CXXFLAGS    +=              /MP
SRC_DIR = $$top_srcdir/extensionsystem

for(var, SRC_DIR){
    SOURCES         += $$files($$join(var, , , /*.cpp) , true)
    SOURCES         += $$files($$join(var, , , /*.c)   , true)
    HEADERS         += $$files($$join(var, , , /*.h)   , true)
    HEADERS         += $$files($$join(var, , , /*.hpp) , true)
    FORMS           += $$files($$join(var, , , /*.ui)  , true)
    RESOURCES       += $$files($$join(var, , , /*.qrc) , true)
    TRANSLATIONS    += $$files($$join(var, , , /*.ts)  , true)
}

