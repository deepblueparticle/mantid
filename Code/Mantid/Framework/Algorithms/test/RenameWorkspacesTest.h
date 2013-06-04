#ifndef RENAMEWORKSPACESTEST_H_
#define RENAMEWORKSPACESTEST_H_

#include <cxxtest/TestSuite.h>

#include "MantidTestHelpers/WorkspaceCreationHelper.h"
#include "MantidAlgorithms/RenameWorkspaces.h"
#include "MantidAPI/AnalysisDataService.h"

using namespace Mantid::API;
using namespace Mantid::Kernel;

class RenameWorkspacesTest: public CxxTest::TestSuite
{
public:
  void testName()
  {
    TS_ASSERT_EQUALS( alg.name(), "RenameWorkspaces");
  }

  void testVersion()
  {
    TS_ASSERT_EQUALS( alg.version(), 1);
  }

  void testInit()
  {
    Mantid::Algorithms::RenameWorkspaces alg2;
    TS_ASSERT_THROWS_NOTHING( alg2.initialize());
    TS_ASSERT( alg2.isInitialized());

    const std::vector<Property*> props = alg2.getProperties();
    TS_ASSERT_EQUALS( props.size(), 4);

    TS_ASSERT_EQUALS( props[0]->name(), "InputWorkspaces");
    TS_ASSERT( props[0]->isDefault());

    TS_ASSERT_EQUALS( props[1]->name(), "WorkspaceNames");
    TS_ASSERT( props[1]->isDefault());

    TS_ASSERT_EQUALS( props[2]->name(), "Prefix");
    TS_ASSERT( props[2]->isDefault());

  }

  void testExec()
  {
    MatrixWorkspace_sptr inputWS1 = createWorkspace();
    MatrixWorkspace_sptr inputWS2 = createWorkspace();
    AnalysisDataService::Instance().add("InputWS1", inputWS1);
    AnalysisDataService::Instance().add("InputWS2", inputWS2);

    Mantid::Algorithms::RenameWorkspaces alg3;
    alg3.setRethrows( true ); // Ensure exceptions are thrown to this test
    alg3.initialize();
    TS_ASSERT_THROWS_NOTHING( alg3.setPropertyValue("InputWorkspaces","InputWS1, InputWS2"));
    TS_ASSERT_THROWS_NOTHING( alg3.setPropertyValue("WorkspaceNames","NewName1, NewName2"));

    TS_ASSERT_THROWS_NOTHING( alg3.execute());
    TS_ASSERT( alg3.isExecuted());

    Workspace_sptr result;
    TS_ASSERT_THROWS_NOTHING(
        result = AnalysisDataService::Instance().retrieveWS<Workspace>("NewName1"));
    TS_ASSERT( result);
    TS_ASSERT_THROWS_NOTHING(
        result = AnalysisDataService::Instance().retrieveWS<Workspace>("NewName2"));
    TS_ASSERT( result);


    TS_ASSERT_THROWS_ANYTHING(
        result = AnalysisDataService::Instance().retrieveWS<Workspace>("InputWS1"));
    TS_ASSERT_THROWS_ANYTHING(
        result = AnalysisDataService::Instance().retrieveWS<Workspace>("InputWS2"));

    AnalysisDataService::Instance().remove("NewName1");
    AnalysisDataService::Instance().remove("NewName2");
  }

  void testPrefix()
  {
    MatrixWorkspace_sptr inputWS3 = createWorkspace();
    MatrixWorkspace_sptr inputWS4 = createWorkspace();
    AnalysisDataService::Instance().add("InputWS3", inputWS3);
    AnalysisDataService::Instance().add("InputWS4", inputWS4);

    Mantid::Algorithms::RenameWorkspaces alg4;
    alg4.initialize();
    alg4.setPropertyValue("InputWorkspaces","InputWS3, InputWS4");
    alg4.setRethrows( true ); // Ensure exceptions are thrown to this test
    TS_ASSERT_THROWS_NOTHING( alg4.setPropertyValue("Prefix","A_"));

    TS_ASSERT_THROWS_NOTHING( alg4.execute());
    TS_ASSERT( alg4.isExecuted());

    Workspace_sptr result;
    TS_ASSERT_THROWS_NOTHING(
        result = AnalysisDataService::Instance().retrieveWS<Workspace>("A_InputWS3"));
    TS_ASSERT( result);
    TS_ASSERT_THROWS_NOTHING(
        result = AnalysisDataService::Instance().retrieveWS<Workspace>("A_InputWS4"));
    TS_ASSERT( result);

    TS_ASSERT_THROWS_ANYTHING(
        result = AnalysisDataService::Instance().retrieveWS<Workspace>("InputWS3"));
    TS_ASSERT_THROWS_ANYTHING(
        result = AnalysisDataService::Instance().retrieveWS<Workspace>("InputWS4"));

    AnalysisDataService::Instance().remove("A_InputWS3");
    AnalysisDataService::Instance().remove("A_InputWS4");
  }

