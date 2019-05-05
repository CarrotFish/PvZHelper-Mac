# Mac PvZ Helper
A tool used to modify the memory of game Plants vs. Zombies 1.0.40 on Mac.

## Build
You need a c++ 11 compiler and Qt5 library to build the project.
The release version was built using Apple LLVM 10.0.1 and Qt 5.12.3.

## Note
To use task_for_pid(), root permission is required when launching this tool.
I used the STPrivilegedTask library to build a wrapper that askes for root permission to launch the tool in the release version. You can also launch the tool directly using the sudo command.
Memory modification of the game may cause crashes and data loses. Please be cautious and back up your gamedata when using the tool.

## Contribution
The code is based on PvZ Tools v2.0.1 by Lmintlcx. The memory modification code is based on the xnumem library by Jonathan Daniel.
