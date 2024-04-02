#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <sys/stat.h>

#include "read_file.h"
using namespace std;

void read_file(const std::string &filename) {
	struct stat dir;
	std::string path = "/share/cs327";
	string home = "/.poke327";

	// Check if the path exists
	if(stat(path.c_str(), &dir) == -1) {
		path = getenv("HOME") + home;
	}

	// Error checking
	if(stat(path.c_str(), &dir)) {
		printf("Path not found.");
		return;
	}

	std::ifstream file(path);
	std::string line;
	std::vector<std::string> pokedex;
	int i = 0;
	if (filename == "pokemon")
    {
        path += "pokemon.csv";
        std::ifstream file(path);

        while (getline(file, line))
        {
            std::stringstream sstr(line);
            std::string word;

            while (getline(sstr, word, ','))
            {
                if (!word.empty())
                {
                    pokedex.push_back(word);
                }
                else
                {
                    pokedex.push_back("INT_MAX");
                }
                i++;
            }

            if (i == 7)
            {
                pokedex.push_back("INT_MAX");
            }

            i = 0;
        }
        file.close();

        for (size_t i = 0; i < pokedex.size(); ++i)
        {
            if (pokedex[i] == "INT_MAX")
            {
                std::cout << " "
                          << " ";
            }
            else
            {
                std::cout << pokedex[i] << " ";
            }
            if ((i + 1) % 8 == 0)
                std::cout << std::endl;
        }
    }

}