void makeFieldFile()
{
    auto field_map_x = new TH3D("field_map_x","",5,-100,100,5,-100,100,5,-100,100);
    auto field_map_y = new TH3D("field_map_y","",5,-100,100,5,-100,100,5,-100,100);
    auto field_map_z = new TH3D("field_map_z","",5,-100,100,5,-100,100,5,-100,100);
    for (auto ix=1; ix<=5; ++ix)
        for (auto iy=1; iy<=5; ++iy)
            for (auto iz=1; iz<=5; ++iz) {
                field_map_x -> SetBinContent(ix,iy,iz,0);
                field_map_y -> SetBinContent(ix,iy,iz,0);
                field_map_z -> SetBinContent(ix,iy,iz,5-(iz-1)*1/4.);
            }

    auto file = new TFile("fieldMap.root","recreate");
    field_map_x -> Write();
    field_map_y -> Write();
    field_map_z -> Write();
    cout << field_map_z -> Interpolate(1,1,1) << endl;
    cout << field_map_z -> Interpolate(1,1,8.999) << endl;
    cout << "This will create error " << field_map_z -> Interpolate(1,1,-81) << endl;
    cout << "This will create error " << field_map_z -> Interpolate(1,1,200) << endl;
}
