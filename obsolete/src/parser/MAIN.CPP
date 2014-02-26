#include <fstream.h>
#include "parser/syn.h"
#include "parser/asprint.h"


void  DummyHolder()
{
  String<char> dummy;
}


void test(istream& is, ostream& os)
{
  Source              source(is);
  ErrorHandler        errorHandler(cout);
  SYN::Syn            syn(&source, &errorHandler);
  syn.ReadModule();
  AbstractSyntaxPrinter::PrintModule(syn.module, os);
  cerr << "\n*******************\n";
  if (errorHandler.nErrors()) {
    cerr << errorHandler.nErrors() << " error(s) found";
  }
  else {
    cerr << "successfully parsed";
  }
}


void  main()
{
  while (true) {
    cerr << "\nFile to parse: ";
    char fname[64];
    cin >> fname;
    if (strcmp(fname, "quit") == 0) {
      break;
    }
    ifstream  source(fname);
    if (!source) {
      cerr << "\nUnable to open file '" << fname << "' !";
      continue;
    }
    cerr << endl << "%%%%%%%%%%%%%%%%%%%" << endl;
    cout << "\n/** source file: " << fname << " **/\n";
    test(source, cout);
  }
}
