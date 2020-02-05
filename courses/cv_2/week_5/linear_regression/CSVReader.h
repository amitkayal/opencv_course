#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
//#include <boost/algorithm/string.hpp>

// To read more about boost string algorithm:  https://theboostcpplibraries.com/boost.stringalgorithms

/*
 * A class to read data from a csv file.
 */
class CSVReader
{
  std::string fileName;
  std::string delimeter;
 
public:
  CSVReader(std::string filename, std::string delm = ",") :
      fileName(filename), delimeter(delm)
  { }
 
  // Function to fetch data from a CSV File
  std::vector<std::vector<std::string> > getData();
  
private:
  void Tokenize(std::string str, std::string delim, 
      std::vector<std::string> &out);
};
 
/*
* Parses through csv file line by line and returns the data
* in vector of vector of strings.
*/
std::vector<std::vector<std::string> > CSVReader::getData()
{
  std::ifstream file(fileName);
 
  std::vector<std::vector<std::string> > dataList;
 
  std::string line = "";
  // Iterate through each line and split the content using delimeter
  while (getline(file, line))
  {
    std::vector<std::string> vec;
    //boost::algorithm::split(vec, line, boost::is_any_of(delimeter));
    
    Tokenize(line, delimeter, vec);
    
    dataList.push_back(vec);
  }
  // Close the File
  file.close();
 
  return dataList;
}

void CSVReader::Tokenize(std::string str, std::string delim,
			std::vector<std::string> &out)
{
	while(str.size())
  {
    int index = str.find(delim);
    if (index != std::string::npos)
    {
      out.push_back(str.substr(0,index));
      str = str.substr(index + delim.size());
      if (str.size()==0)
        out.push_back(str);
    } else
    {
      out.push_back(str);
      str = "";
    }
  }  
}