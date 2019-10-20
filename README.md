# CustomCPPCommandHandler
A Custom Command Handler written in C++

## Initialize Project
This works directly on the start with a test command.
To pull this project use Visual Studio's GitHub plugin.

## Create a Command
Create a new c++ class and import the ``CommandLib.h`` file. Let your class extend from Command and build your constructor.
It needs to initialize the command constructor to make your command valid.

```c++
#include "CommandLib.h"
class ExampleCommand : public Command {
  ExampleCommand()
    : Command("example") // <--
      {}
};
```

Then create the call void function. It has a ``vector<string>`` parameter in wich are the arguments to the command.
You can now choose if you want to handle the arguments yourself or give it to the ArgumentList. This will automatically sort
your argument list to Argument Objects wich have an argument title and argument parameters. It will also check if the user
has written the string arguments right. Here is an example to the call function and argument list:

```c++
#include "CommandLib.h"
#include <vector>
#include <string>

class ExampleCommand : public Command {
  ExampleCommand()
    : Command("example")
      {}
      
    void call(std::vector<std::string> args) override {
      ArgumentList argList(args); // Argument string list get sorted into objects
      
      if (argList.noCll()) { // Checks if command spelling is right
          std::cout << "Command has false spelling." << std::endl;
        } else {
          Argument arg = findBoth("t", "test"); // Checks if command has a ``-t`` or ``--test`` parameter
          // For example you could get the arguments from the parameters with arg.getArg() wich would return a vector with strings
      }
    }
};
```
_More exact description to Argument List is coming soon_

## Register a Command
Commands are getting registered in the main.cpp file. They get passed to the CommandHandler which is then passed to the RunCmdBase function.
Keep the rythm to the first registration and the next commands are getting also registered.

## Simulated console
The simulated console opens if the program is in debug mode, the user clicks on the program or the user only types the command.
Commands are getting the same way executed but the user only needs to type the subcommands to execute them. Example on test command:<br><br>
**Normal Command**

``command test --test argument``

**Command in simulated console**

``test --test argument``

# Zyniac
Zyniac is an open source tool for hosting data. It has a good plugin support and runs on Windows and Linux but it should run on Mac as well.

> Currently Zyniac is in developement and not every Feature is fully ready.

## How to build Zyniac?
Zyniac was built in Visual Studio. Right now there is no cmake configuration but we will work on it in the future.
Because of that Zyniac can only build on Windows at the monment.

First, the Mozilla Firefox build is needed because Spidermonkey is embedded in Zyniac (It is the plugin engine that executes the js files). The build folder must be in the C drive, else the project won't find the required includes for SpiderMonkey.

Rapid Json, Cereal and rlutil are also needed but they are already included in the workspace.

After that, use the Github extension in Visual Studio to clone this project. After this, the project should run but I suggest to look if all the include paths and library paths are setup correctly on your device.

## How does Zyniac run?
Zyniac has it's own servers, protocols, crypt technologies and file handlers and because of that it runs faster and has only the tools that it needs. With the extensions almost everything can be build and because of that there are almost no limits (examples: more commands for servers and clients, other servers, crypt technologies, databases). Zyniac is fully build in c++ and it is a header only project.