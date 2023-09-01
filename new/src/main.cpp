#include <iostream>
#include <istream>
#include <string>
#include <sstream>

/* C libs */
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <pwd.h>
#include <dirent.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "playback.h"

/* functions involving the config file */
namespace conf
{
	char *conf_path;
	constexpr char *def_path = (char*)"/.config/tunez/config";
	uid_t uid;

	typedef struct {
		std::string prompt;
	}OPS;

	/* gets path to the config file */
	int get_path (void)
	{
		conf_path = new char[256];
		struct passwd *pwd;

		/* get the user id */
		uid = getuid();

		/* getting user infomation */
		pwd = getpwuid(uid);

		/* realloc() ? */
		if (sizeof(pwd->pw_dir) > sizeof(conf_path)) {
			delete[] conf_path;
			conf_path = new char[sizeof(pwd->pw_dir)];
		}

		/* setting the path as a string */
		strncpy(conf_path, pwd->pw_dir, strlen(pwd->pw_dir));
		strncat(conf_path, def_path, strlen(def_path));
		std::cout << conf_path << std::endl;
		return 0;
	}

	/* gets prompt from config file */
	char *get_prompt (void)
	{
		char *prompt = new char[1024];

		if (prompt == nullptr)
			return nullptr;

		delete[] prompt;
		return nullptr;	
	}

	int parse (void)
	{
		return 0;
	}

	/* the main function for reading config */
	int read (void)
	{
		FILE *fp;
		conf::get_path();

		/* open the config file if it exists */
		fp = fopen(conf_path, "r");

		/* stop but dont throw error if it does not exist */
		if (fp == nullptr) return 0;

		if (conf::parse() != 0) return 1;

		fclose(fp);
		return 0;
	}
}

/* commands that can be called by the user */
namespace cmd
{
	/* gets the file type */
	static int get_type (const char *str)
	{
		char *ext = new char[sizeof(str)];
		size_t i = 0;

		constexpr size_t MAX_EXT_LEN = 4;
		const char *valid_types[] = {
			".wav",
			".mp3",
		};

		if (strlen(str) < 5) return -1;

		/* find index of the . in the file name */
		for (; str[i] != (char) 0; i++)
			if (str[i] == '.') break;

		/* place the file extension in string */
		for (; i < strlen(str); i++)
			strncat(ext, &str[i], 1);
		
		/* see if file type is valid */ 
		for (size_t i = 0; valid_types[i] != nullptr; i++) {
			if (!strncmp(valid_types[i], ext, MAX_EXT_LEN)) {
				memset(ext, 0, sizeof((char*)ext));
				delete[] ext;
				return 0; 
			}
		}

		memset(ext, 0, sizeof((char*)ext));
		delete[] ext;
		return 1;
	}

	void stop (std::string argv[])
	{
		cleanup();
	}

	void pause (std::string argv[])
	{
		Mix_PauseMusic();
	}

	void unpause (std::string argv[])
	{
		Mix_ResumeMusic();
	}

	void quit (std::string argv[])
	{
		exit(0);
	}

	/* plays given file */
	void play (std::string argv[])
	{
		FILE *fp;
		int8_t type = 0;

		/* check file exists */
		if ((fp = fopen(argv[0].c_str(), "r")) == nullptr) {
			std::cerr << "ERROR: No such file or directory\n";
			return;
		}else fclose(fp);

		/* make sure file type is valid */
		if ((type = get_type(argv[0].c_str())) == 1) {
			std::cerr << "ERROR: Invalid file type\n";
			return;
		}

		playback_entry(argv[0].c_str());
		argv[0].clear();
	}

	/* opens a directory and lists the contents */
	void list (std::string argv[])
	{
		DIR *dp;
		char *dir = new char[256];
		struct dirent *entry;

		if (argv == nullptr)
			dir = (char*) ".";
		else dir = (char*) argv[0].c_str();

		dp = opendir(dir);

		if (dp == nullptr) {
			std::cerr << "ERROR: Couldn't open directory" << std::endl;
			return;
		}

		/* print out folder contents to screen */
		while ((entry = readdir(dp)) != nullptr)
			std::cout << entry->d_name << entry->d_type << "\n";

		closedir(dp);
		memset(dir, 0, sizeof((char*) dir));
		delete[] dir;
	}
}

/* all of the command line functions */
namespace cli
{
	class flags {
		public:
			bool playback = false;
			bool background = false;
			bool playlist = false;
			const char *file;
	};

	char *prompt = new char[1024];
	char *pwd = (char*) ".";

	struct COMMANDS {
		std::string str;
		void (*fn)(std::string argv[]);
	};
	
