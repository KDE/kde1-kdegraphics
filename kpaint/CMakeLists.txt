#add_subdirectory(doc)

include_directories (
    ${KDE1_INCLUDE_DIR}
)

set(Kpaint_SRCS
    app.cpp
    areaselect.cpp
    canvas.cpp
    canvassize.cpp
    circle.cpp
    colorbar.cpp
    depthdialog.cpp
    ellipse.cpp
    formatdialog.cpp
    formats.cpp
    gif.cpp
    infodialog.cpp
    kpaint.cpp
    kundo.cpp
    line.cpp
    main.cpp
    mainview.cpp
    manager.cpp
    navwidget.cpp
    palette.cpp
    palettedialog.cpp
    pen.cpp
    properties.cpp
    QwViewport.cpp
    rectangle.cpp
    roundangle.cpp
    sidebar.cpp
    spraycan.cpp
    text.cpp
    tool.cpp
    view.cpp
)

set(Kpaint_MOC
    app.h
    areaselect.h
    canvas.h
    canvassize.h
    circle.h
    colorbar.h
    commands.h
    depthdialog.h
    ellipse.h
    formatdialog.h
    formats.h
    gif.h
    infodialog.h
    kpaint.h
    kundo.h
    line.h
    main.h
    mainview.h
    manager.h
    navwidget.h
    palettedialog.h
    palette.h
    pen.h
    properties.h
    QwViewport.h
    rectangle.h
    roundangle.h
    sidebar.h
    spraycan.h
    text.h
    tool.h
    version.h
    view.h
    )

QT1_WRAP_MOC(MOC_FILES ${Kpaint_MOC})

add_executable(kpaint ${Kpaint_SRCS} ${MOC_FILES} ${QwSpriteField_SOURCES})
target_link_libraries(kpaint
    Qt::Qt1
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    ${KDE1_KFM}
    ${KDE1_KFILE}
    ${KDE1_KIMGIO}
    GIF::GIF
)

install(TARGETS kpaint RUNTIME DESTINATION ${KDE1_BINDIR})
install(FILES kpaint.kdelnk DESTINATION ${KDE1_APPSDIR}/Graphics)
install_icon(kpaint.xpm)

add_subdirectory(toolpics)
#add_subdirectory(images) TODO

install(FILES
    doc/kpaint.html
    doc/index.html
    doc/Technic.gif
    DESTINATION ${KDE1_HTMLDIR}/en/kpaint)
