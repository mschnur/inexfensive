# Setting Up The Project To Program The Arduinos

### Required Tools
* 1 Arduino Uno
* 1 Arduino Mega
* Computer
* Arduino USB Cable (USB Type A to USB Type B; Male/Male) [[Example](https://www.sparkfun.com/products/512)]

## Configure the Arduino IDE
1. Download the latest Arduino IDE for your operating system from [here](https://www.arduino.cc/en/Main/Software).
2. Depending on which installation package you chose to download, you either need to unzip the `.zip` file that you downloaded or you need to run the `.exe` file that was downloaded.
3. Connect the Arduino Uno to the computer, installing the appropriate drivers, as explained in [sections 3 and 4 of this guide from Arduino](https://www.arduino.cc/en/Guide/Windows#toc3).
4. Once the drivers for the Arduino Uno are successfully install, repeat the previous step for the Arduino Mega.

## Prepare the Project
1. Download this project by going [here](https://github.com/mschnur/inexfensive/tree/master) and clicking the `Download Zip` button, which is circled in the following screenshot: <br/>
!["Download Zip" Button](resources/download-zip-from-github.jpg?raw=true "Download Zip Button")
2. Unzip the `.zip` file downloaded in the previous step.

## Program the Arduino Uno
1. Open the `FencingBox` sketch by navigating to the `inexfensive-master/FencingBox` directory in the just-unzipped project directory and double-clicking on the `FencingBox.ino` file.  This should result in the Arduino IDE starting with the `FencingBox` sketch opened.
2. Re-connect the Arduino Uno to the computer using the USB cable.
3. Click the `Tools` menu item, expand the `Board` drop-down menu, and select `Arduino/Genuino Uno` as shown in the following screenshot: <br/>
![Select Arduino/Genuino Uno Board](resources/select-board-uno.jpg?raw=true "Select Arduino/Genuino Uno Board")
4. Click the `Tools` menu item, expand the `Port` drop-down menu, and select the port to which the Arduino Uno is connected, as shown in the following screenshot: <br/>
![Select Port](resources/select-port.jpg?raw=true "Select Port")
5. Compile and upload the sketch to the Arduino Uno by pressing the `Ctrl + U` hotkey, clicking the `Sketch > Upload` menu item, or by clicking the upload button in the toolbar that is highlighted in the following screeshot: <br/>
![Upload Button](resources/upload-sketch.jpg?raw=true "Upload Button")
6. If the upload is successful, the message "Done uploading." will appear in the status bar.

## Program the Arduino Mega
#### Prepare Libraries 
1. Close the Arduino IDE instance containing the `FencingBox` sketch.
2. Open the `MegaDisplay` sketch by navigating to the `inexfensive-master/MegaDisplay` directory in the unzipped project directory and double-clicking on the `MegaDisplay.ino` file.  This should result in the Arduino IDE starting with the `MegaDisplay` sketch opened.
3. Delete the `<Arduino Root>/libraries/RobotIRremote` directory, where <Arduino Root> refers to the install directory of Arduino (for me this was `C:\Program Files (x86)\Arduino`). The library `RobotIRremote` causes errors because it has similar definitions to the library `IRremote`, which is used by this project.
4. Add the `IRemote` library by doing the following:
  1. Click the `Sketch` menu item, expanding the `Include Library` drop-down menu, and selecting the `Add .ZIP Library...` menu item, as highlighted in the following screenshot: <br/>
![Add .ZIP Library](resources/add-zip-library.jpg?raw=true "Add .ZIP Library")
  2. In the system file chooser dialog that opens, navigate to the `inexfensive-master/lib` directory in the unzipped project directory, select the `IRremote.zip` file, and then clicking the `Open` button. (double-clicking the `IRremote.zip` file will also work)
5. Repeat the previous step to also add the `TimerOne` and `TimerThree` libraries.

#### Compile and Upload the Code
1. Re-connect the Arduino Mega to the computer using the USB cable.
2. Click the `Tools` menu item, expand the `Board` drop-down menu, and select `Arduino/Genuino Mega or Mega 2560` as shown in the following screenshot: <br/>
![Select Arduino/Genuino Mega or Mega 2560 Board](resources/select-board-mega.jpg?raw=true "Select Arduino/Genuino Mega or Mega 2560 Board")
3. Click the `Tools` menu item, expand the `Processor` drop-down menu, and select `ATmega2560 (Mega 2560)` as shown in the following screenshot: <br/>
![Select ATmega2560 (Mega 2560) Processor](resources/select-processor.jpg?raw=true "Select ATmega2560 (Mega 2560) Processor")
4. Click the `Tools` menu item, expand the `Port` drop-down menu, and select the port to which the Arduino Mega is connected, as shown in the following screenshot: <br/>
![Select Port](resources/select-port.jpg?raw=true "Select Port")
5. Compile and upload the sketch to the Arduino Mega by pressing the `Ctrl + U` hotkey, clicking the `Sketch > Upload` menu item, or by clicking the upload button in the toolbar that is highlighted in the following screeshot: <br/>
![Upload Button](resources/upload-sketch.jpg?raw=true "Upload Button")
6. If the upload is successful, the message "Done uploading." will appear in the status bar.


#### All finished!
