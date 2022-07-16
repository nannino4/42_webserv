#pragma once

#include <string.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unistd.h>

bool check_file_exist(std::string full_path);
std::string get_working_path();