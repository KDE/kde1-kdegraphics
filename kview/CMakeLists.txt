#add_subdirectory(doc)

include_directories (
    ${KDE1_INCLUDE_DIR}
    layout/
)

set(KVIEW_SRCS
    canvas.cpp
    colour.cpp
    filter.cpp
    filtlist.cpp
    filtmenu.cpp
    ilistdlg.cpp
    kaccelmenuwatch.cpp
    kcproc.cpp
    khelpidx.cpp
    kview.cpp
    main.cpp
    numdlg.cpp
    prefdlg.cpp
    viewer.cpp

    bagconst.cpp
    baglayout.cpp
    typolayout.cpp
)

set(KVIEW_MOC
    baglayout.moc
    canvas.moc
    colour.moc
    filter.moc
    filtmenu.moc
    ilistdlg.moc
    kaccelmenuwatch.moc
    kview.moc
    prefdlg.moc
    typolayout.moc
    viewer.moc
    )

QT1_WRAP_MOC(MOC_FILES ${KVIEW_MOC})

add_executable(kview ${KVIEW_SRCS} ${MOC_FILES})
target_link_libraries(kview
    Qt::Qt1
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${KDE1_KFM}
    ${KDE1_KFILE}
    ${KDE1_KIMGIO}
)

install(TARGETS kview RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES kview.kdelnk DESTINATION ${KDE1_APPSDIR}/Graphics)
install_icon(kview.xpm)

install(FILES
    doc/en/kview.index
    doc/en/kview.html
    doc/en/kview-1.html
    doc/en/kview-2.html
    doc/en/kview-3.html
    doc/en/kview-4.html
    doc/en/kview-5.html
    doc/en/logotp3.gif
    DESTINATION ${KDE1_HTMLDIR}/en/kview)
