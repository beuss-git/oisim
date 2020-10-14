#pragma once
#include <string>
#include <unordered_map>
#include <Windows.h>

#include "json.hpp"

using json = nlohmann::json;

class Simulator {
public:	
	//Simulator() = default;
	/**
	 * \brief : Set the activation key and exit key, see a key reference here:
	 * \n https://georezo.net/jparis/mb_r/dll/pages_user/virtual_key_codes.htm
	 * \param activation_key key used to start sending input 
	 * \param exit_key key used to stop the program (default is F12)
	 */
	Simulator(int activation_key = 8, int exit_key = 123);

	void start(const std::string& file_path);


	void add_command(const std::string& data, int delay = 0);
	void add_command(const std::wstring& data, int delay = 0);
private:
	[[nodiscard]] static bool is_key_pressed(int keycode);

	[[nodiscard]] bool check_activation_key() const;

	[[nodiscard]] bool check_exit_key() const;

	void load_json_file();

	void do_work() const;

	static void send_data(const std::wstring& str);

	static void my_sleep(int ms);
protected:
	int activation_key_ = 0;
	int exit_key_ = 0;
	int reload_config_key_ = 0;
	bool automatic_newline_ = false;
	bool default_delay_ = false;
	std::vector<std::pair<std::wstring, int>> commands_{};
	std::string config_file_path_{};
};

