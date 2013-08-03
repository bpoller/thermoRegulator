thermoRegulator
===============

A temperature forecasting system using an [Arduino Ethernet board] (http://store.arduino.cc/eu/index.php?main_page=product_info&cPath=11_12&products_id=315) and a simple thermistor.

Analogous thermistor readings are compensated according to the [Steinhart-Hart] (http://en.wikipedia.org/wiki/Steinhart%E2%80%93Hart_equation) equation.

Temperature readings are exposed through a REST API with JSONP callbacks to solve for cross site scripting problems.

A static web page accesses the REST API and visualises the time series using the [Highcharts](http://www.highcharts.com/) JavaScript API.

Temperature trends are calculated using the [Least Squares] (http://en.wikipedia.org/wiki/Least_squares) approximations.

