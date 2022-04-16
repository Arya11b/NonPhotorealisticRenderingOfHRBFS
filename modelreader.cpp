//
// Created by arya on 2022-04-13.
//

#include "modelreader.hpp"

#include <GL/glew.h>
#include <glm/gtx/transform.hpp>

void ModelReader::bufferData() {
    std::cout << "Something";
    if(VAO == 0) {
        glGenVertexArrays(1,&VAO);
        glGenBuffers(1, &vertexBuffer);
        glGenBuffers(1, &indexBuffer);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

        glBufferData(GL_ARRAY_BUFFER,
                     vsize + nsize,
                     NULL,
                     GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER,
                        0,
                        vsize,
                        V.data());


        glBufferSubData(GL_ARRAY_BUFFER,
                        vsize,
                        nsize,
                        N.data());

        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     fsize,
                     F.data(),
                     GL_STATIC_DRAW);
    }


}

void ModelReader::draw (GLuint vertexLocation, GLuint normalLocation) {

    glBindVertexArray(VAO);

    glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);
    glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

    glEnableVertexAttribArray (vertexLocation);
    glVertexAttribPointer (vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
    glEnableVertexAttribArray (normalLocation);
    glVertexAttribPointer (normalLocation, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)(vsize));

    glDrawElements (GL_TRIANGLES, F.size(), GL_UNSIGNED_INT , 0);

    glBindVertexArray(0);

}


glm::mat4 ModelReader::getFitScale () {
    using glm::vec3;
    using glm::mat4;
    using glm::scale;
    float radius = length (max - min) * 0.5f;
    return scale (mat4 (1.0f), vec3 (0.95f / radius));
}

glm::mat4 ModelReader::getFitTranslate () {
    using glm::vec3;
    using glm::mat4;
    using glm::translate;
    vec3 center = (max + min) * 0.5f;
    return translate(mat4(1.0f), -center);
}

ModelReader::ModelReader(const char *path) {
    igl::read_triangle_mesh(path,V,F);

    for (int i = 0; i < V.rows(); ++i) {
        if(V(i,0) > max.x) max.x = V(i,0);
        if(V(i,0) < min.x) max.x = V(i,0);

        if(V(i,1) > max.y) max.y = V(i,1);
        if(V(i,1) < min.y) max.y = V(i,1);

        if(V(i,2) > max.z) max.z = V(i,2);
        if(V(i,2) < min.z) max.z = V(i,2);
    }
    vsize = V.size() * sizeof(Scalar);
    fsize = F.size() * sizeof(u_int);
}

void ModelReader::calculateNormals(){
    igl::per_vertex_normals(V,F,N);
    nsize = N.size() * sizeof(Scalar);
}

ModelReader::ModelReader() {
    V= (Eigen::Matrix<Scalar ,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor>(8,3)<<
            0.f,0.f,0.f,
            0.f,0.f,1.f,
            0.f,1.f,0.f,
            0.f,1.f,1.f,
            1.f,0.f,0.f,
            1.f,0.f,1.f,
            1.f,1.f,0.f,
            1.f,1.f,1.f).finished();
    F = (Eigen::Matrix<u_int ,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor>(12,3)<<
            1,7,5,
            1,3,7,
            1,4,3,
            1,2,4,
            3,8,7,
            3,4,8,
            5,7,8,
            5,8,6,
            1,5,6,
            1,6,2,
            2,6,8,
            2,8,4).finished().array()-1;

    for (int i = 0; i < V.rows(); ++i) {
        if(V(i,0) > max.x) max.x = V(i,0);
        if(V(i,0) < min.x) max.x = V(i,0);

        if(V(i,1) > max.y) max.y = V(i,1);
        if(V(i,1) < min.y) max.y = V(i,1);

        if(V(i,2) > max.z) max.z = V(i,2);
        if(V(i,2) < min.z) max.z = V(i,2);
    }
    vsize = V.size() * sizeof(Scalar);
    fsize = F.size() * sizeof(u_int);

}

std::shared_ptr<ModelReader> readModel(const char *path) {
    auto mr = std::make_shared<ModelReader>(path);
    mr->calculateNormals();
    return mr;
}
