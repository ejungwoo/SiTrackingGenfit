#include <ConstField.h>
#include <MyField.h>
#include <Exception.h>
#include <FieldManager.h>
#include <KalmanFitterRefTrack.h>
#include <StateOnPlane.h>
#include <Track.h>
#include <TrackPoint.h>
#include <TrackCand.h>
#include <MeasurementProducer.h>
#include <MeasurementFactory.h>
#include "mySpacepointDetectorHit.h"
#include "mySpacepointMeasurement.h"
#include <MaterialEffects.h>
#include <RKTrackRep.h>
#include <TGeoMaterialInterface.h>
#include <EventDisplay.h>
#include <HelixTrackModel.h>
#include <MeasurementCreator.h>
#include <TDatabasePDG.h>
#include <TEveManager.h>
#include <TGeoManager.h>
#include <TGeoNode.h>
#include <TEveGeoNode.h>
#include <TRandom.h>
#include <TVector3.h>
#include <vector>
#include "TDatabasePDG.h"
#include <TMath.h>
#include <iostream>
using namespace std;

int main()
{
    gRandom -> SetSeed(time(0));

    // initial parameters for analysis
    bool useFieldMap = false;
    bool useDisplay = true;
    int numberOfEvents = 1;
    double zSpacing = 1.0; // space between layers (cm)
    unsigned int nMeasurements = 10;
    const int pdg = 11; // e- pdg code (-11 for positron)
    const double charge = TDatabasePDG::Instance()->GetParticle(pdg)->Charge()/(3.);
    TVector3 posReco(0,0,0); // position where user want's to know the momentum
    TVector3 posHelix(0,0,0); // initial trial of helix position
    TVector3 momHelix(0,0.5,0.5); // initial trial of helix momentum

    /////////////////////////////////////////////////////////////////////////////////////
    // init geometry and mag. field
    new TGeoManager("Geometry", "Geane geometry");
    TGeoManager::Import("detectorGeometry.root");
    if (useFieldMap) genfit::FieldManager::getInstance() -> init(new genfit::MyField("fieldMap.root")); // 15 kGauss
    else genfit::FieldManager::getInstance()->init(new genfit::ConstField(0.,0.,15.)); // 15 kGauss
    genfit::MaterialEffects::getInstance() -> init(new genfit::TGeoMaterialInterface());

    /////////////////////////////////////////////////////////////////////////////////////
    // init event display
    genfit::EventDisplay* display = nullptr;
    if (useDisplay)
        display = genfit::EventDisplay::getInstance();

    /////////////////////////////////////////////////////////////////////////////////////
    // init fitter and init the factory
    genfit::AbsKalmanFitter* fitter = new genfit::KalmanFitterRefTrack();
    auto myDetectorHitArray = new TClonesArray("genfit::mySpacepointDetectorHit");
    genfit::MeasurementFactory<genfit::AbsMeasurement> factory;
    auto myProducer = new genfit::MeasurementProducer<genfit::mySpacepointDetectorHit,genfit::mySpacepointMeasurement>(myDetectorHitArray);
    int myDetId = 1;
    factory.addProducer(myDetId, myProducer);

    /////////////////////////////////////////////////////////////////////////////////////
    // every event, make point and do fitting
    for (unsigned int iEvent=0; iEvent<numberOfEvents; ++iEvent)
    {
        myDetectorHitArray -> Clear();

        genfit::TrackCand trackCandidate;
        genfit::HelixTrackModel* helix = new genfit::HelixTrackModel(posHelix, mom, charge);

        // covariance
        double resolution = 1;
        TMatrixDSym cov(3);
        for (int i = 0; i < 3; ++i)
            cov(i,i) = resolution*resolution;

        /////////////////////////////////////////////////////////////////////////////////////
        // TODO Put simulation data points here
        /////////////////////////////////////////////////////////////////////////////////////
        // make measurement point
        for (unsigned int i=0; i<nMeasurements; ++i)
        {
            double trackLength = i*zSpacing;
            TVector3 currentPos = helix -> getPos(trackLength);
            cout << "+" << __LINE__ << " " << __FILE__ << " # l/position = " << trackLength << " / " << currentPos.X() << ", " << currentPos.Y() << ", " << currentPos.Z() << endl;
            currentPos.SetX(currentPos.X());
            currentPos.SetY(currentPos.Y());
            currentPos.SetZ(currentPos.Z());

            // Fill the TClonesArray and the TrackCand
            // In a real experiment, you detector code would deliver mySpacepointDetectorHits and fill the TClonesArray.
            // The patternRecognition would create the TrackCand.
            new((*myDetectorHitArray)[i]) genfit::mySpacepointDetectorHit(currentPos, cov);
            trackCandidate.addHit(myDetId, i);
        }
        /////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////

        // approximate covariance initial guess for cov
        TMatrixDSym covInit(6);
        for (int i = 0; i < 3; ++i) covInit(i,i) = resolution*resolution;
        for (int i = 3; i < 6; ++i) covInit(i,i) = pow(resolution / nMeasurements / sqrt(3), 2);

        // set start values and pdg to cand
        TVector3 posInit(0,0,0);
        TVector3 momInit(momHelix);
        trackCandidate.setPosMomSeedAndPdgCode(posInit, momInit, pdg);
        trackCandidate.setCovSeed(covInit);

        // create track
        auto fitTrack = new genfit::Track(trackCandidate, factory, new genfit::RKTrackRep(pdg));

        /////////////////////////////////////////////////////////////////////////////////////
        // do the fit
        try{
            fitter -> processTrack(fitTrack);

            auto trackRep = (genfit::RKTrackRep *) fitTrack -> getTrackRep(0);
            auto fitState = fitTrack -> getFittedState();
            auto fitStatus = fitTrack -> getFitStatus(trackRep);
            trackRep -> extrapolateToPoint(fitState, pos0);

            TVector3 momVertex;
            TVector3 pocaVertex;
            TMatrixDSym covMat(6,6);
            fitState.getPosMomCov(pocaVertex, momVertex, covMat);
            momVertex = momVertex;
            pocaVertex = pocaVertex;
            cout << "Momentum = (" << momVertex.x() << ", " << momVertex.y() << ", " << momVertex.z() << ")" << endl;
            cout << "pocaVertex = (" << pocaVertex.x() << ", " << pocaVertex.y() << ", " << pocaVertex.z() << ")" << endl;
            //cout << << " " << pocaVertex << endl;
        }
        catch(genfit::Exception& e){
            std::cerr << e.what();
            std::cerr << "Exception, next track" << std::endl;
            continue;
        }

        fitTrack -> checkConsistency();

        if (useDisplay)
            if (iEvent < 1000) {
                // add track to event display
                display->addEvent(fitTrack);
            }
    }
    /////////////////////////////////////////////////////////////////////////////////////

    delete fitter;

    /////////////////////////////////////////////////////////////////////////////////////
    // set detector geomtry and draw
    if (useDisplay) {
        TGeoNode* geoNode = gGeoManager -> GetTopNode();
        TEveGeoTopNode* topNode = new TEveGeoTopNode(gGeoManager, geoNode, 1, 3, 1000);
        gEve -> AddGlobalElement(topNode);
        TObjArray* listVolume = gGeoManager -> GetListOfVolumes();
        int nVolumes = listVolume -> GetEntries();
        for (int i=0; i<nVolumes; i++)
            ((TGeoVolume*) listVolume -> At(i)) -> SetTransparency(80);
        display->open();
    }

    return 0;
}
