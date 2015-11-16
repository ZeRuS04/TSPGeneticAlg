import QtQuick 2.5
import QtQuick.Window 2.2

Window {
    id: root
    width: 640
    height: 640

    visible: true
    Flickable{
        anchors.fill: parent
        interactive: true
        contentWidth: repeaterTowns.width; contentHeight: repeaterTowns.height
        Repeater{
            id: repeaterTowns
            width: 800
            height: 800
            model: Adapter.towns
            delegate: Rectangle{
                width: 10
                height: width
                x: modelData.x *10; y: modelData.y*10
                radius: width/2
                color: /*index === 0 ? "green"
                                   : (index === Adapter.towns.length-1 ? "red"
                                                                       : */"orange"
            }
        }
        Canvas {
            id: canvas;
            property var resultRoute: Adapter.resultRoute
            property var towns: Adapter.towns
            onResultRouteChanged: requestPaint()
            onTownsChanged: requestPaint()
            anchors.fill: parent

            onPaint: {
                console.log(towns.length)
                var ctx = canvas.getContext("2d");
                ctx.beginPath();
                ctx.strokeStyle = "skyblue";
                ctx.lineWidth = 2;
//                ctx.moveTo(towns[resultRoute[0]-1].x*10, towns[resultRoute[0]-1].y*10);
                for (var i = 0; i < resultRoute.length; i++) {
                    console.log(i, ")", resultRoute[i], towns[resultRoute[i]].x, towns[resultRoute[i]].y);
//                    ctx.moveTo(towns[resultRoute[i-1]-1].x*10, towns[resultRoute[i-1]-1].y*10);
                    ctx.lineTo(towns[resultRoute[i]].x*10 + 5, towns[resultRoute[i]].y*10)+5;
                }
                ctx.stroke();
            }
        }
        Canvas {
            id: canvas2;
            property var route: Adapter.optimalRoute
            property var towns: Adapter.towns
            onRouteChanged: requestPaint()
            onTownsChanged: requestPaint()
            anchors.fill: parent

            onPaint: {
                console.log(towns.length)
                var ctx = canvas2.getContext("2d");
                ctx.beginPath();
                ctx.strokeStyle = "rgba(100,190,105,0.5)";
                ctx.lineWidth = 5;
//                ctx.moveTo(towns[resultRoute[0]-1].x*10, towns[resultRoute[0]-1].y*10);
                for (var i = 0; i < route.length; i++) {
                    console.log(i, ")", route[i], towns[route[i]].x, towns[route[i]].y);
//                    ctx.moveTo(towns[resultRoute[i-1]-1].x*10, towns[resultRoute[i-1]-1].y*10);
                    ctx.lineTo(towns[route[i]].x*10 + 5, towns[route[i]].y*10)+5;
                }
                ctx.stroke();
            }
        }
    }
}

