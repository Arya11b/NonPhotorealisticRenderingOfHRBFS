#include "hrbf.hpp"
#include <omp.h>
void HRBF::fit(const Eigen::MatrixXd& points, const Eigen::MatrixXd& normals) {
    assert(points.rows() == normals.rows());
    assert((points.cols() == 3) && (normals.cols() == 3));

    int nb_points = points.rows();
    int nb_hrbf_constraints = (3 + 1) * nb_points + (3 + 1);
    int nb_constraints = (3 + 1) * nb_points + (3 + 1);
    int nb_coeffs = (3 + 1) * nb_points + (3 + 1);


    _node_centers.resize(3, nb_points); // points
    _betas.resize(3, nb_points); // not fixed for xyz
    _alphas.resize(nb_points);      // fixed for xyz
    a.resize(3);

    Eigen::MatrixXd D(nb_constraints, nb_coeffs);
    Eigen::VectorXd f(nb_constraints);
    Eigen::VectorXd unknowns(nb_coeffs);

    D.setZero();
    f.setZero();
    unknowns.setZero();
    _alphas.setZero();
    _betas.setZero();
    a.setZero();
    b = 0.f;
    // copy the node centers
    _node_centers = points.transpose();

    for (int i = 0; i < nb_points; ++i)
    {
        Eigen::Vector3d p = points.row(i);
        Eigen::Vector3d n = normals.row(i);

        int io = (3 + 1) * i;
        f(io) = 0;
        f.template segment<3>(io + 1) = n;

        for (int j = 0; j < nb_points; ++j)
        {
            int jo = (3 + 1) * j;
            Eigen::Vector3d x = p - _node_centers.col(j);
            double x_norm = x.norm();
            if (x_norm == 0) {
                D.template block<3 + 1, 3 + 1>(io, jo).setZero();
            }
            else {

                double ddw = 3 / x_norm;
                Eigen::MatrixXd I3x3 = Eigen::Matrix<double, 3, 3>::Identity();

                // ||x||^3
                double x3 = x_norm * x_norm * x_norm;

                // 3 * x * ||x||
                Eigen::Vector3d x_grad = x * 3 * x_norm;
                // 3/||x|| ( ||x||^2 * I3x3 + xx^t)
                Eigen::Matrix<double, 3, 3> hess_x = (3 / x_norm) * ((x_norm * x_norm * Eigen::Matrix<double, 3, 3>::Identity()) + (x * x.transpose()));
                D(io, jo) = x3;
                D.row(io).template segment<3>(jo + 1) = -x_grad.transpose();
                D.col(jo).template segment<3>(io + 1) = x_grad;
                D.template block<3, 3>(1 + io, 1 + jo) = -hess_x;
            }
        }
    }

    Eigen::MatrixXd lin;
    lin.resize((3 + 1) * nb_points, (3 + 1));
    lin.setZero();

    for (int j = 0; j < nb_points; ++j)
    {
        int jo = j * (3 + 1);
        lin.block<3 + 1, 3 + 1>(jo, 0).setIdentity();
        lin.row(jo).segment<3>(1) = points.row(j);
    }
    D.block(0, (3 + 1) * nb_points, (3 + 1) * nb_points, (3 + 1)) = lin;
    D.block((3 + 1) * nb_points, 0, 3 + 1, (3 + 1) * nb_points) = lin.transpose();
    //std::cout << std::endl << "lo and behold D:  " << std::endl << D << std::endl;

    unknowns = D.lu().solve(f);


    //std::cout << "lo and behold:  " << D;
    for (int i = 0; i < nb_points; i++)
    {
        int io = i * (3 + 1);
        _alphas(i) = unknowns(io);
        _betas.col(i).segment<3>(0) = unknowns.segment<3>(io + 1);
    }
    b = unknowns((3 + 1) * nb_points);
    a = unknowns.segment<3>((3 + 1) * nb_points + 1);

}

/// Evaluate f(x)
// takes point in 3D space as input
// returns f(x)
// f(x) > 0 outside
// f(x) < 0 inside
// f(x) = 0 on
double HRBF::eval(const Eigen::RowVector3d& point) const
{
    Eigen::Vector3d pt = point.transpose();
    double ret = 0;
    int nb_nodes = _node_centers.cols();

    for (int i = 0; i < nb_nodes; ++i)
    {
        Eigen::Vector3d diff = pt - _node_centers.col(i);
        double x = diff.norm();

        if (x > 0)
        {
            ret += _alphas(i) * x*x*x;
            ret -= _betas.col(i).dot(diff) * 3 * x;
        }
    }
    ret += a.dot(pt) + b;
    return ret;
}

/// Evaluate gradient nabla f() at position 'x'

Eigen::RowVector3d HRBF::grad(const Eigen::RowVector3d& point) const
{
    Eigen::Vector3d pr = point.transpose();

    Eigen::Vector3d grad = Eigen::Vector3d::Zero();
    int nb_nodes = _node_centers.cols();
    
    for (int i = 0; i < nb_nodes; i++)
    {
        Eigen::Vector3d beta = _betas.col(i);
        float  alpha = _alphas(i);
        Eigen::MatrixXd I3x3 = Eigen::Matrix<double, 3, 3>::Identity();
        Eigen::Vector3d x = pr - _node_centers.col(i);
        float xnorm = x.norm();

        if (xnorm > 0.00001f)
        {
            grad += (alpha * 3*xnorm * x) - ( (3/xnorm) * (xnorm*xnorm*I3x3 + x*x.transpose())) * beta;
        }
    }
    grad += a;
    return grad.transpose();
}

std::shared_ptr<HRBF> getHRBF(const Eigen::MatrixXd &points, const Eigen::MatrixXd &normals) {
    auto hrbf = std::make_shared<HRBF>();
    std::cout << "fitting:\n";
    hrbf->fit(points,normals);
    std::cout << "fit!:\n";
    return hrbf;
}

Eigen::RowVector3d project_p(const HRBF &hrbf, Eigen::RowVector3d p, float threshold) {
    auto p_current = p;
    auto p_next = p_current;
    float fp = hrbf.eval(p_current);
    float fp_next = fp;
    int ik = 0;
    auto checkThreshold = [threshold](float fp) -> bool {
        return fp> 0 ? fp > threshold: fp < -threshold;
    };
    while(fp > threshold){
        float delta = .1f; // [0,1]
        float phi = .01f; // [0,1/2]
        auto gp = hrbf.grad(p_current);
        p_next = p_current - delta * (fp/gp.squaredNorm()) * gp;

        fp = fp_next;
        fp_next = hrbf.eval(p_next);
        p_current = p_next;
        ik = [delta,phi,fp,fp_next]() -> int{
            int new_ik = 0;
            while (fp_next * fp_next > fp*fp * (1 - 2 * phi * powf(delta, new_ik)) && new_ik < 16) new_ik++;
            return new_ik;
        }();

    }
    return p_next;
}
