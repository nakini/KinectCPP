#ifndef __PLY_FUNCTIONS__ 
#define __PLY_FUNCTIONS__ 
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>			// Hold concatenated string
#include <fstream>

// Functions to xyz point to ply file and vice versa.
int xyz2ply(std::string file_name,  std::vector<std::vector<float>> gridMat);
void ply2xyz(std::string file_name,  std::vector<std::vector<float>> gridMat);

#endif
