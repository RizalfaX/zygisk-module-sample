#include <string>
#include <fstream>

void module(const char* target_data_dir)
{
    std::fstream log("/data/data/com.kiarygames.tinyroom/dump.txt", std::ios_base::app);

    log << "Hello World!" << std::endl;

    log.close();
}