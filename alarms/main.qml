import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

ApplicationWindow {
    id: window
    width: 400
    height: 500
    visible: true
    title: qsTr("我的闹钟")

    ListView {
        id: alarmListView
        anchors.fill: parent
        model: AlarmModel {}
        delegate: AlarmDelegate {}
    }

    RoundButton {
        id: addAlarmButton
        text: "+"
        anchors.bottom: alarmListView.bottom
        anchors.bottomMargin: 8
        anchors.horizontalCenter: parent.horizontalCenter
    }
}
