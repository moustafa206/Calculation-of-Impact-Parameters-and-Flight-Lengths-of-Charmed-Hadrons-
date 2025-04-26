#pragma once
//-------------------------------------------------------------------------------

#include <vector>
#include <map>

#include "TH1D.h"
//-------------------------------------------------------------------------------

class TExample1
{
 public:

  TExample1();

  TExample1(const TExample1&) = delete;

  virtual ~TExample1();

  TExample1& operator= (const TExample1&) = delete;

  void Run();

 private:

  ULong64_t mEventId;
  

  size_t mNbOfEvents;

  std::map<ULong64_t, std::vector<Double_t>> mPrimaryVertices; // A structure where the primary vertex coordinates will be stored separately for each event id
  std::map<ULong64_t, std::vector<Double_t>> mSecondaryVertices;
  std::map<ULong64_t, std::vector<Double_t>> mDaughterPoint;
  std::map<unsigned long long, std::vector<std::vector<double>>> mTrackLines;
  Double_t CalculateImpactParameter(const std::vector<Double_t>& V, const std::vector<Double_t>& Line);



  TH1D* mH1; // A pointer to the ROOT histogram object
  TH1D* mH2; // A pointer to the ROOT histogram object

  void FindVertexFiles();

  void ReadVertexFile(const std::string& DataFilePath);
  
  void ReadTrackFile(const std::string& DataFilePath, ULong64_t EventId);
  


  void ClearDataStructures();

  void InitHist();

  void SaveAndDeleteHist();

  void FillHist();
};
//-------------------------------------------------------------------------------
