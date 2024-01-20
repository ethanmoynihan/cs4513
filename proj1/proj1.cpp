#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

/* Handle information about the file for formating/exploring further directories */
struct fileData {
    std::string name;           //name of the file
    int fileType;               //type of the file i.e executable/directory
    int fileSize;               //File size in 1-7 range
    int age;                    //File age in 1-7 range 
};

fileData checkFileAttr(std::string file, std::string filepath, bool age = false){
    //Age flag is included since it effects actual data pulled
    fileData retData;
    retData.name = file;
    //open file and find rest of information

    return  retData;
};

std::string fileFormat(fileData information){
    //Handles file information and returns information string 
    std::string retString = information.name;
    //Check the information type
    switch (information.fileType){
        case 0:
            //directory
            retString.append("/ ");
            break;
        case 1:
            //symbolic link
            retString.append("@ ");
            break;
        case 2:
            //executable
            retString.append("* ");
            break;
        default:
            retString.append(" ");
            break;
    };
    //Check the file size
    switch (information.fileSize){
        case 1:
            retString.append("# ");
            break;
        case 2:
            retString.append("## ");
            break;
        case 3:
            retString.append("### ");
            break;
        case 4:
            retString.append("#### ");
            break;
        case 5:
            retString.append("##### ");
            break;
        case 6:
            retString.append("###### ");
            break;
        case 7:
            retString.append("####### ");
            break;
        
        default:
            //if null unable to fid size (unlikely)
            break;
    }
    //Check the file age and display
    switch (information.age){
        case 1:
            retString.append(". ");
            break;
        case 2:
            retString.append(".. ");
            break;
        case 3:
            retString.append("... ");
            break;
        case 4:
            retString.append(".... ");
            break;
        case 5:
            retString.append("..... ");
            break;
        case 6:
            retString.append("...... ");
            break;
        case 7:
            retString.append("....... ");
            break;
        
        default:
            //if null unable to fid size (unlikely)
            break;
    }
    return retString;
};

std::string fileFormatHTML(fileData information){
    //add flags for handle 
    //todo implement
    return information.name;
};


std::vector<std::string> handle_dir(std::string name, std::string filepath, std::string indentation = "    ", int explore_depth = 1, bool alphabetical = 0, bool fileType = false){
    //serch through a directory recursively with the set of values
    std::vector<std::string> retVector;
    //add these before exploring the dir in main to handle indentation retVector.push_back(name +"/");

    //Check to see if it should keep being recursive
    
    if(explore_depth < 1){

    }
    return retVector;
};

int main(int argc, char* argv[]){ 
    int num_indents = 4;
    int explore_depth = 2;
    bool alphabetical = false;
    bool fileType = false;
    std::vector<int> fileList;
    
    for(int ind = 1; ind < argc; ind++){
        //print arg for testing
        std::cout << "Argument " << ind << ": " << argv[ind] << std::endl;
        std::string temp = argv[ind];
        
        //check to see if arg is a flag
        size_t finder = temp.find('-');
        if(finder != std::string::npos){
            // if a dash is provided it is an argument
            char flag = argv[ind][finder+1];
            switch (flag){
                case 'i':
                    // valid rang 1-8 and def of 4
                    finder = temp.find('=');
                    //if a value is specified extract and set num_indents
                    if(finder != std::string::npos){    
                        std::string numString = temp.substr(finder);
                        try {
                            num_indents = std::stoi(numString);
                            if (num_indents < 1 || num_indents >8){
                                //if given an invalid amount of indents revert to base case
                                num_indents = 4;
                            }
                            //prints for testing
                            std::cout << "Original String: " << numString << std::endl;
                            std::cout << "Substring: " << numString << std::endl;
                            std::cout << "Converted Integer: " << num_indents << std::endl;
                        } catch (const std::invalid_argument& e) {
                            std::cerr << "Conversion error: " << e.what() << std::endl;
                        } catch (const std::out_of_range& e) {
                            std::cerr << "Conversion error: " << e.what() << std::endl;
                            }
                    }
                    break;
                case 'a':
                    //file age instead of last modification
                    break;
                case 'd':
                //generate output in HTML using font size and shading to represent file size and age
                    break;
                case 'h':
                    break;
                case 's':
                    //sort alphabetical
                    alphabetical = true;
                    break;
                case 't':
                    //Show file type 
                    fileType = true;
                    break;
                
            }
        }
        else{  
            // NEED a list of files to check out
            fileList.push_back(ind);
        }
    }
    for (int value : fileList) {
        std::string file  = argv[value];
        std::string filePath = "./" + file;

        fs::path filePath(filePath);
        
        try {
        // Check if the file exists
        if (fs::exists(filePath)) {
            // check if its a directory, 
            std::cout << "File Path: " << filePath << std::endl;
            std::cout << "File Size: " << fs::file_size(filePath) << " bytes" << std::endl;
            std::cout << "Last Write Time: " << fs::last_write_time(filePath) << std::endl;
            std::cout << "Is Directory: " << std::boolalpha << fs::is_directory(filePath) << std::endl;
        } else {
            std::cerr << "File does not exist." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
        
    }

    //actual processsing

    return 0;
 };