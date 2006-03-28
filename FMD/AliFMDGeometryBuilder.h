#ifndef ALIFMDGEOMETRYBUILDER_H
#define ALIFMDGEOMETRYBUILDER_H
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights
 * reserved. 
 *
 * Latest changes by Christian Holm Christensen <cholm@nbi.dk>
 *
 * See cxx source for full Copyright notice                               
 */
/** @file    AliFMDGeometryBuilder.h
    @author  Christian Holm Christensen <cholm@nbi.dk>
    @date    Mon Mar 27 12:41:17 2006
    @brief   Class to build the FMD geometry 
*/
#ifndef ROOT_TTask
# include <TTask.h>
#endif
#ifndef ROOT_TArrayI
# include <TArrayI.h>
#endif
class TGeoVolume;
class TGeoMedium;
class AliFMD;
class AliFMDRing;
class AliFMDDetector;
class AliFMD1;
class AliFMD2;
class AliFMD3;

//____________________________________________________________________
/** @class AliFMDGeometryBuilder 
    @brief Builder of FMD geometry. 
    This class takes care of actually building the geometry using the
    @b TGeo classes.  Various parameters are fecthed from the
    AliFMDGeometry manager. 
    @ingroup FMD_sim
 */
class AliFMDGeometryBuilder : public TTask
{
public:
  /** CTOR */
  AliFMDGeometryBuilder();
  /** CTOR 
      @param detailed Whether to make a detailed geometry. */
  AliFMDGeometryBuilder(Bool_t detailed);
  virtual ~AliFMDGeometryBuilder() {}
  /** Register */
  virtual void Exec(Option_t* option="");
  /** @param use Wheher to assemblies in the geometry definition */
  virtual void UseAssembly(Bool_t use=kTRUE) { fUseAssembly = use; }
  /** Whether to make a detailed geometry or not. 
      @param use If true, make a detailed geometry */
  virtual void SetDetailed(Bool_t use) { fDetailed = use; }
  /** @return Sector offset in volume tree  */
  Int_t GetSectorOff() const { return fSectorOff; }
    /** @return Module offset in volume tree */
  Int_t GetModuleOff() const { return fModuleOff; }
    /** @return Ring offset in the volume tree  */
  Int_t GetRingOff() const { return fRingOff; }
    /** @return Detector offfset in the volume tree  */
  Int_t GetDetectorOff() const { return fDetectorOff; }
protected:
  /** Make a ring volume 
      @param r Ring geometry 
      @return  Ring volume */
  virtual TGeoVolume* RingGeometry(AliFMDRing* r);
  /** Make a detector volume 
      @param d Detector geometry 
      @param motherTop Mother volume (detector volume)
      @param motherBot Mother volume (detector volume)
      @param zmother Z position of mother 
      @param innerTop Inner ring volume 
      @param innerBot Inner ring volume 
      @param outerTop Outer ring volume 
      @param outerBot Outer ring volume 
      @return  Detector volume */
  virtual TGeoVolume* DetectorGeometry(AliFMDDetector* d, 
				       TGeoVolume* motherTop, 
				       TGeoVolume* motherBot, 
				       Double_t    zmother, 
				       TGeoVolume* innerTop, 
				       TGeoVolume* innerBot, 
				       TGeoVolume* outerTop=0,
				       TGeoVolume* outerBot=0);
  /** Make FMD1 volume 
      @param d Detector geometry 
      @param innerTop Inner ring volume 
      @param innerBot Inner ring volume 
      @return FMD1 volume  */
  virtual TGeoVolume* FMD1Geometry(AliFMD1* d, 
				   TGeoVolume* innerTop,
				   TGeoVolume* innerBot);
  /** Make FMD2 volume 
      @param d Detector geometry 
      @param innerTop Inner ring volume 
      @param innerBot Inner ring volume 
      @param outerTop Outer ring volume 
      @param outerBot Outer ring volume 
      @return FMD2 volume  */
  virtual TGeoVolume* FMD2Geometry(AliFMD2* d, 
				   TGeoVolume* innerTop, 
				   TGeoVolume* innerBot, 
				   TGeoVolume* outerTop,
				   TGeoVolume* outerBot);
  /** Make FMD3 volume 
      @param d Detector geometry 
      @param innerTop Inner ring volume 
      @param innerBot Inner ring volume 
      @param outerTop Outer ring volume 
      @param outerBot Outer ring volume 
      @return FMD3 volume  */
  virtual TGeoVolume* FMD3Geometry(AliFMD3* d, 
				   TGeoVolume* innerTop, 
				   TGeoVolume* innerBot, 
				   TGeoVolume* outerTop,
				   TGeoVolume* outerBot);


  TArrayI     fActiveId;      //! Active volume ID's
  Bool_t      fDetailed;      // Whether to make a detailed simulation 
  Bool_t      fUseAssembly;   // Assembly volumes
  Int_t       fSectorOff;     // Sector offset in volume tree 
  Int_t       fModuleOff;     // Module offset in volume tree
  Int_t       fRingOff;       // Ring offset in the volume tree 
  Int_t       fDetectorOff;   // Detector offfset in the volume tree 

  TGeoMedium* fSi;	 //! Si Medium
  TGeoMedium* fC;	 //! C Medium
  TGeoMedium* fAl;	 //! Al Medium
  TGeoMedium* fPCB;	 //! PCB Medium
  TGeoMedium* fChip;	 //! Chip Medium
  TGeoMedium* fAir;	 //! Air Medium
  TGeoMedium* fPlastic;	 //! Plastic Medium
  TGeoMedium* fCopper;	 //! Copper Medium

  static const Char_t* fgkActiveName;	// Name of Active volumes
  static const Char_t* fgkSectorName;	// Name of Sector volumes
  static const Char_t* fgkStripName;	// Name of Strip volumes
  static const Char_t* fgkSensorName;	// Name of Sensor volumes
  static const Char_t* fgkPCBName;	// Name of PCB volumes
  static const Char_t* fgkCuName;	// Name of copper volumes
  static const Char_t* fgkChipName;	// Name of chip volumes
  static const Char_t* fgkLongLegName;	// Name of LongLeg volumes
  static const Char_t* fgkShortLegName;	// Name of ShortLeg volumes
  static const Char_t* fgkFrontVName;	// Name of Front volumes
  static const Char_t* fgkBackVName;	// Name of Back volumes
  static const Char_t* fgkRingTopName;	// Name of Top ring volumes
  static const Char_t* fgkRingBotName;	// Name of Bottom ring volumes
  static const Char_t* fgkHCName;	// Name of Honeycomb volumes
  static const Char_t* fgkIHCName;	// Name of Inner honeycomb volumes
  static const Char_t* fgkNoseName;	// Name of Nose volumes
  static const Char_t* fgkBackName;	// Name of Back volumes
  static const Char_t* fgkBeamName;	// Name of Beam volumes
  static const Char_t* fgkFlangeName;	// Name of Flange volumes
  static const Char_t* fgkFMDName;	// Name of Half FMD volumes

  ClassDef(AliFMDGeometryBuilder,1)
};

#endif
//____________________________________________________________________
//
// Local Variables:
//   mode: C++
// End:
//
// EOF
//

