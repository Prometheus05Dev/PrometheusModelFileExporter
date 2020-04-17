#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <fstream>

int numArguments;
char **consoleArguments;

struct Vertex {
    float x, y, z;
    float normalX, normalY, normalZ;
    float textureX, textureY;
};

std::vector<Vertex> vertices;

std::vector<unsigned int> indices;

unsigned int numIndices = 0;

std::vector<const aiNode*> nodeBuffer;

unsigned int numNodeBuffers = 0;

Assimp::Importer importer;

const aiScene *scene;
const aiNode *modelNode;
const aiMesh *modelMesh;
const aiFace *modelFace;


bool assimpGetMeshData(const aiMesh *mesh) {
    aiFace *face;
    for(unsigned int v = 0; v < mesh->mNumVertices; v++){
        Vertex vertex;
        vertex.x = mesh->mVertices[v].x;
        vertex.y = mesh->mVertices[v].y;
        vertex.z = mesh->mVertices[v].z;
        vertex.normalX = mesh->mNormals[v].x;
        vertex.normalY = mesh->mNormals[v].y;
        vertex.normalZ = mesh->mNormals[v].z;
        if(mesh->mTextureCoords[0]) {
            vertex.textureX = mesh->mTextureCoords[0][v].x;
            vertex.textureY = mesh->mTextureCoords[0][v].y;
        }else{
            vertex.textureX = 0.0f;
            vertex.textureY = 0.0f;
        }
        vertices.push_back(vertex);
    }
    for(unsigned int f = 0; f < mesh->mNumFaces; f++){
        face = &mesh->mFaces[f];
        indices.push_back(face->mIndices[0]);
        indices.push_back(face->mIndices[1]);
        indices.push_back(face->mIndices[2]);
    }
    return true;
}


bool processData() {
    bool repeat = true;
    nodeBuffer.push_back(scene->mRootNode);

    if(scene->mNumMeshes > 0){
        for(unsigned int m = 0; m < scene->mNumMeshes; m++){
            assimpGetMeshData(scene->mMeshes[m]);
        }
    }

    while(repeat){
        for(unsigned int a = 0; a < nodeBuffer.size(); a++){
            modelNode = nodeBuffer.at(a);
            if(modelNode->mNumChildren > 0){
                for(unsigned int c = 0; c < modelNode->mNumChildren; c++){
                    nodeBuffer.push_back(modelNode->mChildren[c]);
                }
            }
            else {
                repeat = false;
            }
        }
    }
    for(unsigned int a = 0; a < nodeBuffer.size(); a++){
        modelNode = nodeBuffer.at(a);
        if(modelNode->mNumMeshes > 0){
            for(unsigned int b = 0; b < modelNode->mNumMeshes; b++){
                assimpGetMeshData(scene->mMeshes[b]);
            }
        }
    }
}

void writeData() {
    std::string exportPath = consoleArguments[numArguments - 1];
    exportPath = exportPath + ".pmf";
    std::cout << "Everything loaded (hopefully) successful, writing .pmf(Prometheus Model File) file: " << exportPath << std::endl;
    std::ofstream exportFile;
    exportFile.open(exportPath);
    exportFile << "Vertices\n";
    for(int i = 0; i <= vertices.size(); i++){
        exportFile << vertices[i].x << " " << vertices[i].y << " " << vertices[i].z << " " <<
                    vertices[i].normalX << " " << vertices[i].normalY << " " << vertices[i].normalZ << " " <<
                    vertices[i].textureX << " " << vertices[i].textureY << "\n";
    }
    exportFile << "Indices\n";
    for(int i = 0; i <= indices.size(); i++) {
        exportFile << indices[i] << "\n";
    }
    exportFile.close();
}

int main(int argc, char **argv){

    bool writeDebug;

    if(argc <= 0) {
        std::cout << "No entries provided!" << std::endl;
        return -1;
    }

    if(argc < 3) {
        std::cout << "You have to insert the path to a model and an export path!" << std::endl;
        return -1;
    }

    numArguments = argc;
    consoleArguments = argv;

    scene = importer.ReadFile(argv[argc - 2], aiProcess_Triangulate |
                                                            aiProcess_OptimizeMeshes |
                                                            aiProcess_OptimizeGraph |
                                                            aiProcess_JoinIdenticalVertices |
                                                            aiProcess_ImproveCacheLocality );
    if(!scene){
        std::cout << "Error importing model: " << argv[argc - 2] << "! " << importer.GetErrorString() <<  std::endl;
    }
    else {
        processData();
    }

    numIndices = indices.size();

    std::cout << "Write debug log? [Y/n]" << std::endl;
    std::string input;
    std::cin >> input;

    if(input == "Y") {
        writeDebug = true;
    }
    else if(input == "n") {
        writeDebug = false;
    }
    else {
        std::cout << "No valid output provided! Aborting." << std::endl;
    }

    if(writeDebug == true){
        for(int i = 0; i <= vertices.size(); i++) {
            std::cout << "Vertex coordinates: " << vertices[i].x <<  " " <<  vertices[i].y << " "<< vertices[i].z << std::endl;
            std::cout << "Normal coordinates: " << vertices[i].normalX << " " << vertices[i].normalY << " " << vertices[i].normalZ << std::endl;
            std::cout << "Texture coordinates: " << vertices[i].textureX << " " << vertices[i].textureY << std::endl;
            std::cout << "Number: " << i << std::endl;
        }
        for(int i = 0; i <= indices.size(); i++) {
            std::cout << "Index: " << indices[i] << " Number: " << i << std::endl;
        }
    }

    std::cout << "Total num of indices: " << numIndices << std::endl;
    std::cout << "Total num of vertices: " << vertices.size() << std::endl;
    writeData();
    std::cout << "Everything finished. Congratulations!" << std::endl;
}