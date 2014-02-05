var API_KEY = "bc60aa60d80f7c104ad1e028a5223e7660da5f8c";
var CFG_KEY = "359aff0298658552ec987b9354ea754b684a4047";
var SWARM_ID = "";
var swarms = [
  "2eaf3f05cd0dd4becc74d30857caf03adb85281e",
  "69df1aea11433b3f85d2ca6e9c3575a9c86f8182",
  "5dbaf819af6eeec879a1a1d6c388664be4595bb3"
];

var RESOURCE_ID = "5cf5ad58fa9ad98a01841fde8e1761b2ca473dbf";
var WEBUI_RESOURCE = "c907c8bb15914a829b256c908aab2c54af48f5f3";

var resources = {};
var selectedResource = "";
var selectedSwarm = "";

var xAxisLength = 100;
var accelX = [];
var accelY = [];
var accelZ = [];
var temp = [];
var light = [];
var pot = [];

var myThemes = new Array("Grey");

var plotOptions = {
    series: { shadowSize: 0 }, // drawing is faster without shadows
    grid: { color: "#FFF" },
    legend: { backgroundColor: "#5C5D60" },
    yaxis: { position: "left"}
};

var accelOptions = {
    series: { shadowSize: 0 }, // drawing is faster without shadows
    grid: { color: "#FFF" },
    legend: { backgroundColor: "#5C5D60" },
    yaxis: { position: "left",
       min: -1.5,
       max: 1.5 }
};

var potOptions = {
    series: { shadowSize: 0 }, // drawing is faster without shadows
    grid: { color: "#FFF" },
    legend: { backgroundColor: "#5C5D60" },
    yaxis: { position: "left",
       min: -10,
       max: 1200 }
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
};


function loadThemes() {
  //console.log('in function');
  for (var i in myThemes) {
    $('option#default').remove();
    $('select#themeselect').append('<OPTION VALUE='+myThemes[i]+' id='+myThemes[i]+'>'+myThemes[i]+'</OPTION>');
  }
}

function changeTheme() {
        var theme = $("#themeselect option:selected").val();
                //console.log('selecting '+theme );
        $("link[id=style]").attr({href : 'css/'+theme+'.css'});

}

function populateResourceList() {
  $('option.reslistitem').remove();
  for (var resource in resources[selectedSwarm]){
    $('select#droplist').append('<OPTION class=reslistitem VALUE='+resource+' id='+resource+'>'+resources[selectedSwarm][resource]+'</OPTION>');
  }
}

//For IE >.<
function createCORSRequest(method, url) {
  var xhr = new XMLHttpRequest();
  if ("withCredentials" in xhr) {

    // Check if the XMLHttpRequest object has a "withCredentials" property.
    // "withCredentials" only exists on XMLHTTPRequest2 objects.
    xhr.open(method, url, true);

  } else if (typeof XDomainRequest != "undefined") {

    // Otherwise, check if XDomainRequest.
    // XDomainRequest only exists in IE, and is IE's way of making CORS requests.
    xhr = new XDomainRequest();
    xhr.open(method, url);

  } else {

    // Otherwise, CORS is not supported by the browser.
    xhr = null;

  }
  return xhr;
}

