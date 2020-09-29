include_directories (
    ${KDE1_INCLUDE_DIR}
)

set(kiconedit_SRCS
    utils.cpp main.cpp kiconedit.cpp kicongrid.cpp kiconcolors.cpp kcolorgrid.cpp
	kiconfilebase.cpp kiconfiledlg.cpp kiconcombiview.cpp kiconfileview.cpp kicondetaillist.cpp
	kicontools.cpp kicon.cpp kresize.cpp knew.cpp debug.cpp properties.cpp
	kiconeditslots.cpp kiconconfig.cpp kdropsite.cpp kdragsource.cpp
)

set(kiconedit_HEADERS
    debug.h
    kcolorgrid.h
    kdragsource.h
    kdropsite.h
    kiconcolors.h
    kiconcombiview.h
    kiconconfig.h
    kicondetaillist.h
    kiconedit.h
    kiconfilebase.h
    kiconfiledlg.h
    kiconfileview.h
    kicongrid.h
    kicon.h
    kicontools.h
    knew.h
    kresize.h
    main.h
    properties.h
    utils.h
    version.h
    )

QT1_WRAP_CPP(MOC_FILES ${kiconedit_HEADERS})

add_executable(kiconedit ${kiconedit_SRCS} ${MOC_FILES})
target_link_libraries(kiconedit
    Qt::Qt1
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${KDE1_KFM}
    ${KDE1_KFILE}
    ${KDE1_KIMGIO}
)

install(TARGETS kiconedit RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES kiconedit.kdelnk DESTINATION ${KDE1_APPSDIR}/Graphics)

add_subdirectory(pics)

install(FILES
    doc/en/logotp3.gif
    doc/en/index.html
    doc/en/index-1.html
    doc/en/index-2.html
    doc/en/index-3.html
    doc/en/index-4.html
DESTINATION ${KDE1_HTMLDIR}/en/kiconedit)

install(FILES
    doc/da/logotp3.gif
    doc/da/index.html
    doc/da/index-1.html
    doc/da/index-2.html
    doc/da/index-3.html
    doc/da/index-4.html
DESTINATION ${KDE1_HTMLDIR}/da/kiconedit)
