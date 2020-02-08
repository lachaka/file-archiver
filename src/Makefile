CXX = g++ -Wall -L/usr/lib64 -lcrypto

binaries = ArgumentParser.o Archiver.o FileManager.o main.o

all: $(binaries)
		$(CXX) -o zipper $(binaries)
		rm -f $(binaries)

Archiver.o: Archiver/Archiver.cpp
		$(CXX) -c Archiver/Archiver.cpp

ArgumentParser.o: ArgumentParser/ArgumentParser.cpp
		$(CXX) -c ArgumentParser/ArgumentParser.cpp

FileManager.o: FileManager/FileManager.cpp
		$(CXX) -c FileManager/FileManager.cpp

main.o: main.cpp
		$(CXX) -c main.cpp

clean:
		rm -f $(binaries)
