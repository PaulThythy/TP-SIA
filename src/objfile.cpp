#include "objfile.h"
#include <string>
#include <sstream>

using namespace std;
//-------------------------------------------
OBJFile::OBJFile(string initFileName)
{
this->fileName = initFileName ;
cout << "dans init OBJFile "<<endl;
}
//-------------------------------------------
OBJFile::OBJFile(void)
{

}
//-------------------------------------------
OBJFile::~OBJFile(void)
{


}
//-------------------------------------------
ostream& operator<<(ostream& os, const OBJFile& OBJf)
{

}
//-------------------------------------------

void OBJFile::readData(void)
{
    string line;
    int nbVertices(0), nbFaces(0) ;


    ifstream myfile ("test.obj");
        if (myfile.is_open())
        {
          while ( getline (myfile,line) )
          {
              float x,y,z ;
              char carLine;

            cout << line << '\n';
            istringstream istr(line);
            istr >> carLine >> x >> y >> z ;

          //**********************************************************
          // AFAIRE
          // ici stoker ces info dans une structure de données
          //**********************************************************
          } //fin while getline
          myfile.close();
        }

        else cout << "Unable to open file";
this->constructTopology();
}
//-----------------------------------------------------
void OBJFile::constructTopology(void)
{
    //**********************************************************
    // AFAIRE
    // ici exploiter les données du fichier OBJ pour construire les tableaux d'indices
    //**********************************************************


}


