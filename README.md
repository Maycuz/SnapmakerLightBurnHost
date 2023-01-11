# Snapmaker LightBurn Host 
[![Windows Visual Studio Build Status](https://img.shields.io/appveyor/ci/PolymerPrints/snapmakerlightburnhost/master.svg?label=build)](https://ci.appveyor.com/project/PolymerPrints/snapmakerlightburnhost)    
Tool that allows you to use your Snapmaker 2.0 10W laser in LightBurn. 

Using the method described by [**Slynold**](https://forum.snapmaker.com/u/slynold) (https://forum.snapmaker.com/t/working-camera-capture-with-lightburn/28397).

![image](https://user-images.githubusercontent.com/6267267/208189576-714fb93c-bdfe-40b4-af4a-2651109a5746.png)

![afbeelding](https://user-images.githubusercontent.com/6267267/208304937-57a387e2-99f0-4d7e-a830-0ab8959cb236.png)

# Dependencies (included or NuGet)
1. https://github.com/tshino/softcam: virtual DirectShow camera.
1. https://github.com/nlohmann/json: JSON parser.
2. https://github.com/nothings/stb: ```stb_image.h``` for simple JPEG loading (note: modified with RGB -> BGR swap).
3. libcurl, libssh, openssl, zlib

Note: Above libraries (static and dynamic) are included in this project for ease-of-use, but can be built separately if desired.

# Usage
1. Build project (or download binary release & extract).
2. Run ```3rdParty\installer\softcam\RegisterSoftcam.bat``` to install softcam.
3. In your favorite terminal: run **```SnapmakerLightburnHost.exe <your-snapmaker-ip>```**.
   - Or use (`config.json`), which can be used to set the base position as well as the IP address of your Snapmaker. Config file takes priority over the command line. If no config file is available, one will be generated in the directory of the `.exe`.

   - Note that the config file is only read on startup, so the tool needs to be restarted for the changes to take effect.
4. In LightBurn settings, verify that ```Default Capture System``` is selected under ```Camera Capture System```.
5. In LightBurn, select ```DirectShow Softcam``` as your camera.
6. Press ```ENTER``` to request a new image from your Snapmaker 10W laser camera.
6. Press ```SPACE``` to perform a material thickness measurement with your Snapmaker 10W laser camera.

The coordinates where the laser will "park" to take a picture can be configured in `config.json`. Suggested coordinates (thanks to Slynold & theevl):

| Machine  | Coordinates |
| ------------- | ------------- |
| Snapmaker 2.0 A350  | X: 232.0 Y: 178.0 Z: 290.0 |
| Snapmaker 2.0 A250  | X: 160.0 Y: 120.0 Z: 175.0 |
| Snapmaker 2.0 A150  | ???  |
| Snapmaker Original  | ???  |
