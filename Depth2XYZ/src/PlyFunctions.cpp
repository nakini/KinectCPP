#include "PlyFunctions.h"

// Given a point xyz values in the form of a vector, this function creates a ply file and
// also returns total number of points in the ply file. The input vector is of size Nx3.
int xyz2ply(std::string file_name, std::vector<std::vector<float>> grid_mat){
    int num_points = 0;					// Total number of points
    std::ofstream plyFile;
    plyFile.open(file_name.c_str());
    num_points = grid_mat.size();

    // Insert the header
    plyFile << "ply\n";
    plyFile << "format   ascii 1.0\n";
    plyFile << "comment made by Tushar Nakini, ViGIR\n";
    plyFile << "element vertex " << grid_mat.size() <<"\n";
    plyFile << "property float32 x\n";
    plyFile << "property float32 y\n";
    plyFile << "property float32 z\n";
    plyFile << "property uchar red\n";
    plyFile << "property uchar green\n";
    plyFile << "property uchar blue\n";
    plyFile << "end_header\n";

    // Write the values into the ply file.
    for (int iGM=0; iGM<num_points; iGM++){
        plyFile << grid_mat[iGM][0] << " " << grid_mat[iGM][1] << " " << grid_mat[iGM][2] << " "\
                                   << 255 << " " << 255 << " " << 255 << "\n";
    }

    plyFile.close();						// Close the stream file.

    return num_points;
}

// This function return xyz coordinates of a point cloud in the form of a Nx3 vector.
std::vector<std::vector<float>> ply2xyz(std::string file_name){
    // Vector to hold all the XYZ points.
    std::vector<std::vector<float>> grid_mat;

    // Check the file and open it if exists.
    std::ifstream in_file(file_name);
    if(in_file.is_open()){
        std::string line_str;						// Container to hold each line
        unsigned long num_vert;				// Number of vertices in the point cloud

        // Read the header first.
        for(int line=0; line<11; line++){
            std::getline(in_file, line_str);
            if (line==3){
                std::istringstream iss(line_str);
                std::string word;
                while(iss){
                    iss>>word;
                }
                num_vert = std::stoul(word);      // Convert the number of vertics into an integer
            }
            in_file >> std::ws;					// Read end of the white space including new line
        }
    }else{
        // Unable to open the file so throw an error.
        std::cout << "Cannot open the file" << std::endl;
        std::exit;
    }
    in_file.close();
    return grid_mat;
}
