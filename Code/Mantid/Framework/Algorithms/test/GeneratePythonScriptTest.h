#ifndef MANTID_ALGORITHMS_GENERATEPYTHONSCRIPTTEST_H_
#define MANTID_ALGORITHMS_GENERATEPYTHONSCRIPTTEST_H_

#include <cxxtest/TestSuite.h>
#include "MantidKernel/Timer.h"
#include "MantidKernel/System.h"
#include <iostream>
#include <fstream>
#include <iomanip>

#include "MantidAlgorithms/GeneratePythonScript.h"
#include "MantidDataHandling/Load.h"
#include "MantidAPI/AlgorithmManager.h"

using namespace Mantid;
using namespace Mantid::Algorithms;
using namespace Mantid::API;

class GeneratePythonScriptTest : public CxxTest::TestSuite
{
public:
    // This pair of boilerplate methods prevent the suite being created statically
    // This means the constructor isn't called when running other tests
    // TODO - Needed?
    static GeneratePythonScriptTest *createSuite() { return new GeneratePythonScriptTest(); }
    static void destroySuite( GeneratePythonScriptTest *suite ) { delete suite; }


    void test_Init()
    {
        GeneratePythonScript alg;
        TS_ASSERT_THROWS_NOTHING( alg.initialize() )
        TS_ASSERT( alg.isInitialized() )
    }

    void test_exec()
    {
        std::string result[] = {
            "######################################################################",
            "#Python Script Generated by Algorithm History Display",
            "######################################################################",
            "LoadRaw(Filename='G:/Spencer/Science/Raw/irs26173.raw',OutputWorkspace='IPG',SpectrumMin='3',SpectrumMax='53')",
            "ConvertUnits(InputWorkspace='IPG',OutputWorkspace='Spec',Target='Wavelength')",
            "LoadRaw(Filename='G:/Spencer/Science/Raw/irs26173.raw',OutputWorkspace='Mon_in',SpectrumMax='1')",
            "Unwrap(InputWorkspace='Mon_in',OutputWorkspace='Mon',LRef='37.86')",
            "RemoveBins(InputWorkspace='Mon',OutputWorkspace='Mon',XMin='6.14600063416',XMax='6.14800063416',Interpolation='Linear')",
            "FFTSmooth(InputWorkspace='Mon',OutputWorkspace='Mon')",
            "RebinToWorkspace(WorkspaceToRebin='Spec',WorkspaceToMatch='Mon',OutputWorkspace='Spec')",
            "LoadRaw(Filename='G:/Spencer/Science/Raw/irs26173.raw',OutputWorkspace='Mon_in',SpectrumMax='1')",
            "Unwrap(InputWorkspace='Mon_in',OutputWorkspace='Mon',LRef='37.86')",
            "RemoveBins(InputWorkspace='Mon',OutputWorkspace='Mon',XMin='6.14600063416',XMax='6.14800063416',Interpolation='Linear')",
            "FFTSmooth(InputWorkspace='Mon',OutputWorkspace='Mon')",
            "Divide(LHSWorkspace='Spec',RHSWorkspace='Mon',OutputWorkspace='Spec')",
            "ConvertUnits(InputWorkspace='Spec',OutputWorkspace='Spec',Target='DeltaE',EMode='Indirect',EFixed='1.84')",
            "GroupDetectors(InputWorkspace='Spec',OutputWorkspace='IPG_3',MapFile='G:/Spencer/Science/Mantid/IRIS/PG1op3.map')",
            ""
        };
        
        // Load test file into workspace
        Mantid::DataHandling::Load loader;
        loader.initialize();
        loader.setPropertyValue("Filename", "IRS26173_ipg.nxs");
        loader.setPropertyValue("OutputWorkspace","LoadedWorkspace");
        loader.setRethrows(true);

        TS_ASSERT_THROWS_NOTHING(loader.execute());
        TS_ASSERT_EQUALS(loader.isExecuted(), true);

        MatrixWorkspace_sptr ws = boost::dynamic_pointer_cast<MatrixWorkspace>(AnalysisDataService::Instance().retrieve("LoadedWorkspace"));

        TS_ASSERT(NULL != ws);

        // Set up and execute the algorithm.
        GeneratePythonScript alg;
        TS_ASSERT_THROWS_NOTHING( alg.initialize() );
        TS_ASSERT( alg.isInitialized() );
        TS_ASSERT_THROWS_NOTHING( alg.setPropertyValue("InputWorkspace", ws->getName()) ); 
        TS_ASSERT_THROWS_NOTHING( alg.setPropertyValue("Filename", "GeneratePythonScriptTest.py") );
        TS_ASSERT_THROWS_NOTHING( alg.execute(); );
        TS_ASSERT( alg.isExecuted() );

        // Read in the file, and parse each line into a vector of strings.
        std::string filename = alg.getProperty("Filename");
        std::ifstream file(filename.c_str(), std::ifstream::in);
        std::vector<std::string> lines;

        while (file.good())
        {
            char testArray[256];
            file.getline(testArray, 256);
            std::string line = testArray;
            lines.push_back(line);
        }

        std::vector<std::string>::iterator lineIter = lines.begin();

        int lineCount = 0;

        for( ; lineIter != lines.end(); ++lineIter) 
        {
            TS_ASSERT_EQUALS((*lineIter),result[lineCount]);
            lineCount++;
        }

        // Remove workspace from the data service.
        // AnalysisDataService::Instance().remove(outWSName);
    }
};


#endif /* MANTID_ALGORITHMS_GENERATEPYTHONSCRIPTTEST_H_ */

