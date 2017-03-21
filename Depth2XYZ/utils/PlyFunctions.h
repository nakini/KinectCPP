#ifndef __PLY_FUNCTIONS__ 
#define __PLY_FUNCTIONS__ 
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>			// Hold concatenated string
#include <fstream>

// Functions to xyz point to ply file and vice versa.
int xyz2ply(std::string,  std::vector<std::vector<float>>);
std::vector<std::vector<float> > ply2xyz(std::string );

#endif
