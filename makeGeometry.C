void makeGeometry()
{
    new TGeoManager("genfitGeom", "GENFIT geometry");

    unsigned int mediumIndex = 0;
    Double_t mediumPar[10];
    //TAG sollte wieder 0 werden sens flag
    mediumPar[0]=0.; //sensitive volume flag
    mediumPar[1]=1.; //magnetic field flag
    mediumPar[2]=30.; //max fiel in kGauss
    mediumPar[3]=0.1; //maximal angular dev. due to field
    mediumPar[4]=0.01; //max step allowed (in cm)
    mediumPar[5]=1.e-5; //max fractional energy loss
    mediumPar[6]=1.e-3; //boundary crossing accuracy
    mediumPar[7]=1.e-5; //minimum step
    mediumPar[8]=0.; //not defined
    mediumPar[9]=0.; //not defined

    TGeoMaterial *siliconMat = new TGeoMaterial("siliconMat",28.0855,14.,2.329);
    siliconMat -> SetRadLen(1.); //calc automatically, need this for elemental mats.
    TGeoMedium *silicon = new TGeoMedium("silicon",mediumIndex++,siliconMat,mediumPar);

    TGeoMixture *vacuumMat = new TGeoMixture("vacuumMat",3);
    vacuumMat -> AddElement(14.01,7.,.78);
    vacuumMat -> AddElement(16.00,8.,.21);
    vacuumMat -> AddElement(39.95,18.,.01);
    vacuumMat -> SetDensity(1.2e-15);
    TGeoMedium *vacuum = new TGeoMedium("vacuum",mediumIndex++,vacuumMat,mediumPar);

    TGeoVolume *top = gGeoManager -> MakeBox("TOPPER", vacuum, 1000., 1000., 1000.);
    gGeoManager -> SetTopVolume(top); // mandatory !

    // Detector dimensions (cm)
    double dx = 5.0; // 100 mm
    double dy = 5.0; // 100 mm
    double dz = 0.05; // 1 mm
    double zSpacing = 1.0; // space between layers (cm)
    int nLayers = 10;

    for (int i=0; i<nLayers; ++i)
    {
        TString volName = Form("siliconLayer_%d",i);
        TGeoVolume* siliconLayer = gGeoManager -> MakeBox(volName, silicon, dx, dy, dz);
        siliconLayer -> SetLineColor(kGray+1);
        double zPos = i * zSpacing;
        top -> AddNode(siliconLayer, i, new TGeoTranslation(0, 0, zPos));
    }

    gGeoManager -> CloseGeometry();
    gGeoManager -> SetVisLevel(10);
    //top -> Draw("ogl");

    auto outfile = new TFile("detectorGeometry.root","RECREATE");
    gGeoManager -> Write();
    outfile -> Close();
}
