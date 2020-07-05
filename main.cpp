#include "FileMMap.h"
#include "Parser.h"
#include <thread>
#include <mutex>
#include <memory>
#include <boost/exception/diagnostic_information.hpp>

int main(int argc, char* argv[])
{
    try
    {
        if (argc < 1)
            std::cout << "Pls enter path to file as first parameter: X:\\file" << std::endl;

        FileMMap fileMap(argv[1]);
    
        size_t processorCount = std::thread::hardware_concurrency();
    
        if (processorCount > 1)
            --processorCount;
        
        uintmax_t fileSize = fileMap.getSize();
        uintmax_t blockSize = fileSize/processorCount;
        
        uintmax_t begin = 0;
        uintmax_t end = 0;
    
        double pathPositive{}; // 1 direction
        double pathNegative{}; // -1 direction
        std::vector<std::unique_ptr<std::thread>> vec;
        std::mutex mutex;
        for(size_t i = 0; i < processorCount; ++i)
        {
            if (auto result = Parser::GetBlockBegin(fileMap, begin))
                begin = result.value();
            else
                break;
            
            if (auto result = Parser::GetBlockEnd(fileMap,
                (begin + blockSize > fileSize) ? fileSize : begin + blockSize))
                end = result.value();
            else
                break;
            
            vec.push_back(std::unique_ptr<std::thread> (new std::thread(
                [&fileMap, &pathPositive, &pathNegative, begin, end, &mutex]()
                {
                    try
                    {
                        Parser parser(fileMap);
                        auto result = parser.Parse(begin, end);
                        std::lock_guard<std::mutex> lock(mutex);
                        pathPositive += result.pathPositive;
                        pathNegative += result.pathNegative;
                    }
                    catch (boost::exception const& e)
                    {
                        std::cerr << boost::diagnostic_information(e); // add logger because cerr is not thread safety
                    }
                    catch (std::exception const& e)
                    {
                        std::cerr << e.what();
                    }
                    catch (...)
                    {
                        std::cerr << " unknown error ";
                    }
                }
            )));
            
            if (end + 1 < fileSize)
                begin = end + 1;
            else
                break;
        }
        
        for (auto &job : vec)
        {
            if (job->joinable())
                job->join();
        }
        
        std::cout << "Result calculating positive path(direction = 1) = " << pathPositive << std::endl;
        std::cout << "Result calculating negative path(direction = -1) = " << pathNegative << std::endl;
    }
    catch (boost::exception const& e)
    {
        std::cerr << boost::diagnostic_information(e);
    }
    catch (std::exception const& e)
    {
        std::cerr << e.what();
    }
    catch (...)
    {
        std::cerr << " unknown error ";
    }
    return 0;
}