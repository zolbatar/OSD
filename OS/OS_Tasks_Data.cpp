#include "OS_Tasks_Data.h"

void OSDTaskData::AddDataElement(DataElement de)
{
    data_elements.push_back(std::move(de));
    data_element_index++;
}

DataElement *OSDTaskData::GetDataElement()
{
    if (data_element_index >= data_elements.size())
        data_element_index = 0;
    auto de = &data_elements[data_element_index];
    data_element_index++;
    return de;
}

void OSDTaskData::AddDataLabel(std::string s)
{
    data_labels.insert(std::make_pair(s, data_element_index));
}

void OSDTaskData::RestoreDataLabel(std::string s)
{
    auto f = data_labels.find(s);
    if (f == data_labels.end())
        return;
    data_element_index = f->second;
}
