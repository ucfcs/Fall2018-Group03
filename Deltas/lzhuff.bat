pushd "%~dp0"
del /f /q %2.temp
lzss -c -i %1 -o %2.temp
java -cp C:\Users\Ryan\workspace\HuffmanStuffman\bin HuffmanCompress %2.temp %2
popd