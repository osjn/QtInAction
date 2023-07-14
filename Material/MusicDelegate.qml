import QtQuick
import QtQuick.Controls

ItemDelegate {
    id: root
    width: parent.width

    Text {
        id: musicName
        text: name + " - " + duration
        color: "white"
    }
}
