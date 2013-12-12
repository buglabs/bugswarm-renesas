# bugswarm-renesas: 

**Table of Contents**  *generated with [DocToc](http://doctoc.herokuapp.com/)*

- [bugswarm-renesas:](#bugswarm-renesas)
	- [Supported Boards](#supported-boards)
	- [The RL78/G14 board with built-in Gainspan module](#the-rl78g14-board-with-built-in-gainspan-module)
		- [Usage Instructions](#usage-instructions)
		- [Programming Instructions](#programming-instructions)
		- [Install the Driver](#install-the-driver)
		- [Implementation Details](#implementation-details)
			- [Known Issues](#known-issues)
		- [Troubleshooting](#troubleshooting)

A bugswarm connector for Renesas 8- and 16-bit microcontrollers.  This code turns a compatible evaluation board into a real-time internet enabled device.  Once the connector is deployed to a device, the device will automatically connect to the bugswarm platform and share it's peripherals using a standardized API.  This enables developers to create applications for the evaluation board in a wide variety of languages, without needing to download an SDK or physical access to the device.  

## Supported Boards

*  RL78/G13 Demonstration Kit (YRDKRL78G13) [yrdkrl78g13 branch](https://github.com/buglabs/bugswarm-renesas/tree/yrdkrl78g13)
    *  Redpine companion WiFi Card (RS-RL78G13-2200CC)
*  RL78/G14 Demonstration Kit (YRDKRL78G14) (this branch)
    *  Built-in Gainspan GS1011-MIPS module

## The RL78/G14 board with built-in Gainspan module

This is the most recent bugswarm-enabled device.  The firmware is currently in beta, using the IAR RL78 Workbench development environment.  For the final firmware release, the code will be ported to the CubeSuite+ environment and Applilet3 driver generation tool.  Notable improvements:
*  Swarm credentials are automatically generated based on the device mac address, no user intervention required
*  Wifi access point can be set using the web provisioning feature of the gainspan module
*  A single firmware can be used for all RL78G14 development boards.

### Usage Instructions

1.  Connect the RDK board to a USB power source using a USB-Mini cable.

	![USB Power connection](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/RL78G13\_RDK\_PC\_connection.jpg)

1.  The RDK board now needs to be configured for an active wireless access point with an internet connection.  Hold down ```Switch 2```, Press and release the ```Reset``` button, and then release the ```Switch 2``` button.  See this video for a demonstration: [Enter Wifi Provisioning Mode](https://docs.google.com/open?id=0B_kD7ktKdpaQeTdfT3YtZERMNXM).
1.  Using a smartphone, tablet, or laptop, connect to the wireless access point indicated on the LCD screen.

	![LCD Screen Provisioning Message](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/Provision\_Message.JPG)
	![Provisioning Wireless AP](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/webprovisionAP.png)

1.  Open a web browser and go to the URL indicated on the LCD screen.  Click on the ```Wireless and Network Configuration``` link.

	![Provisioning Wireless Frontpage](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/webprovisionFrontpage.png)

1.  To scan for an access point to connect to, click on ```Select an Existing Network```

	![Provisioning Wireless Existing Network](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/webprovisionExisting.png)

1.  Pick a wireless AP from the list and click on it's ```Select``` button:

	![Provisioning Wireless Network List](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/webprovisionList.png)

1.  Enter the wireless password, leaving the other fields at default values.  Click on ```next```:

	![Provisioning Wireless Network Settings](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/webprovisionSettings.png)

1.  Click on ```Save And Apply``` to confirm the selection.

	![Provisioning Wireless Confirmation](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/webprovisionConfirmation.png)

1.  You should see the following message displayed on the LCD screen.  Press the ```RESET``` button.

	![LCD Screen Provisioning Done](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/Provision\_Done.JPG)

1.  After rebooting, the device will automatically connect and begin producing data.  Note the ```ID: ``` field on the third line, and keep an eye on the screen for any errors.

	![LCD Screen when Running](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/connectorRunning.JPG)

1.  You will need to re-connect to a wireless access point with an internet connection.  Navigate to [buglabs.github.com/bugswarm-renesas/](http://buglabs.github.com/bugswarm-renesas/).  In the first dropdown box, select the YRDKRL78G14 board and click "Go!".

	![Select Board](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/swarmSelectBoard.png)

1.  Click on the second dropdown box.  You should see the ```ID``` from the LCD screen in the list.  Select that entry and click "Go!".

	![Select Device](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/swarmSelectDevice.png)

1.  You should now see live data from your Renesas device.  See the [tutorial folder](https://github.com/buglabs/bugswarm-renesas/tree/yrdkrl78g14/tutorial) for examples of how to use this data in your own application.

	![Running demo](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/swarmRunning.png)


### Programming Instructions

1.  Download and install the [Renesas Flash Programmer V2](http://am.renesas.com/products/tools/flash_prom_programming/rfp/downloads.jsp#) from [http://am.renesas.com/products/tools/flash_prom_programming/rfp/downloads.jsp#](http://am.renesas.com/products/tools/flash_prom_programming/rfp/downloads.jsp#)
1.  Locate SW5 in the middle of the RL78G14 RDK board.  Move Switch 2 of SW5 in the Off position, or towards the green square in this photo:

	![RL78G14 RDK Debug Enabled](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/RL78G14debugenabled.JPG)

1.  Connect the RDK board to the PC using a USB-Mini cable.

	![USB Power connection](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/RL78G13\_RDK\_PC\_connection.jpg)

1.  The PC should detect a new device.  If you see the following message, the driver was installed successfully and you can continue.  Otherwise, you must click the following link to [Install the Driver](#install-the-driver).

	![programming device detected by system](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/flashBoardDetected.png)

1.  Open ```Renesas Flash Programmer V2.00``` from the start menu.  Click on Next to create a new workspace for the RL78G14.
 
	![programming create new workspace](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/flashNewWorkspace.png)

1.  Select ```RL78``` from the Microcontroller dropdown.  Then select the ```R5F104PJ``` MCU from the Device Name Column.  Finally, enter ```YRDKRL78G14``` as the worksapce and project name, then click Next.
 
	![programming select the MCU](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/flashSelectMCU.png)

1.  In the next dialog, open the Tool dropdown list, and select the lowermost COM port.  This should correspond to the COM port listed when the device was first added to the system.  Then click Next.
 
	![programming select the port](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/flashSelectProgrammer.png)

1.  Click next through the following two dialog boxes, leaving the default values intact.
 
	![programming power supply](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/flashPowerSupply.png)
	![programming confirmation](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/flashSettingsConfirmation.png)

1.  Select ```Erase``` from the Microcontroller drop down menu, then click the large Start button.  If successful, the ```PASS``` text should be visible in green.

	![programming select erase](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/flashSelectErase.png)
	![programming erase complete](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/flashEraseComplete.png)

1.  Select ```Program``` from the Microcontroller drop down menu.

	![programming select program](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/flashSelectProgram.png)

1.  Download the newest software release [from this link](https://github.com/buglabs/bugswarm-renesas/tags).  Each release is a snapshot of the entire repository.  Extract the downloaded zip file to the PC, then click on the ```Browse``` button on the Renesas Flash Progreammer, and select the .mot file that corresponds to the software release you would like to deploy:

	![programming select firmware file](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/flashSelectFirmware.png)

1.  Click on the large Start button.  The firmware will be deployed to the device, displaying a progress bar and percentage readout.  If successful, the ```PASS``` text should be visible in green.

	![programming in progress](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/flashProgrammingProgress.png)
	![programming complete](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/flashProgrammingComplete.png)

1.  Move Switch 2 of SW5 in the On position, or towards the green square in this photo:

	![RL78G14 RDK Debug Disabled](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/RL78G14debugdisabled.JPG)

1.  The new software should begin running on the device, see the user guide above.  Repeat the Erase and Program steps for each board to be programmed.

### Install the Driver

Follow these instructions to install the Virtual USB Com Port driver, required ONLY to deploy firmware to the board.  This is not required to use the board.

1.  After following the programming instructions above, and after plugging in the RL78 board to your PC, you may have seen the following error:

	![Driver Not Found](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/DriverNotFound.png)

1.  Download the following ZIP file and extract it to a known location (like the Desktop) [Renesas USB Drivers](http://am.renesas.com/media/products/tools/introductory_evaluation_tools/renesas_demo_kits/yrpbrl78g13/USB_Drivers.zip)	
1.  Open the Device Manager on your PC.  If you are unsure of how to do this, see [this tutorial](http://www.computerhope.com/issues/ch000833.htm)
1.  Find the RL78 board in your list of devices.  It should appear with a small yellow exclaimation point under the heading ```Other Devices``` or ```USB Devices```.  Right click on the ```Unknown Device``` and click ```Update Driver Software...```

	![Update the driver software](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/DriverUpdateSoftware.png)
	
1.  On the next screen, direct Windows to install the driver manually by clicking on the ```Browse my computer for driver software``` button.

	![Install the driver Manually](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/DriverInstallManually.png)
	
1.  Click on the ```Browse``` button and navigate to the folder in which you extracted the USB Drivers earlier.  In this case, they were extracted to the Desktop.  Then click ```Next```.

	![Select the driver folder](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/DriverSelectLocation.png)
	
1.  Windows will now install the drivers.  You should be greeted by the following success screen:

	![Driver installation complete](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/DriverInstallComplete.png)

1.  You can now delete the driver files you extracted earlier.  If you open up the Device Manager again, you will see the RL78 board correctly installed.  Note the COM Port number:

	![The correct RL78 driver entry](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/DriverCorrectInstall.png)

### Implementation Details

Pseudocode for the firmware:

1.  Check status of switches.  If SW1 is held down, enter Gainspan web demo.  If SW2 is held down, enter web provisioning mode
1.  Initialize hardware and read wireless AP data from nonvolatile storage.  If no SSID has been saved, enter web provisioning mode
1.  Read wifi MAC address and execute API call to swarm server.  Swarm credentials will be created if the device is new, and then returned.
1.  If Swarm credentials could not be retreived from server, use default ID of "UnknownDevice".
1.  Open a streaming "producer" session with the swarm API, and wait for message from server acknowledging the "presence" of the module.
1.  Enter production loop, in which each sensor on the board is sampled and transmitted to the server, every second.

#### Known Issues

1.  The watchdog timer has not been enabled in the beta release, adding this will improve reliability when the module is suddenly disconnected from the internet.
1.  Serial debugging occurs over UART0 (at 115200 baud) which is used by the debugger.  We haven't been able to use the USB debugger tool to read this data, we instead used a seperate USB->Serial converter connected to pins 54 and 55. 
1.  Some boards come pre-loaded with an older version of firmware. If the board connects to the Swarm backend with the old firmware, we will need to delete this resource. Please email your board ID to: support@buglabs.net You can then upload the new bi-directional firmware.
1.  In order to enable the buzzer, please make sure the jumper on JP3 covers the top two pins, which moves the speaker output from the DAC to the PWM.

### Troubleshooting

1.  If the device appears on the Renesas Web Portal as "UnknownDevice", try rebooting it.  Occasionally the first API call can fail if the wireless access point has not finished opening the connection.
1.  If an error is printed to the LCD screen on line 4 or line 8, and the error does not go away for a few minutes, try manually resetting the board.
1.  If the RDK board never appears on the web portal, use a smartphone or laptop to verify that the selected wireless access point has an internet connection, particularly to demo.bugswarm.com or developer.bugswarm.net.
1.  Rebooting your device will often solve connection issues, please try this first.
1.  Please use the most up-to-date browsers when connecting to http://bugcommunity.com/rdknext/ (IE8 or lower will not work, and older versions of Safari, Firefox and Chrome may have difficulty displaying data).  The latest version of Firefox has shown to be the most robust.
1.  Conference center hotspots often have trouble sustaining a connection. Some hotspots do not work well with the Gainspan WiFi chipset. Please try a clean hotspot with little traffic if you experience connection issues (Verizon jetpack hotspots have been tested to work).

### Relevant Documentation

* [bugswarm API documentation](http://developer.bugswarm.net/)
* [bugswarm javascript library](https://github.com/buglabs/bugswarm-js)
* [bugswarm preliminary configuration interface](http://demo.bugswarm.com/)
* [YRDKRL78G13 Quick Start Guide (how to install the SDK)](http://am.renesas.com/media/products/tools/introductory_evaluation_tools/renesas_demo_kits/yrdkrl78g13/child_folder/YRDKRL78G13_Quick_Start_Guide.pdf)
* [YRDKRL78G13 Getting Started DVD](http://am.renesas.com/media/products/tools/introductory_evaluation_tools/renesas_demo_kits/yrdkrl78g13/child_folder/YRDKRL78G13_DVD10.zip)

