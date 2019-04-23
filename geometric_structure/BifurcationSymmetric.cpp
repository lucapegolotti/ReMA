#include "BifurcationSymmetric.hpp"

namespace RedMA
{

BifurcationSymmetric::
BifurcationSymmetric(commPtr_Type comm, bool verbose) :
  BuildingBlock(comm, verbose)
{
    M_name = "BifurcationSymmetric";

    M_datafileName = "bifurcation_symmetric_coarse_data";

    // values are referred to the reference configuration

    M_inletCenterRef[0] = 0;
    M_inletCenterRef[1] = 0;
    M_inletCenterRef[2] = 0;

    M_inletNormalRef[0] = 0;
    M_inletNormalRef[1] = -1;
    M_inletNormalRef[2] = 0;

    M_outlet1CenterRef[0] = 9;
    M_outlet1CenterRef[1] = 30;
    M_outlet1CenterRef[2] = 0;

    M_outlet1NormalRef[0] = 0.3714;
    M_outlet1NormalRef[1] = 0.9285;
    M_outlet1NormalRef[2] = 0.0;

    M_outlet2CenterRef[0] = -9;
    M_outlet2CenterRef[1] = 30;
    M_outlet2CenterRef[2] = 0.0;

    M_outlet2NormalRef[0] = -0.3714;
    M_outlet2NormalRef[1] = 0.9285;
    M_outlet2NormalRef[2] = 0.0;

    M_inletRadiusRef = 3.0;
    M_outlet1RadiusRef = 3.0;
    M_outlet2RadiusRef = 3.0;

    GeometricFace inlet(M_inletCenterRef, M_inletNormalRef, M_inletRadiusRef);
    GeometricFace outlet1(M_outlet1CenterRef, M_outlet1NormalRef, M_outlet1RadiusRef);
    GeometricFace outlet2(M_outlet2CenterRef, M_outlet2NormalRef, M_outlet2RadiusRef);

    M_inlet = inlet;
    M_outlets.push_back(outlet1);
    M_outlets.push_back(outlet2);

    const bool randomizible = true;
    const double maxAngle = 0.3;
    M_parametersHandler.registerParameter("out1_alpha_plane", 0.0, -maxAngle,
                                          maxAngle, randomizible);
    M_parametersHandler.registerParameter("out1_alphax", 0.0, -maxAngle,
                                          maxAngle, randomizible);
    M_parametersHandler.registerParameter("out2_alpha_plane", 0.0, -maxAngle,
                                          maxAngle, randomizible);
    M_parametersHandler.registerParameter("out2_alphax", 0.0, -maxAngle,
                                          maxAngle, randomizible);
}

void
BifurcationSymmetric::
bend(const double& out1_alpha_plane, const double& out1_alphax,
     const double& out2_alpha_plane, const double& out2_alphax,
     Transformer& transformer)
{
    using namespace std::placeholders;

    double angleOutlet1 = std::atan(M_outlet1NormalRef[0] /
                                    M_outlet1NormalRef[1]);

    double yRotationCenter = M_outlet2CenterRef[1] -
                             M_outlet1CenterRef[0] * angleOutlet1;

    // handle rotation for outlet 1
    Vector3D rotationCenter(0, yRotationCenter, 0);
    Matrix3D rotationMatrix =
             computeRotationMatrix(2, out1_alpha_plane) *
             computeRotationMatrix(0, out1_alphax);

    Vector3D rotatedCenterOutlet1;
    Vector3D rotatedNormalOutlet1;
    rotateGeometricFace(M_outlets[0], rotatedCenterOutlet1, rotatedNormalOutlet1,
                        rotationMatrix, rotationCenter);

    auto fooOutlet1 = std::bind(outletMapFunction,
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3,
                                std::placeholders::_4,
                                std::placeholders::_5,
                                M_outlets[0], rotatedCenterOutlet1,
                                rotationMatrix);

    M_outlets[0].M_center = rotatedCenterOutlet1;
    M_outlets[0].M_normal = rotatedNormalOutlet1;

    // handle rotation for outlet 2
    rotationMatrix =
             computeRotationMatrix(2, -out2_alpha_plane) *
             computeRotationMatrix(0, out2_alphax);

    Vector3D rotatedCenterOutlet2;
    Vector3D rotatedNormalOutlet2;
    rotateGeometricFace(M_outlets[1], rotatedCenterOutlet2, rotatedNormalOutlet2,
                        rotationMatrix, rotationCenter);

    auto fooOutlet2 = std::bind(outletMapFunction,
                                std::placeholders::_1,
                                std::placeholders::_2,
                                std::placeholders::_3,
                                std::placeholders::_4,
                                std::placeholders::_5,
                                M_outlets[1], rotatedCenterOutlet2,
                                rotationMatrix);

    M_outlets[1].M_center = rotatedCenterOutlet2;
    M_outlets[1].M_normal = rotatedNormalOutlet2;

    NonAffineDeformer nAffineDeformer(M_mesh, M_comm, M_verbose);

    LifeV::BCFunctionBase zeroFunction(BuildingBlock::fZero);
    LifeV::BCFunctionBase outletFunction1(fooOutlet1);
    LifeV::BCFunctionBase outletFunction2(fooOutlet2);

    std::shared_ptr<LifeV::BCHandler> bcs(new LifeV::BCHandler);
    bcs->addBC("Inflow", 1, LifeV::Essential, LifeV::Full,
               zeroFunction, 3);
    bcs->addBC("Outflow", 2, LifeV::Essential, LifeV::Full,
               outletFunction1, 3);
    bcs->addBC("Outflow", 3, LifeV::Essential, LifeV::Full,
               outletFunction2, 3);

    CoutRedirecter ct;
    ct.redirect();
    nAffineDeformer.applyBCs(bcs);
    std::string xmlFilename = M_datafile("geometric_structure/xml_file",
                                         "SolverParamList.xml");
    nAffineDeformer.setXMLsolver(xmlFilename);
    nAffineDeformer.deformMesh(transformer);
    printlog(CYAN, ct.restore(), M_verbose);
}

double
BifurcationSymmetric::
outletMapFunction(const double& t, const double& x,
                  const double& y, const double& z,
                  const LifeV::ID& i, const GeometricFace& targetFace,
                  const Vector3D& desiredCenter, const Matrix3D& rotationMatrix)
{
    Vector3D cur(x, y, z);
    Vector3D diff = cur - targetFace.M_center;

    Vector3D modifiedDif = rotationMatrix * diff;
    Vector3D newPoint = desiredCenter + modifiedDif;

    return newPoint[i] - cur[i];
}

void
BifurcationSymmetric::
rotateGeometricFace(const GeometricFace& face, Vector3D& rotatedCenter,
                    Vector3D& rotatedNormal, const Matrix3D& rotationMatrix,
                    const Vector3D& rotationCenter)
{
    rotatedNormal = rotationMatrix * face.M_normal;
    rotatedCenter = rotationMatrix * (face.M_center - rotationCenter) +
                    rotationCenter;
}


void
BifurcationSymmetric::
applyNonAffineTransformation()
{
    LifeV::MeshUtility::MeshTransformer<mesh_Type> transformer(*M_mesh);
    bend(M_parametersHandler["out1_alpha_plane"],
         M_parametersHandler["out1_alphax"],
         M_parametersHandler["out2_alpha_plane"],
         M_parametersHandler["out2_alphax"], transformer);
}

}
