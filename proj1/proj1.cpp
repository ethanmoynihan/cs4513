#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <filesystem>
#include <sys/stat.h>
#include <ctime>
#include <algorithm>
namespace fs = std::filesystem;

/* Handle information about the file for formating/exploring further directories */
struct fileData {
    std::string name;           //name of the file
    // SET AS INT FOR USE IN NORMAL OUTPUT AND HTML
    int fileType;               //type of the file i.e executable/directory
    int fileSize;               //File size in 1-7 range
    int age;                    //File age in 1-7 range 
    int depth;                  //Multiply by depth for indentation
    std::string extraInfo = "";
};

//time of running the file search for consistency 
time_t currentTime;

fileData checkFileAttr(std::string file, int depth, bool age = false, bool extraInfo = false){
    //Age flag is included since it effects actual data pulled
    fileData retData;
    struct stat fileStat;
    if(stat(file.c_str(), &fileStat) == 0){
        size_t filePos = file.find_last_of('/');
        if(filePos != std::string::npos){
            retData.name  = file.substr(filePos+1);
        }
        else{
            retData.name = file;    
        }
        retData.depth = depth;
        //Check the filetype for specific end var
        
        if(S_ISDIR(fileStat.st_mode)){
            retData.fileType = 0;
        }else if(S_ISLNK(fileStat.st_mode)){
            retData.fileType = 1;
        }else if(S_IEXEC & fileStat.st_mode){
            retData.fileType = 2;
        }else {
            //regular file
            retData.fileType = 3;
            if(extraInfo){
                char buffer[100];
                FILE *fp;
                std::string cmnd = "file " + file;
                fp = popen(cmnd.c_str(), "r");
                while (fgets(buffer, sizeof(buffer), fp) != NULL) {
                    ;
                }
                std::string cmp = buffer;
                size_t cmpPos = cmp.find(':');
                if(cmpPos != std::string::npos){
                    std::string output;
                    for (char c : cmp.substr(cmpPos+1)) {
                        if (c != '\n') {
                            output += c;
                        }
                    }
                    retData.extraInfo.append(output);
                }
                pclose(fp);
            }
            }
        //Check the fileSize for specific end var
        if( fileStat.st_size <100){
            retData.fileSize = 1;
        }else if (fileStat.st_size >=100 && fileStat.st_size <1000) {
            retData.fileSize = 2;
        }else if (fileStat.st_size >=1000 && fileStat.st_size <10000) {
            retData.fileSize = 3;
        }else if (fileStat.st_size >=10000 && fileStat.st_size <100000) {
            retData.fileSize = 4;
        }else if (fileStat.st_size >=100000 && fileStat.st_size <1000000) {
            retData.fileSize = 5;
        }else if (fileStat.st_size >=1000000 && fileStat.st_size <10000000) {
            retData.fileSize = 6;
        }else if (fileStat.st_size >=10000000) {
            retData.fileSize = 7;
        }
        //handle diffence in age after
        // if age true use the last access time instead of last modified
        if(age){
            //last accessed
            fileStat.st_atim;
            time_t lastAccess = currentTime - fileStat.st_mtim.tv_sec;
             if(lastAccess < 60){
                //in seconds
                retData.age = 0;
            } else if (lastAccess >= 60 && lastAccess < 3600){
                //minutes
                retData.age = 1;
            }else if (lastAccess >=3600 && lastAccess < 86400){
                //hours
                retData.age = 2;

            }else if (lastAccess >=604800 && lastAccess < 1814400){
                //days
                retData.age = 3;

            }else if (lastAccess >=1814400 && lastAccess < 2592000){
                //weeks
                retData.age = 4;

            }else if (lastAccess >=2592000 && lastAccess < 3110400){
                //months
                retData.age = 5;
            }else if (lastAccess >=3110400){
                //years
                retData.age = 6;
            }
        }
        else{
            //last modified
            time_t lastMod = currentTime - fileStat.st_mtim.tv_sec;
            if(lastMod < 60){
                //in seconds
                retData.age = 0;
            } else if (lastMod >= 60 && lastMod < 3600){
                //minutes
                retData.age = 1;
            }else if (lastMod >=3600 && lastMod < 86400){
                //hours
                retData.age = 2;
            }else if (lastMod >=604800 && lastMod < 1814400){
                //days
                retData.age = 3;
            }else if (lastMod >=1814400 && lastMod < 2592000){
                //weeks
                retData.age = 4;
            }else if (lastMod >=2592000 && lastMod < 3110400){
                //months
                retData.age = 5;
            }else if (lastMod >=3110400){
                //years
                retData.age = 6;
            }
        }

        return  retData;
    }
    else{
        std::cerr <<"The file you are looking for does not exist in " << file << " directory." << std::endl;
        return retData;
    }
};

