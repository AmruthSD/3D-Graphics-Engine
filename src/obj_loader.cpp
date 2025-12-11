#include "obj_loader.hpp"

vector<triangle> convertObjToTriangles(string &filepath){
    vector<triangle> triangles;
    vector<vertex> vertices;
    ifstream file(filepath);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return triangles;
    }

    string line;
    while(getline(file,line)){
        if (line.empty())
            continue;
        
        stringstream ss(line);
        string prefix;
        ss >> prefix;

        if(prefix=="v"){
            vertex new_vertex;
            ss >> new_vertex.x >> new_vertex.y >> new_vertex.z;
            vertices.push_back(new_vertex);
        }else if(prefix=="f"){
            vector<int> vertex_idx;
            string new_info;
            while(ss >> new_info){
                int idx = 0;
                for(int i=0;i<new_info.length();i++){
                    if(new_info[i]=='/')break;
                    idx*=10;
                    idx += new_info[i]-'0';
                }
                vertex_idx.push_back(idx-1);
                for (int i=2;i<vertex_idx.size();i++){
                    triangles.push_back(triangle{vertices[0],vertices[vertex_idx[i-1]],vertices[vertex_idx[i]]});
                }
            }
        }
    }
    file.close();
    return triangles;
}

