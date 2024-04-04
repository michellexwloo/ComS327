#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <sys/stat.h>

#include "readfile.h"
using namespace std;

void read_file(char filename[50]);

void read_file_helper(std::ifstream& file, int fields){
	std::string line;
	std::vector<std::string> data;
	int i = 0;

	while(getline(file, line)){
			std::stringstream sstr(line);
			std::string word;
			while(getline(sstr, word, ',')){
				if(!word.empty()){
					data.push_back(word);
				}else{
					data.push_back("INT_MAX");
				}
				i++;
			}
			if(i == fields-1){
				data.push_back("INT_MAX");
			}
			i = 0;
		}
		file.close();
		// Printing
		for(size_t i = 0; i < data.size(); i++){
			if(data[i] == "INT_MAX"){
				std::cout << " " << " ";
			}else{
				std::cout << data[i] << " ";
			}
			if ((i+1) % fields == 0){
				std::cout << std::endl;
			}
		}

}

void read_file(char filename[50]){
	struct stat buffer;
	std::string path =  "/share/cs327/";
	std::string home = "/.poke327/";
	std::string directory = "pokedex/pokedex/data/csv/";

	if(stat(path.c_str(), &buffer) == -1){
		path = getenv("HOME") + home;
	}

	std::string fullPath = path + directory + filename + ".csv";
	std::ifstream file(fullPath);

	// If file not found
	if (!file.is_open()){
		std::cerr << "Error: " << filename << " not found." << std::endl;
		return;
	}

	// std::string line;
	// std::vector<std::string> data;
	// int i = 0;
	int fields = 0;

	if(strcmp(filename, "pokemon") == 0){
		fields = 8;
	}else if(strcmp(filename, "moves") == 0){
		fields = 15;
	}else if(strcmp(filename, "pokemon_moves") == 0){
		fields = 6;
	}else if(strcmp(filename, "pokemon_species") == 0){
		fields = 20;
	}else if(strcmp(filename, "experience") == 0){
		fields = 3;
	}else if(strcmp(filename, "type_names") == 0){
		fields = 3;
	}else if(strcmp(filename, "pokemon_stats") == 0){
		fields = 4;
	}else if(strcmp(filename, "stats") == 0){
		fields = 5;
	}else if(strcmp(filename, "pokemon_types") == 0){
		fields = 3;
	}else{
		std::cerr << filename << ": file unable to parse now." << std::endl;
	}

	read_file_helper(file, fields);
}


// void read_file_test(char filename[50]) {
//     // Construct the file path
//     std::string path = "/share/cs327/pokedex/pokedex/data/csv/";
//     std::string home = "/.poke327/pokedex/pokedex/data/csv/";
// 	std::string local = "../csv/";

//     // Check if /share/cs327 exists
//     struct stat buf;
//     if (stat(path.c_str(), &buf) == -1) {
//         path = getenv("HOME") + home;
//     } else {	// For local copy. Only for my own copy.
// 		path = local;
// 	}

//     // Construct the full file path
//     std::string fullFilePath = path + filename + ".csv";

//     // Open the file
//     std::ifstream file(fullFilePath);
	
//     if (!file.is_open()) {
//         std::cerr << "Error: Unable to open file: " << fullFilePath << std::endl;
//         return;
//     }

//     file.close();
// }
