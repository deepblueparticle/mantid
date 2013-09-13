#include "MantidQtMantidWidgets/ICatSearchTwo.h"

namespace MantidQt
{
  namespace MantidWidgets
  {
    /**
     * Constructor
     */
    ICatSearchTwo::ICatSearchTwo(QWidget* parent) : QWidget(parent)
    {
      initLayout();
    }

    /**
     * Destructor
     */
    ICatSearchTwo::~ICatSearchTwo(){}

    /**
     * Initialise the  default layout.
     */
    void ICatSearchTwo::initLayout()
    {
      // Draw the GUI from .ui header generated file.
      icatUiForm.setupUi(this);

      // Hide the three frames
      icatUiForm.searchFrame->hide();
      icatUiForm.resFrame->hide();
      icatUiForm.dataFileFrame->hide();

      // Hide the "investigation found" label until user has searched.
      icatUiForm.searchResultsLbl->hide();

      // Hide advanced input fields until "Advanced search" is checked.
      advancedSearchChecked();

      // Disable buttons except search by default.
      icatUiForm.searchResultsCbox->setEnabled(false);
      icatUiForm.dataFileCbox->setEnabled(false);

      // Show "Search" frame when user clicks "Catalog search" check box.
      connect(icatUiForm.searchCbox,SIGNAL(clicked()),this,SLOT(showCatalogSearch()));
      // Show advanced search options if "Advanced search" is checked.
      connect(icatUiForm.advSearchCbox,SIGNAL(clicked()),this,SLOT(advancedSearchChecked()));
      // Clear all fields when reset button is pressed.
      connect(icatUiForm.resetBtn,SIGNAL(clicked()),this,SLOT(onReset()));
      // Show "Search results" frame when user tries to "Search".
      connect(icatUiForm.searchBtn,SIGNAL(clicked()),this,SLOT(searchClicked()));
      // Show "Search results" frame when user clicks related check box.
      connect(icatUiForm.searchResultsCbox,SIGNAL(clicked()),this,SLOT(showSearchResults()));

      // Resize to improve UX.
      this->resize(minimumSizeHint());
    }

    /**
     * Opens the login dialog to allow the user to log into another facility.
     */
    void ICatSearchTwo::onFacilityLogin()
    {

    }

    /**
     * Sends the user to relevant search page on the Mantid project site.
     */
    void ICatSearchTwo::onHelp()
    {

    }

    /**
     * Shows/hides the "Catalog search" frame when search combo box is checked.
     */
    void ICatSearchTwo::showCatalogSearch()
    {
      if (icatUiForm.searchCbox->isChecked())
      {
        icatUiForm.searchFrame->show();
      }
      else
      {
        icatUiForm.searchFrame->hide();
      }
    }

    /**
     * Hides the search frame, and shows search results frame when "Search" button pressed.
     */
    void ICatSearchTwo::searchClicked()
    {
      if (icatUiForm.searchBtn)
      {
        icatUiForm.resFrame->show();
        icatUiForm.searchResultsCbox->setEnabled(true);
        icatUiForm.searchResultsCbox->setChecked(true);
      }
    }

    /**
     * Shows/Hides the "Search results" frame when search results combo box is checked.
     */
    void ICatSearchTwo::showSearchResults()
    {
      if (icatUiForm.searchResultsCbox->isChecked())
      {
        icatUiForm.resFrame->show();
      }
      else
      {
        icatUiForm.resFrame->hide();
      }
    }

    /**
     * Hides "Search results" frame when a result is double clicked.
     */
    void ICatSearchTwo::showDataFileInfo()
    {

    }

    ///////////////////////////////////////////////////////////////////////////////
    /// Methods for "Catalog Search".
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * Populates the "Instrument" drop-box
     */
    void ICatSearchTwo::populateInstrumentBox()
    {

    }

    /**
     * Populates the "Investigation type" drop-box.
     */
    void ICatSearchTwo::populateInvestigationTypeBox()
    {

    }

    /**
     * Perform the search.
     */
    bool ICatSearchTwo::executeSearch()
    {

    }

    ///////////////////////////////////////////////////////////////////////////////
    /// SLOTS for "Catalog Search"
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * Opens the DateTime Calender box when spe.
     */
    void ICatSearchTwo::openCalendar()
    {

    }

    /**
     * Search through the user's data only when "My data" is checked.
     */
    void ICatSearchTwo::onMyDataOnlyChecked()
    {

    }