function onPresence(presence) {
    if (("swarm" in presence.from) &&
        (presence.from.resource !== RESOURCE_ID) &&
        (presence.from.resource !== WEBUI_RESOURCE)){
        var resource = presence.from.resource;
        var swarm = presence.from.swarm;
    if (presence.type === "unavailable"){
      delete resources[swarm][resource];
      //console.log('Presence ('+resource+'): REMOVE');
      if (swarm === selectedSwarm){
        $('option#' + resource).remove();
      }
    } else {
      
      resources[swarm][resource] = resource;
      if ('XDomainRequest' in window && window.XDomainRequest !== null) {
				// Use Microsoft XDR	
				var xdr = new XDomainRequest();
				if(xdr) {
					xdr.onerror = function() { console.log('xdr error!');};
					xdr.ontimeout = function() { console.log('timed out'); return;};
					xdr.onprogress = function() { return; };
					xdr.timeout = 5000;
					xdr.onload = function () {
						var dom = new ActiveXObject("Microsoft.XMLDOM");
						dom.async = false;
						dom.loadXML(xdr.responseText);
						console.log('response: '+xdr.responseText);
						resources[swarm][resource] = xdr.responseText;
						$('option').filter('#'+resource).html(xdr.responseText);
					};
					xdr.open("GET", "http://api.staging.bugswarm.com/renesas/getmac/"+resource);
					xdr.send(null);
				}
			}
      
      else {
        var url = 'http://api.staging.bugswarm.com/resources/' + resource;
        var xhr = createCORSRequest('GET', url);
        xhr.onload = function() {
          var responseText = xhr.responseText;
          var data = {};
          try {
            data = JSON.parse(xhr.responseText);
          } catch(e) {
            console.error(e);
          }
          // process the response.
          console.log(data.id+' is named '+data.name);
              //$('option').filter('#'+resource).html(data.name);
          resources[swarm][resource] = data.name;
          $('option').filter('#'+resource).html(data.name);
        };
  
        xhr.onerror = function() {
          console.log('CORS request: There was an error!');
        };
  
        xhr.setRequestHeader(
          'x-bugswarmapikey', CFG_KEY);
        xhr.send();
      }

      //console.log('Presence ('+resource+'): ADD');
     //  $.ajax({  url:'http://api.staging.bugswarm.com/resources/' + resource,
     //      type: 'GET',
     //      data: null,
     //      dataType: 'json',
     //      beforeSend: function(xhr) {
     //          xhr.setRequestHeader("x-bugswarmapikey", CFG_KEY);
     //      },
     //      error: function(xhr, status, errorThrown){
              //console.log('ajax probs: ' +errorThrown+'\n'+status+'\n'+xhr.statusText);
          // },
     //      success: function(data){
     //        console.log(data.id+' is named '+data.name);
     //        //$('option').filter('#'+resource).html(data.name);
     //        resources[swarm][resource] = data.name;
     //        $('option').filter('#'+resource).html(data.name);
     //      }
     //  });
        //console.log('Resources, ',resources);
      if (swarm === selectedSwarm){
        populateResourceList();
      }
    }
    } else {
        //console.log('presence -> ' + JSON.stringify(presence));
    }
}


(function($){
  drawGuager = function(){  gaugeData = google.visualization.arrayToDataTable([
  ['Temperature'],
  [0]
  ]);
  gauge = new google.visualization.Gauge(document.getElementById('temp'));};
})(jQuery);

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

function updateUI() {
        if (accelX.length > xAxisLength){
            accelX.shift();
            accelY.shift();
            accelZ.shift();
        }
        accelPlot = $.plot($('#accelChart'), [ accelX, accelY, accelZ ], accelOptions);
        if (light.length > xAxisLength){
            light.shift();
        }
        lightPlot = $.plot($('#lightChart'), [ light ], plotOptions);
        if (pot.length > xAxisLength){
            pot.shift();
        }
        potPlot = $.plot($('#potChart'), [ pot ], potOptions);
}

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
        /*
        if (accelX.length > xAxisLength){
            accelX.shift();
            accelY.shift();
            accelZ.shift();
        }
        accelPlot = $.plot($('#accelChart'), [ accelX, accelY, accelZ ], accelOptions);
        */
    } else if (payload.name === "Temperature"){
        //console.log('temp: '+payload.feed.TempF);
        temp.push([(currentTime-startTime)/1000,payload.feed.TempF]);
	    changeTemp(Math.round(payload.feed.TempF*100)/100);

    } else if (payload.name === "Light"){
        //console.log('light: '+payload.feed.Value);
        light.push([(currentTime-startTime)/1000,payload.feed.Value]);
//         if (light.length > xAxisLength){
//             light.shift();
//         }
//         lightPlot = $.plot($('#lightChart'), [ light ], plotOptions);
    } else if (payload.name === "Potentiometer"){
        //console.log('Potentiometer: '+payload.feed.Raw);
        pot.push([(currentTime-startTime)/1000,payload.feed.Raw]);
//         if (pot.length > xAxisLength){
//             pot.shift();
//         }
//         potPlot = $.plot($('#potChart'), [ pot ], potOptions);
    } else if (payload.name === "Button"){
        $('#b1').html(payload.feed.b1);
        $('#b2').html(payload.feed.b2);
        $('#b3').html(payload.feed.b3);
    } else if (payload.name === "Sound Level"){
        //console.log(JSON.stringify(payload));
        $('span#soundlevel').html((parseInt(payload.feed.Raw, 10)/5)+20);
        $('meter').attr('value',parseInt(payload.feed.Raw, 10));
    } else if (payload.name === "LED") {
      for (var key in payload.feed) {
        if (payload.feed[key]) {
          $('td.led').filter('#'+key).html('1');
        } else {
          $('td.led').filter('#'+key).html('0');
        }
      }
    }
}