std::string fileFormat(fileData information){
    //Handles file information and returns information string
    std::string retString = "";
    size_t filePos = information.name.find_last_of('/');
    if(filePos != std::string::npos){
        retString.append(information.name.substr(filePos+1));
    }
    else{
        retString.append(information.name);
    }
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
        case 0:
            retString.append(". ");
            break;
        case 1:
            retString.append(".. ");
            break;
        case 2:
            retString.append("... ");
            break;
        case 3:
            retString.append(".... ");
            break;
        case 4:
            retString.append("..... ");
            break;
        case 5:
            retString.append("...... ");
            break;
        case 6:
            retString.append("....... ");
            break;
        
        default:
            //if null unable to fid size (unlikely)
            break;
    }
    
    retString.append(information.extraInfo);
    return retString;
};

std::string fileFormatHTML(fileData information, std::string num_indents = "    "){
    //add flags for handle 
    //todo implement
    //might just 
    std::string retString = "";
    
    switch (information.fileSize)
    {
        case 1:
            retString.append("<p style=\"font-size:10px");
            break;
        case 2:
            retString.append("<p style=\"font-size:15px");
            break;
        case 3:
            retString.append("<p style=\"font-size:20px");
            break;
        case 4:
            retString.append("<p style=\"font-size:25px");
            break;
        case 5:
            retString.append("<p style=\"font-size:30px");
            break;
        case 6:
            retString.append("<p style=\"font-size:35px");
            break;
        case 7:
            retString.append("<p style=\"font-size:40px");
            break;
        
        default:
            retString.append("<p style=\"font-size:10px");
            break;
    }
    if (information.fileSize <=7 && information.fileSize >=1){
        retString.append(";white-space: pre;");
    }

    switch (information.age){

        case 0:
            retString.append("color:black\">");
            break;
        case 1:
            retString.append("color:DimGray\">");
            break;
        case 2:
            retString.append("color:Gray\">");
            break;
        case 3:
            retString.append("color:DarkGray\">");
            break;
        case 4:
            retString.append("color:Gray\">");
            break;
        case 5:
            retString.append("color:LightGray\">");
            break;
        case 6:
            retString.append("color:Gainsboro\">");
            break;
        
        default:
            //if null unable to fid size (unlikely)
            retString.append(">");
            break;
    }
    for(int i = 0; i<information.depth;i++){
                    retString.append(num_indents);
                }
    retString.append(information.name);
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
    retString.append("</p>");
    
    return retString;
};

int alphabetize(std::vector<fileData> *unsorted, fileData *fileToInsert){
    //implement vector sorting by name, ignore any objects with a greater depth return place to put
    int retVal = 0;
    for(fileData entry : *unsorted){
        if(fileToInsert->depth == entry.depth){
            //check char value for the first char, if equal check second and so on
            //choose the shorter of the words
            int compare = fileToInsert->name.compare(entry.name);
            if(compare > 0){
                retVal ++;
            }else{
                //same or earlier
                return retVal;
            }            
        }
        else if(fileToInsert->depth < entry.depth){
            //if the depth of the entry is higher it is in a different directory so skip
            retVal++;
            continue;
        }else{
            //hits a value of a lower depth i.e different outside of directory
            return retVal;
        }

    }

    return retVal;
};


std::vector<fileData> handle_dir(std::string name, int explore_depth, int current_depth, bool alphabetical = false, bool fileType = false, bool age = false){
    //serch through a directory recursively with the set of values
    std::vector<fileData> retVector;
    //add these before exploring the dir in main to handle indentation retVector.push_back(name +"/");
    fileData temp;
    

    if(explore_depth >= 1){
        DIR * startingdir = opendir(name.c_str());
        struct dirent *entry;
        int place = 0;
                
        while ((entry = readdir(startingdir)) != NULL) {
            if( strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
                continue;
            }
            std::string path = name;
            path.append("/");
            path.append(entry->d_name);
            temp = checkFileAttr(path, current_depth, age, fileType);
            //find place to insert entry or directory
            if(alphabetical){
                place = alphabetize(&retVector, &temp);
            }

            if(temp.fileType == 0){
                //Handle exploration of directory here, explore depth decreases while current depth increases
                
                std::vector<fileData> tempVec = handle_dir(path, explore_depth-1, current_depth + 1, alphabetical,  fileType, age);
                if(alphabetical){
                    retVector.insert(retVector.begin() + place, temp);
                    retVector.insert(retVector.begin() + place + 1, tempVec.begin(), tempVec.end());
                }else{
                    retVector.insert(retVector.end(), temp);
                    retVector.insert(retVector.end(), tempVec.begin(), tempVec.end());
                }
            }
            else{
                if(alphabetical){
                    retVector.insert(retVector.begin() + place, temp);
                }else{
                    retVector.insert(retVector.end(), temp);
                }
            }
            
        }
        closedir(startingdir);
    }
    if( strcmp(name.c_str(), ".") == 0){
        //edge case of current directory for no file list
        
    }else{
        //Create a value and add to return vector as the first value, even for alphabetical
        //temp = checkFileAttr(name, current_depth, age);
        //retVector.push_back(temp);
    }

    return retVector;
};

