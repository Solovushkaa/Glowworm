qt_add_library(CustomButtonsLib)

qt_standard_project_setup(REQUIRES 6.8)

set_target_properties(CustomButtonsLib PROPERTIES AUTOMOC ON)

qt_add_qml_module(CustomButtonsLib
    URI CustomButtons
    VERSION 1.0
    QML_FILES
        BurgerButton.qml
        CustomButton.qml
    OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/qml/buttons/CustomButtons"
)

target_link_libraries(CustomButtonsLib
    PRIVATE
    Qt6::Quick
    Qt6::Qml
)
