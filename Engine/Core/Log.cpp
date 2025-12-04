#include "Core/Log.h"
#include "Core/Engine.h"
#include <iostream>

Log::Log(Engine* engine) : System(engine) {
    std::cout.rdbuf(verboseStream.rdbuf());
    std::cerr.rdbuf(errorStream.rdbuf());
}

Log::~Log() {
    std::cout.rdbuf(std::cout.rdbuf());
    std::cerr.rdbuf(std::cerr.rdbuf());
}

void Log::Write(const std::string& message, Level level) {
    switch (level) {
    case Level::Verbose:
        verboseStream << message << std::endl;
        break;
    case Level::Info:
        infoStream << message << std::endl;
        break;
    case Level::Warning:
        warningStream << message << std::endl;
        break;
    case Level::Error:
        errorStream << message << std::endl;
        break;
    }
}

std::stringstream& Log::GetStream(Level level) {
    switch (level) {
    case Level::Verbose:
        return verboseStream;
    case Level::Info:
        return infoStream;
    case Level::Warning:
        return warningStream;
    case Level::Error:
        return errorStream;
    default:
        return infoStream;
    }
}

void Log::Clear() {
    verboseStream.str("");
    infoStream.str("");
    warningStream.str("");
    errorStream.str("");
}

void Log::Update(double deltaTime) {
    pullMessagesFromStream(verboseStream, Level::Verbose);
    pullMessagesFromStream(infoStream, Level::Info);
    pullMessagesFromStream(warningStream, Level::Warning);
    pullMessagesFromStream(errorStream, Level::Error);
}

void Log::pullMessagesFromStream(std::stringstream& stream, Level level) {
    std::string line;
    stream.seekg(0, std::ios::end);
    if (stream.tellg() > 0) {
        stream.seekg(0, std::ios::beg);
        while (std::getline(stream, line)) {
            MessageLogged.Fire(line, level);
        }
        stream.str("");
    }
    stream.clear();

}