int main(int argc, char* argv[]){ 
    std::string num_indents = "    ";
    int explore_depth = 2;
    bool html = false;
    bool alphabetical = false;
    bool age = false;
    bool fileType = false;
    std::vector<int> fileList;
    
    //Get the current time to check against for age of file
    time(&currentTime);

    for(int ind = 1; ind < argc; ind++){
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
                        std::string numString = temp.substr(finder+1);
                        try {
                            int indents = std::stoi(numString);                 
                            //if given an invalid amount of indents revert to base case
                            switch (indents)
                            {
                            case 1:
                                num_indents = " ";
                                break;
                            case 2:
                                num_indents = "  ";
                                break;
                            case 3:
                                num_indents = "   ";
                                break;
                            case 5:
                                num_indents = "     ";
                                break;
                            case 6:
                                num_indents = "      ";
                                break;
                            case 7:
                                num_indents = "       ";
                                break;
                            case 8:
                                num_indents = "        ";
                                break;                                
                            default:
                                break;
                            }
                            
                            //prints for testing
                        } catch (const std::invalid_argument& e) {
                            std::cerr << "Conversion error: " << e.what() << std::endl;
                        } catch (const std::out_of_range& e) {
                            std::cerr << "Conversion error: " << e.what() << std::endl;
                            }
                    }
                    break;
                case 'a':
                //last access
                    age = true;
                    break;
                case 'd':
                    // valid rang 1-8 and def of 4
                    finder = temp.find('=');
                    //if a value is specified extract and set explore_depth, 
                    if(finder != std::string::npos){    
                        std::string numString = temp.substr(finder+1);
                        try {
                            explore_depth = std::stoi(numString);
                            if(explore_depth < 0 || explore_depth > 8) {
                                //if given an invalid amount of indents revert to base case
                                explore_depth = 2;
                            }
                        } catch (const std::invalid_argument& e) {
                            std::cerr << "Conversion error: " << e.what() << std::endl;
                        } catch (const std::out_of_range& e) {
                            std::cerr << "Conversion error: " << e.what() << std::endl;
                            }
                    }
                    break;
                case 'h':
                    //generate output in HTML using font size and shading to represent file size and age
                    html = true;
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
            // does not handle the > opp yet for html output
            fileList.push_back(ind);
        }
    }

    //if no files are listed use the current directory
    fileData temp;
    //vector of fileData might be better than strings for formating
    std::vector<fileData> formated;
    if (fileList.empty()){
        //open curr directory if no files were provided
        const char * dir = ".";
        //might just do a handledir call w current depth set to 0 and explore depth the same
        std::vector<fileData> tempVec = handle_dir(dir, explore_depth, 0, alphabetical, fileType, age);
        formated.insert(formated.begin(), tempVec.begin(), tempVec.end());
    } else{
        //handle all files in the list
        int current_depth = 0;

        for (int value : fileList) {
            std::string file  = argv[value];
            int place = 0;
            temp = checkFileAttr(file, current_depth, age, fileType);
            if(alphabetical){
                place = alphabetize(&formated, &temp);
            }
            //if it is a directory handle it
            if(temp.fileType == 0 && explore_depth !=0){
                //Handle exploration of directory here
                std::vector<fileData> tempVec = handle_dir(file, explore_depth-1, current_depth + 1, alphabetical,  fileType, age);
                if(alphabetical){
                    formated.insert(formated.begin() + place, tempVec.begin(), tempVec.end());
                }else{
                    formated.insert(formated.end(), tempVec.begin(), tempVec.end());
                }
            }
            if(alphabetical){
                formated.insert(formated.begin() + place, temp);
            }else{
                formated.insert(formated.end(), temp);
            }


        }
    }

    if(html){
        std::cout << "<!DOCTYPE html>"<< std::endl;
        std::cout << "<html> </body> " << std::endl;
        for(fileData entry : formated){
            
            std::string printString = fileFormatHTML(entry, num_indents);
            std::cout << printString<< std::endl;
             
        }
        std::cout << "</body></html>" <<  std::endl;
    } else{   
        for(fileData entry : formated){
            std::string printString = "";
            if(entry.depth > 0){
                for(int i = 0; i<entry.depth;i++){
                    printString.append(num_indents);
                }
                std::string ret = fileFormat(entry);
                printString.append(ret);

            }else{
                printString = fileFormat(entry);
            }       
            std::cout << printString << std::endl;     
        }
    }

    //actual processsing

    return 0;
 };