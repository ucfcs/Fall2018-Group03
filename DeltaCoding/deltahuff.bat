pushd "%~dp0"
del /f /q %2.tmp
lzss_deltas -c -i %1 -o %2.tmp
java -cp C:\Users\Ryan\workspace\HuffmanStuffman\bin HuffmanCompress %2.tmp %2
popd
