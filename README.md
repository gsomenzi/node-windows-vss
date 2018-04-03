# node-windows-vss
Native bindings to take advantage of Microsoft Windows Volume Shadow Copies.

## Support

 * Node.js (tested on >= 8.x)
 
## Installation

To take advantage of Windows Volume Shadow Copies, we've some Windows requirements:
 * The command must to be executed as administrator.
 * Remember to check LUA.
 * Shadow Copies Service must tu be enabled as automatic or default option.

```sh
> npm install node-windows-vss --save
```

## API

### takeSnapshot(volume, callback)
It takes a snapshot from a specified volume. Returns a callback with (Error err, String snapshotPath)

```javascript
const WindowsVss = require('node-windows-vss')

WindowsVss.takeSnapshot("C:", (err, res) => {
	if (err) return console.log(err)
	return console.log('res', res)
})
```