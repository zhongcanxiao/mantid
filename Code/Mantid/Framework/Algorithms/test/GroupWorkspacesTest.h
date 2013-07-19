#ifndef GROUPWORKSPACESTEST_H_
#define GROUPWORKSPACESTEST_H_

#include <cxxtest/TestSuite.h>
#include "MantidAlgorithms/GroupWorkspaces.h"
#include "MantidDataHandling/LoadRaw3.h"
#include "MantidDataHandling/LoadNexusProcessed.h"
#include "MantidAlgorithms/FindPeaks.h"
#include "MantidDataHandling/LoadInstrument.h"

using namespace Mantid::API;
using namespace Mantid::Kernel;
using namespace Mantid::DataHandling;
using namespace Mantid::Algorithms;

class GroupWorkspacesTest : public CxxTest::TestSuite
{
public:
  void testName()
  {
    GroupWorkspaces grpwsalg;
    TS_ASSERT_EQUALS( grpwsalg.name(), "GroupWorkspaces" );
  }

  void testVersion()
  {
    GroupWorkspaces grpwsalg;
    TS_ASSERT_EQUALS( grpwsalg.version(), 1 );
  }

  void testInit()
  {
    Mantid::Algorithms::GroupWorkspaces alg2;
    TS_ASSERT_THROWS_NOTHING( alg2.initialize() );
    TS_ASSERT( alg2.isInitialized() );

    const std::vector<Property*> props = alg2.getProperties();
    TS_ASSERT_EQUALS( props.size(), 2 );

    TS_ASSERT_EQUALS( props[0]->name(), "InputWorkspaces" );
    TS_ASSERT( props[0]->isDefault() );

    TS_ASSERT_EQUALS( props[1]->name(), "OutputWorkspace" );
    TS_ASSERT( props[1]->isDefault() );
    TS_ASSERT( dynamic_cast<WorkspaceProperty<WorkspaceGroup>* >(props[1]) );


  }

