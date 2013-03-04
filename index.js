var join  = require('path').join
  , binding = require(join(__dirname, 'build/Release/geoip.node'));
exports.City     = binding.City;

