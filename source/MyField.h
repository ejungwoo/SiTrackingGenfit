#ifndef GENFIT_MYFIELD_H
#define GENFIT_MYFIELD_H

#include "AbsBField.h"
#include "TH3D.h"

namespace genfit
{
    class MyField : public AbsBField
    {
        public:
            MyField(TString fileName);
            TVector3 get(const TVector3& pos) const;
            void get(const double& posX, const double& posY, const double& posZ, double& Bx, double& By, double& Bz) const;

        private:
            TH3D *fHistX = nullptr;
            TH3D *fHistY = nullptr;
            TH3D *fHistZ = nullptr;
    };

}

#endif
