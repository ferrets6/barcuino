import 'dart:io';
import 'package:flutter/material.dart';
import 'package:gateway/gateway.dart';
import 'package:wifi/wifi.dart';
import 'package:control_pad/control_pad.dart';

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
  Socket channel;
  String host = '192.168.4.1';
  String host2 = '';
  int port = 80;
  String ssid = 'BarkiFi';
  String password = 'ciaociao';
  String socketState = 'Disconnected';
  TextEditingController _controller;
  bool _isSocketConnected = false;
  bool _isWiFiConnected = false;

  void initState() {
    super.initState();
    _controller = TextEditingController(text: host);
  }

  void _wifiConnect() {
    Wifi.connection(ssid, password)
        .then((WifiState state) {
          print('WiFi state: $state');
          _isWiFiConnected = true;
        })
        .catchError((error) {
          print('Error connecting to $ssid');
          _isWiFiConnected = false;
        });
  }

  void _getGw() {
    Gateway.info
        .then((gw) => setState(() => host2 = gw.ip))
        .catchError((error) => print('Error on get gw'));
  }

  void _socketConnect() {
    Socket.connect(host, port)
        .then(_handleSocketConnection)
        .catchError(_handleSocketException, test: (e) => e is SocketException);
  }

  void _socketDisconnect() {
    channel.close();
  }

  void _handleSocketConnection(Socket chan) {
    setState(() {
      channel = chan;
      socketState = 'Connected to ' + chan.address.address;
      _isSocketConnected = true;
    });

    channel.done
        .then(_handleSocketClosure)
        .catchError(_handleSocketException, test: (e) => e is SocketException);
  }

  void _handleSocketException(Object e) {
    print('Socket Exception! Closing...');
    channel.close();
    setState(() {
      socketState = 'Disconnected';
      _isSocketConnected = false;
    });
  }

  void _handleSocketClosure(dynamic chan) {
    print('Socket closed');
    channel.close();
    setState(() {
      socketState = 'Disconnected';
      _isSocketConnected = false;
    });
  }

  _handleNewIp(String value) {
    setState(() => host = value);
  }

  void _ejectPastura() {
    print('ejectPastura;');
    channel.write("ejectPastura;\n");
  }

  void _fermete() {
    print('stopMotors;');
    channel.write("stopMotors;\n");
  }

  void _aTuttoBiroccio() {
    print('setMotorsSpeed;1000;1000;');
    channel.write("setMotorsSpeed;1000;1000;\n");
  }

  void _onPadToggle(double degrees, double normalizedDistance) {
    int degreesInt = degrees.floor();
    double distanceShort = (normalizedDistance * 10).floor() / 10;
    print("setMotorsSpeedFromPad;$degreesInt;$distanceShort;");
    channel.write("setMotorsSpeedFromPad;$degreesInt;$distanceShort;\n");
  }

  @override
  void dispose() {
    _controller.dispose();
    channel.close();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(widget.title),
      ),
      body: Container(
        child: Center(
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: <Widget>[
              Row(
                mainAxisAlignment: MainAxisAlignment.center,
                children: <Widget>[
                  RaisedButton(
                    child: Text(
                      "Get GW",
                      style: TextStyle(color: Colors.white, fontSize: 20.0),
                    ),
                    color: Colors.green,
                    onPressed: _getGw,
                  ),
                  Text(
                    '$host2',
                    style: TextStyle(color: Colors.blue, fontSize: 20.0),
                  ),
                ],
              ),
              TextField(
                controller: _controller,
                decoration: InputDecoration(
                  border: OutlineInputBorder(),
                  labelText: 'Server IP',
                ),
                onChanged: _handleNewIp,
              ),
              RaisedButton(
                child: Text(
                  "Connect WiFi",
                  style: TextStyle(color: Colors.white, fontSize: 20.0),
                ),
                color: Colors.blue,
                onPressed: _wifiConnect,
              ),
              RaisedButton(
                child: Text(
                  "Connect Socket",
                  style: TextStyle(color: Colors.white, fontSize: 20.0),
                ),
                color: Colors.blue,
                onPressed: _isSocketConnected ? null : _socketConnect,
              ),
              RaisedButton(
                child: Text(
                  "Eject Pastura",
                  style: TextStyle(color: Colors.white, fontSize: 20.0),
                ),
                color: Colors.green,
                onPressed: !_isSocketConnected ? null : _ejectPastura,
              ),
              Container(
                color: Colors.white,
                child: JoystickView(
                  onDirectionChanged: _onPadToggle,
                  interval: Duration(milliseconds: 300),
                ),
              ),
              // RaisedButton(
              //   child: Text(
              //     "A tutto biroccio",
              //     style: TextStyle(color: Colors.white, fontSize: 20.0),
              //   ),
              //   color: Colors.green,
              //   onPressed: !_isSocketConnected ? null : _aTuttoBiroccio,
              // ),
              // RaisedButton(
              //   child: Text(
              //     "Fermete!",
              //     style: TextStyle(color: Colors.white, fontSize: 20.0),
              //   ),
              //   color: Colors.green,
              //   onPressed: !_isSocketConnected ? null : _fermete,
              // ),
              RaisedButton(
                child: Text(
                  "Disconnect Socket",
                  style: TextStyle(color: Colors.white, fontSize: 20.0),
                ),
                color: Colors.red,
                onPressed: !_isSocketConnected ? null : _socketDisconnect,
              ),
              Text(
                '$socketState',
                style: TextStyle(color: Colors.blue, fontSize: 20.0),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
