#include <string>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "basicGit.h"
#include <vector>
#include <map>

        void basicGit::init() {
            // Create Folder
            std::filesystem::create_directory(".basicgit");
            std::filesystem::create_directory(".basicgit/objects");
            std::filesystem::create_directory(".basicgit/refs");

            //Subdirectories
            std::filesystem::create_directory(".basicgit/refs/heads");
            std::filesystem::create_directory(".basicgit/refs/tags");


        }

        void basicGit::add(const std::string& fileName) { // create and store blob object
        std::ifstream file(fileName, std::ios::binary);
        if(!file) {
            std::cout << "Error Opening File" << std::endl;
            return;
        }
        std::ostringstream ss;
        ss << file.rdbuf();
        std::string content = ss.str();

        std::string header = "blob " + std::to_string(content.size()) + '\0';
        std::string blobData = header + content;


            //SHA-1 Hash of Content
            std::string hashedFile = hash(blobData);

            std::filesystem::create_directory(".BasicGit/objects/" + hashedFile.substr(0,2));
            
            //Write Blob file to BasicGit/object/hast
            std::string blobPath = ".BasicGit/objects/" + hashedFile.substr(0,2) + "/" + hashedFile.substr(2);

            std::ofstream blobFile(blobPath, std::ios::binary);
            blobFile.write(blobData.data(), blobData.size());
            blobFile.close();
            //Update index File (file path/hash of blob)
            updateIndex(hashedFile, fileName);
        }

        std::string basicGit::hash(const std::string& fileName){
            std::string Encoded = "";
            std::vector<int> Array(fileName.size());
            //String to ASCI
            for(size_t i=0;i<fileName.size();i++){
                Array[i]= static_cast<int>(fileName[i]);
            }

            //ASCI to Binary
           std::vector<std::string> binary(fileName.size());
            for(size_t i=0;i<fileName.size();i++){
                binary[i] = binaryConverter(Array[i]);
            }

            //Join and Append a 1
            std::string total = "";
            for(size_t i=0;i<fileName.size();i++){
                total += binary[i];
            }
            total += "1";

            //Pad Binary with zeroes until its length 512 mod 448
            while(size(total) % 512 != 448){
                total += "0";
            }

            //Append Length
            const uint64_t lengthArray = fileName.size()*8; 
            std::string lengthBis = _64BitBinary(lengthArray);
            total += lengthBis;

            //Break message into 512-bit chunks
            std::vector<std::string> chunks;
            for (size_t i = 0; i < total.size(); i += 512) {
                chunks.push_back(total.substr(i, 512));
            }
            //Break each chunk into subarray of 16 32-bit words
            std::vector<std::vector<std::string>> totalSubArray;
            for(const auto& chunk: chunks) {
                std::vector<std::string> subarray;
                for (size_t j = 0; j < 512; j += 32) {
                    subarray.push_back(chunk.substr(j,32));
                }
                totalSubArray.push_back(subarray);
            }
            //Loop through each chunk array of 16 32-bit words and 
            //extend array to 80 words using bitwise opperations
            std::vector<std::vector<uint32_t>> expandedChunks;

            for(const auto& array: totalSubArray){
                std::vector<uint32_t> words80(80);
                for(size_t i=0;i<16;i++){
                    words80[i] = static_cast<uint32_t>(std::stoul(array[i],nullptr,2));
                }

                for(auto i=16;i<=79;i++){
                    auto wordA = words80[i-3];
                    auto wordB = words80[i-8];
                    auto wordC = words80[i-14];
                    auto wordD = words80[i-16];

                    auto xorC = wordA ^ wordB ^ wordC ^ wordD;

                    auto newWord = ((xorC << 1) | xorC >> (32-1));

                    words80[i] = newWord;
                }
                expandedChunks.push_back(words80);

            }

            //Initialize Hash Values
            uint32_t h0 = 0x67452301;
            uint32_t h1 = 0xEFCDAB89;
            uint32_t h2 = 0x98BADCFE;
            uint32_t h3 = 0x10325476;
            uint32_t h4 = 0xC3D2E1F0;

            //Looping through each chunk: bitwise operations and variable reassignment
            for(auto word80 : expandedChunks){
                uint32_t a = h0;
                uint32_t b = h1;
                uint32_t c = h2;
                uint32_t d = h3;
                uint32_t e = h4;
                for(auto j=0;j<80;j++){
                    uint32_t f,k;

                    if(j<20){
                        f = (b & c) | ((~b) & d);
                        k = 0x5A827999;
                    } else if (j < 40) {
                        f = b ^ c ^ d;
                        k = 0x6ED9EBA1;
                    } else if (j < 60) {
                        f = (b & c) | (b & d) | (c & d);
                        k = 0x8F1BBCDC;
                    } else {
                        f = b ^ c ^ d;
                        k = 0xCA62C1D6;
                    }


                    auto temp = ((a << 5) | (a >> (32 - 5))) + f + e + k + word80[j];
                    //(x << n) | (x >> (32 - n))
                    e=d;
                    d=c;
                    c= (b << 30) | (b >> (2));
                    b=a;
                    a=temp;
                }
                h0 += a;
                h1 += b;
                h2 += c;
                h3 += d;
                h4 += e;
                
            }
            //Convert each of the five resulting variables to hexadecimal
            std::stringstream ss;
            ss << std::hex << std::setfill('0');
            ss << std::setw(8) << h0;
            ss << std::setw(8) << h1;
            ss << std::setw(8) << h2;
            ss << std::setw(8) << h3;
            ss << std::setw(8) << h4;
            //join hexadecimal together and return
            std::string finalHash = ss.str();
            return finalHash;
        }

        std::string basicGit::binaryConverter(int value){
            std::string result;
            if(value==0){
               return "00000000";  
            }
            for (int i = 7; i >= 0; --i) {
                int shifted = value >> i;
                int modifiedValue = shifted & 1;
                if(modifiedValue!=0){
                    result += "1";
                } else {
                    result += "0";
                }

            }
            return result;
        }

        std::string basicGit::_64BitBinary(uint64_t value) {
            std::string result;
            for (int i = 63; i >= 0; --i) {
                int shifted = value >> i;
                int modifiedValue = shifted & 1;
                if(modifiedValue!=0){
                    result += "1";
                } else {
                    result += "0";
                }
            }
            return result;
        }
        
        void basicGit::updateIndex(std::string hashedFile, std::string fileName){
            std::map<std::string, std::string> file_Content;
            std::ifstream indexFile(".BasicGit/index");
            std::string line_Hash, line_Name;
            while(indexFile >> line_Hash >> line_Name ){
                file_Content.insert({line_Name,line_Hash});
            }
            indexFile.close();
            file_Content[fileName] = hashedFile;

            //Remake Index File
            std::ofstream newIndexFile(".BasicGit/index");
            for(auto files : file_Content){
                newIndexFile << files.second << " " << files.first << "\n";
            }
            newIndexFile.close();
        }

        void basicGit::status(){
            std::ifstream file(".basicgit/index");
            
            std::string data;

            std::vector<std::string> unstaged;            
            std::vector<std::string> staged;
            std::vector<std::string> untracked;
            std::string hashName;
            std::string fileName;
            
            while(file >> hashName >> fileName){

                std::ifstream file(fileName, std::ios::binary);
                std::ostringstream ss;
                ss << file.rdbuf();
                std::string content = ss.str();

                std::string header = "blob " + std::to_string(content.size()) + '\0';
                std::string blobData = header + content;
                std::string newHashcode = hash(blobData);

                if(hashName == newHashcode){
                    staged.push_back(fileName);
                } else if (hashName != newHashcode){
                    unstaged.push_back(fileName);
                }
            }

            std::string path = ".";
            for (auto it = std::filesystem::recursive_directory_iterator(path); it != std::filesystem::recursive_directory_iterator(); ++it) {
            const auto& entry = *it;

            if (entry.is_directory() && entry.path().filename().string().rfind(".", 0) == 0) {
                it.disable_recursion_pending(); // Prevent going deeper into this directory
                continue;
            } else if (entry.is_regular_file()) {
                std::string newFile = entry.path().filename().string();
                bool exists= false;
                for(size_t i=0;i<staged.size();i++){
                    if(staged.at(i) ==newFile){
                        exists= true;
                    }
                }
                for(size_t i=0;i<unstaged.size();i++){
                    if(unstaged.at(i) ==newFile){
                        exists= true;
                    }
                }
                if(exists==false){
                    untracked.push_back(newFile);
                }
            }
        }
            
            //Output
            std::cout << "Changes to be committed: " << std::endl;
            for(size_t i=0;i<staged.size();i++){
                std::cout << "    "<< "modified: " << staged.at(i) << std::endl;
            }
            std::cout << "\nChanges not staged for Commit: " << std::endl;
            for(size_t i=0;i<unstaged.size();i++){
                std::cout << "    "<< "modified: " << unstaged.at(i) << std::endl;
            }

            std::cout << "\nUntracked Files:" << std::endl;
            for(size_t i=0;i<untracked.size();i++){
                std::cout  << "    " << untracked.at(i) << std::endl;
            }
            
           
        }

        void basicGit::commit(const std::string& commitMessage) { // create tree + commit
            // Code to commit changes
        }

