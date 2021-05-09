import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import net.olink 1.0


Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")
    GridLayout {
        columns: 6
        anchors.fill: parent
        Button {
            text: "Call"
            onClicked: wamp.doCall()
        }
        Button {
            text:  "Subscribe"
            onClicked: wamp.doSubscribe()
        }
        Button {
            text:  "Register"
            onClicked: wamp.doRegister()
        }
        Button {
            text:  "Unsubscribe"
            onClicked: wamp.doSubscribe()
        }
        Button {
            text:  "Publish"
            onClicked: wamp.doPublish()
        }
    }


    Wamp {
        id: wamp
    }

    Component.onCompleted: {
        wamp.open("ws://127.0.0.1:8000")
    }
}
