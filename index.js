const addon = require('bindings')('addon');
const regexpVolume = /(?=^[A-Za-z]\:)((?=^.{3}$)(.*\\$))|((?=^.{2}$)(.*\:$))/g

module.exports = {
	takeSnapshot: takeSnapshot
}

/**
 * @description Take a snapshot of a specific volume.
 * @param {string} volume Volume letter with or without slash. E.g: "C:" or "C:\".
 * @param {function} callback Callback function. (Err err, String snapshotPath).
 */
function takeSnapshot(volume, callback) {
	if (typeof callback !== 'function') throw new TypeError('Callback function must to be a valid function.')
	if (typeof volume !== 'string') return callback(new TypeError('Volume name must to be a valid string.'))
	if (!regexpVolume.test(volume)) return callback(new Error('The volume you entered does not appear to be in a valid format.'))
	addon.snapshot(volume, (err, snapshotPath) => {
		if (err) {
			let newError = new Error(err)
			return callback(newError, null)
		}
		if (!snapshotPath) return callback(new Error('Fail while trying to take the snapshot.'))
		return callback(null, snapshotPath)
	})
}