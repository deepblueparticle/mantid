from MantidFramework import *
from mantidsimple import *
import os

class ConjoinSpectra(PythonAlgorithm):
    """
    Conjoins spectra from several workspaces into a single workspaceExists
    
    Spectra to be conjoined must be equally binned in order for ConjoinSpectra to work. If necessary use RebinToWorkspace first.         
    """
   
    def category(self):
        return "General"

    def name(self):
        return "ConjoinSpectra"

    def PyInit(self):
        self.declareProperty("InputWorkspaces","", Validator=MandatoryValidator(), Description="Comma seperated list of workspaces to use, group workspaces will automatically include all members.")
        self.declareWorkspaceProperty("OutputWorkspace", "", Direction=Direction.Output, Description="Name the workspace that will contain the result")
        self.declareProperty("WorkspaceIndex", 0, Description="The workspace index of the spectra in each workspace to extract. Default: 0")
        self.declareProperty("LabelUsing", "", Description="The name of a log value used to label the resulting spectra. Default: The source workspace name")
        labelValueOptions =  ["Mean","Median","Maximum","Minimum","First Value"]
        self.declareProperty("LabelValue", "Mean", Validator=ListValidator(labelValueOptions), Description="How to derive the value from a time series property")
      
      
    def PyExec(self):
        # get parameter values
        wsOutput = self.getPropertyValue("OutputWorkspace")
        wsIndex = int(self.getPropertyValue("WorkspaceIndex"))
        wsString = self.getPropertyValue("InputWorkspaces").strip()
        labelUsing = self.getPropertyValue("LabelUsing").strip()
        labelValue = self.getPropertyValue("LabelValue")

        #internal values
        wsTemp = "__ConjoinSpectra_temp"
        loopIndex=0
        
        #get the wokspace list
        wsNames = []
        for wsName in wsString.split(","):
        #check the ws is in mantid
            ws = mtd[wsName.strip()]
            #if we cannot find the ws then stop
            if ws == None:
                raise RuntimeError ("Cannot find workspace '" + wsName.strip() + "', aborting")
            if ws.isGroup():
                wsNames.extend(ws.getNames())
            else:
                wsNames.append(wsName)

        ta = createTextAxis(len(wsNames))
        if mtd.workspaceExists(wsOutput):
            DeleteWorkspace(Workspace=wsOutput)
        for wsName in wsNames:
            #extract the spectrum
            ExtractSingleSpectrum(InputWorkspace=wsName,OutputWorkspace=wsTemp,WorkspaceIndex=wsIndex)
            
            labelString =""
            if (labelUsing != ""):
                labelString = self.GetLogValue(mtd[wsName.strip()],labelUsing,labelValue)
            if (labelString == ""):
                labelString =wsName+"_"+str(wsIndex)
            ta.setValue(loopIndex,labelString)
            loopIndex += 1
            if mtd.workspaceExists(wsOutput):
                ConjoinWorkspaces(InputWorkspace1=wsOutput,InputWorkspace2=wsTemp,CheckOverlapping=False)
                if mtd.workspaceExists(wsTemp):
                    DeleteWorkspace(Workspace=wsTemp)
            else:
                RenameWorkspace(InputWorkspace=wsTemp,OutputWorkspace=wsOutput)

        wsOut = mtd[wsOutput]
        #replace the spectrun axis
        wsOut.replaceAxis(1,ta)


        self.setProperty("OutputWorkspace",wsOut)
        
    def GetLogValue(self,ws,labelUsing,labelValue):
        labelString = ""
        run=ws.getRun()
        try:
            prop = run.getProperty(labelUsing)
            try:
                stats = prop.getStatistics()
                if (labelValue == "Mean"):
                    labelString = str(stats.mean)
                elif (labelValue == "Median"):
                    labelString = str(stats.median)
                elif (labelValue == "Maximum"):
                    labelString = str(stats.maximum)
                elif (labelValue == "Minimum"):
                    labelString = str(stats.minimum)
                else:
                    labelString =  str(prop.value[0])
            except:
                #this is not a time series property - just return the value
                labelString =  str(prop.value)
        except:
            #failed to find the property
            #log and pass out an empty string
            mtd.sendLogMessage("Could not find log " + labelUsing + " in workspace " + str(ws) + " using workspace label instead.")
        return labelString
        
mtd.registerPyAlgorithm(ConjoinSpectra())
