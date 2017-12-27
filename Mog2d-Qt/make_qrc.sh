#/bin/bash

cd `dirname $0`

rm -fr ./assets
cp -fr ../assets ./assets

rm -fr ./assets_qt
cp -fr ../assets_qt ./assets_qt

echo "<RCC>
    <qresource prefix=\"/\">" > assets.qrc

FILES=`find assets -type f ! -name ".*"`
for f in ${FILES}; do
    echo "        <file>$f</file>" >> assets.qrc
done

FILES=`find assets_qt -type f ! -name ".*"`
for f in ${FILES}; do
    echo "        <file>$f</file>" >> assets.qrc
done

echo "    </qresource>
</RCC>" >> assets.qrc
