//
// Created by arya on 2022-04-13.
//

#ifndef MODELREADER_HPP
#define MODELREADER_HPP


#include <Eigen/Core>
#include <GL/glew.h>

#include <igl/read_triangle_mesh.h>
#include <igl/per_vertex_normals.h>
#include <igl/per_face_normals.h>

#include <glm/glm.hpp>

class ModelReader {
    typedef GLfloat Scalar;
    typedef GLuint u_int;
    GLuint vertexBuffer, indexBuffer, VAO{};
    Eigen::Matrix<Scalar,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> V,N;

    Eigen::Matrix<u_int ,Eigen::Dynamic,Eigen::Dynamic,Eigen::RowMajor> F;

    glm::vec3 min,max;
    int vsize,fsize,nsize;

public:
    ModelReader(const char* path);
    ModelReader(); // for test
    void bufferData();
    void draw(GLuint vertexLocation, GLuint normalLocation);

    void calculateNormals();

    glm::mat4 getFitScale();

    glm::mat4 getFitTranslate();
};

std::shared_ptr<ModelReader> readModel(const char* path);

#endif //MODELREADER_HPP
