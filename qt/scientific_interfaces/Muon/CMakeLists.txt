set ( SRC_FILES
	ALCBaselineModellingModel.cpp
	ALCBaselineModellingPresenter.cpp
	ALCBaselineModellingView.cpp
	ALCDataLoadingPresenter.cpp
	ALCDataLoadingView.cpp
	ALCInterface.cpp
	ALCLatestFileFinder.cpp
	ALCPeakFittingModel.cpp
	ALCPeakFittingPresenter.cpp
	ALCPeakFittingView.cpp
	IO_MuonGrouping.cpp
	MuonAnalysis.cpp
	MuonAnalysisDataLoader.cpp
	MuonAnalysisFitDataPresenter.cpp
	MuonAnalysisFitDataTab.cpp
	MuonAnalysisFitFunctionPresenter.cpp
	MuonAnalysisHelper.cpp
	MuonAnalysisOptionTab.cpp
	MuonAnalysisResultTableCreator.cpp
	MuonAnalysisResultTableTab.cpp
	MuonSequentialFitDialog.cpp
)

# Include files aren't required, but this makes them appear in Visual Studio
# IMPORTANT: Include files are required in the MOC_FILES set. Scroll down to find it.
set ( INC_FILES
	ALCBaselineModellingModel.h
	ALCBaselineModellingPresenter.h
	ALCBaselineModellingView.h
	ALCDataLoadingPresenter.h
	ALCDataLoadingView.h
	ALCInterface.h
	ALCLatestFileFinder.h
	ALCPeakFittingModel.h
	ALCPeakFittingPresenter.h
	ALCPeakFittingView.h
        DllConfig.h
	IALCBaselineModellingModel.h
	IALCBaselineModellingView.h
	IALCDataLoadingView.h
	IALCPeakFittingModel.h
	IALCPeakFittingView.h
	IO_MuonGrouping.h
	MuonAnalysis.h
	MuonAnalysisDataLoader.h
	MuonAnalysisFitDataPresenter.h
	MuonAnalysisFitDataTab.h
	MuonAnalysisFitFunctionPresenter.h
	MuonAnalysisHelper.h
	MuonAnalysisOptionTab.h
	MuonAnalysisResultTableCreator.h
	MuonAnalysisResultTableTab.h
	MuonSequentialFitDialog.h
)

set ( MOC_FILES
    ALCBaselineModellingPresenter.h
    ALCBaselineModellingView.h
    ALCDataLoadingPresenter.h
    ALCInterface.h
    ALCPeakFittingPresenter.h
    IALCBaselineModellingView.h
    IALCBaselineModellingModel.h
    IALCDataLoadingView.h
    IALCPeakFittingView.h
    IALCPeakFittingModel.h
    MuonAnalysis.h
    MuonAnalysisFitDataPresenter.h
    MuonAnalysisFitDataTab.h
    MuonAnalysisFitFunctionPresenter.h
    MuonAnalysisHelper.h
    MuonAnalysisOptionTab.h
    MuonAnalysisResultTableTab.h
    MuonSequentialFitDialog.h
)

set ( UI_FILES
    ALCBaselineModellingView.ui
    ALCDataLoadingView.ui
    ALCInterface.ui
    ALCPeakFittingView.ui
    MuonAnalysis.ui
    MuonSequentialFitDialog.ui
)

mtd_add_qt_library (TARGET_NAME MantidScientificInterfacesMuon
  QT_VERSION 4
  SRC ${SRC_FILES}
  MOC ${MOC_FILES}
  NOMOC ${INC_FILES}
  UI ${UI_FILES}
  DEFS IN_MANTIDQT_MUONINTERFACE
  PRECOMPILED PrecompiledHeader.h
  LINK_LIBS
    ${TCMALLOC_LIBRARIES_LINKTIME}
    ${CORE_MANTIDLIBS}
    ${POCO_LIBRARIES}
    ${Boost_LIBRARIES}
    ${JSONCPP_LIBRARIES}
  QT4_LINK_LIBS
      Qwt5
  MTD_QT_LINK_LIBS
    MantidQtWidgetsCommon
    MantidQtWidgetsLegacyQwt
    MantidScientificInterfacesMultiDatasetFit
  INSTALL_DIR_BASE
    ${PLUGINS_DIR}
  OSX_INSTALL_RPATH
    @loader_path/../../Contents/MacOS
    @loader_path/../../plugins/qt4
  LINUX_INSTALL_RPATH
    "\$ORIGIN/../../${LIB_DIR};\$ORIGIN/../../plugins/qt4/"
)
