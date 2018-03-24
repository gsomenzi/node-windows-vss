const VssController = require('./')

VssController.takeSnapshot("C:", (err, res) => {
	if (err) return console.log(err)
	return console.log(res)
})