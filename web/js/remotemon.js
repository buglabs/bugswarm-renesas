var API_KEY = "7a849e6548dbd6f8034bb7cc1a37caa0b1a2654b";
var CFG_KEY = "53ebd64e50ed786ef13f3e64f0cdbdd6223013bc";
var SWARM_ID = "27e5a0e7e2e5445c51be56de44f45b19701f36d3";
var RESOURCE_ID = "87486e2239df55126a1f14b51a6e7b35e96b2422";
var WEBUI_RESOURCE = "8f0d348f7163b793cbe0b0f00117540e9339b7f8";

var resources = new Object();
var selectedResource = "";

var xAxisLength = 100;
var accelX = new Array();
var accelY = new Array();
var accelZ = new Array();
var temp = new Array();
var light = new Array();
var pot = new Array();

var plotOptions = {
    series: { shadowSize: 0 }, // drawing is faster without shadows
    grid: { color: "#FFF" },
    legend: { backgroundColor: "#5C5D60" },
    yaxes: [ { position: "left"}, { position: "right"} ]
};
    var gauge;
    var gaugeData;
    var gaugeOptions = {
          width: 240,
		  min: 0,
          max: 120,
          yellowFrom: 80,
          yellowTo: 90,
          redFrom: 90,
          redTo: 120,
          minorTicks: 5
      }


function onPresence(presence) {
    if (("swarm" in presence.from)&&(presence.from.resource != RESOURCE_ID)&&
            (presence.from.resource != WEBUI_RESOURCE)){
        var resource = presence.from.resource
        if ((resource in resources)&&(!("type" in presence))){
            //console.log('Adding one '+resource);
            resources[resource].count += 1;
        } else if (resource in resources) {
            resources[resource].count -= 1;
            //console.log('Removing one '+resource+', now '+resources[resource]);
            if (resources[resource].count === 0){
                $('li#'+resource).remove();
                delete resources[resource]
            }
        } else {
            //console.log('Welcome new '+resource);
            resources[resource] = {count:1};
            $('select#droplist').append('<OPTION VALUE='+resource+' id='+resource+'>'+resource+'</OPTION>');
            $('button').click(function(e){
                var resource = $("#droplist option:selected").val();
                //console.log('selecting '+resource);
                selectedResource = resource
                startTime = (new Date()).getTime();
                accelX = new Array();
                accelY = new Array();
                accelZ = new Array();
                temp = new Array();
                light = new Array();
                pot = new Array();
            });
            $.ajax({ url:'http://api.bugswarm.net/resources/'+resource, 
                type: 'GET',
                data: null,
                dataType: 'json',
                beforeSend: function(xhr) {
                    xhr.setRequestHeader("x-bugswarmapikey", CFG_KEY);
                },
                success: function(data){
                    //console.log(data.id+' is named '+data.name);
					$('option').filter('#'+resource).html(data.name);
                }});
        }
    } else {
        //console.log('presence -> ' + JSON.stringify(presence));
    }
}


	function drawGauge() {
      gaugeData = google.visualization.arrayToDataTable([
        ['Temperature'],
        [0]
      ]);
	 	gauge = new google.visualization.Gauge(document.getElementById('temp'));
		//gauge.draw(gaugeData, gaugeOptions);
	}
	
    function changeTemp(temp) {
      gaugeData.setValue(0, 0, temp);
      gauge.draw(gaugeData, gaugeOptions);
    }
    
    
    google.setOnLoadCallback(drawGauge);


function onMessage(message) {
    if (message.from.resource !== selectedResource){
        return;
    }
    var currentTime = (new Date()).getTime();
    var payload = message.payload;
    if (!("name" in payload)){
        //console.log('data -> '+JSON.stringify(message));
    } else if (payload.name === "Acceleration"){
		
        //console.log('accel: '+payload.feed.x+','+payload.feed.y+','+payload.feed.z);
        accelX.push([(currentTime-startTime)/1000,payload.feed.x]);
        accelY.push([(currentTime-startTime)/1000,payload.feed.y]);
        accelZ.push([(currentTime-startTime)/1000,payload.feed.z]);
        if (accelX.length > xAxisLength){
            accelX.shift();
            accelY.shift();
            accelZ.shift();
        }
        accelPlot = $.plot($('#accelChart'), [ accelX, accelY, accelZ ], plotOptions);
    } else if (payload.name === "Temperature"){
        //console.log('temp: '+payload.feed.TempF);
        temp.push([(currentTime-startTime)/1000,payload.feed.TempF]);
/*        if (temp.length > xAxisLength){
            temp.shift();
        }
*/		
		changeTemp(Math.round(payload.feed.TempF*100)/100);
		
        //tempPlot = $.plot($('#tempChart'), [ temp ], plotOptions);
    } else if (payload.name === "Light"){
        //console.log('light: '+payload.feed.Value);
        light.push([(currentTime-startTime)/1000,payload.feed.Value]);
        if (light.length > xAxisLength){
            light.shift();
        }
        lightPlot = $.plot($('#lightChart'), [ light ], plotOptions);
    } else if (payload.name === "Potentiometer"){
        //console.log('Potentiometer: '+payload.feed.Raw);
        pot.push([(currentTime-startTime)/1000,payload.feed.Raw]);
        if (pot.length > xAxisLength){
            pot.shift();
        }
        potPlot = $.plot($('#potChart'), [ pot ], plotOptions);
    } else if (payload.name === "Button"){
        $('#b1').html(payload.feed.b1);
        $('#b2').html(payload.feed.b2);
        $('#b3').html(payload.feed.b3);
    } else if (payload.name === "Sound Level"){
        //console.log(JSON.stringify(payload));
        $('span#soundlevel').html(payload.feed.Raw);
        $('meter').attr('value',parseInt(payload.feed.Raw));
    }
   //var payload = JSON.parse(message).message.payload;
   //console.log('Got data '+JSON.stringify(payload));
      //$.each(payload, function(key, value) {
      //   $('span#'+key).text(value);
      //   if (key == "Temperature"){
      //      if (tempdata.length > xAxisLength)
      //         tempdata.shift(); 
      //   }
}





function onError(error) {
	alert(JSON.stringify(error));
   //console.log('error! -> ' + JSON.stringify(error));
}
function onConnect() {
   //console.log('connected');
   startTime = (new Date()).getTime();
}
SWARM.connect({ apikey: API_KEY, 
               resource: RESOURCE_ID, 
               swarms: [SWARM_ID], 
               onmessage: onMessage, 
               onpresence: onPresence,
               onerror: onError,
               onconnect: onConnect});
