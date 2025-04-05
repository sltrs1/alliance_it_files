#include "helpers.h"

/*
Эта функция проверяет, подходит ли переданная строка под
один из заданных паттернов, а также удаляел ведущие нули и пробелы
*/
std::string validateString(const std::string& input)
{
    std::string res;

    std::vector<std::regex> recordPatterns;

    /*0*/ recordPatterns.emplace_back( R"([A-Z]{3}[0-9]{1,4})" ); // [A-Z][A-Z][A-Z]
    /*1*/ recordPatterns.emplace_back( R"([A-Z][A-Z] [0-9]{1,4})" ); // [A-Z][A-Z] space
    /*2*/ recordPatterns.emplace_back( R"([A-Z]{2}[0-9]{1,5})" ); // [A-Z][A-Z]
    /*3*/ recordPatterns.emplace_back( R"((?=(?:[A-Z][0-9]|[0-9][A-Z]|[A-Z]{2}))[A-Z0-9]{2} [0-9]{1,4})" ); // [A-Z 0-9][A-Z 0-9] space, минимум одна буква
    /*4*/ recordPatterns.emplace_back( R"((?=(?:[A-Z][A-Z0-9]|[A-Z0-9][A-Z]))[A-Z0-9]{2}[0-9]{1,5})" ); // [A-Z 0-9][A-Z 0-9], минимум одна буква
    /*5*/ recordPatterns.emplace_back( R"(^[0-9]{1,5}$)" ); // код авиакомпании отсутствует, есть только номер рейса

    bool isValid = false;

    if(input.length() > RECORD_MAX_LEN)
    {
        isValid = false;
        return "";
    }

    int reCount = 0;
    for(const auto & re: recordPatterns)
    {
        if ( std::regex_match(input, re) )
        {
            isValid = true;
            break;
        }
        else
        {
            ++reCount;
        }
    }

    if (isValid == false)
    {
        return "";
    }

    std::string companyCode;
    std::string flightNumber;

    switch (reCount)
    {
        case 0:
            companyCode = input.substr(0,3);
            flightNumber = input.substr(3);
            break;
        case 1:
            // Пропуск пробела
            companyCode = input.substr(0,2);
            flightNumber = input.substr(3);
            break;
        case 2:
            companyCode = input.substr(0,2);
            flightNumber = input.substr(2);
            break;
        case 3:
            // Пропуск пробела
            companyCode = input.substr(0,2);
            flightNumber = input.substr(3);
            break;
        case 4:
            companyCode = input.substr(0,2);
            flightNumber = input.substr(2);
            break;
        case 5:
            companyCode = "";
            flightNumber = input;
            break;
        default:
            companyCode = "";
            flightNumber = "";
            break;
    }

    // Удалить ведущие нули, если номер рейса не пустой
    // Для случая, когда вся строка состоит из нулей, оставить один ноль
    if (!flightNumber.empty())
    {
        flightNumber.erase(0, flightNumber.find_first_not_of('0'));

        if (flightNumber.empty())
        {
            flightNumber = "0";
        }
    }

    // Полная информация, очищенная от ведущих нулей и пробелов
    return companyCode + flightNumber;

}


/*
Тут происходит проверка валидности строк и непосредственное сравнение
записей, приведенных к унифицированному виду
*/
bool compareFlightRecords(const std::string& first, const std::string& second)
{
    std::string firstRecord = validateString(first);
    std::string secondRecord = validateString(second);

    if (firstRecord.empty() || secondRecord.empty())
    {
        return false;
    }
    else
    {
        return firstRecord == secondRecord;
    }


}


std::string getOutFileName(const std::string& fileName)
{
    std::string outName = fileName;
    size_t pos = outName.find("in");
    if (pos != std::string::npos)
    {
        outName.replace(pos, 2, "out");
    }
    else
    {
        return "";
    }

    return outName;


}

// Мьютекс на вывод сообщения об ошибке, чтобы
// сообщения от разных потоков не смешивались.
std::mutex cerr_mtx;

int processFile(const std::string& fileName)
{

    std::ifstream inFile(fileName);
    if (!inFile.is_open())
    {
        std::lock_guard<std::mutex> lock(cerr_mtx);
        std::cerr << "Failed to open input file " << fileName << std::endl;
        return -1;
    }

    std::string outFileName = getOutFileName(fileName);
    std::ofstream outFile(outFileName);
    if (!outFile.is_open())
    {
        std::lock_guard<std::mutex> lock(cerr_mtx);
        std::cerr << "Failed to open output file " << outFileName << std::endl;
        return -2;
    }


    // Для проверки уникальности используется unordered_set,
    // т.к. он содержит уникальные значения и сигнализирует
    // о попытке вставить дубликат.
    // Также unordered_set представляет собой хэш-таблицу,
    // что означает, что решение задачи имеет сложность O(n).
    std::unordered_set<std::string> entries;
    std::string line{""};
    while (std::getline(inFile, line))
    {
        std::string validatedEntry = validateString(line);
        if ( !validatedEntry.empty() )
        {
            if (entries.insert(validatedEntry).second)  // вставка удалась (строка уникальна)
            {
                outFile << validatedEntry << std::endl;
            }
        }
    }

    return entries.size();

}
