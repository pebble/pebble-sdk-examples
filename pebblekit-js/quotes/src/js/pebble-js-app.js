// Fetch saved symbol from local storage (using standard localStorage webAPI)
var symbol = localStorage.getItem("symbol");

// We use the fake "PBL" symbol as default
if (!symbol) {
  symbol = "PBL";
}

// Fetch stock data for a given stock symbol (NYSE or NASDAQ only) from markitondemand.com
// & send the stock price back to the watch via app message
// API documentation at http://dev.markitondemand.com/#doc
function fetchStockQuote(symbol) {
  var response;
  var req = new XMLHttpRequest();
  // build the GET request
  req.open('GET', "http://dev.markitondemand.com/Api/Quote/json?" +
    "symbol=" + symbol, true);
  req.onload = function(e) {
    if (req.readyState == 4) {
      // 200 - HTTP OK
      if(req.status == 200) {
        console.log(req.responseText);
        response = JSON.parse(req.responseText);
        var price;
        if (response.Message) {
          // the merkitondemand API sends a response with a Message
          // field when the symbol is not found
          Pebble.sendAppMessage({
            "price": "Not Found"});
        }
        if (response.Data) {
          // data found, look for LastPrice
          price = response.Data.LastPrice;
          console.log(price);
          Pebble.sendAppMessage({
            "price": "$" + price.toString()});
        }
      } else {
        console.log("Request returned error code " + req.status.toString());
      }
    }
  }
  req.send(null);
}

// Set callback for the app ready event
Pebble.addEventListener("ready",
                        function(e) {
                          console.log("connect!" + e.ready);
                          console.log(e.type);
                        });

// Set callback for appmessage events
Pebble.addEventListener("appmessage",
                        function(e) {
                          console.log("message");
                          if (e.payload.symbol) {
                            symbol = e.payload.symbol;
                            localStorage.setItem("symbol", symbol);
                            fetchStockQuote(symbol);
                          }
                          if (e.payload.fetch) {
                            Pebble.sendAppMessage({"symbol": symbol});
                            fetchStockQuote(symbol);
                          }
                          if (e.payload.price) {
                            fetchStockQuote(symbol);
                          }
                        });

