NVCC = nvcc
NVCCFLAGS = -O3
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

all: main

main: main.cu
	$(NVCC) $(NVCCFLAGS) main.cu -o main $(LDFLAGS)

run: main
	./main

clean:
	rm -f main