import 'package:flutter/material.dart';

enum ConfirmAction { CANCEL, ACCEPT }

class Utils {
// Alert async, with title, message and Ok button.
// Can be closed by user clicking anywhere
  static Future asyncAlert(
          BuildContext context, String title, String message) async =>
      showDialog(
        context: context,
        barrierDismissible: true, // user must tap button for close dialog!
        builder: (BuildContext context) {
          return AlertDialog(
            title: Text(title),
            content: Text(message),
            actions: <Widget>[
              FlatButton(
                child: const Text('Ok'),
                onPressed: () {
                  Navigator.of(context).pop();
                },
              ),
            ],
          );
        },
      );

// Confirm alert async for
// Can be closed by user clicking anywhere
  static Future<ConfirmAction> asyncConfirmEject(BuildContext context) async {
    return showDialog<ConfirmAction>(
      context: context,
      barrierDismissible: false, // user must tap button for close dialog!
      builder: (BuildContext context) {
        return AlertDialog(
          title: Text('Lanciamo?'),
          content: const Text(
              'Guarda che poi non cen\'hai n\'altra!\r\n\r\nLanciamo qua, sei sicuro?'),
          actions: <Widget>[
            FlatButton(
              child: const Text('BONO, MORTACCI!'),
              onPressed: () {
                Navigator.of(context).pop(ConfirmAction.CANCEL);
              },
            ),
            FlatButton(
              child: const Text('LANCIA ZIO!'),
              onPressed: () {
                Navigator.of(context).pop(ConfirmAction.ACCEPT);
              },
            ),
          ],
        );
      },
    );
  }
}
