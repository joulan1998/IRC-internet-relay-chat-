#include <iostream>
#include <sstream>
#include <string>
#include <vector>

std::vector<std::string> split(std::string &str, char delem, bool flag)
{
    (void)delem;
    std::vector<std::string> st;
    std::istringstream iss(str, delem);
    std::string cmd;

    if(flag == false)
    {
        while(iss >> cmd)
        {
            st.push_back(cmd);
        }
    }
    else
    {
        size_t start = 0;
        size_t pos = str.find(delem);
        while (pos != std::string::npos)
        {
            st.push_back(str.substr(start, pos - start));
            start = pos+ 1;
            pos = str.find(delem, start);
        }
        st.push_back(str.substr(start));
    }
    return st;
}
int main()
{
    std::string str = "join #ch1,#ch2,,#ch3,#ch4,#ch5 k1,k2,k3,k4,k5";

    std::vector<std::string> str2 = split(str, ' ', false);
    for (size_t i = 0; i < str2.size(); i++)
    {
        std::cout<< str2[i]<<std::endl;
    }
        std::cout<< "-------------------------------------------------"<<std::endl;
    std::vector<std::string> str3 = split(str2[1], ',', true);
    for (size_t i = 0; i < str3.size(); i++)
    {
        std::cout<< "{"<<  str3[i]<< "}"<<std::endl;
    }    

}