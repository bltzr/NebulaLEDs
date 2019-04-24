var socket;

var onoff;
var onOffMess;

$(document).ready( function() {
	setupSocket();
	

	onoff = document.getElementById("onoff");


    onoff.onclick = function(e){
		sendOnOff();
	};
});


// send value from onoff input
function sendOnOff(){
	if (onoff.checked == true){
    onOffMess = "block";
  } else {
    onOffMess = "none";
  }
	socket.send(onoff.checked);
		//checkBox.checked);
}


// setup web socket
function setupSocket(){

	// setup websocket
	// get_appropriate_ws_url is a nifty function by the libwebsockets people
	// it decides what the websocket url is based on the broswer url
	// e.g. https://mygreathost:9099 = wss://mygreathost:9099

	
	// setup!
	socket = new WebSocket( get_appropriate_ws_url());
	
	// open
	try {

	} catch(exception) {
		alert('<p>Error' + exception);  
	}
}