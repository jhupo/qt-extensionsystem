CONFIG      +=      skip_target_version_ext
unix:!mac{
    QMAKE_LFLAGS_RPATH=
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'" -Wl,--disable-new-dtags
}
