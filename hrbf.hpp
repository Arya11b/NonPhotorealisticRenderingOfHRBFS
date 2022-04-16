#ifndef HRBF_H
#define HRBF_H

#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <iostream>
#include <memory>

// calculating HRBFs
// defined on 3D
// the function itself can be generalized in n-dimensions, but this class can only do it in 3D
// in case of needing this for higher dimensions templates can be used
class HRBF
{
public:
	Eigen::MatrixXd _node_centers;
	Eigen::MatrixXd _betas;
	Eigen::VectorXd _alphas;

    Eigen::VectorXd a;
    double b{};

    // create function
    // radial basis function is x^3
    void fit(const Eigen::MatrixXd& points, const Eigen::MatrixXd& normals);

    /// Evaluate potential f() at position 'x'
    double eval(const Eigen::RowVector3d& point) const;

    /// Evaluate gradient nabla f() at position 'x'
    Eigen::RowVector3d grad(const Eigen::RowVector3d& point) const;
};

std::shared_ptr<HRBF> getHRBF(const Eigen::MatrixXd& points, const Eigen::MatrixXd& normals);

Eigen::RowVector3d project_p(const HRBF& hrbf, Eigen::RowVector3d p,float threshold = 0.001f);

#endif