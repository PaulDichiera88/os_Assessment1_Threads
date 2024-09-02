all: mscopier mmcopier

# Compile mmcopier from task1/mmcopier.cpp
mmcopier: task1/mmcopier.cpp
	g++ -Wall -Werror -std=c++17 -O -o mmcopier task1/mmcopier.cpp -lpthread

# Compile mscopier from task2/mscopier.cpp
mscopier: task2/mscopier.cpp
	g++ -Wall -Werror -std=c++17 -O -o mscopier task2/mscopier.cpp -lpthread

# Clean target to remove compiled executables and generated files
clean:
	rm -f mmcopier mscopier
	rm -f task2/destination.txt
	rm -rf task1/destination_dir/source*
