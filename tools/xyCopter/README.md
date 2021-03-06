# xyCopter

This Android App is able to connect to the Quadrocopter via Bluetooth ([BTM-222](http://www.xythobuz.org/index.php?p=bt)) and send and receive control commands and debug informations.
You can find the latest version as unsigned APK in the parent directory.

This is using my fork of the [GraphView Library](https://github.com/Lauszus/GraphView). I hacked it a bit, so now zero is always the center of the Y-Axis and the legend is displayed on the left.

Also included are the official Dropbox Android Core SDK libraries, Version 1.5.3.
You should register your own Dropbox App and replace the Dropbox Key and Secret in the AndroidManifest.xml and the MainActivity.java...
You can then send Firmware HEX Files from Dropbox to the [YASAB](https://github.com/xythobuz/yasab) Bootloader running on the Quadrocopter.

![xyCopter Flashing Firmware](http://www.xythobuz.org/img/xyCopterFirmware.png)
![xyCopter Screenshot](http://www.xythobuz.org/img/xyCopterNew.png)
![Old xyCopter Screenshot](http://www.xythobuz.org/img/xyCopter.png)