  void testExecGroupOneNormalWorkspace()
  {
    LoadRaw3 alg;
    alg.initialize();
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("FileName","LOQ48097.raw"));
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("OutputWorkspace","LOQ48097"));
    TS_ASSERT_THROWS_NOTHING( alg.execute());
    TS_ASSERT( alg.isExecuted() );

    GroupWorkspaces grpwsalg;
    grpwsalg.initialize();
    std::vector<std::string >input;
    input.push_back("LOQ48097");
    TS_ASSERT_THROWS_NOTHING( grpwsalg.setProperty("InputWorkspaces",input));
    TS_ASSERT_THROWS_NOTHING( grpwsalg.setProperty("OutputWorkspace","NewGroup"));
    TS_ASSERT_THROWS_NOTHING( grpwsalg.execute());
    TS_ASSERT( grpwsalg.isExecuted() );

    AnalysisDataServiceImpl& ads = AnalysisDataService::Instance();
    WorkspaceGroup_sptr result;
    TS_ASSERT_THROWS_NOTHING( result = ads.retrieveWS<WorkspaceGroup>("NewGroup") );
    std::vector<std::string> grpVec = result->getNames();
    TS_ASSERT_EQUALS(grpVec.size(),1);
    if(grpVec.size() == 1)
    {
      TS_ASSERT_EQUALS("LOQ48097",grpVec[0])
    }
    ads.remove("LOQ48097");
    ads.remove("NewGroup");

  }

  void testExecGroupTwoNormalWorkspaces()
  {
    LoadRaw3 alg;
    alg.initialize();
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("FileName","LOQ48097.raw"));
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("OutputWorkspace","LOQ48097"));
    TS_ASSERT_THROWS_NOTHING( alg.execute());
    TS_ASSERT( alg.isExecuted() );

    LoadRaw3 alg1;
    alg1.initialize();
    TS_ASSERT_THROWS_NOTHING(alg1.setPropertyValue("FileName","LOQ48098.raw"));
    TS_ASSERT_THROWS_NOTHING(alg1.setPropertyValue("OutputWorkspace","LOQ48098"));
    TS_ASSERT_THROWS_NOTHING( alg1.execute());
    TS_ASSERT( alg1.isExecuted() );

    GroupWorkspaces grpwsalg;
    grpwsalg.initialize();
    std::vector<std::string >input;
    input.push_back("LOQ48097");
    input.push_back("LOQ48098");
    TS_ASSERT_THROWS_NOTHING( grpwsalg.setProperty("InputWorkspaces",input));
    TS_ASSERT_THROWS_NOTHING( grpwsalg.setProperty("OutputWorkspace","NewGroup"));
    TS_ASSERT_THROWS_NOTHING( grpwsalg.execute());
    TS_ASSERT( grpwsalg.isExecuted() );
    WorkspaceGroup_sptr result;
    TS_ASSERT_THROWS_NOTHING( result = AnalysisDataService::Instance().retrieveWS<WorkspaceGroup>("NewGroup") );
    std::vector<std::string> grpVec=result->getNames();
    TS_ASSERT_EQUALS(grpVec.size(),2);

    Workspace_sptr result1;
    TS_ASSERT_THROWS_NOTHING( result1 = AnalysisDataService::Instance().retrieveWS<Workspace>("LOQ48097") );
    TS_ASSERT_THROWS_NOTHING( result1 = AnalysisDataService::Instance().retrieveWS<Workspace>("LOQ48098") );
    AnalysisDataService::Instance().remove("NewGroup");
    AnalysisDataService::Instance().remove("LOQ48097");
    AnalysisDataService::Instance().remove("LOQ48098");
  }
  void testExecGroupThreeNormalWorkspaces()
  {
    LoadRaw3 alg;
    alg.initialize();
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("FileName","LOQ48094.raw"));
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("OutputWorkspace","LOQ48094"));
    TS_ASSERT_THROWS_NOTHING( alg.execute());
    TS_ASSERT( alg.isExecuted() );

    LoadRaw3 alg1;
    alg1.initialize();
    TS_ASSERT_THROWS_NOTHING(alg1.setPropertyValue("FileName","LOQ48098.raw"));
    TS_ASSERT_THROWS_NOTHING(alg1.setPropertyValue("OutputWorkspace","LOQ48098"));
    TS_ASSERT_THROWS_NOTHING( alg1.execute());
    TS_ASSERT( alg1.isExecuted() );

    LoadRaw3 alg2;
    alg2.initialize();
    TS_ASSERT_THROWS_NOTHING(alg2.setPropertyValue("FileName","LOQ48097.raw"));
    TS_ASSERT_THROWS_NOTHING(alg2.setPropertyValue("OutputWorkspace","LOQ48097"));
    TS_ASSERT_THROWS_NOTHING( alg2.execute());
    TS_ASSERT( alg2.isExecuted() );

    GroupWorkspaces grpwsalg;
    grpwsalg.initialize();
    std::vector<std::string >input;
    input.push_back("LOQ48094");
    input.push_back("LOQ48098");
    input.push_back("LOQ48097");
    TS_ASSERT_THROWS_NOTHING( grpwsalg.setProperty("InputWorkspaces",input));
    TS_ASSERT_THROWS_NOTHING( grpwsalg.setProperty("OutputWorkspace","NewGroup"));
    TS_ASSERT_THROWS_NOTHING( grpwsalg.execute());
    TS_ASSERT( grpwsalg.isExecuted() );
    WorkspaceGroup_sptr result;
    TS_ASSERT_THROWS_NOTHING( result = AnalysisDataService::Instance().retrieveWS<WorkspaceGroup>("NewGroup") );
    std::vector<std::string> grpVec=result->getNames();
    TS_ASSERT_EQUALS(grpVec.size(),3);
    Workspace_sptr result1;
    TS_ASSERT_THROWS_NOTHING( result1 = AnalysisDataService::Instance().retrieveWS<Workspace>("LOQ48094") );
    TS_ASSERT_THROWS_NOTHING( result1 = AnalysisDataService::Instance().retrieveWS<Workspace>("LOQ48098") );
    TS_ASSERT_THROWS_NOTHING( result1 = AnalysisDataService::Instance().retrieveWS<Workspace>("LOQ48097") );

    AnalysisDataService::Instance().remove("NewGroup");
    AnalysisDataService::Instance().remove("LOQ48094");
    AnalysisDataService::Instance().remove("LOQ48098");
    AnalysisDataService::Instance().remove("LOQ48097");
  }
  void testExecGroupNormalWorkspaceandGroupWorkspace()
  {
    LoadRaw3 alg;
    alg.initialize();
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("FileName","CSP79590.raw"));
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("OutputWorkspace","CSP79590"));
    TS_ASSERT_THROWS_NOTHING( alg.execute());
    TS_ASSERT( alg.isExecuted() );

    LoadRaw3 alg1;
    alg1.initialize();
    TS_ASSERT_THROWS_NOTHING(alg1.setPropertyValue("FileName","LOQ48098.raw"));
    TS_ASSERT_THROWS_NOTHING(alg1.setPropertyValue("OutputWorkspace","LOQ48098"));
    TS_ASSERT_THROWS_NOTHING( alg1.execute());
    TS_ASSERT( alg1.isExecuted() );

    GroupWorkspaces grpwsalg;
    grpwsalg.initialize();
    std::vector<std::string >input;
    input.push_back("CSP79590_1");
    input.push_back("CSP79590_2");
    input.push_back("LOQ48098");

    TS_ASSERT_THROWS_NOTHING( grpwsalg.setProperty("InputWorkspaces",input));
    TS_ASSERT_THROWS_NOTHING( grpwsalg.setProperty("OutputWorkspace","NewGroup"));
    TS_ASSERT_THROWS_NOTHING( grpwsalg.execute());
    TS_ASSERT( grpwsalg.isExecuted() );
    WorkspaceGroup_sptr result;
    TS_ASSERT_THROWS_NOTHING( result = AnalysisDataService::Instance().retrieveWS<WorkspaceGroup>("NewGroup") );
    std::vector<std::string> grpVec=result->getNames();
    TS_ASSERT_EQUALS(grpVec.size(),3);
    Workspace_sptr result1;
    TS_ASSERT_THROWS_NOTHING( result1 = AnalysisDataService::Instance().retrieveWS<Workspace>("CSP79590_1") );
    TS_ASSERT_THROWS_NOTHING( result1 = AnalysisDataService::Instance().retrieveWS<Workspace>("CSP79590_2") );
    TS_ASSERT_THROWS_NOTHING( result1 = AnalysisDataService::Instance().retrieveWS<Workspace>("LOQ48098") );

    AnalysisDataService::Instance().remove("NewGroup");
    AnalysisDataService::Instance().remove("CSP79590_1");
    AnalysisDataService::Instance().remove("CSP79590_2");
    AnalysisDataService::Instance().remove("LOQ48098");
  }
  void testExecGroupWithDifferentWorkspaces()
  {
    LoadRaw3 alg;
    alg.initialize();
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("FileName","LOQ48094.raw"));
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("OutputWorkspace","LOQ48094"));
    TS_ASSERT_THROWS_NOTHING( alg.execute());
    TS_ASSERT( alg.isExecuted() );

    Mantid::DataHandling::LoadNexusProcessed loader;
    loader.initialize();

    loader.setProperty("Filename","focussed.nxs");
    loader.setProperty("OutputWorkspace","peaksWS");
    TS_ASSERT_THROWS_NOTHING(loader.execute());

    loader.setProperty("Filename","LOQ49886.nxs");
    loader.setProperty("OutputWorkspace","LOQ49886");
    TS_ASSERT_THROWS_NOTHING(loader.execute());

    Mantid::Algorithms::FindPeaks finder;
    finder.initialize();
    TS_ASSERT_THROWS_NOTHING(finder.setPropertyValue("InputWorkspace","peaksWS"));
    TS_ASSERT_THROWS_NOTHING(finder.setPropertyValue("PeaksList","foundpeaks"));
    TS_ASSERT_THROWS_NOTHING( finder.execute());
    ITableWorkspace_sptr result1;
    TS_ASSERT_THROWS_NOTHING( result1 = AnalysisDataService::Instance().retrieveWS<ITableWorkspace>("foundpeaks") );
    TS_ASSERT( finder.isExecuted() );

    GroupWorkspaces grpwsalg;
    grpwsalg.initialize();
    std::vector<std::string >input;
    input.push_back("LOQ48094");
    input.push_back("foundpeaks");
    input.push_back("LOQ49886");
    TS_ASSERT_THROWS_NOTHING( grpwsalg.setProperty("InputWorkspaces",input));
    TS_ASSERT_THROWS_NOTHING( grpwsalg.setProperty("OutputWorkspace","NewGroup"));
    TS_ASSERT_THROWS_NOTHING( grpwsalg.execute());
    TS_ASSERT( !grpwsalg.isExecuted() );

    AnalysisDataService::Instance().remove("LOQ48094");
    AnalysisDataService::Instance().remove("foundpeaks");
    AnalysisDataService::Instance().remove("LOQ49886");
  }

  void testExecGroupWithTableWorkspaces()
  {
    LoadRaw3 alg;
    alg.initialize();
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("FileName","LOQ48094.raw"));
    TS_ASSERT_THROWS_NOTHING(alg.setPropertyValue("OutputWorkspace","LOQ48094"));
    TS_ASSERT_THROWS_NOTHING( alg.execute());
    TS_ASSERT( alg.isExecuted() );

    boost::shared_ptr<Workspace>tw(new Mantid::DataObjects::TableWorkspace);
    AnalysisDataService::Instance().add("table",tw);

    GroupWorkspaces grpwsalg;
    grpwsalg.initialize();
    std::vector<std::string >input;
    input.push_back("LOQ48094");
    input.push_back("table");
    TS_ASSERT_THROWS_NOTHING( grpwsalg.setProperty("InputWorkspaces",input));
    TS_ASSERT_THROWS_NOTHING( grpwsalg.setProperty("OutputWorkspace","NewGroup"));
    TS_ASSERT_THROWS_NOTHING( grpwsalg.execute());
    TS_ASSERT( grpwsalg.isExecuted() );

    AnalysisDataService::Instance().remove("LOQ48094");
    AnalysisDataService::Instance().remove("table");
  }

private:
  //Mantid::Algorithms::GroupWorkspaces grpwsalg;

};
#endif
