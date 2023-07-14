import QtQuick
import QtQuick.Controls

Window {
    width: 500
    height: 400
    title: qsTr("示例")
    visible: true

    property alias inputText: input.text

    Column{
        Label {
            height: 16
            width: 200
            text: inputText
        }

        TextInput {
            id: input
            width: 200
            text: "上面的文字会随着你的输入而改变"
            cursorVisible: true
        }
    }
}
