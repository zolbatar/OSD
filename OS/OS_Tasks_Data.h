#pragma once
#include <string>
#include <vector>
#include <map>
#include "../Compiler/Tokeniser/Types.h"

struct DataElement
{
    ValueType type;
    int64_t iv = 0;
    double rv = 0.0;
    int64_t sv = 0;
};

class OSDTaskData
{
  public:
    void AddDataElement(DataElement de);
    DataElement *GetDataElement();
    void AddDataLabel(std::string s);
    void RestoreDataLabel(std::string s);

  private:
    size_t data_element_index = 0;
    std::map<std::string, size_t> data_labels;
    std::vector<DataElement> data_elements;
};