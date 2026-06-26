function(add_resources TARGET_NAME)
    file(GLOB IMAGES Content/Images/*.png)
    file(GLOB ICONS Content/Icons/*.svg)

    qt_add_resources(${TARGET_NAME} "app_resources"
        PREFIX
            "/"
        BASE
            "Content/"
        FILES
            ${IMAGES}
        FILES
            ${ICONS}
    )
endfunction()
