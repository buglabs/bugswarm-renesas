## bugswarm-renesas: 

A bugswarm connector for Renesas 8- and 16-bit microcontrollers.

### Supported Boards

*  RL78/G13 Demonstration Kit (YRDKRL78G13) with the Redpine companion WiFi Card (RS-RL78G13-2200CC)

### Installation Instructions

1.  Follow the Quick Start Guide for your Renesas board.  [EXAMPLE](http://am.renesas.com/products/tools/introductory_evaluation_tools/renesas_demo_kits/yrdkrl78g13/child_folder/doc_child.jsp)
2.  After deploying a few of the provided test applications, clone this repository into a convient location, such as `My Documents\IAR Embeded Workbench`.
3.  Open IAR Embedded Workbench, select File->Open->Workspace, and locate the .eww file in the root of the cloned repository directory.
4.  Right click on the Project in the Workspace frame (Marked by a small cube), and click `Set As Active`
5.  Double click the file `rsi_global.h` in the `wifi_api` folder and edit the following sections: security info, scan info, join info.
6.  Open `CG_main.c` in the `applilet_src` folder, and edit any fields indicated by the code comments.
7.  Select the `Debug` target in the Workspace frame (at the top).  Right click on the Project and select `Rebuild All`
8.  Connect the Renesas board to your computer and select Project->Make & Restart Debugger.  Wait for the code to deploy, and the debugger to halt on main
9.  Select Debug->Go.  The connector will now begin running.  Watch the LCD screen for status messages!

### Usage

1.  Once the code has been deployed to the board, you can toggle Switch 2 on the DIP switch SW5.  That will allow the code to run whenever the board is powered (IE, you will not need to use the debugger).  Remember to re-set this switch in order to use the debugger. 
2.  Before powering on the board, open www.bugswarm.net and log in with username atergis_dp.  
3.  Click on the "RenesasDemo" swarm in the "My Swarms" tab.  Now you can either power on the Renesas board, or reset the application by pressing the RESET button.  
4.  Wait until you see the RenesasMicro resource come online (the small circle in the upper left hand corner of the resource icon will turn green, and the RenesasMicro text will turn black).  
5.  Click on the RenesasMicro resource, and on the Acceleration service.  If you cannot click on the Acceleration service, you may need to reset the Renesas board.  
6.  You should see raw acceleration data streaming on the screen.  Click on the grey "App" button to view a graph of the acceleration data.  Values have been normalized to G-Force (1G = 9.81N).

## Note

You may notice that the Renesas board is unable to connect immediately, displaying "Can't connect to SWARM, retrying" on the LCD screen.  This is caused by a previous connection being kept alive by a firewall in-between the Renesas board and the Swarm server.  The board will automatically attempt to reconnect, and should succeed on it's own in less than 2 minutes.  To expedite the process, you can kick the ea8da467c44d4e3f2327b9346f0869787aa8342a resource from the Multi User Chat.