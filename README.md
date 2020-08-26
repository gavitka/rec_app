## Time Lapse Rec

Application for creating time-lapse screen recordnigs. Main purpose is creating speed-up "making of"
videos of digital artwork. For Windows only.

## Features:

* Records entire screen or specific window/windows.
* Select time-lapse multiplier 1x/2x/4x/8x/16x
* Recording pauses when window is idle (Not recieving windows events). Thus reducing file size.

## TODO:

- [ ] Support x86 processes
- [ ] Improve status window (it's kinda ugly now)
- [ ] Textarea don't lose focus
- [ ] Get rid of pesky QML binding loops
- [ ] Improve implementation of frameless window
- [ ] Save widow selecttion state
- [ ] Improve Capture cursor (not always working now)

- [x] Update UI for window selection
- [x] Not recordnig alert
- [x] Refactor usage of QThread

## Notes to myself

- Hooks are working, however they shouldn't, gonna think more about that.
- To think about this: https://stackoverflow.com/questions/33440966/setwindowshookex-is-injecting-32-bit-dll-into-64-bit-process-and-vice-versa
- Hooks not working for MinGW (bruh...)
- Wintoast not working for MinGW

## Screenshot

![image](https://user-images.githubusercontent.com/28277100/88574246-b15c6080-d04a-11ea-92a8-74b62826e1d2.png)
