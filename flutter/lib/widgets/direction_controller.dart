import 'package:flutter/material.dart';

import '../flutter_control_pad/lib/control_pad.dart';
import '../flutter_control_pad/lib/models/pad_button_item.dart';

class DirectionController extends StatelessWidget {
  final Function onDirectionChanged;
  // type 0 = Joystick, type 1 = arrows
  final int controllerType;

  DirectionController({@required this.onDirectionChanged, this.controllerType});

  @override
  Widget build(BuildContext context) {
    if (controllerType == null || controllerType == 0) {
      return Container(
        padding: EdgeInsets.all(20),
        color: Colors.transparent,
        child: JoystickView(
          onDirectionChanged: onDirectionChanged,
          interval: Duration(milliseconds: 300),
        ),
      );
    } else {
      return Container(
        padding: EdgeInsets.all(20),
        child: PadButtonsView(
          buttons: const [
            PadButtonItem(
              index: 0,
              buttonIcon: Icon(Icons.arrow_forward, size: 30),
              backgroundColor: Colors.white54,
            ),
            PadButtonItem(
              index: 1,
              buttonIcon: Icon(Icons.stop, size: 40,),
              backgroundColor: Colors.white54,
              pressedColor: Colors.red,
              //buttonImage: TODO
            ),
            PadButtonItem(
              index: 2,
              buttonIcon: Icon(Icons.arrow_back, size: 30,),
              backgroundColor: Colors.white54,
              //buttonImage: TODO
            ),
            PadButtonItem(
              index: 3,
              buttonIcon: Icon(Icons.arrow_upward, size: 30),
              backgroundColor: Colors.white54,
              //buttonImage: TODO
            ),
          ],
        ),
      );
    }
  }
}
