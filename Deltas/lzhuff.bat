Pushd "%~dp0"
lzss -c -i %1 -o temp
java -cp "Reference-Huffman-coding-master\java\src" HuffmanCompress temp %2
popd