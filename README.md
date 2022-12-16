# Snapmaker LightBurn Host
Tool that allows you to use your Snapmaker 2.0 10W laser in LightBurn. Based on the method described by [**Slynold**](https://forum.snapmaker.com/u/slynold) (https://forum.snapmaker.com/t/working-camera-capture-with-lightburn/28397).

# Dependencies (included or NuGet)
1. https://github.com/tshino/softcam: virtual DirectShow camera.
2. https://github.com/nothings/stb: ```stb_image.h``` for simple JPEG loading (note: modified with RGB -> BGR swap).
3. libcurl, libssh, openssl, zlib

Note: Above libraries (static and dynamic) are included in this project for ease-of-use, but can be built separately if desired.

# Usage
1. Build project (or download binary release & extract).
2. Run ```3rdParty\installer\softcam\RegisterSoftcam.bat``` to install softcam.
3. In your favorite terminal: run **```SnapmakerLightburnHost.exe <your-snapmaker-ip>```**.
4. In LightBurn settings, verify that ```Default Capture System``` is selected under ```Camera Capture System```.
5. In LightBurn, select ```DirectShow Softcam``` as your camera.
6. Press ```ENTER``` to request a new image from your Snapmaker 10W laser camera.

![image](https://user-images.githubusercontent.com/6267267/208189576-714fb93c-bdfe-40b4-af4a-2651109a5746.png)
