# lua_safeBase64
url safe base64 for lua

以"-_"替换"+/"，并去掉了"=="，可以安全地在url中传输。
用法：
```
local luaSafeBase64 = require('safeBase64')
local encoded = luaSafeBase64.encode("abc")
print("encoded: ", encoded)

local decoded = luaSafeBase64.decode(encoded)
print("decoded: ", decoded)
```
