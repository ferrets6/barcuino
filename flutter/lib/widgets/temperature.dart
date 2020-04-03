import 'package:flutter/material.dart';

class TemperatureSensor extends StatelessWidget {
  final value;

  TemperatureSensor({this.value});

  @override
  Widget build(BuildContext context) {
    return Card(
      elevation: 5,
      margin: EdgeInsets.all(10),
      child: Padding(
        padding: const EdgeInsets.symmetric(
          horizontal: 20,
          vertical: 10,
        ),
        child: Row(
          mainAxisAlignment: MainAxisAlignment.start,
          children: <Widget>[
            Text(
              "Temperatura:",
              style: TextStyle(
                fontWeight: FontWeight.bold,
              ),
            ),
            Expanded(
              child: Text(
                " ${value != null ? value.toString() : "--"}",
                style: TextStyle(
                  color: Colors.black,
                  fontSize: 20.0,
                ),
                textAlign: TextAlign.center,
              ),
            ),
          ],
        ),
      ),
    );
  }
}
