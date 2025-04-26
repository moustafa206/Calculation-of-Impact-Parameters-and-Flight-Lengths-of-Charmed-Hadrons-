#include "Riostream.h"  
#include "TFile.h"
#include "TSystemDirectory.h"
#include "TSystem.h"
#include "TMath.h"
#include "TExample1.h"

using namespace std;

TExample1::TExample1()
{
  InitHist();
}

TExample1::~TExample1()
{
  SaveAndDeleteHist();
  ClearDataStructures();
}

void TExample1::Run()
{
  FindVertexFiles();
  FillHist();
}

void TExample1::InitHist()
{
  mH1 = new TH1D("H1", "Flight Length", 9, 0, 5000);
  mH1->SetFillColor(kBlue);
  mH1->SetXTitle("Flight Length");
  mH1->SetYTitle("Frequency");

  mH2 = new TH1D("H2", "Impact Parameters", 7, 0, 500);
  mH2->SetFillColor(kRed);
  mH2->SetXTitle("Impact Parameter");
  mH2->SetYTitle("Frequency");
}

void TExample1::SaveAndDeleteHist()
{
  TFile* HistFile = new TFile("HistFile.root", "recreate");
  mH1->Write();
  mH2->Write();
  SafeDelete(mH1);
  SafeDelete(mH2);
  SafeDelete(HistFile);
}

void TExample1::ClearDataStructures()
{
  for (auto& PrimaryVertices : mPrimaryVertices) PrimaryVertices.second.clear();
  mPrimaryVertices.clear();
  mSecondaryVertices.clear();
  mTrackLines.clear();
}

void TExample1::FindVertexFiles()
{
  const string DataDirName = "data/CharmSample_EmulsionDataset";
  TSystemDirectory DataDir(DataDirName.data(), DataDirName.data());
  TList* DataDirFileList = DataDir.GetListOfFiles();

  if (!DataDirFileList || (DataDirFileList->GetSize() < 3))
  {
    cerr << "Error: No files in " << DataDirName << "!" << endl;
    exit(0);
  }

  TIter nextFile(DataDirFileList);

  for (TSystemFile* DataFile; (DataFile = (TSystemFile*)nextFile());)
  {
    if (DataFile->IsDirectory()) continue;

    string DataFileName = DataFile->GetName();
    size_t StrPos;

    // Process vertex files
    StrPos = DataFileName.find("_Vertices.csv");
    if (StrPos != string::npos)
    {
      mEventId = stoull(DataFileName.substr(0, StrPos));
      ReadVertexFile(DataDirName + "/" + DataFileName);
      continue;
    }

    // Process track files
    StrPos = DataFileName.find("_TrackLines.csv");
    if (StrPos != string::npos)
    {
      ULong64_t TrackEventId = stoull(DataFileName.substr(0, StrPos));
      ReadTrackFile(DataDirName + "/" + DataFileName, TrackEventId);
    }
  }
}

void TExample1::ReadVertexFile(const string& DataFilePath)
{
  ifstream ifs(DataFilePath);
  if (!ifs)
  {
    cerr << "Error: Can't open " << DataFilePath << "!" << endl;
    exit(0);
  }

  string fstring;
  getline(ifs, fstring);

  vector<Double_t>& V1 = mPrimaryVertices[mEventId];
  vector<Double_t>& V2 = mSecondaryVertices[mEventId];

  V1.resize(3);
  V2.resize(3);

  while (getline(ifs, fstring))
  {
    istringstream istr(fstring);
    int vertType;
    Double_t posX, posY, posZ;

    istr >> vertType;
    istr.ignore();
    istr >> posX;
    istr.ignore();
    istr >> posY;
    istr.ignore();
    istr >> posZ;

    if (vertType == 1)
    {
      V1[0] = posX;
      V1[1] = posY;
      V1[2] = posZ;
    }
    else if (vertType == 2)
    {
      V2[0] = posX;
      V2[1] = posY;
      V2[2] = posZ;
    }
  }
  ifs.close();
}

