#ifndef COOPY_SHEETPATCHER
#define COOPY_SHEETPATCHER

#include <coopy/Patcher.h>
#include <coopy/DataSheet.h>
#include <coopy/PolySheet.h>
#include <coopy/TextBook.h>
#include <coopy/CsvSheet.h>
#include <coopy/NameSniffer.h>

#include <vector>

namespace coopy {
  namespace cmp {
    class SheetPatcher;
  }
}

class coopy::cmp::SheetPatcher : public Patcher {
private:
  ConfigChange config;
  coopy::store::CsvSheet activeRow;
  coopy::store::CsvSheet activeCol;
  coopy::store::CsvSheet statusCol;
  std::vector<int> columns;
  std::vector<std::string> column_names;
  std::map<std::string,int> name2col;
  std::map<std::string,std::string> syn2name;
  std::map<std::string,std::string> name2syn;
  int rowCursor;
  bool summary;
  Patcher *chain;
  int changeCount;
  bool descriptive;
  int xoff;
  int yoff;
  bool killNeutral;
  coopy::store::NameSniffer *sniffer;
  coopy::store::DataSheet *sniffedSheet;

  int matchRow(const std::vector<int>& active_cond,
	       const std::vector<coopy::store::SheetCell>& cond,
	       int width);

  int matchCol(const std::string& mover);

  int updateCols();

  bool moveColumn(int idx, int idx2);

public:
  SheetPatcher(bool descriptive = false) : descriptive(descriptive) {
    rowCursor = -1;
    summary = false;
    chain = 0/*NULL*/;
    changeCount = 0;
    xoff = 0;
    yoff = 0;
    sniffer = 0/*NULL*/;
    sniffedSheet = 0/*NULL*/;
    killNeutral = false;
  }

  virtual ~SheetPatcher() {
    clearNames();
    if (chain!=0/*NULL*/) {
      delete chain;
      chain = 0/*NULL*/;
    }
  }

  void setNames();

  void clearNames() {
    if (sniffer!=0/*NULL*/) {
      delete sniffer;
      sniffer = 0/*NULL*/;
    }
    sniffedSheet = 0/*NULL*/;
  }

  void showSummary(Patcher *chain) {
    summary = true;
    this->chain = chain;
  }

  int getChangeCount() {
    return changeCount;
  }

  virtual bool changeConfig(const ConfigChange& change) { 
    if (chain) chain->changeConfig(change);
    this->config = change;
    return true; 
  }
  virtual bool changeColumn(const OrderChange& change);
  virtual bool changeRow(const RowChange& change);
  virtual bool declareNames(const std::vector<std::string>& names, bool final);

  virtual bool setSheet(const char *name);

  virtual bool mergeStart();

  virtual bool mergeDone();

  virtual bool mergeAllDone();

  virtual bool outputStartsFromInput() {
    return descriptive;
  }

};

#endif

