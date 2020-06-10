cd ../web
npm run build
cd ../device/data
rm *
cp ../../web/dist/* .
cd ..
platformio run   --target uploadfs --upload-port $DEVICE.local -e ota
