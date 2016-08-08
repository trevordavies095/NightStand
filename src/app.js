Pebble.addEventListener("ready", function(e)
{
    console.log("PebbleKit JS ready!");
});

Pebble.addEventListener("showConfiguration", function(e)
{
    //Load the remote config page
	Pebble.openURL("http://www.trevordavies095.com/NightStand/config.html");
});

Pebble.addEventListener("webviewclosed", function(e)
{
    //Get JSON dictionary
    var configuration = JSON.parse(decodeURIComponent(e.response));
    console.log("Configuration window returned: " + JSON.stringify(configuration));
 
    //Send to Pebble, persist there     
    Pebble.sendAppMessage(
      {"KEY_BGMIN": configuration.bgmin},
      {"KEY_FGMIN": configuration.fgmin},
      {"KEY_BGHOUR": configuration.bghour},
      {"KEY_FGHOUR": configuration.fghour},
      {"KEY_CRIM": configuration.fgrim},
      function(e) {
        console.log("Sending settings data...");
      },
      function(e) {
        console.log("Settings feedback failed!");
      }
    );
  }
);