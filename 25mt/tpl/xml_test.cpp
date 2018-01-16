//   $Id:  $
/*
   Copyright&copy; 2012 University of Hawaii, Pelagic Fisheries Research Program
   Test code for ADMB_XMLDoc class.
 */

#include "ADMB_XMLDoc.h"
#include <iostream>
#include "trace.h"

using namespace std;
ofstream clogf;

int main(int argc, char *argv[])
{
   clogf.open("xml_test.log");
   TTRACE(argc,argv)
   if (argc < 2)
   {
      cerr << "Usage: xml_test string" << endl;
      cerr << "       string is a xml file name, e.g., gen2-p08.xml." << endl;
      exit(1);
   }
   adstring xml_file(argv[1]);
   TRACE(xml_file)
 

   //XMLDocument2 doc;
   ADMB_XMLDoc doc;

   int ret;
   ret = doc.read(xml_file);

   TRACE(ret)

   double test1 = 0.0;
   TRACE(test1)
   test1 = doc.getDouble("test");
   clogf << setprecision(16) << test1 << endl;;

   test1 = M_PI;
   
   clogf << M_PI << endl;;
   clogf << setprecision(16) << test1 << endl;;
   ret = doc.set("test",test1);
   TRACE(ret)
   //double test2 = 0.0;
   //test2 = doc.getDouble("test");
   //clogf << setprecision(16) << test2;

   double tmort = doc.getDouble("mort");
   TRACE(tmort)
   tmort *=2.0;
   ret = doc.set("mort",tmort);
   TRACE(ret)

   dvector tq = doc.getDvector("q");
   TRACE(tq)
 
   dvector rr = doc.getDvector("report_rates");
   TRACE(rr)

   double sm = doc.getDouble("special_mort");
   TRACE(sm)

   dvector nbp = doc.getDvector("nb_par");
   TRACE(nbp)

   dmatrix sigma = doc.getDmatrix("seasonGridSigma");
   TRACE(sigma)

   dmatrix V = doc.getDmatrix("seasonGridV");
   TRACE(V)

   dmatrix U = doc.getDmatrix("seasonGridU");
   TRACE(U)

   V = sigma;
   doc.set("seasonGridU",V);

   sigma /= 10.0;
   doc.set("seasonGridSigma",sigma);

   int nbyte = doc.write("newparfile.xml");
   TRACE(nbyte)

   return 0;
}
