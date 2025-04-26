void DrawHist(string HistFileName = "HistFile.root")
{
    // Load a histogram from a file
    TFile* HistFile = TFile::Open(HistFileName.data());

    if (!HistFile)
    {
        cerr << "Can't open the " << HistFileName << " file! Exit..." << endl;
        exit(0);
    }

    // Load the first histogram
    TH1D* H1 = (TH1D*)HistFile->Get("H1");
    if (!H1)
    {
        cerr << "Can't load the H1 histogram from the " << HistFileName << " file! Exit..." << endl;
        exit(0);
    }

    // Load the second histogram
    TH1D* H2 = (TH1D*)HistFile->Get("H2");
    if (!H2)
    {
        cerr << "Can't load the H2 histogram from the " << HistFileName << " file! Exit..." << endl;
        exit(0);
    }

    // Draw and save H1
    TCanvas* C1 = new TCanvas("C1", "Flight Length Histogram");
    H1->Draw("hist");
    C1->SaveAs("H1.png");

    // Draw and save H2
    TCanvas* C2 = new TCanvas("C2", "Impact Parameter Histogram");
    H2->Draw("hist");
    C2->SaveAs("H2.png");

    // Clean up
    HistFile->Close();
    delete HistFile;
    delete C1;
    delete C2;
}
