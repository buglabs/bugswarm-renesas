# bugswarm-renesas: 

A bugswarm connector for Renesas 8- and 16-bit microcontrollers.  This code turns a compatible evaluation board into a real-time internet enabled device.  Once the connector is deployed to a device, the device will automatically connect to the bugswarm platform and share it's peripherals using a standardized API.  This enables developers to create applications for the evaluation board in a wide variety of languages, without needing to download an SDK or physical access to the device.  

### Supported Boards

*  RL78/G13 Demonstration Kit (YRDKRL78G13) 
    *  Redpine companion WiFi Card (RS-RL78G13-2200CC)

### How to use an RL78/G13 board that has already been programmed

1. Set up a wireless access point, or Mifi tether with the SSID ```renesasdemo``` and password ```renesaspsk```.  The wireless security can be WPA1 or WPA2 Personal.  See the deployment instructions below to connect to a different ESSID.
1. (Optional) Connect a laptop or cellphone to this wireless access point, verify it can connect and verify that it has internet connectivity.
1. Make sure that Switch 2 of SW5 is set to the up or 'On' position.  [See the quick start guide](http://am.renesas.com/media/products/tools/introductory_evaluation_tools/renesas_demo_kits/yrdkrl78g13/child_folder/YRDKRL78G13_Quick_Start_Guide.pdf) for more details.

	![Switch 5 Location on YRDKRL78G13 board](https://raw.github.com/buglabs/bugswarm-renesas/master/tutorial/images/RL78\_SW5\_location.jpg)

1. Using a USB Micro cable, connect the RL78 board to a USB power source.  This can be a PC, Laptop, or USB wall charger.
1. The demo board will take several seconds to connect to the wireless access point.  When it has successfully connected, LED6 (the green led to the left of the LCD screen) will blink.
1. Navigate to [buglabs.github.com/bugswarm-renesas/](http://buglabs.github.com/bugswarm-renesas/)
1. Wait a few seconds and then click on the ```Select an RL78/G13 board to view" dropdown menu```
1. Select the entry that matches the MAC address printed on the Wifi Companion card, and hit ```Go!```
1. The graphs should update with data as it is sent by the RL78 board.

See the troubleshooting section below if board does not appear in the list, or data is not produced.

### How to Deploy the connector to an RL78/G13 demo board

1. Download and install the newest version of CubeSuite+ from [the following link](http://am.renesas.com/support/downloads/download_results/C1000000-C9999999/tools/evaluation_cubesuite_plus.jsp)
1. Open CubeSuite
1. Download the newest version of the bugswarm connecter [from this repository link](https://github.com/buglabs/bugswarm-renesas/zipball/master)
1. Extract the zip file to a reasonable location, like My Documents/CubeSuite/bugswarm-renesas/
1. Within CubeSuite, select ```File -> Open``` and navigate to ```bugswarm-renesas.Buglabs.mtud``` within the directory from the previous step.
1. In the pane on the left hand side of the application, double click on the file ```rsi_global.h``` within the ```redpine``` folder.  

	![rsi_global](https://raw.github.com/buglabs/bugswarm-renesas/master/tutorial/images/rsi_global.png)

1. The board is configured to connect to ESSID ```renesasdemo``` with password ```renesaspsk```.  If you would like to change these settings, modify the variables ```PSK```, ```SCAN_SSID```, and ```JOIN_SSID``` accordingly

	![rsi_global variables](https://raw.github.com/buglabs/bugswarm-renesas/master/tutorial/images/rsi_global_variables.png)

1. Log in to [demo.bugswarm.com](http://demo.bugswarm.com)
1. Click on the New Resource button
1. Fill out the "Name" field with a unique value, such as the MAC address printed on the redpine companion card
1. Fill out the "Description" field with "RL78/G13 Demonstration Board"
1. Click on "Create"

	![create a new resource process](https://raw.github.com/buglabs/bugswarm-renesas/master/tutorial/images/new_resource.png)

1. Click on the "My Swarms" tab.
1. Select the "rl78\_web\_portal" swarm.
1. Click the "Edit" button.

	![swarm edit button](https://raw.github.com/buglabs/bugswarm-renesas/master/tutorial/images/Swarm_edit.png)

1. Find the resource that was created previously in the right hand side of the screen, and click on the grey "producer" button (do NOT select the "consumer" button).
1. Click on the "Update" button.

	![add resource to swarm](https://raw.github.com/buglabs/bugswarm-renesas/master/tutorial/images/add_resource_to_swarm.png)

1. In the next screen, find the resource we just created and copy the text next to the ```ID:``` label.

	![swarm config panel](https://raw.github.com/buglabs/bugswarm-renesas/master/tutorial/images/swarm_config_panel.png)

1. In the CubeSuite IDE, double click on the file ```r_main.c``` within the ```gen``` folder.
1. Replace the text immediately following ```const char resource_id[] = ``` with the ID copied from the bugswarm webpage, similarly in quotes.

	![add resource to source code](https://raw.github.com/buglabs/bugswarm-renesas/master/tutorial/images/add_resource_to_code.png)

1. Connect the Redpine Wifi Card to the App header slot on the RL78/G13 board ([See this Diagram](http://am.renesas.com/products/tools/introductory_evaluation_tools/renesas_demo_kits/yrdkrl78g13/sub/layout_child.jsp)).  The components on the wifi card should be facing inwards.

	![Redpine wifi code location](https://raw.github.com/buglabs/bugswarm-renesas/master/tutorial/images/RL78_redpine_card.jpg)

1. On the RL78G13 demo board, make sure that switch 2 on SW5 is in the downward (or 'Off') position.  [See the quick start guide](http://am.renesas.com/media/products/tools/introductory_evaluation_tools/renesas_demo_kits/yrdkrl78g13/child_folder/YRDKRL78G13_Quick_Start_Guide.pdf) for more details.
1. Connect the RL78G13 demo board to the PC using a USB cable
1. Within CubeSuite, press the ```<F6>``` button.  The project should compile and deploy to the demo board.
1. When the download has completed, the screen should change and should highlight a single line in ```r_main.c``` in yellow.  When this happens, press the ```<F5>``` key.

	![debugger waiting at top of main](https://raw.github.com/buglabs/bugswarm-renesas/master/tutorial/images/debugger_waiting_at_main.png)

1. The application should start running.

### Implementation Details

This bugswarm connector works by opening a socket to api.bugswarm.net, creating a new production session, and continually publishing sensor data (See [the swarm API documentation](http://developer.bugswarm.net/participation_api.html)).  This application currently requires the following credentials to be acquired and inserted into the source code before use:

* Wireless ESSID ([rsi_global.h](https://github.com/buglabs/bugswarm-renesas/blob/master/redpine/rsi_global.h#L114))
    * default: ```renesasdemo```
* Wireless password (if applicable) ([rsi_global.h](https://github.com/buglabs/bugswarm-renesas/blob/master/redpine/rsi_global.h#L102))
    * default: ```renesaspsk```
* Swarm Producer API Key ([r_main.c](https://github.com/buglabs/bugswarm-renesas/blob/master/gen/r_main.c#L106))
    * default: ```7a849e6548dbd6f8034bb7cc1a37caa0b1a2654b```
* A valid Swarm ID ([r_main.c](https://github.com/buglabs/bugswarm-renesas/blob/master/gen/r_main.c#L104))
    * default: ```27e5a0e7e2e5445c51be56de44f45b19701f36d3```
* A valid Resource ID, having already been added to the supplied swarm. ([r_main.c](https://github.com/buglabs/bugswarm-renesas/blob/master/gen/r_main.c#L105))

All Renesas devices will share the same producer API Key - this is linked to the Renesas account.  In order for all of the Renesas RL78/G13 demo boards to be listed together, they all need to be added to the same swarm.  Devices should use the default swarm ID above, unless they are being configured for a custom application.
Resource IDs, on the other hand, MUST be unique for each demo board.  When provisioning a new demo board, a new resource ID will need to be generated.  We suggest that the resource ID have a unique name, such as the MAC address of the wireless module, but the resource ID will always be a unique 40 character string.  

#### Known Issues

* The Redpine device cannot send data at the same time that data is being recieved.  Data reception is fully asynchronous, and could occur at any time.  The current libraries do not successfully detect that data reception is in progress while sending.  When this occurs, depending on the timing, it may cause the redpine device to abruptly close the socket.  The current code will automatically attempt to reconnect.  In the future, we should aggressively check that the UART is not in the middle of reading a line before sending any new AT commands.  This needs to occur at the UART API level of the redpine libraries.
* The current Redpine libraries cannot reliably read a multi-line response from an open socket.  The first line of a repsonse is prefixed with ```AT+RSI_READ``` and is successfully detected.  However, since the response is not byte-stuffed, subsequent lines within the same packet are detected as junk data (since they are not prefixed with ```AT+RSI_READ```).  This requires a larger overhaul of the ```rsi_read()``` function. 
* Even with a modified ```rsi_read()``` function, no mechanism exists to ensure that an entire packet-worth of data has been successfully read.  The total packet size is returned in the initial ```AT+RSI_READ``` command, but very often data is either lost or read twice while reading.  Data is being read successfully by the ```rsi_receive()``` function, but a valid state machine has not been created for the ```rsi_read()``` function with multi-line packets.  

### Troubleshooting

If the RL78 demonstration board does not appear or remain online, you will need to connect to a serial console on the demo board.  

1. [Obtain an RS232 to USB converter](http://www.amazon.com/s/ref=nb_sb_noss_1?url=search-alias%3Daps&field-keywords=RS232+USB)
1. Connect the RS232 adapter to the RS232 port on the RL78 demontration board, and into a computer or laptop.
1. Open a serial terminal emulator like HyperTerm or Putty and connect to the USB-Serial port.  Use Baud rate 115200, 8 bits, no parity, 1 stop bit, and no hardware flow control.  Here is [an example tutorial](http://www.laser-interceptorusa.com/DownloadFiles/rs232_to_usb_problem_solving.pdf)
1. Press the ```Reset``` button on the RL78 demo board, and observe the serial output
1. Reproduce the error, and copy/paste all of the relevant serial output into [pastebin.com](http://pastebin.com/).  press "Submit".
1. Contact [buglabs support](http://www.buglabs.net/support), and provide the URL of the pastebin website.

### Relevant Documentation

* [bugswarm API documentation](http://developer.bugswarm.net/)
* [bugswarm javascript library](https://github.com/buglabs/bugswarm-js)
* [bugswarm preliminary configuration interface](http://demo.bugswarm.com/)
* [Redpine Signals Companion Card resources](http://www.redpinesignals.com/Renesas/Wi-Fi_with%20_RL78.html)
* [Renesas RL78G13 Demo Kit resources](http://am.renesas.com/products/tools/introductory_evaluation_tools/renesas_demo_kits/yrdkrl78g13/index.jsp)
* [YRDKRL78G13 Quick Start Guide (how to install the SDK)](http://am.renesas.com/media/products/tools/introductory_evaluation_tools/renesas_demo_kits/yrdkrl78g13/child_folder/YRDKRL78G13_Quick_Start_Guide.pdf)
* [YRDKRL78G13 Getting Started DVD](http://am.renesas.com/media/products/tools/introductory_evaluation_tools/renesas_demo_kits/yrdkrl78g13/child_folder/YRDKRL78G13_DVD10.zip)
* [RL78G13 CPU Hardware Manual](http://documentation.renesas.com/doc/products/mpumcu/doc/rl78/r01uh0146ej0200_rl78g13.pdf)
* Further redpine documenation only available by contacting redpine suppport.

