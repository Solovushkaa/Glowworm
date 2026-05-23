qt_add_library(CustomButtonsLib)

qt_add_qml_module(CustomButtonsLib
    URI CustomButtons
    VERSION 1.0
    QML_FILES
        BurgerButton.qml
        CustomButton.qml
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml/buttons/CustomButtons"
)

target_link_libraries(CustomButtonsLib
    PUBLIC
    Qt6::Quick
    Qt6::Qml
)
