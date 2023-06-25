CXX = g++
CFLAGS = -shared -std=c++17 -O2 -Wall -fPIC -fopenmp $(shell python3 -m pybind11 --includes) $(shell python3-config --includes) \
			 #-ftree-vectorize

SRCDIR = src
SRC = wrapper.cpp $(SRCDIR)/pyFunc.h $(SRCDIR)/histEqSerial.h 

OBJDIR = obj
OBJ = $(OBJDIR)/wrapper.o

# CUDA = $(OBJDIR)/cuda.o
# CUDA_SRC = $(SRCDIR)/cuda.cu $(SRCDIR)/histEqCuda.h
# NVCC = nvcc
# CUDA_LINK_FLAGS =  -rdc=true -Xcompiler '-fPIC'
# CUDA_COMPILE_FLAGS = --device-c -Xcompiler '-fPIC' -g -O3

EXE = myHist$(shell python3-config --extension-suffix)

all: dirs $(EXE)

dirs:
	mkdir -p $(OBJDIR)

$(EXE): $(OBJ) 
	$(CXX) $(CFLAGS) $(SRC) -o $(EXE)

$(OBJ): $(SRC)
	$(CXX) $(CFLAGS) -c $(SRC) -o $(OBJ)


.PHONY: test clean

test: $(TARGET)
	make
	python3 -m pytest

clean:
	rm -rf *.so __pycache__ .pytest_cache *.a $(OBJDIR)