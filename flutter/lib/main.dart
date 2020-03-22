import 'package:flutter/material.dart';
import 'package:gateway/gateway.dart';
import 'package:wifi/wifi.dart';
import 'package:control_pad/control_pad.dart';
import 'websockets.dart';
import 'utils.dart';

void main() => runApp(MyApp());

class MyApp extends StatelessWidget {
  final String _title = 'Barkino';

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: _title,
      home: MyHomePage(_title),
    );
  }
}

class MyHomePage extends StatefulWidget {
  final String title;

  MyHomePage(this.title);

  @override
  _MyHomePageState createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  WebSocketsNotifications webSocket = new WebSocketsNotifications();
  String wsServerAddress = '192.168.4.1';
  int wsServerPort = 81;
  bool _isSocketConnected = false;
  bool _isPasturaEjected = false;
  bool _isLedOn = false;

  String ipGateway = '';

  String ssid = 'BarkiFi';
  String password = 'ciaociao';

  TextEditingController _controller;

  bool _isWiFiConnected = false;
  String showMessage = '';

  void initState() {
    super.initState();
    _controller = TextEditingController(text: wsServerAddress);
  }

  void _wifiConnect() {
    Wifi.connection(ssid, password).then((WifiState state) {
      print('WiFi state: $state');
      _isWiFiConnected = true;
    }).catchError((error) {
      print('Error connecting to $ssid');
      _isWiFiConnected = false;
    });
  }

  void _getGw() {
    Gateway.info
        .then((gw) => setState(() => ipGateway = gw.ip))
        .catchError((error) => print('Error on get gw'));
  }

  void _socketConnect() {
    webSocket.initCommunication(wsServerAddress, wsServerPort);
    webSocket.addListener(_onMessageReceived);
    webSocket.isOn.stream
        .listen((state) => setState(() => _isSocketConnected = state));
  }

  void _onMessageReceived(String serverMessage) {
    setState(() => showMessage += serverMessage + '\r\n');
  }

  void _socketDisconnect() {
    webSocket.removeListener(_onMessageReceived);
    webSocket.reset();
  }

  _handleNewIp(String value) {
    setState(() => wsServerAddress = value);
  }

  void _switchOnLed() {
    setState(() => _isLedOn = true);
    webSocket.send('#led;on;\n');
  }

  void _switchOffLed() {
    setState(() => _isLedOn = false);
    webSocket.send('#led;off;\n');
  }

  void _ejectPastura() {
    if (_isSocketConnected) {
      if (!_isPasturaEjected) {
        Utils.asyncConfirmEject(
          context: context,
          title: 'Lanciamo?',
          message: 'Guarda che poi non cen\'hai n\'altra!\r\n\r\nLanciamo qua, sei sicuro?',
          //confirmButtonText: 'BONO, MORTACCI!',
          //cancelButtonText: 'LANCIA ZIO!',
        ).then((ConfirmAction response) {
          switch (response) {
            case ConfirmAction.ACCEPT:
              webSocket.send('#ejectPastura;\n');
              Utils.asyncAlert(
                context: context,
                title: 'Fatto!',
                message: 'Pastura lanciata!\r\nIn bocca al lupo.',
              );
              setState(() => _isPasturaEjected = true);
              break;
            case ConfirmAction.CANCEL:
              print('User aborted');
              break;
            default:
          }
        });
      } else {
        Utils.asyncAlert(
          context: context,
          title: 'Errore',
          message: 'Pastura già lanciata!\r\nNon cen\'è più...',
        );
        print('No more pastura to eject');
      }
    } else {
      // alert
      Utils.asyncAlert(
        context: context,
        title: 'Errore',
        message: 'Socket non connesso!',
      );
      print('Socket not connected');
    }
  }

  void _onPadToggle(double degrees, double normalizedDistance) {
    int degreesInt = degrees.floor();
    double distanceShort = (normalizedDistance * 10).floor() / 10;
    webSocket.send('#setMotorsSpeedFromPad;$degreesInt;$distanceShort;\n');
  }

  @override
  void dispose() {
    _controller.dispose();
    _socketDisconnect();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(widget.title),
      ),
      // backgroundColor: Colors.white,
      body: Container(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.start,
          children: <Widget>[
            RaisedButton(
              child: Text(
                "Connect WiFi",
                style: TextStyle(color: Colors.white, fontSize: 20.0),
              ),
              color: Colors.blue,
              onPressed: _wifiConnect,
            ),
            Row(
              mainAxisAlignment: MainAxisAlignment.center,
              children: <Widget>[
                Container(
                  margin: EdgeInsets.all(10),
                  width: 150,
                  height: 45,
                  child: TextField(
                    controller: _controller,
                    decoration: InputDecoration(
                      border: OutlineInputBorder(),
                      labelText: 'Server IP',
                    ),
                    onChanged: _handleNewIp,
                  ),
                ),
                RaisedButton(
                  child: Text(
                    _isSocketConnected ? "Disconnect" : "Connect",
                    style: TextStyle(color: Colors.white, fontSize: 20.0),
                  ),
                  color: _isSocketConnected ? Colors.red : Colors.blue,
                  onPressed:
                      _isSocketConnected ? _socketDisconnect : _socketConnect,
                ),
              ],
            ),
            // Row(
            //   mainAxisAlignment: MainAxisAlignment.center,
            //   children: <Widget>[
            //     RaisedButton(
            //       child: Text(
            //         "Get GW",
            //         style: TextStyle(color: Colors.white, fontSize: 20.0),
            //       ),
            //       color: Colors.green,
            //       onPressed: _getGw,
            //     ),
            //     Text(
            //       '$ipGateway',
            //       style: TextStyle(color: Colors.blue, fontSize: 20.0),
            //     ),
            //   ],
            // ),
            Container(
              color: Colors.transparent,
              child: JoystickView(
                onDirectionChanged: _onPadToggle,
                interval: Duration(milliseconds: 300),
              ),
            ),
            RaisedButton(
              child: Text(
                "Eject Pastura",
                style: TextStyle(color: Colors.white, fontSize: 20.0),
              ),
              color: Colors.green,
              onPressed: _ejectPastura,
            ),
            RaisedButton(
              child: Text(
                "Switch ${_isLedOn ? "off" : "on"} LED!",
                style: TextStyle(color: Colors.white, fontSize: 20.0),
              ),
              color: Colors.green,
              onPressed: _isLedOn ? _switchOffLed : _switchOnLed,
            ),
            // Text(
            //   _isSocketConnected ? 'Connected!' : 'Disconnected',
            //   style: TextStyle(color: Colors.blue, fontSize: 20.0),
            // ),
            Text(
              showMessage,
              style: TextStyle(color: Colors.black, fontSize: 20.0),
            ),
          ],
        ),
      ),
    );
  }
}
