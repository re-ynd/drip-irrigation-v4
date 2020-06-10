#!/bin/bash

cd dist
find . -name '*.map' | xargs -I{} rm {}
sed -i "s|polyfills/webcomponents.js|./wc.js|g" *.*
mv polyfills/webcomponents.js wc.js
sed -i "s|polyfills/dynamic-import.js|./di.js|g" *.*
mv polyfills/dynamic-import.js di.js
sed -i "s|polyfills/custom-elements-es5-adapter.js|./ce.js|g" *.*
mv polyfills/custom-elements-es5-adapter.js ce.js
rm -rf polyfills

cp ../node_modules/web-animations-js/web-animations-next-lite.min.js ./wanl.js
sed -i "s|node_modules/web-animations-js/web-animations-next-lite.min.js|./wanl.js|g" *.*