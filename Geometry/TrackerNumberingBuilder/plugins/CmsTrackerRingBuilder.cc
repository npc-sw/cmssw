#include "Geometry/TrackerNumberingBuilder/plugins/CmsTrackerRingBuilder.h"
#include "DetectorDescription/Core/interface/DDFilteredView.h"
#include "Geometry/TrackerNumberingBuilder/interface/GeometricDet.h"
#include "Geometry/TrackerNumberingBuilder/plugins/ExtractStringFromDDD.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "Geometry/TrackerNumberingBuilder/plugins/CmsDetConstruction.h"
#include "Geometry/TrackerNumberingBuilder/plugins/TrackerStablePhiSort.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <vector>

#include <bitset>

void CmsTrackerRingBuilder::buildComponent(DDFilteredView& fv, GeometricDet* g, std::string s){
  CmsDetConstruction theCmsDetConstruction;
  theCmsDetConstruction.buildComponent(fv,g,s);  
}

void CmsTrackerRingBuilder::sortNS(DDFilteredView& fv, GeometricDet* det){
  GeometricDet::GeometricDetContainer & comp = det->components();
  fv.firstChild(); 
  GeometricDet::GeometricDetContainer compfw;
  GeometricDet::GeometricDetContainer compbw;


  switch(comp.front()->type()) {
  
  case GeometricDet::mergedDet: 
    TrackerStablePhiSort(comp.begin(), comp.end(), ExtractPhiGluedModule()); 
    break;
  case GeometricDet::DetUnit: 
    TrackerStablePhiSort(comp.begin(), comp.end(), ExtractPhi()); 
    break;
  default:
    edm::LogError("CmsTrackerRingBuilder")<<"ERROR - wrong SubDet to sort..... "<<det->components().front()->type(); 
  }
  
  static std::string const part("TkDDDStructure");
  static std::string const TECGluedDet("TECGluedDet");
  static std::string const TECDet("TECDet");

  std::string const pname = ExtractStringFromDDD::getString(part,&fv); 

  // TEC
  // Module Number: 3 bits [1,...,5 at most]
  if(pname== TECGluedDet || pname == TECDet){
    
    // TEC- 
    if( det->translation().z() < 0 && pname == TECDet) {
      TrackerStablePhiSort(comp.begin(), comp.end(), ExtractPhiMirror());
    }
    
    if( det->translation().z() < 0 && pname == TECGluedDet) {
      TrackerStablePhiSort(comp.begin(), comp.end(), ExtractPhiGluedModuleMirror());
    }

    for(uint32_t i=0; i<comp.size();i+1+)
      comp[i]->setGeographicalID(i+1);

  } else {
    // TID
    // Ring Side: 2 bits [back:1 front:2]
    // Module Number: 5 bits [1,...,20 at most]
    //
    for(uint32_t i=0; i<comp.size();i++){
      if(fabs(comp[i]->translation().z())<fabs(det->translation().z())){      
	compfw.push_back(comp[i]);
      } else {
	compbw.push_back(comp[i]);
      }
    }
    
    for(uint32_t i=0; i<compbw.size();i++){
      uint32_t temp = i+1;
      temp |=(1<<5);
      compbw[i]->setGeographicalID(temp);
    }
    
    for(uint32_t i=0; i<compfw.size();i++){
      uint32_t temp = i+1;
      temp |=(2<<5);
      compfw[i]->setGeographicalID(temp);
    }
    
    det->deleteComponents();
    det->addComponents(compfw);
    det->addComponents(compbw);
    
  }
  
  fv.parent();
  
}





