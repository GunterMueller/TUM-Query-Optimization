#include "Database.hpp"
#include "operator/Tablescan.hpp"
#include "operator/CrossProduct.hpp"
#include "operator/Selection.hpp"
#include "operator/Projection.hpp"
#include "operator/Printer.hpp"
#include "operator/Chi.hpp"
#include <string>
#include <iostream>
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
int main()
{
   Database db;
   db.open("data/uni");
   Table& studenten=db.getTable("studenten");
   Table& hoeren=db.getTable("hoeren");
   Table& studenten2=db.getTable("studenten");
   Table& hoeren2=db.getTable("hoeren");

   unique_ptr<Tablescan> scanStudenten2(new Tablescan(studenten2));
   unique_ptr<Tablescan> scanHoeren2(new Tablescan(hoeren2));
   unique_ptr<Tablescan> scanHoeren(new Tablescan(hoeren));	

   const Register* name=scanStudenten2->getOutput("name");
   const Register* matrnr=scanStudenten2->getOutput("matrnr");
   const Register* hnr=scanHoeren2->getOutput("matrnr");
   const Register* vorlnr=scanHoeren2->getOutput("vorlnr");

// join studenten2 and hoeren2 on matrNr

   unique_ptr<CrossProduct> cp(new CrossProduct(move(scanStudenten2),move(scanHoeren2)));
   unique_ptr<Selection> select(new Selection(move(cp),matrnr,hnr));

//only choose name and vorlnr
   unique_ptr<Projection> project(new Projection(move(select),{name,vorlnr}));

//select only entries with name schopenhauer
  Register schop; schop.setString("Schopenhauer");
  unique_ptr<Selection> schopenhauer(new Selection(move(project),name,&schop));

//now get the lectures schopenhauer has visited
  unique_ptr<Projection> lectures(new Projection(move(schopenhauer),{vorlnr}));  
  const Register* vorlesung = lectures->getOutput()[0];
  unique_ptr<Chi> chi(new Chi(move(scanHoeren),Chi::Div,vorlnr,vorlesung));
  
   Printer out(move(chi));
   out.open();
   while (out.next());
   out.close();
}
//---------------------------------------------------------------------------
