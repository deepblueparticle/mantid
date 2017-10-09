#include "MantidAlgorithms/FindPeakBackground.h"
#include "MantidAPI/MatrixWorkspace.h"
#include "MantidAPI/TableRow.h"
#include "MantidAPI/WorkspaceFactory.h"
#include "MantidAPI/WorkspaceProperty.h"
#include "MantidAlgorithms/FindPeaks.h"
#include "MantidDataObjects/TableWorkspace.h"
#include "MantidDataObjects/Workspace2D.h"
#include "MantidKernel/ArrayProperty.h"
#include "MantidKernel/ListValidator.h"
#include "MantidKernel/Statistics.h"

#include <sstream>

using namespace Mantid;
using namespace Mantid::API;
using namespace Mantid::Kernel;
using namespace Mantid::DataObjects;
using namespace std;
using Mantid::HistogramData::HistogramX;
using Mantid::HistogramData::HistogramY;

namespace Mantid {
namespace Algorithms {

DECLARE_ALGORITHM(FindPeakBackground)

//----------------------------------------------------------------------------------------------
/** Define properties
  */
void FindPeakBackground::init() {
  declareProperty(Kernel::make_unique<WorkspaceProperty<MatrixWorkspace>>(
                      "InputWorkspace", "Anonymous", Direction::Input),
                  "Name of input MatrixWorkspace that contains peaks.");

  declareProperty("WorkspaceIndex", EMPTY_INT(),
                  "workspace indices to have peak and background separated. "
                  "No default is taken. ");

  declareProperty(
      "SigmaConstant", 1.0,
      "Multiplier of standard deviations of the variance for convergence of "
      "peak elimination.  Default is 1.0. ");

  declareProperty(Kernel::make_unique<ArrayProperty<double>>("FitWindow"),
                  "Optional: enter a comma-separated list of the minimum and "
                  "maximum X-positions of window to fit.  "
                  "The window is the same for all indices in workspace. The "
                  "length must be exactly two.");

  std::vector<std::string> bkgdtypes{"Flat", "Linear", "Quadratic"};
  declareProperty("BackgroundType", "Linear",
                  boost::make_shared<StringListValidator>(bkgdtypes),
                  "Type of Background.");

  // The found peak in a table
  declareProperty(
      Kernel::make_unique<WorkspaceProperty<API::ITableWorkspace>>(
          "OutputWorkspace", "", Direction::Output),
      "The name of the TableWorkspace in which to store the background found "
      "for each index.  "
      "Table contains the indices of the beginning and ending of peak "
      "and the estimated background coefficients for the constant, linear, and "
      "quadratic terms.");
}

void FindPeakBackground::findWindowIndex(size_t &l0, size_t &n) {
  auto &inpX = m_histogram->x();
  auto &inpY = m_histogram->y();
  size_t sizey = inpY.size(); // inpWS->y(inpwsindex).size();

  // determine the fit window with their index in X (or Y)
  n = sizey;
  l0 = 0;
  if (m_vecFitWindows.size() > 1) {
    Mantid::Algorithms::FindPeaks fp;
    l0 = fp.getIndex(inpX, m_vecFitWindows[0]);
    n = fp.getIndex(inpX, m_vecFitWindows[1]);
    if (n < sizey)
      n++;
  }
}

//----------------------------------------------------------------------------------------------
/** Execute body
  */
void FindPeakBackground::exec() {
  // Get input and validate
  processInputProperties();

  size_t l0, n;
  findWindowIndex(l0, n);

  // m_vecFitWindows won't be used again form this point till end.

  // Set up output table workspace
  createOutputWorkspaces();

  // 3. Get Y values
  Progress prog(this, 0.0, 1.0, 1);

  std::vector<size_t> peak_min_max_indexes;
  std::vector<double> bkgd3;
  int goodfit = findBackground(l0, n, peak_min_max_indexes, bkgd3);

  if (goodfit > 0) {
    size_t min_peak = peak_min_max_indexes[0];
    size_t max_peak = peak_min_max_indexes[1];
    double a0 = bkgd3[0];
    double a1 = bkgd3[1];
    double a2 = bkgd3[2];
    API::TableRow t = m_outPeakTableWS->getRow(0);
    t << static_cast<int>(m_inputWSIndex) << static_cast<int>(min_peak)
      << static_cast<int>(max_peak) << a0 << a1 << a2 << goodfit;
  }

  prog.report();

  // 4. Set the output
  setProperty("OutputWorkspace", m_outPeakTableWS);
}

int FindPeakBackground::findBackground(
    const size_t &l0, const size_t &n,
    std::vector<size_t> &peak_min_max_indexes, std::vector<double> &bkgd3) {
  auto &inpX = m_histogram->x();
  auto &inpY = m_histogram->y();
  size_t sizex = inpX.size(); // inpWS->x(inpwsindex).size();
  size_t sizey = inpY.size(); // inpWS->y(inpwsindex).size();

  int goodfit(0);

  // Find background
  double Ymean, Yvariance, Ysigma;
  MantidVec maskedY;
  auto in = std::min_element(inpY.cbegin(), inpY.cend());
  double bkg0 = inpY[in - inpY.begin()];
  for (size_t l = l0; l < n; ++l) {
    maskedY.push_back(inpY[l] - bkg0);
  }
  MantidVec mask(n - l0, 0.0);
  double xn = static_cast<double>(n - l0);
  if ((0. == xn) || (0. == xn - 1.0))
    throw std::runtime_error(
        "The number of Y values in the input workspace for the "
        "workspace index given, minus 'l0' or minus 'l0' minus 1, is 0. This "
        "will produce a "
        "divide-by-zero");
  do {
    Statistics stats = getStatistics(maskedY);
    Ymean = stats.mean;
    Yvariance = stats.standard_deviation * stats.standard_deviation;
    Ysigma = std::sqrt((moment4(maskedY, static_cast<size_t>(xn), Ymean) -
                        (xn - 3.0) / (xn - 1.0) * Yvariance) /
                       xn);
    MantidVec::const_iterator it =
        std::max_element(maskedY.begin(), maskedY.end());
    const size_t pos = it - maskedY.begin();
    maskedY[pos] = 0;
    mask[pos] = 1.0;
  } while (std::abs(Ymean - Yvariance) > m_sigmaConstant * Ysigma);

  if (n - l0 > 5) {
    // remove single outliers
    if (mask[1] == mask[2] && mask[2] == mask[3])
      mask[0] = mask[1];
    if (mask[0] == mask[2] && mask[2] == mask[3])
      mask[1] = mask[2];
    for (size_t l = 2; l < n - l0 - 3; ++l) {
      if (mask[l - 1] == mask[l + 1] &&
          (mask[l - 1] == mask[l - 2] || mask[l + 1] == mask[l + 2])) {
        mask[l] = mask[l + 1];
      }
    }
    if (mask[n - l0 - 2] == mask[n - l0 - 3] &&
        mask[n - l0 - 3] == mask[n - l0 - 4])
      mask[n - l0 - 1] = mask[n - l0 - 2];
    if (mask[n - l0 - 1] == mask[n - l0 - 3] &&
        mask[n - l0 - 3] == mask[n - l0 - 4])
      mask[n - l0 - 2] = mask[n - l0 - 1];

    // mask regions not connected to largest region
    // for loop can start > 1 for multiple peaks
    vector<cont_peak> peaks;
    if (mask[0] == 1) {
      peaks.emplace_back();
      peaks.back().start = l0;
    }
    for (size_t l = 1; l < n - l0; ++l) {
      if (mask[l] != mask[l - 1] && mask[l] == 1) {
        peaks.emplace_back();
        peaks.back().start = l + l0;
      } else if (!peaks.empty()) {
        size_t ipeak = peaks.size() - 1;
        if (mask[l] != mask[l - 1] && mask[l] == 0) {
          peaks[ipeak].stop = l + l0;
        }
        if (inpY[l + l0] > peaks[ipeak].maxY)
          peaks[ipeak].maxY = inpY[l + l0];
      }
    }
    size_t min_peak, max_peak;
    double a0 = 0., a1 = 0., a2 = 0.;
    if (!peaks.empty()) {
      g_log.debug() << "Peaks' size = " << peaks.size()
                    << " -> esitmate background. \n";
      if (peaks.back().stop == 0)
        peaks.back().stop = n - 1;
      std::sort(peaks.begin(), peaks.end(), by_len());

      // save endpoints
      min_peak = peaks[0].start;
      // extra point for histogram input
      max_peak = peaks[0].stop + sizex - sizey;
      goodfit = 1;
    } else {
      // assume the whole thing is background
      g_log.debug("Peaks' size = 0 -> whole region assumed background");
      min_peak = n;
      max_peak = l0;

      goodfit = 2;
    }

    estimateBackground(inpX, inpY, l0, n, min_peak, max_peak, (!peaks.empty()),
                       a0, a1, a2);

    // Add a new row
    peak_min_max_indexes.resize(2);
    peak_min_max_indexes[0] = min_peak;
    peak_min_max_indexes[1] = max_peak;

    bkgd3.resize(3);
    bkgd3[0] = a0;
    bkgd3[1] = a1;
    bkgd3[2] = a2;
  }

  return goodfit;
}

//----------------------------------------------------------------------------------------------
/** Estimate background
* @param X :: vec for X
* @param Y :: vec for Y
* @param i_min :: index of minimum in X to estimate background
* @param i_max :: index of maximum in X to estimate background
* @param p_min :: index of peak min in X to estimate background
* @param p_max :: index of peak max in X to estimate background
* @param hasPeak :: ban data in the peak range
* @param out_bg0 :: interception
* @param out_bg1 :: slope
* @param out_bg2 :: a2 = 0
*/
void FindPeakBackground::estimateBackground(
    const HistogramX &X, const HistogramY &Y, const size_t i_min,
    const size_t i_max, const size_t p_min, const size_t p_max,
    const bool hasPeak, double &out_bg0, double &out_bg1, double &out_bg2) {
  // Validate input
  if (i_min >= i_max)
    throw std::runtime_error("i_min cannot larger or equal to i_max");
  if ((hasPeak) && (p_min >= p_max))
    throw std::runtime_error("p_min cannot larger or equal to p_max");

  // set all parameters to zero
  out_bg0 = 0.;
  out_bg1 = 0.;
  out_bg2 = 0.;

  // accumulate sum
  double sum = 0.0;
  double sumX = 0.0;
  double sumY = 0.0;
  double sumX2 = 0.0;
  double sumXY = 0.0;
  double sumX2Y = 0.0;
  double sumX3 = 0.0;
  double sumX4 = 0.0;
  for (size_t i = i_min; i < i_max; ++i) {
    if (i >= p_min && i < p_max)
      continue;
    sum += 1.0;
    sumX += X[i];
    sumX2 += X[i] * X[i];
    sumY += Y[i];
    sumXY += X[i] * Y[i];
    sumX2Y += X[i] * X[i] * Y[i];
    sumX3 += X[i] * X[i] * X[i];
    sumX4 += X[i] * X[i] * X[i] * X[i];
  }

  // Estimate flat background
  double bg0_flat = 0.;
  if (sum != 0.)
    bg0_flat = sumY / sum;

  // Estimate linear - use Cramer's rule for 2 x 2 matrix
  double bg0_linear = 0.;
  double bg1_linear = 0.;
  double determinant = sum * sumX2 - sumX * sumX;
  if (determinant != 0) {
    bg0_linear = (sumY * sumX2 - sumX * sumXY) / determinant;
    bg1_linear = (sum * sumXY - sumY * sumX) / determinant;
  }

  // Estimate quadratic - use Cramer's rule for 3 x 3 matrix

  // | a b c |
  // | d e f |
  // | g h i |
  // 3 x 3 determinate:  aei+bfg+cdh-ceg-bdi-afh

  double bg0_quadratic = 0.;
  double bg1_quadratic = 0.;
  double bg2_quadratic = 0.;
  determinant = sum * sumX2 * sumX4 + sumX * sumX3 * sumX2 +
                sumX2 * sumX * sumX3 - sumX2 * sumX2 * sumX2 -
                sumX * sumX * sumX4 - sum * sumX3 * sumX3;
  if (determinant != 0) {
    bg0_quadratic =
        (sumY * sumX2 * sumX4 + sumX * sumX3 * sumX2Y + sumX2 * sumXY * sumX3 -
         sumX2 * sumX2 * sumX2Y - sumX * sumXY * sumX4 - sumY * sumX3 * sumX3) /
        determinant;
    bg1_quadratic =
        (sum * sumXY * sumX4 + sumY * sumX3 * sumX2 + sumX2 * sumX * sumX2Y -
         sumX2 * sumXY * sumX2 - sumY * sumX * sumX4 - sum * sumX3 * sumX2Y) /
        determinant;
    bg2_quadratic =
        (sum * sumX2 * sumX2Y + sumX * sumXY * sumX2 + sumY * sumX * sumX3 -
         sumY * sumX2 * sumX2 - sumX * sumX * sumX2Y - sum * sumXY * sumX3) /
        determinant;
  }

  double chisq_flat = 0.;
  double chisq_linear = 0.;
  double chisq_quadratic = 0.;
  if (sum != 0) {
    double num_points = 0.;
    // calculate the chisq - not normalized by the number of points
    for (size_t i = i_min; i < i_max; ++i) {
      if (i >= p_min && i < p_max)
        continue;

      num_points += 1.;

      // accumulate for flat
      chisq_flat += (bg0_flat - Y[i]) * (bg0_flat - Y[i]);

      // accumulate for linear
      double temp = bg0_linear + bg1_linear * X[i] - Y[i];
      chisq_linear += (temp * temp);

      // accumulate for quadratic
      temp = bg0_quadratic + bg1_quadratic * X[i] +
             bg2_quadratic * X[i] * X[i] - Y[i];
      chisq_quadratic += (temp * temp);
    }

    // convert to <reduced chisq> = chisq / (<number points> - <number
    // parameters>)
    chisq_flat = chisq_flat / (num_points - 1.);
    chisq_linear = chisq_linear / (num_points - 2.);
    chisq_quadratic = chisq_quadratic / (num_points - 3.);
  }
  const double INVALID_CHISQ(1.e10); // big invalid value
  if (m_backgroundType == "Flat") {
    chisq_linear = INVALID_CHISQ;
    chisq_quadratic = INVALID_CHISQ;
  } else if (m_backgroundType == "Linear") {
    chisq_quadratic = INVALID_CHISQ;
  }

  g_log.debug() << "flat: " << bg0_flat << " + " << 0. << "x + " << 0.
                << "x^2 reduced chisq=" << chisq_flat << "\n";
  g_log.debug() << "line: " << bg0_linear << " + " << bg1_linear << "x + " << 0.
                << "x^2  reduced chisq=" << chisq_linear << "\n";
  g_log.debug() << "quad: " << bg0_quadratic << " + " << bg1_quadratic << "x + "
                << bg2_quadratic << "x^2  reduced chisq=" << chisq_quadratic
                << "\n";

  // choose the right background function to apply
  if ((chisq_quadratic < chisq_flat) && (chisq_quadratic < chisq_linear)) {
    out_bg0 = bg0_quadratic;
    out_bg1 = bg1_quadratic;
    out_bg2 = bg2_quadratic;
  } else if ((chisq_linear < chisq_flat) && (chisq_linear < chisq_quadratic)) {
    out_bg0 = bg0_linear;
    out_bg1 = bg1_linear;
  } else {
    out_bg0 = bg0_flat;
  }

  g_log.information() << "Estimated background: A0 = " << out_bg0
                      << ", A1 = " << out_bg1 << ", A2 = " << out_bg2 << "\n";
}
//----------------------------------------------------------------------------------------------
/** Calculate 4th moment
* @param X :: vec for X
* @param n :: length of vector
* @param mean :: mean of X
*/
double FindPeakBackground::moment4(MantidVec &X, size_t n, double mean) {
  double sum = 0.0;
  for (size_t i = 0; i < n; ++i) {
    sum += (X[i] - mean) * (X[i] - mean) * (X[i] - mean) * (X[i] - mean);
  }
  sum /= static_cast<double>(n);
  return sum;
}

//----------------------------------------------------------------------------------------------
void FindPeakBackground::processInputProperties() {
  // process input workspace and workspace index
  MatrixWorkspace_const_sptr inpWS = getProperty("InputWorkspace");

  int inpwsindex = getProperty("WorkspaceIndex");
  if (isEmpty(inpwsindex)) {
    // Default
    if (inpWS->getNumberHistograms() == 1) {
      inpwsindex = 0;
    } else {
      throw runtime_error("WorkspaceIndex must be given. ");
    }
  } else if (inpwsindex < 0 ||
             inpwsindex >= static_cast<int>(inpWS->getNumberHistograms())) {
    stringstream errss;
    errss << "Input workspace " << inpWS->getName() << " has "
          << inpWS->getNumberHistograms() << " spectra.  Input workspace index "
          << inpwsindex << " is out of boundary. ";
    throw runtime_error(errss.str());
  }
  m_inputWSIndex = static_cast<size_t>(inpwsindex);

  setHistogram(inpWS->histogram(inpwsindex));

  std::vector<double> fitwindow = getProperty("FitWindow");
  setFitWindow(fitwindow);

  // background
  m_backgroundType = getPropertyValue("BackgroundType");
  size_t bkgdorder = 0;
  if (m_backgroundType == "Linear")
    bkgdorder = 1;
  else if (m_backgroundType == "Quadratic")
    bkgdorder = 2;
  setBackgroundOrder(bkgdorder);

  // sigma constant
  double k = getProperty("SigmaConstant");
  setSigma(k);
}

/// set histogram data to find background
void FindPeakBackground::setHistogram(
    const HistogramData::Histogram &histogram) {
  m_histogram = boost::make_shared<HistogramData::Histogram>(histogram);
}

/// set sigma constant
void FindPeakBackground::setSigma(const double &sigma) {
  m_sigmaConstant = sigma;
}

/// set background order
void FindPeakBackground::setBackgroundOrder(size_t order) {
  m_backgroundOrder = order;
}

//----------------------------------------------------------------------------------------------
/** set fit window
 * @brief FindPeakBackground::setFitWindow
 * @param fitwindow
 */
void FindPeakBackground::setFitWindow(const std::vector<double> &fitwindow) {
  // validate input
  if (m_vecFitWindows.size() == 0) {
    m_vecFitWindows.resize(2);
    m_vecFitWindows[0] = m_histogram->y().front();
    m_vecFitWindows[1] = m_histogram->y().back();
  } else if (m_vecFitWindows.size() != 2 ||
             m_vecFitWindows[0] >= m_vecFitWindows[1]) {
    throw std::invalid_argument("Fit window has either wrong item number or "
                                "window value is not in ascending order.");
  }

  // m_vecFitWindows.resize(2);
  // copy the input to class variable
  m_vecFitWindows = fitwindow;
}

//----------------------------------------------------------------------------------------------
/**
 * @brief FindPeakBackground::createOutputWorkspaces
 */
void FindPeakBackground::createOutputWorkspaces() {
  // Set up output table workspace
  m_outPeakTableWS = WorkspaceFactory::Instance().createTable("TableWorkspace");
  m_outPeakTableWS->addColumn("int", "wksp_index");
  m_outPeakTableWS->addColumn("int", "peak_min_index");
  m_outPeakTableWS->addColumn("int", "peak_max_index");
  m_outPeakTableWS->addColumn("double", "bkg0");
  m_outPeakTableWS->addColumn("double", "bkg1");
  m_outPeakTableWS->addColumn("double", "bkg2");
  m_outPeakTableWS->addColumn("int", "GoodFit");

  m_outPeakTableWS->appendRow();
}

} // namespace Algorithms
} // namespace Mantid
