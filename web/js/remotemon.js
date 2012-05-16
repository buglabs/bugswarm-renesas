var API_KEY = "7a849e6548dbd6f8034bb7cc1a37caa0b1a2654b";
var SWARM_ID = "27e5a0e7e2e5445c51be56de44f45b19701f36d3";
var RESOURCE_ID = "87486e2239df55126a1f14b51a6e7b35e96b2422";
var WEBUI_RESOURCE = "8f0d348f7163b793cbe0b0f00117540e9339b7f8";

var resources = new Object();

//var plotOptions = {
//        series: { shadowSize: 0 }, // drawing is faster without shadows
//        grid: { color: "#FFF" },
//        legend: { backgroundColor: "#5C5D60" },
//        yaxes: [ { position: "left"}, { position: "right"} ]
//    };

function onPresence(presence) {
    if (("swarm" in presence.from)&&(presence.from.resource != RESOURCE_ID)&&
            (presence.from.resource != WEBUI_RESOURCE)){
        var resource = presence.from.resource
        if ((resource in resources)&&(!("type" in presence))){
            console.log('Adding one '+resource);
            resources[resource] += 1;
        } else if (resource in resources) {
            resources[resource] -= 1;
            console.log('Removing one '+resource+', now '+resources[resource]);
            if (resources[resource] === 0){
                $('li#'+resource).remove();
                delete resources[resource]
            }
        }else {
            console.log('Welcome new '+resource);
            resources[resource] = 1;
            $('ul#resources').append('<li id='+resource+
                '><button class="reslist disabled" id='+resource+'>'+resource+'</button></li>');
            $('button.reslist#'+resource).click(function(e){
                var resource = e.target.id;
                console.log('selecting '+resource);
                $('button.reslist').addClass('disabled');
                $('button.reslist#'+resource).removeClass('disabled');
            });
        }
    } else {
        //console.log('presence -> ' + JSON.stringify(presence));
    }
}

function onMessage(message) {
    //console.log('data -> '+JSON.stringify(message));
   //var payload = JSON.parse(message).message.payload;
   //console.log('Got data '+JSON.stringify(payload));
   //var currentTime = (new Date()).getTime();
      //$.each(payload, function(key, value) {
      //   $('span#'+key).text(value);
      //   if (key == "Temperature"){
      //      tempdata.push([(currentTime-startTime)/1000,value]);
      //      if (tempdata.length > xAxisLength)
      //         tempdata.shift(); 
      //   }
}
function onError(error) {
   console.log('error! -> ' + JSON.stringify(error));
}
function onConnect() {
   console.log('connected');
   startTime = (new Date()).getTime();
}

SWARM.connect({ apikey: API_KEY, 
               resource: RESOURCE_ID, 
               swarms: [SWARM_ID], 
               onmessage: onMessage, 
               onpresence: onPresence,
               onerror: onError,
               onconnect: onConnect});
