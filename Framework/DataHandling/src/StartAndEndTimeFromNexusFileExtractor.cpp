#include "MantidDataHandling/StartAndEndTimeFromNexusFileExtractor.h"
#include "MantidAPI/FileFinder.h"
#include "MantidDataHandling/LoadNexus.h"
#include "MantidKernel/Exception.h"
#include "MantidKernel/Logger.h"
#include "MantidNexus/NexusClasses.h"
#include "MantidNexus/NexusFileIO.h"

#include <vector>

namespace {
Mantid::Kernel::Logger g_log("StartAndEndTimeFromNexusFileExtractor");
}

namespace Mantid {
namespace DataHandling {

/**
 * Gets the start time from the nexus file
 * @param filename: the file name
 * @return the start time
 * @throws if the the start time cannot be extracted
 */
Mantid::Kernel::DateAndTime
StartAndEndTimeFromNexusFileExtractor::extractStartTime(
    std::string filename) const {
  return extractDateAndTime(TimeType::StartTime, filename);
}

/**
 * Gets the start time from the nexus file
 * @param filename: the file name
 * @return the start time
 * @throws if the the start time cannot be extracted
 */
Mantid::Kernel::DateAndTime
StartAndEndTimeFromNexusFileExtractor::extractEndTime(
    std::string filename) const {
  return extractDateAndTime(TimeType::EndTime, filename);
}

/**
 * Get the nexus file type
 * @param filename
 * @return
 */
StartAndEndTimeFromNexusFileExtractor::NexusType
StartAndEndTimeFromNexusFileExtractor::whichNexusType(
    std::string filename) const {
  std::vector<std::string> entryName;
  std::vector<std::string> definition;
  auto count =
      Mantid::NeXus::getNexusEntryTypes(filename, entryName, definition);

  // Issues with the file
  if (count <= -1) {
    g_log.error("Error reading file " + filename);
    throw Mantid::Kernel::Exception::FileError("Unable to read data in File:",
                                               filename);
  } else if (count == 0) {
    g_log.error("Error no entries found in " + filename);
    throw Mantid::Kernel::Exception::FileError("Error no entries found in ",
                                               filename);
  }

  NexusType nexusType;
  if (definition[0] == LoadNexus::muonTD ||
      definition[0] == LoadNexus::pulsedTD) {
    nexusType = NexusType::Muon;
  } else if (entryName[0] == "mantid_workspace_1") {
    nexusType = NexusType::Processed;

  } else if (entryName[0] == "raw_data_1") {
    nexusType = NexusType::ISIS;
  } else {
    Mantid::NeXus::NXRoot root(filename);
    Mantid::NeXus::NXEntry entry = root.openEntry(root.groups().front().nxname);
    try {
      Mantid::NeXus::NXChar nxc =
          entry.openNXChar("instrument/SNSdetector_calibration_id");
    } catch (...) {
      g_log.error("File " + filename +
                  " is a currently unsupported type of NeXus file");
      throw Mantid::Kernel::Exception::FileError("Unable to read File:",
                                                 filename);
    }
    nexusType = NexusType::TofRaw;
  }

  return nexusType;
}

Mantid::Kernel::DateAndTime
StartAndEndTimeFromNexusFileExtractor::extractDateAndTime(
    TimeType type, std::string filename) const {
  auto fullFileName = Mantid::API::FileFinder::Instance().getFullPath(filename);
  // Figure out the type of the Nexus file. We need to handle them individually
  // since they store the datetime differently
  auto nexusType = whichNexusType(fullFileName);
  Mantid::Kernel::DateAndTime dateAndTime;

  switch (nexusType) {
  case NexusType::Muon:
    dateAndTime = handleMuonNexusFile(type, fullFileName);
    break;
  case NexusType::ISIS:
    dateAndTime = handleISISNexusFile(type, fullFileName);
    break;
  case NexusType::Processed:
    dateAndTime = handleProcessedNexusFile(type, fullFileName);
    break;
  case NexusType::TofRaw:
    dateAndTime = handleTofRawNexusFile(type, fullFileName);
    break;
  default:
    throw std::runtime_error(
        "Unkown Nexus format. Not able to extract a date and time.");
  };

  return dateAndTime;
}

Mantid::Kernel::DateAndTime
StartAndEndTimeFromNexusFileExtractor::handleMuonNexusFile(
    TimeType type, std::string filename) const {
  Mantid::NeXus::NXRoot root(filename);
  if (type == TimeType::StartTime) {
    return Mantid::Kernel::DateAndTime(root.getString("run/start_time"));
  } else {
    return Mantid::Kernel::DateAndTime(root.getString("run/stop_time"));
  }
}

Mantid::Kernel::DateAndTime
StartAndEndTimeFromNexusFileExtractor::handleProcessedNexusFile(
    TimeType type, std::string filename) const {
  Mantid::NeXus::NXRoot root(filename);
  if (type == TimeType::StartTime) {
    return Mantid::Kernel::DateAndTime(
        root.getString("mantid_workspace_1/logs/run_start/value"));
  } else {
    return Mantid::Kernel::DateAndTime(
        root.getString("mantid_workspace_1/logs/run_end/value"));
  }
}

Mantid::Kernel::DateAndTime
StartAndEndTimeFromNexusFileExtractor::handleISISNexusFile(
    TimeType type, std::string filename) const {
  Mantid::NeXus::NXRoot root(filename);
  if (type == TimeType::StartTime) {
    return Mantid::Kernel::DateAndTime(root.getString("raw_data_1/start_time"));
  } else {
    return Mantid::Kernel::DateAndTime(root.getString("raw_data_1/end_time"));
  }
}

Mantid::Kernel::DateAndTime
StartAndEndTimeFromNexusFileExtractor::handleTofRawNexusFile(
    TimeType type, std::string filename) const {
  Mantid::NeXus::NXRoot root(filename);
  if (type == TimeType::StartTime) {
    return Mantid::Kernel::DateAndTime(root.getString("entry/start_time"));
  } else {
    return Mantid::Kernel::DateAndTime(root.getString("entry/end_time"));
  }
}

} // namespace DataHandling
} // namespace Mantid
