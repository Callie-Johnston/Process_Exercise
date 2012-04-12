all: process_test

process_test: process_test.cpp Process.cpp Process.hpp
	g++ -Wall -g -o process_test process_test.cpp Process.cpp Process.hpp
