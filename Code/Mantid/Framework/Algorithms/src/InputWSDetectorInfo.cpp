#include "MantidAlgorithms/InputWSDetectorInfo.h"
#include "MantidKernel/Exception.h"
#include <vector>
#include <string>
#include <sstream>


namespace Mantid
{
namespace Algorithms
{
using namespace API;
/** Copies pointers to the Instrument and ParameterMap to data members
* @param input :: A pointer to a workspace that contains instrument information
* @throw invalid_argument if there is no instrument information in the workspace
*/
InputWSDetectorInfo::InputWSDetectorInfo(API::MatrixWorkspace_sptr input) :
  m_Input(input), m_Pmap(NULL)
{
  // first something that points to the detectors
  m_WInstru = input->getBaseInstrument();
  m_RInstru = input->getInstrument();

  if ( !m_RInstru || !m_WInstru )
  {
    throw std::invalid_argument("Error loading instrument or base instrument data from the input workspace.");
  }
  // the space that contains which are masked
  m_Pmap = &m_Input->instrumentParameters();
}
/** To find out if there is a detector in a spectrum that is masked
*  @param SpecIndex :: Workspace index
*  @return True if there is a masked detector, otherwise false
*/
bool InputWSDetectorInfo::aDetecIsMaskedinSpec(specid_t SpecIndex) const
{
  const std::set<detid_t> & dets = m_Input->getSpectrum(SpecIndex)->getDetectorIDs();
  // we are going to go through all of them, if you know this is not neccessary then change it
  std::set<detid_t>::const_iterator it;
  for ( it = dets.begin(); it != dets.end(); ++it)
  {
    if (m_RInstru->getDetector(*it).get()->isMasked()) return true;
  }
  // we didn't find any that were masked
  return false;
}
/** Masks all the detectors that contribute to the specified spectrum
*  @param SpecIndex :: Workspace index to mask
*  @return True if there is a masked detector, otherwise false
*  @throw IndexError if the spectra index number isn't in the workspace
*  @throw NotFoundError if we can't get a pointer to the detector that the detector map says is linked to the spectrum
*/
void InputWSDetectorInfo::maskAllDetectorsInSpec(specid_t SpecIndex)
{
  const std::set<detid_t> & dets = m_Input->getSpectrum(SpecIndex)->getDetectorIDs();
  // there may be many detectors that are responsible for the spectrum, loop through them
  std::set<detid_t>::const_iterator it;
  int64_t missing = 0;
  for ( it = dets.begin(); it != dets.end(); ++it)
  {
    Geometry::Detector* det =
      dynamic_cast<Geometry::Detector*>( m_WInstru->getDetector(*it).get() );
    if ( det )
    {
      try{
        m_Pmap->addBool(det, "masked", true);
      }
      catch (Kernel::Exception::NotFoundError e)
      {// I believe this happens if the workspace doesn't contain some information, carry on the best we can and we'll notify people of the problem at the end
        missing ++;
      }
    }
  }
  if ( missing > 0 )
  {
    std::ostringstream missingReport;
    missingReport << "Information missing in the workspace for " << missing
        << " detector" << (missing > 1 ? "s" : "");
    throw Kernel::Exception::NotFoundError(
      std::string("InputWSDetectorInfo::maskAllDetectorsInSpec() ") + missingReport.str(),
      SpecIndex);
  }
}

}// end namespace Algorithms
}// end namespace Mantid