  void testSuffix()
  {
    MatrixWorkspace_sptr inputWS5 = createWorkspace();
    MatrixWorkspace_sptr inputWS6 = createWorkspace();
    AnalysisDataService::Instance().add("InputWS5", inputWS5);
    AnalysisDataService::Instance().add("InputWS6", inputWS6);

    Mantid::Algorithms::RenameWorkspaces alg5;
    alg5.initialize();
    alg5.setPropertyValue("InputWorkspaces","InputWS5, InputWS6");
    alg5.setRethrows( true ); // Ensure exceptions are thrown to this test
    TS_ASSERT_THROWS_NOTHING( alg5.setPropertyValue("Suffix","_1"));

    TS_ASSERT_THROWS_NOTHING( alg5.execute());
    TS_ASSERT( alg5.isExecuted());

    Workspace_sptr result;
    TS_ASSERT_THROWS_NOTHING(
        result = AnalysisDataService::Instance().retrieveWS<Workspace>("InputWS5_1"));
    TS_ASSERT( result);
    TS_ASSERT_THROWS_NOTHING(
        result = AnalysisDataService::Instance().retrieveWS<Workspace>("InputWS6_1"));
    TS_ASSERT( result);

    TS_ASSERT_THROWS_ANYTHING(
        result = AnalysisDataService::Instance().retrieveWS<Workspace>("InputWS5"));
    TS_ASSERT_THROWS_ANYTHING(
        result = AnalysisDataService::Instance().retrieveWS<Workspace>("InputWS6"));

    AnalysisDataService::Instance().remove("InputWS5_1");
    AnalysisDataService::Instance().remove("InputWS6_1");
  }

  void testInvalidArguments()
  {
    MatrixWorkspace_sptr inputWS7 = createWorkspace();
    MatrixWorkspace_sptr inputWS8 = createWorkspace();
    AnalysisDataService::Instance().add("InputWS7", inputWS7);
    AnalysisDataService::Instance().add("InputWS8", inputWS8);
    Mantid::Algorithms::RenameWorkspaces alg6;
    alg6.setRethrows( true );  // Ensure it will throw any exceptions to this test
    alg6.initialize();
    alg6.setPropertyValue("InputWorkspaces","InputWS1, InputWS2");
    // Check throw if no workspace name set
    TS_ASSERT_THROWS(alg6.execute(),std::invalid_argument);
    // Check throw if conflicting workspace names are set
    alg6.setPropertyValue("WorkspaceNames","NewName1, NewName2");
    alg6.setPropertyValue("Prefix","A_");
    TS_ASSERT_THROWS(alg6.execute(),std::invalid_argument);
    alg6.setPropertyValue("Suffix","_1");
    TS_ASSERT_THROWS(alg6.execute(),std::invalid_argument);
    alg6.setPropertyValue("Prefix","");
    TS_ASSERT_THROWS(alg6.execute(),std::invalid_argument);
   
    AnalysisDataService::Instance().remove("InputWS7");
    AnalysisDataService::Instance().remove("InputWS8");

  }

  void testPrefixAndSuffix()
  {
    MatrixWorkspace_sptr inputWS9 = createWorkspace();
    MatrixWorkspace_sptr inputWSA = createWorkspace();
    AnalysisDataService::Instance().add("InputWS9", inputWS9);
    AnalysisDataService::Instance().add("InputWSA", inputWSA);

    Mantid::Algorithms::RenameWorkspaces alg7;
    alg7.initialize();
    alg7.setPropertyValue("InputWorkspaces","InputWS9, InputWSA");
    alg7.setRethrows( true ); // Ensure exceptions are thrown to this test
    TS_ASSERT_THROWS_NOTHING( alg7.setPropertyValue("Prefix","A_"));
    TS_ASSERT_THROWS_NOTHING( alg7.setPropertyValue("Suffix","_1"));

    TS_ASSERT_THROWS_NOTHING( alg7.execute());
    TS_ASSERT( alg7.isExecuted());

    Workspace_sptr result;
    TS_ASSERT_THROWS_NOTHING(
        result = AnalysisDataService::Instance().retrieveWS<Workspace>("A_InputWS9_1"));
    TS_ASSERT( result);
    TS_ASSERT_THROWS_NOTHING(
        result = AnalysisDataService::Instance().retrieveWS<Workspace>("A_InputWSA_1"));
    TS_ASSERT( result);

    TS_ASSERT_THROWS_ANYTHING(
        result = AnalysisDataService::Instance().retrieveWS<Workspace>("InputWS9"));
    TS_ASSERT_THROWS_ANYTHING(
        result = AnalysisDataService::Instance().retrieveWS<Workspace>("InputWSA"));

    AnalysisDataService::Instance().remove("A_InputWS9_1");
    AnalysisDataService::Instance().remove("A_InputWSA_1");
  }


  MatrixWorkspace_sptr createWorkspace()
  {
    MatrixWorkspace_sptr inputWS = WorkspaceCreationHelper::Create2DWorkspaceBinned(4, 4, 0.5);
    return inputWS;
  }

 
private:
  Mantid::Algorithms::RenameWorkspaces alg;

};

#endif /*RENAMEWORKSPACESTEST_H_*/
