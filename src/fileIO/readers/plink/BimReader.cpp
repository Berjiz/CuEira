#include "BimReader.h"

namespace CuEira {
namespace FileIO {

BimReader::BimReader(const Configuration& configuration) :
    configuration(configuration), numberOfSNPs(0), bimFileStr(configuration.getBimFilePath()) {
  std::string line;
  std::ifstream bimFile;

  /*
   * Columns in the file
   * chromosome (1-22, X, Y or 0 if unplaced)
   * rs# or snp identifier
   * Genetic distance (morgans)
   * Base-pair position (bp units)
   * Allele 1
   * Allele 2
   * */

  //Read whole file once to count the number of SNPs
  bimFile.open(bimFileStr, std::ifstream::in);
  if(!bimFile){
    std::ostringstream os;
    os << "Problem opening bim file " << bimFileStr << std::endl;
    const std::string& tmp = os.str();
    throw FileReaderException(tmp.c_str());
  }

  while(std::getline(bimFile, line)){
    numberOfSNPs++;
  }
  bimFile.close();

}

BimReader::~BimReader() {

}

int BimReader::getNumberOfSNPs() const {
  return numberOfSNPs;
}

std::vector<SNP*>* BimReader::readSNPInformation() const {
  std::ifstream bimFile;
  std::string line;
  std::vector<SNP*>* SNPInformation = new std::vector<SNP*>(numberOfSNPs);
  int pos = 0;
  SNP* snp = nullptr;
  long int baseSNP;
  char * temp; //Used for error checking of string to long int conversion (strtol)

  bimFile.open(bimFileStr, std::ifstream::in);
  if(!bimFile){
    std::ostringstream os;
    os << "Problem opening bim file " << bimFileStr << std::endl;
    const std::string& tmp = os.str();
    throw FileReaderException(tmp.c_str());
  }

  while(std::getline(bimFile, line)){
    std::vector<std::string> lineSplit;
    boost::split(lineSplit, line, boost::is_any_of("\t "));

    Id id(lineSplit[1]);

    baseSNP = strtol(lineSplit[3].c_str(), &temp, 0);
    if(*temp != '\0'){ //Check if there was an error with strtol
      std::ostringstream os;
      os << "Problem with string to int conversion of basepositions in bim file " << bimFileStr << std::endl;
      const std::string& tmp = os.str();
      throw FileReaderException(tmp.c_str());
    }

    //Read alleles
    std::string alleleOneName = lineSplit[4];
    std::string alleleTwoName = lineSplit[5];

    //Should SNPs with negative position be excluded?
    if(configuration.excludeSNPsWithNegativePosition()){
      if(baseSNP < 0){
        snp = new SNP(id, alleleOneName, alleleTwoName, pos, NEGATIVE_POSITION);
      }else{
        snp = new SNP(id, alleleOneName, alleleTwoName, pos);
      }
    }else{
      snp = new SNP(id, alleleOneName, alleleTwoName, pos);
    }
    (*SNPInformation)[pos] = snp;

    pos++;
  } /* while getline */
  bimFile.close();

  return SNPInformation;
}

} /* namespace FileIO */
} /* namespace CuEira */
