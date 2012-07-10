bugswarm-renesas web tutorial
-----------------------------

### Prerequisites ###

* Some small knowledge of HTML and Javascript
* Any PC or Laptop (preferably running a recent Chrome or Firefox browser with firebug)
* The contents of this directory saved to a location on your PC

### Background ###

The bugswarm platform enables developers to quickly and easily utilize networked Renesas hardware without needing to modify the embedded code.  This tutorial demontrates how to get started creating a new web application.  Before diving into sample code, lets take a moment to get familiar with the swarm platform:

![Renesas bugswarm organization](https://raw.github.com/buglabs/bugswarm-renesas/master/tutorial/images/swarm\_org.png)

* Each resource is any agent within the system - web browser, server daemon, demo board
* A Swarm is a collection of resources that can communicate together.
* All Renesas devices will be contained within the 'renesas' account
* All RL78 devices will by default be located in the 'rl78\_web\_portal' swarm
* The 'web\_rl78' resource is shared by all web clients to the system
* The 'Simulator01' resource is a server thread that simulates an online RL78 board
* All other resources are assumed to be RL78 demo boards, named by their unique MAC address

In this default configuration, the simulator and live RL78 boards all continually 'produce' information to the 'rl78\_web\_portal' swarm.  Any web-browser, using the 'web\_rl78' resource, can 'consume' the information produced by the boards.  These elements can be viewed and configured using [the bug swarm configuration panel](http://demo.bugswarm.com).  Renesas users can log in using the username 'renesas' and the password 'renesaspsk'.  If you select the 'My Swarms' tab, and then click on the 'rl78\_web\_portal' swarm, all of the important ID values can be read:

![bugswarm configuration panel](https://raw.github.com/buglabs/bugswarm-renesas/master/tutorial/images/swarm\_config\_panel.png)

1. Configuration API Key for the 'renesas' account
1. Participation API Key for the 'renesas' account
1. SwarmID for the 'rl78\_web\_portal' swarm
1. ResourceID for the 'web\_rl78' resource
1. ResourceID for the 'Simulator01' resource
1. ResourceID for a demonstration board with MAC address '00:23:A7:1F:22:2F'

These numbers will be needed to customize your web application.  Lets look at the first example, 'barebones.html' step-by-step:

### Barebones.html ###

This is the starting place for most bugswarm applications.  This simple example connects to the 'rl78\_web\_portal' swarm, and merely prints any activity or errors to the javascript console.  Drag the 'barebones.html' file into an open chrome or firefox window.  The webpage itself is blank - right click on the empty space and select 'inspect element' or 'inspect element with firebug'.  Then click on the 'Console' tab.  You should now see live swarm data streaming on the screen.

There are three main elements to this example:

#### Include the swarm library ####

        <head>
            <script type="text/javascript" src="http://cdn.buglabs.net/swarm/swarm-v0.4.0.js"></script>
        </head>

At the top of the HTML document, in the HEAD section, the swarm library must be loaded.  The 'script' tag will download the swarm library from the CDN server, and make it available to javascript executed below.

#### Define the call-back functions ####

        function onPresence(presence) {
            console.log('presence -> ' + Date.now() + ':' + JSON.stringify(presence));
        }
    
        function onMessage(message) {
            console.log('message -> ' + Date.now() + ': ' + JSON.stringify(message));
        }
    
        function onError(error) {
            console.log('error! -> ' + JSON.stringify(error));
        }
    
        function onConnect() {
            console.log('connected!');
        }

The main application code resides within the next 'script' tag, within the BODY.  The meat of this code is the definition of these four call-back functions.  Each is aptly named:

* onPresence will be executed whenever any resource leaves or enters the swarm
* onMessage will be executed whenever a message is received
* onError will be executed whenever an error is thrown
* onConnect will be executed when the client successfully connects to the swarm server

Here we are only printing the retrieved data.  The data is stored in the [JSON format](http://www.json.org/).  Fortunately, this is the native data-storage format for javascript - the next example will demonstrate that the 'message' object can be directly interacted with.  Take note of the data printed to the console when the applicaiton is run - this demonstrates what format to expect.

#### Provide credentials and connect to swarm ####

        SWARM.connect({ apikey: 'bc60aa60d80f7c104ad1e028a5223e7660da5f8c',
                        resource: '5cf5ad58fa9ad98a01841fde8e1761b2ca473dbf',
                        swarms: ['2eaf3f05cd0dd4becc74d30857caf03adb85281e'],
                        onmessage: onMessage,
                        onpresence: onPresence,
                        onerror: onError,
                        onconnect: onConnect
        });

The actual swarm connection is established in a single function call with many arguments.  Upon executing this call, the swarm client will attempt to connect to the server, and will announce it's success by executing the 'onConnect' function.  The other functions defined above are specified here: 'onMessage', 'onPresence', and 'onError'.  The first three arguments are all data from the swarm detail page above:

* 'apikey' is the Participation API Key from above
* 'resource' is the 'web\_rl78' resource ID from above
* 'swarms' is a list with a single entry: the 'rl78\_web\_portal' swarm ID.

This is the only swarm-specific part of most swarm interactions.  Application development generally involves starting with this barebone code, adding page content, and modifying the page content based on data returned by the onMessage callback.  

### display\_values.html ###
