Código para compilar:

```
gcc code.c -o code `pkg-config --libs --cflags opencv` -ldl -lm && ./code
```

Compilar com vetorização:

```
1. gcc code.c -o code -Wall -O2 -ftree-vectorize -fopt-info-vec-missed `pkg-config --libs --cflags opencv` -ldl -lm && time ./code
2. gcc code.c -o code -Wall -O2 -ftree-vectorize -fopt-info-vec-optimized `pkg-config --libs --cflags opencv` -ldl -lm && time ./code
3. gcc code.c -o code -Wall -O2 -ftree-vectorize -fopt-info-vec-all `pkg-config --libs --cflags opencv` -ldl -lm && time ./code
```

Compilar com vetorização ICC:

1. icc code.c -o code -Wall -vec -O3 -axAVX -qopt-report -simd `pkg-config --libs --cflags opencv` -ldl -lm && time ./code
