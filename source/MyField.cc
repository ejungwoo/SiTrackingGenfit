#include "MyField.h"
#include "TFile.h"
#include <iostream>
using namespace std;

namespace genfit {

MyField::MyField(TString fileName)
{
    auto file = new TFile(fileName);
    fHistX = (TH3D*) file -> Get("field_map_x");
    fHistY = (TH3D*) file -> Get("field_map_y");
    fHistZ = (TH3D*) file -> Get("field_map_z");
    if (fHistX==nullptr) cout << "ERROR field_map_x from " << fileName << " is nullptr!" << endl;
    if (fHistY==nullptr) cout << "ERROR field_map_y from " << fileName << " is nullptr!" << endl;
    if (fHistZ==nullptr) cout << "ERROR field_map_z from " << fileName << " is nullptr!" << endl;
}

TVector3 MyField::get(const TVector3& pos) const {
    double Bx, By, Bz;
    get(pos.x(),pos.y(),pos.z(),Bx,By,Bz);
    return TVector3(Bx,By,Bz);
}

void MyField::get(const double& posX, const double& posY, const double& posZ, double& Bx, double& By, double& Bz) const
{
    Bx = fHistX -> Interpolate(posX,posY,posZ);
    By = fHistY -> Interpolate(posX,posY,posZ);
    Bz = fHistZ -> Interpolate(posX,posY,posZ);
}

}
