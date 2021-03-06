#include "BedReader.h"

namespace CuEira {
namespace FileIO {

template<typename Vector>
BedReader<Vector>::BedReader(const Configuration& configuration,
    const Container::SNPVectorFactory<Vector>* snpVectorFactory, const PersonHandlerLocked& personHandler,
    const int numberOfSNPs) :
    configuration(configuration), snpVectorFactory(snpVectorFactory), personHandler(personHandler), bedFileStr(
        configuration.getBedFilePath()), numberOfSNPs(numberOfSNPs), numberOfIndividualsTotal(
        personHandler.getNumberOfIndividualsTotal()){

  std::ifstream bedFile;
  openBedFile(bedFile);

  //Read header to check version and mode.
  char buffer[headerSize];
  bedFile.seekg(0, std::ios::beg);
  bedFile.read(buffer, headerSize);

  if(!bedFile){
    std::ostringstream os;
    os << "Problem reading header of bed file " << bedFileStr << std::endl;
    const std::string& tmp = os.str();
    throw FileReaderException(tmp.c_str());
  }

  //Check version
  if(!(buffer[0] == 108 && buffer[1] == 27)){ //If first byte is 01101100 and second is 00011011 then we have a bed file
    std::ostringstream os;
    os << "Provided bed file is not a bed file " << bedFileStr << std::endl;
    const std::string& tmp = os.str();
    throw FileReaderException(tmp.c_str());
  }

  //Check mode
  if(buffer[2] == 1){      // 00000001
#ifdef DEBUG
      std::cerr << "Bed file is SNP major" << std::endl;
#endif
    mode = SNPMAJOR;
  }else if(buffer[2] == 0){      // 00000000
#ifdef DEBUG
      std::cerr << "Bed file is individual major" << std::endl;
#endif
    mode = INDIVIDUALMAJOR;
  }else{
    std::ostringstream os;
    os << "Unknown major mode of the bed file " << bedFileStr << std::endl;
    const std::string& tmp = os.str();
    throw FileReaderException(tmp.c_str());
  }

  closeBedFile(bedFile);

  if(mode == SNPMAJOR){
    numberOfBitsPerRow = numberOfIndividualsTotal * 2;
    //Each individuals genotype is stored as 2 bits, there are no incomplete bytes so we have to round the number up
    numberOfBytesPerRow = ceil(((double) numberOfBitsPerRow) / 8);

    //The number of bits at the start of the last byte(due to the reversing of the bytes) that we don't care about
    numberOfUninterestingBitsAtEnd = (8 * numberOfBytesPerRow) - numberOfBitsPerRow;
  }else{
    throw FileReaderException("Individual major mode is not implemented yet.");
  }
}

template<typename Vector>
BedReader<Vector>::BedReader(const Configuration& configuration, const PersonHandlerLocked& personHandler) :
    numberOfSNPs(0), numberOfIndividualsTotal(0), configuration(configuration), snpVectorFactory(nullptr), numberOfBitsPerRow(
        0), numberOfBytesPerRow(0), numberOfUninterestingBitsAtEnd(0), personHandler(personHandler), mode(
        INDIVIDUALMAJOR){

}

template<typename Vector>
BedReader<Vector>::~BedReader(){
  delete snpVectorFactory;
}

template<typename Vector>
Container::SNPVector<Vector>* BedReader<Vector>::readSNP(SNP& snp) const{
  const int numberOfIndividualsToInclude = personHandler.getNumberOfIndividualsToInclude();
  Vector* snpDataOriginal = new Vector(numberOfIndividualsToInclude);
  std::set<int>* snpMissingData = new std::set<int>();

  if(mode == SNPMAJOR){
    readSNPModeSNPMajor(snp, *snpMissingData, *snpDataOriginal);
  }else if(mode == INDIVIDUALMAJOR){
    readSNPModeIndividualMajor(snp, *snpMissingData, *snpDataOriginal);
  }else{
    std::ostringstream os;
    os << "No mode set for the file " << bedFileStr << std::endl;
    const std::string& tmp = os.str();
    throw FileReaderException(tmp.c_str());
  }

  return snpVectorFactory->constructSNPVector(snp, snpDataOriginal, snpMissingData);
}

template<>
Container::SNPVector<Container::DeviceVector>* BedReader<Container::DeviceVector>::readSNP(SNP& snp) const{
  const int numberOfIndividualsToInclude = personHandler.getNumberOfIndividualsToInclude();
  Container::PinnedHostVector* snpDataOriginal = new Container::PinnedHostVector(numberOfIndividualsToInclude);
  std::set<int>* snpMissingData = new std::set<int>();

  if(mode == SNPMAJOR){
    readSNPModeSNPMajor(snp, *snpMissingData, *snpDataOriginal);
  }else if(mode == INDIVIDUALMAJOR){
    readSNPModeIndividualMajor(snp, *snpMissingData, *snpDataOriginal);
  }else{
    std::ostringstream os;
    os << "No mode set for the file " << bedFileStr << std::endl;
    const std::string& tmp = os.str();
    throw FileReaderException(tmp.c_str());
  }

  return snpVectorFactory->constructSNPVector(snp, snpDataOriginal, snpMissingData);
}

template<typename Vector>
void BedReader<Vector>::readSNPModeSNPMajor(SNP& snp, std::set<int>& snpMissingData,
    Container::HostVector& snpDataOriginal) const{
  std::ifstream bedFile;
  const int snpPos = snp.getPosition();
  const int numberOfIndividualsToInclude = personHandler.getNumberOfIndividualsToInclude();

  char buffer[numberOfBytesPerRow];
  long int seekPos = headerSize + numberOfBytesPerRow * snpPos;

  openBedFile(bedFile);
  bedFile.seekg(seekPos);
  bedFile.read(buffer, numberOfBytesPerRow);
  if(!bedFile){
    std::ostringstream os;
    os << "Problem reading SNP " << snp.getId().getString() << " from bed file " << bedFileStr << std::endl;
    const std::string& tmp = os.str();
    throw FileReaderException(tmp.c_str());
  }

  closeBedFile(bedFile);

  int individualNumberIncMissing = 0;
  int individualNumberExMissing = 0;
  int individualNumberAll = 0;

  //Go through all the bytes
  for(int byteNumber = 0; byteNumber < numberOfBytesPerRow; ++byteNumber){
    char currentByte = buffer[byteNumber];

    int numberOfBitPairsPerByte;
    if(byteNumber == (numberOfBytesPerRow - 1)){ //Are we at the last byte
      numberOfBitPairsPerByte = (8 - numberOfUninterestingBitsAtEnd) / 2;
    }else{
      numberOfBitPairsPerByte = 4;
    }

    //Go through all the pairs of bit in the byte
    for(int bitPairNumber = 0; bitPairNumber < numberOfBitPairsPerByte; ++bitPairNumber){
      //It's in reverse due to plinks format that has the bits in each byte in reverse
      int posInByte = 2 * bitPairNumber;
      bool firstBit = getBit(currentByte, posInByte);
      bool secondBit = getBit(currentByte, posInByte + 1);

      const Person& person = personHandler.getPersonFromRowAll(individualNumberAll);
      if(person.getInclude()){

        //If we are missing the genotype for at least one individual(that should be included) we excluded the SNP
        if(firstBit && !secondBit){
          //Store the number for the SNP, it's already skipped in the SNP data. However it's needed to know what to skip in the other data(environment, etc)
          snpMissingData.insert(snpMissingData.end(), individualNumberIncMissing);
        }else{
          //Store the genotype as 0,1,2 until we can recode it. We have to know the risk allele before we can recode.
          if(!firstBit && !secondBit){
            //Homozygote primary
            snpDataOriginal(individualNumberExMissing) = 0;
          }else if(!firstBit && secondBit){
            //Hetrozygote
            snpDataOriginal(individualNumberExMissing) = 1;
          }else if(firstBit && secondBit){
            //Homozygote secondary
            snpDataOriginal(individualNumberExMissing) = 2;
          }

          ++individualNumberExMissing;
        }/* if check missing data */

        ++individualNumberIncMissing;
      }

      individualNumberAll++;
    }/* for bitPairNumber */

  }/* for byteNumber */
}

template<typename Vector>
void BedReader<Vector>::readSNPModeIndividualMajor(SNP& snp, std::set<int>& snpMissingData,
    Container::HostVector& snpDataOriginal) const{
  //TODO Bed file individual major mode
  throw FileReaderException("Individual major mode is not supported.");
}

// Position in range 0-7
template<typename Vector>
bool BedReader<Vector>::getBit(unsigned char byte, int position) const{
  return (byte >> position) & 0x1; //Shift the byte to the right so we have bit at the position as the last bit and then use bitwise and with 00000001
}

template<typename Vector>
void BedReader<Vector>::openBedFile(std::ifstream& bedFile) const{
  bedFile.open(bedFileStr, std::ifstream::binary);
  if(!bedFile){
    std::ostringstream os;
    os << "Problem opening bed file " << bedFileStr << std::endl;
    const std::string& tmp = os.str();
    throw FileReaderException(tmp.c_str());
  }
}

template<typename Vector>
void BedReader<Vector>::closeBedFile(std::ifstream& bedFile) const{
  if(bedFile.is_open()){
    bedFile.close();
  }
  if(!bedFile){
    std::ostringstream os;
    os << "Problem closing bed file " << bedFileStr << std::endl;
    const std::string& tmp = os.str();
    throw FileReaderException(tmp.c_str());
  }
}

} /* namespace FileIO */
} /* namespace CuEira */
