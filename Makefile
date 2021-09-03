CP=g++
CPFLAGS=-std=c++11 -pthread -pedantic -Wall -O3 -g
CPP_FILES := $(model.cc)


all: model

model :
	$(CP) $(CPFLAGS) -o model model.cc -lsimlib -lm 
	
run:	
	 ./model $(endtime)

zip:
	zip 08_xjurka08_xspavo00.zip *.cc Makefile dokumentace.pdf
	
clean:
	rm model 