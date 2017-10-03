##########################################################
#	关于编译环境的一些设定
##########################################################
CXX:=g++
CFLAGS:= -O2 -g -Wall -std=c++11 -pthread
LIBS:= 
LIBPATH:=
INCLUDE:= 
SRC:=  ./src/*.cpp 
TARGETS:= HS
##########################################################
#	从这里开始,不用改变任何东西
##########################################################

INCLUDES:=$(addprefix -I,$(INCLUDE))
CFLAGS:=$(CFLAGS) $(INCLUDES)
SOURCES:=$(wildcard $(SRC))  
OBJS:=$(patsubst %.cpp,%.o,$(SOURCES))

all:$(TARGETS)


.PHONY:clean cleanobj cleantarget cleandeps all

cleanobj:
	-rm -f $(OBJS)

cleantarget:
	-rm -f $(TARGETS)


clean:cleanobj cleantarget


$(TARGETS):$(OBJS)
	$(CXX) $(CFLAGS)   $^ -o $@ $(addprefix -L,$(LIBPATH)) $(addprefix -l,$(LIBS))


%.o:%.cpp
	$(CXX) -c $(CFLAGS)   $^ -o $@




