pragma Singleton
import QtQuick

QtObject {
    function getDirectoryLayout() {
        fileModel.clear();
        console.log("Запрос на ресурс");

        Client.getDirectory(currentPath);
    }
}
