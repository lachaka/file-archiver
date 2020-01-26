CXX = g++ -Wall

binaries = Archiver.o FileManager.o FileTree.o main.o

all: $(binaries)
		$(CXX) -o zipper $(binaries)
		rm -f $(binaries)

Archiver.o: Archiver.cpp
		$(CXX) -c Archiver.cpp

FileManager.o: FileManager/FileManager.cpp
				$(CXX) -c FileManager/FileManager.cpp

FileTree.o: FileManager/FileTree/FileTree.cpp
				$(CXX) -c FileManager/FileTree/FileTree.cpp

main.o: main.cpp
		$(CXX) -c main.cpp

clean:
		rm -f $(binaries)
