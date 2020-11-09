/*************************************************************************
*
* VULCANFORMS CONFIDENTIAL
* __________________
*  Copyright, VulcanForms Inc.
*  [2016] - [2020] VulcanForms Incorporated
*  All Rights Reserved.
*
*  "VulcanForms", "Vulcan", "Fusing the Future"
*       are trademarks of VulcanForms, Inc.
*
* NOTICE:  All information contained herein is, and remains
* the property of VulcanForms Incorporated and its suppliers,
* if any.  The intellectual and technical concepts contained
* herein are proprietary to VulcandForms Incorporated
* and its suppliers and may be covered by U.S. and Foreign Patents,
* patents in process, and are protected by trade secret or copyright law.
* Dissemination of this information or reproduction of this material
* is strictly forbidden unless prior written permission is obtained
* from VulcanForms Incorporated.
*/

/*
* Prgram to convert Excel .csv file into LCR ascii and g-code files
* Here is an example of Excel file:
Position,Laser Off/On,Power,Duty Cycle,Period,SOT,EOT,"From X, mm","From Y, mm","To X, mm","To Y, mm","Speed, mm/sec"
100,0,,,,1,,414.265,261.820,598.575,261.820,600
200,1,100,50,125,,,598.645,262.880,414.265,262.880,600
300,1,100,25,125,,,,,,,
300,1,200,,,,,,,,,
400,0,,,,1,1,,,,,
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

string processLineForLCR(string inputLine);
string addLeadingZeros(const string str, int size);
vector<string> processLineForGCode(string inputLine);
vector<string> split(const string& str, const string& delim);

// Convert LCR Excel .csv file into LCR ascii command file 

static int lineNumber = 0;

string Nfield(int lineNumber)
{
    string num = to_string(lineNumber);
    int addZeros = 4 - num.length();
    string result = num;
    if (addZeros > 0)
        result = std::string(addZeros, '0').append(num);
    return "N" + result;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cout << "Usage: LCRExelConverter <csv file>" << endl;
        exit(1);
    }
    string fName(argv[1]);
    ifstream iFile(fName);
    ofstream oLCRFile(fName.substr(0, fName.length() - 3) + "txt");
    ofstream oGCodeFile(fName.substr(0, fName.length() - 3) + "cnc");
    string str;
    getline(iFile,str);       // Skip first line in the file
    while (!iFile.eof())
    {
        getline(iFile, str);
        if (str.length() != 0)
        {
            string outLCR = processLineForLCR(str);
            oLCRFile << outLCR << ";";
            vector<string> outCNC = processLineForGCode(str);
            for (size_t i = 0; i < outCNC.size(); i++)
            {
                oGCodeFile << outCNC[i];
                oGCodeFile << std::endl;
            }
        }
    }
    // Write tail
    lineNumber++;
    oGCodeFile << Nfield(lineNumber) + " M22"; // M11
    oGCodeFile << std::endl;
    lineNumber++;
    oGCodeFile << Nfield(lineNumber) + " M30";
    oGCodeFile << std::endl;
}

string processLineForLCR(string inputLine)
{
    string delim = ",";
    const char* absent = "lwdpse";
    const char* present = "LWDPSE";
    vector<string> tokens = split(inputLine, delim);
    string command;
    string args;

    for (unsigned int i = 1; i<tokens.size(); i++)
    {
        if (tokens[i].size() == 0)
            command += absent[i - 1];
        else
        {
            if (i != 1)
            {
                command += present[i - 1];
                if (i > 1 && i < 5)       // Laser on/off, SOT, EOT commands doe not need argument to be added
                    args += "," + addLeadingZeros(tokens[i], 3);
            }
            else
            {
                command += tokens[1] == "1" ? present[0] : absent[0];
            }
        }
    }
    return command + "," + addLeadingZeros(tokens[0],8) + args;
}

vector<string> processLineForGCode(string inputLine)
{
    static string fromX = "0.0";
    static string fromY = "0.0";
    static string toX = "0.0";
    static string toY = "0.0";
    static int speedMMsec = 0;
    static bool firstGCode = true;
    string delim = ",";
    vector<string> result;
    vector<string> tokens = split(inputLine, delim);
    bool generateResult = false;
    for (unsigned int i = 7; i < tokens.size(); i++)
    {
        if (tokens[i].size() == 0)
            continue;
        else
        {
            generateResult = true;
            if (i == 7)
                fromX = tokens[i];
            if (i == 8)
                fromY = tokens[i];
            if (i == 9)
                toX = tokens[i];
            if (i == 10)
                toY = tokens[i];
            if (i == 11)
            {
                int mmInSec = std::stoi(tokens[i]);
                speedMMsec = 60 * mmInSec;
            }
        }
    }
    if (generateResult)
    {
        lineNumber++;
        string s1 = Nfield(lineNumber) + " G90 G01 X" + fromX + " Y" + fromY + " F"
            + to_string(speedMMsec);
        if (!firstGCode)
            s1 += " M22"; // M11
        lineNumber++;
        string s2 = Nfield(lineNumber); // + M10
        if (fromY == toY)
            s2 += " M20";
        else if (fromX == toX)
            s2 += " M21";
        else  //Error: Diagonal move is not allowed
        {
            cerr << endl;
            cerr << "File Error: Diagonal move is not allowed" << endl;
            cerr << endl;
            exit(1);
        }

        lineNumber = (lineNumber / 10 + 1) * 10;
        string s3 = Nfield(lineNumber) + " G90 G01 X" + toX + " Y" + toY + " F"
            + to_string(speedMMsec);
        result.push_back(s1);
        result.push_back(s2);
        result.push_back(s3);
    }
    firstGCode = false;
    return result;
}

vector<string> split(const string& str, const string& delim)
{
    vector<string> tokens;
    if (str.length() == 0)
        return tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == string::npos) pos = str.length();
        string token = str.substr(prev, pos - prev);
        tokens.push_back(token);
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    if (str.substr(str.length() - 1, 1) == ",")
        tokens.push_back(string(""));
    return tokens;
}

string addLeadingZeros(const string str, int size)
{
    string out;
    for (unsigned int i = 0; i < size - str.size(); i++)
        out += "0";
    out += str;
    return out;
}