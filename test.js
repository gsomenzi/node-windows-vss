const VssController = require('./')

VssController.takeSnapshot("C:", (err, res) => {
	if (err) console.log(err)
	return console.log('res', res)
})