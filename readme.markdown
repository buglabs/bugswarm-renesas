## bugswarm-renesas: 

A bugswarm connector for Renesas 8- and 16-bit microcontrollers.

### Supported Boards

*  RL78/G13 Demonstration Kit (YRDKRL78G13) with the Redpine companion WiFi Card (RS-RL78G13-2200CC)

### Status

The rewrite branch is a rewrite of the master branch, from the ground up.  It involves a switch from IAR compiler to CubeSuite (for the updated Code Generator).  This project now properly uses the Code Generator built into CubeSuite, enabling additional peripherals to be implemented with greater ease.  Attempts are also being made to keep the code clean and organized.