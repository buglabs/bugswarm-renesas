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
* All Renesas devices will be contained within the ```renesas``` account
* All RL78 devices will by default be located in the ```rl78_web_portal``` swarm
* The ```web_rl78``` resource is shared by all web clients to the system
* The ```Simulator01``` resource is a server thread that simulates an online RL78 board
* All other resources are assumed to be RL78 demo boards, named by their unique MAC address

In this default configuration, the simulator and live RL78 boards all continually 'produce' information to the ```rl78_web_portal``` swarm.  Any web-browser, using the ```web_rl78``` resource, can 'consume' the information produced by the boards.  These elements can be viewed and configured using [the bug swarm configuration panel](http://demo.bugswarm.com).  Renesas users can log in using the username ```renesas``` and the password ```renesaspsk```.  If you select the ```My Swarms``` tab, and then click on the ```rl78_web_portal``` swarm, all of the important ID values can be read:

![bugswarm configuration panel](https://raw.github.com/buglabs/bugswarm-renesas/master/tutorial/images/swarm\_config\_panel.png)

1. Configuration API Key for the ```renesas``` account
1. Participation API Key for the ```renesas``` account
1. SwarmID for the ```rl78_web_portal``` swarm
1. ResourceID for the ```web_rl78``` resource
1. ResourceID for the ```Simulator01``` resource
1. ResourceID for a demonstration board with MAC address ```00:23:A7:1F:22:2F```

These numbers will be needed to customize your web application.  Lets look at the first example, ```barebones.html``` step-by-step:

## Barebones example ##

This is the starting place for most bugswarm applications.  This simple example connects to the ```rl78_web_portal``` swarm, and merely prints any activity or errors to the javascript console.  Drag the ```barebones.html``` file into an open chrome or firefox window.  The webpage itself is blank - right click on the empty space and select ```inspect element``` or ```inspect element with firebug```.  Then click on the ```Console``` tab.  You should now see live swarm data streaming on the screen.

There are three main elements to this example:

#### Include the swarm library ####

        <head>
            <script type="text/javascript" src="http://cdn.buglabs.net/swarm/swarm-v0.4.0.js"></script>
        </head>

At the top of the HTML document, in the HEAD section, the swarm library must be loaded.  The ```script``` tag will download the swarm library from the CDN server, and make it available to javascript executed below.

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

The main application code resides within the next ```script``` tag, within the BODY.  The meat of this code is the definition of these four call-back functions.  Each is aptly named:

* onPresence will be executed whenever any resource leaves or enters the swarm
* onMessage will be executed whenever a message is received
* onError will be executed whenever an error is thrown
* onConnect will be executed when the client successfully connects to the swarm server

Here we are only printing the retrieved data.  The data is stored in the [JSON format](http://www.json.org/).  Fortunately, this is the native data-storage format for javascript - the next example will demonstrate that the ```message``` object can be directly interacted with.  Take note of the data printed to the console when the applicaiton is run - this demonstrates what format to expect.

#### Provide credentials and connect to swarm ####

        SWARM.connect({ apikey: 'bc60aa60d80f7c104ad1e028a5223e7660da5f8c',
                        resource: '5cf5ad58fa9ad98a01841fde8e1761b2ca473dbf',
                        swarms: ['2eaf3f05cd0dd4becc74d30857caf03adb85281e'],
                        onmessage: onMessage,
                        onpresence: onPresence,
                        onerror: onError,
                        onconnect: onConnect
        });

The actual swarm connection is established in a single function call with many arguments.  Upon executing this call, the swarm client will attempt to connect to the server, and will announce it's success by executing the ```onConnect``` function.  The other functions defined above are specified here: ```onMessage```, ```onPresence```, and ```onError```.  The first three arguments are all data from the swarm detail page above:

* ```apikey``` is the Participation API Key from above
* ```resource``` is the ```web_rl78``` resource ID from above
* ```swarms``` is a list with a single entry: the ```rl78_web_portal``` swarm ID.

This is the only swarm-specific part of most swarm interactions.  Application development generally involves starting with this barebone code, adding page content, and modifying the page content based on data returned by the onMessage callback.  

## Display values example ##

The barebones example is a good tool for getting started with swarm, but we need to be able to link incoming data to DOM elements.  The next demo, ```display_values_final.html```, demonstrates the most basic interaction between swarm javascript code and HTML elements.  Drag the file into your chrome or firefox browser.  This time the javascript isn't completely necessary.  Paste in the following ResourceID into the text box, and click "Submit": ```51a8cbfc68de1a72578a0f8d844c8646e1fb7309```.  You should see the fields below update with live data.  We shall implement this demo in a few stages.  Load ```display_values_01.html``` into your browser, open up the javascript console, and take a look at the source code: 

#### Add resource selection - HTML Form ####

We first start by adding a filtering mechanism to the incoming data stream.  If you look carefully at the data printed to the javascript console from the barebones example, you will notice that each received message is labeled by the resource that transmitted the message.  We will create a new variable and will compare the incoming resourceID to the variable to create our filter:

    	var selectedID = "";
    	function onMessage(message) {
    		if (message.from.resource == selectedID){
    			console.log('message -> ' + Date.now() + ': ' + JSON.stringify(message));
    		}
    	}

Notice that the syntax for accessing nested objects in javascript. Message has three objects inside it: ```from```, ```payload```, and ```public```.  From has two objects inside it: ```swarm``` and ```resource.  ```message.from.resource``` selects the resource string from within the nested objects.  Next we create a simple helper function to change that selectedID variable:

    	function selectResource(id) {
    		console.log('setting ID to '+id);
    		selectedID = id;
    	}     

This function is not strictly necessary, but makes it a little easier for us to change the resourceID we want to filter.  More importantly, we need some way for the user to execute ```selectResource()```.  How about an HTML Form:

        Enter ResourceID to follow:
        <form action="" onsubmit="selectResource(this.resourceid.value); return false;">
                <input type="text" id="resourceid" name="resourceid" />
                <input type="submit" value="Submit" />
        </form></br>

This HTML form will present the user with a text box and a submit button.  Note that we need to use a trick: normally an HTML form will redirect the web browser to a new page.  We are avoiding that action by specifying a blank ```action``` parameter, and having the ```onsubmit``` parameter return false when done.  This will execute the ```selectResource()``` function with whatever text the user has entered in the text box.

You can test this intermediate example.  it's a nice upgrade from the barebones example, you can view the output from a particular resource, and then enter a blank string and click submit to stop the output.  Then you can copy the text from your javascript window for further analysis.  Onwards to ```display_values_02.html```

#### Linking data to the page ####

We need a way to display the incoming swarm data on the page.  The first order of business will be to create a place within the HTML of the page to insert the data.  The basic workflow here is:

1. Create the structure of the page without any data
1. Wherever you would like to display data,leave a ```<span>``` or ```<div>``` tag.
1. Give each tag a unique ```id``` attribute, and a default value.
1. Add javascript code to replace the contents of the tag when new data arrives.

For this basic example, we will use basic lines of HTML with ```<span>``` tags to delineate data:

        Acceleration: ( <span id=accelx>0</span>,
                        <span id=accely>0</span>,
                        <span id=accelz>0</span>)</br>
        Temperature: <span id=temp>0</span> &deg;F </br>
        Light Level: <span id=light>0</span> lux </br>
        Potentiometer: <span id=pot>0</span> </br>
        Sound Level: <span id=sound>0</span> </br>
        Buttons: button1:   <span id=but1>0</span>
                 button2:   <span id=but2>0</span>
                 button3:   <span id=but3>0</span> </br >

Next we need to make sure that we are accessing the correct data.  If you examine the data being produced by the renesas board, you will notice the common ```name``` and ```feed``` objects within the ```payload``` object.  We can use the ```name``` object to identify which sensor is being referenced, but first we need to double check that the object exists:

    if ((message.from.resource == selectedID)&&("name" in message.payload)){
    	var payload = message.payload;
    	if (payload.name == "Acceleration"){
    	} else if (payload.name == "Temperature"){
    	} else if (payload.name == "Light"){
    	} else if (payload.name == "Potentiometer"){
    	} else if (payload.name == "Sound Level"){
    	} else if (payload.name == "Button"){
    	}

We kept the ```message.from.resource``` filtering from before, but added an additional check to verify the ```name``` object exists.  Next we look at the swarm data, and make a list of all the different names being produced by the renesas board.  Finally, we create a bunch of if statements to filter by sensor.  Now lets do something with the data:

    if ((message.from.resource == selectedID)&&("name" in message.payload)){
    	var payload = message.payload;
    	if (payload.name == "Acceleration"){
    		document.getElementById("accelx").innerHTML = payload.feed.x;
    		document.getElementById("accely").innerHTML = payload.feed.y;
    		document.getElementById("accelz").innerHTML = payload.feed.z;
    	} else if (payload.name == "Temperature"){
    		document.getElementById("temp").innerHTML = payload.feed.TempF;
    	} else if (payload.name == "Light"){
    		document.getElementById("light").innerHTML = payload.feed.Value;
    	} else if (payload.name == "Potentiometer"){
    		document.getElementById("pot").innerHTML = payload.feed.Raw;
    	} else if (payload.name == "Sound Level"){
    		document.getElementById("sound").innerHTML = payload.feed.Raw;
    	} else if (payload.name == "Button"){
    		document.getElementById("but1").innerHTML = payload.feed.b1;
    		document.getElementById("but2").innerHTML = payload.feed.b2;
    		document.getElementById("but3").innerHTML = payload.feed.b3;
    	}

The ```document.getElementById()``` function is built in to every web browser, and will return a reference to the object with the specified ```id``` attribute.  The ```innerHTML``` parameter can be used to replace the contents of the tag.  In this case we will be replacing the contents of the span tag ('0') not the span tag itself.  Finally we extract the correct data for each sensor by looking at the raw swarm data from before.

With the example thus far, we should see changing values on the screen.  It may not look very pretty, but standard web design practices can be employed to make a beautiful display.
