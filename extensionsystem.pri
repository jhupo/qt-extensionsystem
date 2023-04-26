INCLUDEPATH             += $$top_srcdir/extensionsystem/

win32{
QMAKE_CXXFLAGS          += /MP
LIBS                    += $$top_builddir/bin/extensionsystem.lib
}else{
LIBS                    += $$top_builddir/bin/libextensionsystem.so
}