    /**
     * Show or hide advanced options if "Advanced Search" checked.
     */
    void ICatSearchTwo::advancedSearchChecked()
    {
      if (icatUiForm.advSearchCbox->isChecked())
      {
        icatUiForm.advNameLbl->show();
        icatUiForm.advNameTxt->show();
        icatUiForm.advAbstractLbl->show();
        icatUiForm.advAbstractTxt->show();
        icatUiForm.advSampleLbl->show();
        icatUiForm.advSampleTxt->show();
        icatUiForm.advTypeLbl->show();
        icatUiForm.advTypeLbox->show();
      }
      else
      {
        icatUiForm.advNameLbl->hide();
        icatUiForm.advNameTxt->hide();
        icatUiForm.advAbstractLbl->hide();
        icatUiForm.advAbstractTxt->hide();
        icatUiForm.advSampleLbl->hide();
        icatUiForm.advSampleTxt->hide();
        icatUiForm.advTypeLbl->hide();
        icatUiForm.advTypeLbox->hide();
      }
    }

    /**
     * Perform the search operation when the "Search" button is clicked.
     */
    void ICatSearchTwo::onSearch()
    {

    }

    /**
     * Reset all fields when the "Reset" button is pressed.
     */
    void ICatSearchTwo::onReset()
    {
      // Clear normal search fields.
      icatUiForm.invesNameTxt->clear();
      icatUiForm.startDateTxt->clear();
      icatUiForm.instrumenLbox->clear();
      icatUiForm.endDateTxt->clear();
      icatUiForm.runRangeTxt->clear();
      icatUiForm.keywordsTxt->clear();
      // Clear advanced options as well.
      icatUiForm.advNameTxt->clear();
      icatUiForm.advAbstractTxt->clear();
      icatUiForm.advSampleTxt->clear();
      icatUiForm.advTypeLbox->clear();
    }


    ///////////////////////////////////////////////////////////////////////////////
    // Methods for "Search results"
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * Outputs the results of the search into the "Search results" table.
     */
    void ICatSearchTwo::populateResultTable()
    {

    }

    /**
     * When an investigation is double clicked open we want to call populateDataFileTable using the investigation name.
     */
    void ICatSearchTwo::investigationClicked()
    {

    }

    /**
     * Updates the "Displaying info" text box with relevant result info (e.g. 500 of 18,832)
     */
    void ICatSearchTwo::resultInfoUpdate()
    {

    }

    /**
     * Updates the page numbers (e.g. m & n in: Page m of n )
     */
    void ICatSearchTwo::pageNumberUpdate()
    {

    }

    ///////////////////////////////////////////////////////////////////////////////
    // SLOTS for "Search results"
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * Populate the result table, and update the page number.
     */
    bool ICatSearchTwo::nextPageClicked()
    {

    }

    /**
     * Populate the result table, and update the page number.
     */
    bool ICatSearchTwo::prevPageClicked()
    {

    }

    /**
     * Populate's result table depending page number input by user.
     */
    bool ICatSearchTwo::goToInputPage()
    {

    }

    /**
     * Checks that the investigation is selected and performs investigationClicked.
     */
    bool ICatSearchTwo::investigationSelected()
    {

    }

    ///////////////////////////////////////////////////////////////////////////////
    // Methods for "DataFile information"
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * Outputs related dataFiles (from selected investigation) into the "DataFile information" table.
     */
    void ICatSearchTwo::populateDataFileTable()
    {

    }

    /**
     * Obtains the names of the selected dataFiles, in preparation for download.
     */
    void ICatSearchTwo::getCheckedFileNames()
    {

    }

    /**
     * Updates the dataFile text boxes with relevant info regarding the selected dataFile.
     */
    void ICatSearchTwo::updateDataFileLabel()
    {

    }

    /**
     * Filters the "DataFile information" table to display user specified files (based on file extension).
     */
    void ICatSearchTwo::filterDataFileType()
    {

    }

    ///////////////////////////////////////////////////////////////////////////////
    // SLOTS for: "DataFile information"
    ///////////////////////////////////////////////////////////////////////////////

    /**
     * Performs filter option for specified filer type.
     */
    void ICatSearchTwo::doFilter()
    {

    }

    /**
     * Downloads selected datFiles to a specified location.
     */
    void ICatSearchTwo::downloadDataFiles()
    {

    }

    /**
     * Loads the selected dataFiles into workspaces.
     */
    void ICatSearchTwo::loadDataFile()
    {

    }

  }
}
