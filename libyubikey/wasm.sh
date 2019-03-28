emcc ykwasm.c ykhex.c ykcrc.c yktoken.c ykaes.c -s WASM=1  -s "EXPORTED_FUNCTIONS=['_ykgenerate']" -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall"]' -o hello.html
