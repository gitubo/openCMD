#pragma once

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace opencmd {
    class Logger {
	 	public:
			enum class Level {
	   			DEBUG,
	   			INFO,
	   			WARNING,
	   			ERROR,
	   			CRITICAL
	   		};

   			struct Output {
     			enum Type {
     				STDOUT,
     				FILE
     			};
    			Type type;
    			std::string filename;
   			};

	  		~Logger();

			static Logger& getInstance(Output output = {Output::STDOUT, ""});

   			Level getSeverity();
			void setSeverity(Level severity);

	  		void log(const std::string& message, Level level);
	  		void debug(const std::string& message);
	  		void info(const std::string& message);
	  		void warning(const std::string& message);
	  		void error(const std::string& message);
	  		void critical(const std::string& message);

		private:
	   		explicit Logger(Output output);
			Logger(const Logger&) = delete;
	  		Logger& operator=(const Logger&) = delete;
			Level severity = Level::DEBUG;
			Output output;
	  		std::ofstream file;
    };
}