/*function switch_style ( css_title )
{
  var i, link_tag ;
  for (i = 0, link_tag = document.getElementsByTagName("link") ;
    i < link_tag.length ; i++ ) {
    if ((link_tag[i].rel.indexOf( "stylesheet" ) != -1) &&
      link_tag[i].title) {
      link_tag[i].disabled = true ;
      if (link_tag[i].title == css_title) {
        link_tag[i].disabled = false ;
      }
    }
    set_cookie( style_cookie_name, css_title,
      style_cookie_duration );
  }
}*/


function onError(error) {
  alert(JSON.stringify(error));
  console.error(error);
}
function onConnect() {
  console.log('connected');
  startTime = (new Date()).getTime();
}

$(document).ready(function() {
  for (var i=0;i<swarms.length;i++) {
    resources[swarms[i]] = {};
  }
  $('button#swarm_select').click(function(e){
    var swarmid = $("#boardlist option:selected").val();
    if (swarmid.length == 40) {
      //console.log('Changing to swarm '+swarmid);
      selectedSwarm = swarmid;
      populateResourceList();
    }
  });
  $('button#populate').click(function(e){
        var resource = $("#droplist option:selected").val();
        //console.log('selecting '+resource);
        selectedResource = resource;
        startTime = (new Date()).getTime();
        accelX = [];
        accelY = [];
        accelZ = [];
        temp = [];
        light = [];
        pot = [];
        var toswarms = [{swarm: "5dbaf819af6eeec879a1a1d6c388664be4595bb3",
                          resource: selectedResource}];
        $('#sendbuzzer').click(function(e) {
          SWARM.send({
            name: 'Beep',
            feed: {
              freq: parseInt($('#freqval').html(), 10),
              duration: parseInt($('#durval').html(),10) }}, toswarms);
        });
        $('td.led').click(function(e) {
          var ledid = this.id;
          var message = {name: 'LED', feed:{}};
          //console.log('Value: '+$(this).html());
          message.feed[ledid] = ($(this).html() === '0');
          //console.log('sending ',message);
          SWARM.send(message, toswarms);
        });
        $('#sendlcd').click(function(e) {
          SWARM.send({
            name: 'LCD',
            feed: {
              text: $('textarea#lcdtext').val()}}, toswarms);
        });
        $("input[name='einkdemo']").change(function(e) {
          SWARM.send({
            name: 'Eink',
            feed: {
              demo: parseInt($(this).val(),10)}}, toswarms);
        }) ;
    });
    $('#freq').slider({
      max: 2000,
      min: 50,
      step: 50,
      value: 450
    }).on("slide", function(event, ui) {
      $('#freqval').html(ui.value);
    });
    $('#duration').slider({
      max: 5000,
      min: 100,
      step: 100,
      value: 500
    }).on("slide", function(event, ui) {
      $('#durval').html(ui.value);
    });
    
    setInterval(function(){updateUI;},1000);

    SWARM.connect({
      apikey: API_KEY,
      resource: RESOURCE_ID,
      swarms: swarms,
      onmessage: onMessage,
      onpresence: onPresence,
      onerror: onError,
      onconnect: onConnect
    });
});
