#include "Parser.hpp"
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------

/* TEST QUERY
SELECT s.MatrNr, s.Name, s.Semester
FROM Studenten s, hoeren h, Vorlesungen v, Professoren p
WHERE s.MatrNr=h.MatrNr AND h.VorlNr=v.VorlNr AND
v.gelesenVon=p.PersNr AND p.Name=’Curie’
*/

Parser::Parser() 
{

}

Parser::~Parser()
{

}

void Parser::parse(const std::string& query) 
{

}