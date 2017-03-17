#include "plyFunctions.h"

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

void ply2xyz(std::string file_name, std::vector<std::vector<float>> grid_mat){

}

