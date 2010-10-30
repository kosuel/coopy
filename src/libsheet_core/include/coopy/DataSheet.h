#ifndef COOPY_DATASHEET
#define COOPY_DATASHEET

#include <coopy/Dbg.h>
#include <coopy/SheetStyle.h>
#include <coopy/SheetSchema.h>
#include <coopy/RowRef.h>
#include <coopy/ColumnRef.h>
#include <coopy/RefCount.h>

#include <string>

namespace coopy {
  namespace store {
    class DataSheet;
  }
}

class coopy::store::DataSheet : public RefCount {
public:
  virtual ~DataSheet() {}

  virtual int width() const = 0;
  virtual int height() const = 0;

  virtual std::string cellString(int x, int y) const = 0;

  virtual bool cellString(int x, int y, const std::string& str) {
    return false;
  }

  std::string encode(const SheetStyle& style) const;

  static std::string encodeCell(const std::string& str, 
				const SheetStyle& style);

  // remove any cached values, e.g. in remote connections.
  virtual bool clearCache() { return true; }

  virtual SheetSchema *getSchema() {
    return 0 /*NULL*/;
  }

  virtual bool deleteColumn(const ColumnRef& column) {
    return false;
  }

  // insert a column before base; if base is invalid insert after all columns
  virtual ColumnRef insertColumn(const ColumnRef& base) {
    return ColumnRef(); // invalid column
  }

  // move a column before base; if base is invalid move after all columns
  virtual ColumnRef moveColumn(const ColumnRef& src, const ColumnRef& base) {
    return ColumnRef(); // invalid column
  }

  virtual bool deleteRow(const RowRef& src) {
    return false;
  }

  // insert a row before base; if base is invalid insert after all rows
  virtual RowRef insertRow(const RowRef& base) {
    return RowRef(); // invalid row
  }

  // move a row before base; if base is invalid move after all rows
  virtual RowRef moveRow(const RowRef& src, const RowRef& base) {
    return RowRef(); // invalid column
  }
};

#endif
