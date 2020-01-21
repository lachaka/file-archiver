CXX = g++ -Wall

binaries = FileManager.o FileTree.o main.o

all: $(binaries)
		$(CXX) -o zipper $(binaries)
		rm -f $(binaries)

FileManager.o: FileManager/FileManager.cpp
				$(CXX) -c FileManager/FileManager.cpp

FileTree.o: FileTree/FileTree.cpp
				$(CXX) -c FileTree/FileTree.cpp

main.o: main.cpp
		$(CXX) -c main.cpp

clean:
		rm -f $(binaries)
