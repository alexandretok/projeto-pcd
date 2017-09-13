Código para compilar:

```
gcc code.c -o code `pkg-config --libs --cflags opencv` -ldl -lm && ./code img.png
```
