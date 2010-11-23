#include <coopy/SheetPatcher.h>
#include <coopy/NameSniffer.h>

#include <map>
#include <algorithm>

#include <stdio.h>

using namespace std;
using namespace coopy::cmp;
using namespace coopy::store;

bool SheetPatcher::changeColumn(const OrderChange& change) {
  if (sheet==NULL) return false;
  switch (change.mode) {
  case ORDER_CHANGE_DELETE:
    return sheet->deleteColumn(ColumnRef(change.subject));
    //return sheet->deleteColumn(ColumnRef(change.identityToIndex(change.subject)));
    break;
  case ORDER_CHANGE_INSERT:
    return sheet->insertColumn(ColumnRef(change.subject)).isValid();
    //return sheet->insertColumn(ColumnRef(change.identityToIndex(change.subject))).isValid();
    break;
  case ORDER_CHANGE_MOVE:
    return sheet->moveColumn(ColumnRef(change.subject),
			     ColumnRef(change.object)
			     ).isValid();
    //return sheet->moveColumn(ColumnRef(change.identityToIndex(change.subject)),
    //			     ColumnRef(change.identityToIndex(change.object))
    //			     ).isValid();
  default:
    fprintf(stderr,"* ERROR: Unknown column operation\n");
    break;
  }
  return false;
}

bool SheetPatcher::changeRow(const RowChange& change) {
  if (sheet==NULL) return false;
  if (!change.sequential) rowCursor = -1;
  map<string,int> dir;
  vector<int> active_cond;
  vector<SheetCell> cond;
  vector<int> active_val;
  vector<SheetCell> val;
  vector<string> allNames = change.allNames;
  int width = (int)change.allNames.size();
  if (width==0) {
    if (column_names.size()==0) {
      NameSniffer sniffer(*sheet);
      column_names = sniffer.suggestNames();
    }
    allNames = column_names;
    width = (int)allNames.size();
  }
  for (int i=0; i<width; i++) {
    dir[allNames[i]] = i;
    active_cond.push_back(0);
    cond.push_back(SheetCell());
    active_val.push_back(0);
    val.push_back(SheetCell());
  }
  for (RowChange::txt2cell::const_iterator it = change.cond.begin();
       it!=change.cond.end(); it++) {
    int idx = dir[it->first];
    //printf("  [cond] %d %s -> %s\n", idx, it->first.c_str(), it->second.c_str());
    active_cond[idx] = 1;
    cond[idx] = it->second;
  }
  for (RowChange::txt2cell::const_iterator it = change.val.begin();
       it!=change.val.end(); it++) {
    int idx = dir[it->first];
    //printf("  [val] %d %s -> %s\n", idx, it->first.c_str(), it->second.c_str());
    active_val[idx] = 1;
    val[idx] = it->second;
  }
  
  switch (change.mode) {
  case ROW_CHANGE_INSERT:
    {
      RowRef tail(rowCursor);
      int r = sheet->insertRow(tail).getIndex();
      if (rowCursor!=-1) {
	rowCursor++;
      }
      if (r>=0) {
	//printf("Row %d -- %d\n", r, sheet->height());
	for (int c=0; c<width; c++) {
	  if (active_val[c]) {
	    //printf("  %d %d %s\n", c, r, val[c].c_str());
	    sheet->cellSummary(c,r,val[c]);
	  }
	}
      }
    }
    break;
  case ROW_CHANGE_DELETE:
    {
      bool success = false;
      int r;
      for (r=0; r<sheet->height(); r++) {
	bool match = true;
	for (int c=0; c<width; c++) {
	  if (active_cond[c]) {
	    if (sheet->cellSummary(c,r)!=cond[c]) {
	      match = false;
	      break;
	    }
	  }
	}
	if (match) {
	  RowRef row(r);
	  rowCursor = r;
	  sheet->deleteRow(row);
	  success = true;
	  break;
	}
      }
      return success;
    }
    break;
  case ROW_CHANGE_UPDATE:
    {
      bool success = false;
      int r;
      for (r=0; r<sheet->height(); r++) {
	bool match = true;
	for (int c=0; c<width; c++) {
	  if (active_cond[c]) {
	    dbg_printf("compare %s and %s\n",
		       sheet->cellSummary(c,r).toString().c_str(),
		       cond[c].toString().c_str());
	    if (sheet->cellSummary(c,r)!=cond[c]) {
	      match = false;
	      break;
	    }
	  }
	}
	if (match) {
	  rowCursor = r;
	  dbg_printf("Match\n");
	  for (int c=0; c<width; c++) {
	    if (active_val[c]) {
	      sheet->cellSummary(c,r,val[c]);
	    }
	  }
	  success = true;
	  break;
	} else {
	  dbg_printf("No match\n");
	  rowCursor = -1;
	}
      }
      return success;
    }
    break;
  default:
    fprintf(stderr,"* ERROR: unsupported row operation\n");
    return false;
    break;
  }
  return true;
}

bool SheetPatcher::declareNames(const std::vector<std::string>& names, 
				bool final) {
  if (sheet==NULL) return false;
  if (config.trustNames==false) {
    if ((int)names.size()!=sheet->width()) {
      fprintf(stderr,"* ERROR: name mismatch\n");
      return false;
    }
  } else {
    for (int i=0; i<(int)names.size(); i++) {
      printf("Checking %s\n", names[i].c_str());
    }
    fprintf(stderr,"Named columns not implemented yet\n");
    exit(1);
  }
  return true;
}

