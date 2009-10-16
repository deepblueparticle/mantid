#ifndef MANTID_ALGORITHMS_REBINPRESERVEVALUE_H_
#define MANTID_ALGORITHMS_REBINPRESERVEVALUE_H_

//----------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------
#include "MantidAPI/Algorithm.h"

namespace Mantid
{
namespace Algorithms
{
/** A specialised rebin algorithm, created for the SANS instruments, in which a an
    input workspace with a single bin is taken and, for each spectrum, the value is
    distributed into the new bins according to the following scheme:
    <UL>
    <LI> No overlap between old and new bins: 0 (as regular rebin) </LI>
    <LI> New bin entirely within old bin range: Full value of input bin copied </LI>
    <LI> New bin partially covered by old bin: Old value scaled according to fraction of new bin covered by old bin </LI>
    </UL>

    Required Properties:
    <UL>
    <LI> InputWorkspace  - The name of the Workspace2D to take as input </LI>
    <LI> OutputWorkspace - The name of the workspace in which to store the result </LI>
    <LI> RebinParameters - The new bin boundaries in the form X1,deltaX1,X2,deltaX2,X3,... </LI>
    </UL>

    The algorithms used in the VectorHelper::rebin() and VectorHelper::createAxisFromRebinParams()
    are based on the algorithms used in OPENGENIE /src/transform_utils.cxx (Freddie Akeroyd, ISIS)
    When calculating the bin boundaries, if the last bin ends up with a width being less than 25%
    of the penultimate one, then the two are combined. 

    @author Roman Tolchenov, Tessella Support Services plc
    @date 28/01/2009

    Copyright &copy; 2009 STFC Rutherford Appleton Laboratory

    This file is part of Mantid.

    Mantid is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    Mantid is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    File change history is stored at: <https://svn.mantidproject.org/mantid/trunk/Code/Mantid>
    Code Documentation is available at: <http://doxygen.mantidproject.org>
    */
class DLLExport RebinPreserveValue : public API::Algorithm
{
public:
  /// Default constructor
  RebinPreserveValue() : API::Algorithm() {};
  /// Destructor
  virtual ~RebinPreserveValue() {};
  /// Algorithm's name for identification overriding a virtual method
  virtual const std::string name() const { return "RebinPreserveValue";}
  /// Algorithm's version for identification overriding a virtual method
  virtual const int version() const { return 1;}
  /// Algorithm's category for identification overriding a virtual method
  virtual const std::string category() const { return "Rebin\\Specialized";}

private:
  // Overridden Algorithm methods
  void init();
  void exec();
};

} // namespace Algorithms
} // namespace Mantid

#endif /*MANTID_ALGORITHMS_REBINPRESERVEVALUE_H_*/
