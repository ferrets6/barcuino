import 'package:flutter/foundation.dart';
import 'package:web_socket_channel/io.dart';
import 'package:rxdart/rxdart.dart';

///
/// Application-level global variable to access the WebSockets
///
// WebSocketsNotifications sockets = new WebSocketsNotifications();

///
/// Put your WebSockets server IP address
///
// const String _SERVER_IP = "192.168.4.1";

///
/// Put your WebSockets server port number
///
// const int _SERVER_PORT = 81;

// const String _SERVER_ADDRESS = "ws://$_SERVER_IP:$_SERVER_PORT"; 

class WebSocketsNotifications {
  static final WebSocketsNotifications _sockets = new WebSocketsNotifications._internal();

  factory WebSocketsNotifications() {
    return _sockets;
  }

  WebSocketsNotifications._internal();

  ///
  /// The WebSocket "open" channel
  ///
  IOWebSocketChannel _channel;

  ///
  /// Is the connection established?
  ///
  bool _isOn = false;
  final BehaviorSubject isOn = BehaviorSubject<bool>();
  
  // subject.stream.listen();
  
  ///
  /// Listeners
  /// List of methods to be called when a new message
  /// comes in.
  ///
  ObserverList<Function> _listeners = new ObserverList<Function>();

  /// ----------------------------------------------------------
  /// Initialization the WebSockets connection with the server
  /// ----------------------------------------------------------
  initCommunication(String serverAddress, int serverPort) async {
    ///
    /// Just in case, close any previous communication
    ///
    reset();

    ///
    /// Open a new WebSocket communication
    ///
    try {
      _channel = new IOWebSocketChannel.connect('ws://$serverAddress:$serverPort');
      // _isOn = true;
      // isOn.add(_isOn);

      ///
      /// Start listening to new notifications / messages
      ///
      _channel.stream.listen(_onReceptionOfMessageFromServer);
    } catch(e){
      ///
      /// General error handling
      /// TODO
      ///
      // _isOn = false;
      // isOn.add(_isOn);
    }
  }

  /// ----------------------------------------------------------
  /// Closes the WebSocket communication
  /// ----------------------------------------------------------
  reset(){
    if (_channel != null){
      if (_channel.sink != null){
        _channel.sink.close();
        _isOn = false;
        isOn.add(_isOn);
      }
    }
  }

  /// ---------------------------------------------------------
  /// Sends a message to the server
  /// ---------------------------------------------------------
  send(String message){
    if (_channel != null){
      if (_channel.sink != null && _isOn){
        print('Sending message: $message');
        _channel.sink.add(message);
      }
      else {
        print('Channel is down or sink is null, not possible to send: $message');
      }
    } else {
      print('Channel is null, not possible to send: $message');
    }
  }

  /// ---------------------------------------------------------
  /// Adds a callback to be invoked in case of incoming
  /// notification
  /// ---------------------------------------------------------
  addListener(Function callback){
    _listeners.add(callback);
  }
  removeListener(Function callback){
    _listeners.remove(callback);
  }

  /// ----------------------------------------------------------
  /// Callback which is invoked each time that we are receiving
  /// a message from the server
  /// ----------------------------------------------------------
  _onReceptionOfMessageFromServer(message){
    _isOn = true;
    isOn.add(_isOn);
    _listeners.forEach((Function callback){
      callback(message);
    });
  }
}