# bugswarm-renesas: 

**Table of Contents**  *generated with [DocToc](http://doctoc.herokuapp.com/)*

- [bugswarm-renesas:](#bugswarm-renesas)
	- [Supported Boards](#supported-boards)
	- [The RL78/G14 board with built-in Gainspan module](#the-rl78g14-board-with-built-in-gainspan-module)
		- [Usage Instructions](#usage-instructions)
		- [Programming Instructions](#programming-instructions)
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

1.  The RL78 will enter web provisioning mode on first boot.  Using a smartphone, tablet, or laptop, connect to the wireless access point indicated on the LCD screen.

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

1.  Navigate to [buglabs.github.com/bugswarm-renesas/](http://buglabs.github.com/bugswarm-renesas/).  In the first dropdown box, select the YRDKRL78G14 board and click "Go!".

	![Select Board](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/swarmSelectBoard.png)

1.  Click on the second dropdown box.  You should see the ```ID``` from the LCD screen in the list.  Select that entry and click "Go!".

	![Select Device](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/swarmSelectDevice.png)

1.  You should now see live data from your Renesas device.  See the [tutorial folder](https://github.com/buglabs/bugswarm-renesas/tree/yrdkrl78g14/tutorial) for examples of how to use this data in your own application.

	![Running demo](https://raw.github.com/buglabs/bugswarm-renesas/yrdkrl78g14/tutorial/images/swarmRunning.png)


### Programming Instructions

Forthcoming.

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

### Troubleshooting

1.  If the device appears on the Renesas Web Portal as "UnknownDevice", try rebooting it.  Occasionally the first API call can fail if the wireless access point has not finished opening the connection.
1.  If an error is printed to the LCD screen on line 4 or line 8, and the error does not go away for a few minutes, try manually resetting the board.
1.  If the RDK board never appears on the web portal, use a smartphone or laptop to verify that the selected wireless access point has an internet connection, particularly to demo.bugswarm.com or developer.bugswarm.net.

### Relevant Documentation

* [bugswarm API documentation](http://developer.bugswarm.net/)
* [bugswarm javascript library](https://github.com/buglabs/bugswarm-js)
* [bugswarm preliminary configuration interface](http://demo.bugswarm.com/)
* [YRDKRL78G13 Quick Start Guide (how to install the SDK)](http://am.renesas.com/media/products/tools/introductory_evaluation_tools/renesas_demo_kits/yrdkrl78g13/child_folder/YRDKRL78G13_Quick_Start_Guide.pdf)
* [YRDKRL78G13 Getting Started DVD](http://am.renesas.com/media/products/tools/introductory_evaluation_tools/renesas_demo_kits/yrdkrl78g13/child_folder/YRDKRL78G13_DVD10.zip)

