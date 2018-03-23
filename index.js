var addon = require('bindings')('addon');

addon('C:\\', function(err, msg) {
  console.log(msg)
})