	constexpr size_t COMMANDS_MAX = 11;
	struct COMMANDS commands[COMMANDS_MAX] = {
		{ "play", cmd::play },
		{ "ls", cmd::list },
		{ "stop", cmd::stop },
		{ "pause", cmd::pause },
		{ "unpause", cmd::unpause },
		{ "cd", nullptr },
		{ "clear", nullptr },
		{ "exit", cmd::quit },
		{ "quit", cmd::quit },
		{ "help", nullptr },
	};

	class cstring {
		public:

			/* returns first word from a C string */
			static char *first_word (char *str)
			{
				char *word = new char[sizeof(str)];

				for (size_t i = 0; i < strlen(str); i++) {
					if (isspace(str[i])) break;
					strncat(word, &str[i], 1);
				}

				delete[] word;
				return word;
			}
			
			/* returns a given word from a string */
			static char *get_word (char *str, size_t index)
			{
				char *tmp = new char[sizeof(str)];

				if (str == nullptr)
					return nullptr;

				for (size_t i = 0, w = 0; str[i] != (char) 0; i++) {
					if (isspace(str[i])) w++;

					if (w == index) strncat(tmp, &str[i], 1);
				}

				delete[] tmp;
				return tmp;
			}
	};
	
	flags flags;
	cstring cstring;

	static void exit (void)
	{
		delete[] prompt;
	}

	/* goes through commands struct for matching string with cmd */
	static signed int is_valid_cmd (std::string str) 
	{
		for (size_t i = 0; i < COMMANDS_MAX; i++)
			if (commands[i].str == str)
				return i;

		return -1; 
	}

	/* the main loop of the command line interface */
	static void cli_loop (void)
	{
		int8_t exe;
		std::string str, cmd, tmp;
		std::string args[256];

		while (true) {
			std::cout << prompt;
			std::getline(std::cin, str);
			std::istringstream iss(str);

			/* splitting up the string by words */
			for (size_t i = 0, k = 0; std::getline(iss, tmp, ' '); i++) {
				if (i == 0) cmd = tmp;
				else {
					/* setting args */
					args[k] = tmp;
					k++;
				}
			}

			/* sees if command is valid, and calls function */
			if ((exe = is_valid_cmd(cmd)) == -1)			
				 std::cout << "ERROR: No such command " << cmd << std::endl;
			else commands[exe].fn(args);

			/* getting rid of the values */
			str.clear();
			cmd.clear();
			tmp.clear();

			/* clear the string array */
			std::fill(std::begin(args), std::end(args), 0);
		}
	}
	
	/* a setup function for the cli */
	void entry (void)
	{
		if ((prompt = conf::get_prompt()) == nullptr)
			prompt = (char*) "> ";

		cli_loop();
		exit();
	}

	/* goes through argv looking for args and sets them */
	int parse_args (char *argv[])
	{
		for (size_t i = 0; argv[i] != nullptr; i++)
			if (argv[i][0] == '-' && argv[i][1] != (char) 0)
				for (size_t k = 1; argv[i][k] != (char) 0 && argv[i][k] != ' '; k++)
					switch (argv[i][k]) {
						case 'p':
							flags.playback = true;
							break;
						case 'b':
							flags.background = true;
							break;
						case 'f':
							flags.playlist = true;
							break;
						default:
							std::cerr << "ERROR: No such flag " << argv[i][k] << std::endl;
							return 1;
					}

			return 0;
		}

	/* sets the file/folder to play from command line args */
	int set_file (char *argv[])
	{
		bool set = false;

		for (size_t i = 0; argv[i] != nullptr; i++)
			if (argv[i][0] != '-') {
				flags.file = strndup(argv[i], 256);
				set = true;
			}

		/* making sure a file is passed at the command line */
		if (set == false) {
			std::cerr << "ERROR: No valid arg set" << std::endl;
			return 1;
		}

		return 0;
	}

	/* 'executes' arguments passed from the command line */
	int exec_args (void)
	{
		std::string args[256];

		args[0] = flags.file;

		if (flags.playback == true && flags.playlist == true) {
			std::cerr << "ERROR: -f and -p cannot be used at the same time\n";
			return 1;
		}

		if (flags.playback == true)
			cmd::play(args);

		for (size_t i = 0; i < 200000; i++);
		return 0;
	}
}

int main (int argc, char *argv[])
{
	if (conf::read() != 0) return 1;

	if (argc < 2)
		cli::entry();
	else {
		if (cli::parse_args(argv) != 0) return 1;
		if (cli::set_file(argv) != 0) return 1;
		if (cli::exec_args() != 0) return 1;
	}

	return 0;
}
