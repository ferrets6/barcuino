import 'package:flutter/material.dart';

enum ConfirmAction { CANCEL, ACCEPT }

class Utils {
// Alert async, with title, message and Ok button.
// Can be closed by user clicking anywhere
  static Future asyncAlert({
    @required BuildContext context,
    @required String title,
    @required String message,
  }) async =>
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

// Confirm alert async with title, message and confirm and cancel button.
// confirmButtonText and cancelButtonText are optional.
  static Future<ConfirmAction> asyncConfirmEject({
    @required BuildContext context,
    @required String title,
    @required String message,
    String confirmButtonText = 'Confirm',
    String cancelButtonText = 'Cancel',
  }) async {
    return showDialog<ConfirmAction>(
      context: context,
      barrierDismissible: false, // user must tap button for close dialog!
      builder: (BuildContext context) {
        return AlertDialog(
          title: Text(title),
          content: Text(message),
          actions: <Widget>[
            FlatButton(
              child: Text(cancelButtonText),
              onPressed: () {
                Navigator.of(context).pop(ConfirmAction.CANCEL);
              },
            ),
            FlatButton(
              child: Text(confirmButtonText),
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