void TExample1::ReadTrackFile(const string& DataFilePath, ULong64_t EventId)
{
    ifstream ifs(DataFilePath);
    if (!ifs)
    {
        cerr << "Error: Can't open " << DataFilePath << "!" << endl;
        exit(0);
    }

    string fstring;
    getline(ifs, fstring); // Skip header line

    vector<vector<Double_t>> TrackLines;  // Store multiple track lines

    cout << "Event " << EventId << " - Available Tracks: " << endl;

    while (getline(ifs, fstring))
    {
        istringstream istr(fstring);
        int trType;
        Double_t posX1, posY1, posZ1, posX2, posY2, posZ2;

        istr >> trType; istr.ignore();
        istr >> posX1; istr.ignore();
        istr >> posY1; istr.ignore();
        istr >> posZ1; istr.ignore();
        istr >> posX2; istr.ignore();
        istr >> posY2; istr.ignore();
        istr >> posZ2;

        cout << "  trType=" << trType << " Start=(" << posX1 << ", " << posY1 << ", " << posZ1 
             << ") End=(" << posX2 << ", " << posY2 << ", " << posZ2 << ")" << endl;

        if (trType == 10)  // Consider all type 10 tracks
        {
            TrackLines.push_back({posX1, posY1, posZ1, posX2, posY2, posZ2});
        }
    }
    ifs.close();

    if (TrackLines.empty())
    {
        cerr << "Warning: No track of type 10 found for event " << EventId << endl;
        return;
    }

    mTrackLines[EventId] = TrackLines; // Store all track lines

    // Debug print
    for (const auto& LinePoints : TrackLines)
    {
        cout << "Selected Track for Event " << EventId << ": (" 
             << LinePoints[0] << ", " << LinePoints[1] << ", " << LinePoints[2] << ") -> (" 
             << LinePoints[3] << ", " << LinePoints[4] << ", " << LinePoints[5] << ")" << endl;
    }
}


Double_t TExample1::CalculateImpactParameter(const vector<Double_t>& V, const vector<Double_t>& Line)
{
    Double_t x0 = V[0], y0 = V[1], z0 = V[2];  // Primary vertex
    Double_t x1 = Line[0], y1 = Line[1], z1 = Line[2];  // Track start
    Double_t x2 = Line[3], y2 = Line[4], z2 = Line[5];  // Track end

    // Direction vector of the line (AB = B - A)
    Double_t ABx = x2 - x1, ABy = y2 - y1, ABz = z2 - z1;

    // Vector from the line start to the primary vertex (AP = P - A)
    Double_t APx = x0 - x1, APy = y0 - y1, APz = z0 - z1;

    // Cross product of AB and AP
    Double_t crossX = ABy * APz - ABz * APy;
    Double_t crossY = ABz * APx - ABx * APz;
    Double_t crossZ = ABx * APy - ABy * APx;

    // Compute the magnitude of the cross product (numerator)
    Double_t numerator = sqrt(crossX * crossX + crossY * crossY + crossZ * crossZ);

    // Compute the magnitude of the line direction vector (denominator)
    Double_t denominator = sqrt(ABx * ABx + ABy * ABy + ABz * ABz);

    // Distance formula: d = |AP × AB| / |AB|
    return numerator / denominator;
}


void TExample1::FillHist()
{
    // Loop for Flight Length Histogram (mH1)
    for (const auto& PrimaryVertices : mPrimaryVertices)
    {
        ULong64_t EventId = PrimaryVertices.first;
        const vector<Double_t>& V1 = PrimaryVertices.second;
        const vector<Double_t>& V2 = mSecondaryVertices.at(EventId);
        
        Double_t FlightLength = sqrt(pow(V2[0] - V1[0], 2) +
                                     pow(V2[1] - V1[1], 2) +
                                     pow(V2[2] - V1[2], 2));
        
        mH1->Fill(FlightLength);
        
        cout << "Event ID: " << EventId << endl;
        cout << "Primary Vertex: (" << V1[0] << ", " << V1[1] << ", " << V1[2] << ")" << endl;
        cout << "Flight Length: " << FlightLength << endl;
        cout << "-----------------------------------" << endl;
    }

    // Loop for Impact Parameter Histogram (mH2)
    for (const auto& PrimaryVertices : mPrimaryVertices)
    {
        ULong64_t EventId = PrimaryVertices.first;
        const vector<Double_t>& V1 = PrimaryVertices.second;
        const vector<vector<Double_t>>& TrackLines = mTrackLines.at(EventId);  // Multiple tracks

        for (const auto& TrackLine : TrackLines)
        {
            Double_t ImpactPar = CalculateImpactParameter(V1, TrackLine);
            mH2->Fill(ImpactPar);

            cout << "Track Line: (" << TrackLine[0] << ", " << TrackLine[1] << ", " 
                 << TrackLine[2] << ") -> (" << TrackLine[3] << ", " << TrackLine[4] << ", " 
                 << TrackLine[5] << ")" << endl;
            cout << "Impact Parameter (Distance to Track Line): " << ImpactPar << endl;
            cout << "-----------------------------------" << endl;
        }
    }
}