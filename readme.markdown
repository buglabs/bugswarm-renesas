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