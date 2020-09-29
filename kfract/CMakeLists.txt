include_directories (
    ${KDE1_INCLUDE_DIR}
)

set(kfract_SRCS
    drawview.C
    fract.C
    julia.C
    kfract.C
    kfractdlgs.C
    main.C
    mandel.C
)

set(kfract_MOC
    kfract.moc kfractdlgs.moc
    )

QT1_WRAP_MOC(MOC_FILES ${kfract_MOC})

add_executable(kfract ${kfract_SRCS} ${MOC_FILES})
target_link_libraries(kfract
    Qt::Qt1
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
)

install(TARGETS kfract RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES kfract.kdelnk DESTINATION ${KDE1_APPSDIR}/Graphics)
install_icon(kfract.xpm)

install(FILES index.html kfract.gif DESTINATION ${KDE1_HTMLDIR}/en/kfract)

#add_subdirectory(doc) TODO
