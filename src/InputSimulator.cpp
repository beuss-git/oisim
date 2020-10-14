#include "InputSimulator.h"

#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>
#include <Windows.h>
#include <fstream>
#include <iomanip>

#include "Utf8Utils.h"
#include "termcolor.hpp"

Simulator::Simulator(const int activation_key, const int exit_key) : activation_key_(activation_key), exit_key_(exit_key) {
	std::locale::global(std::locale(""));
}

void Simulator::start(const std::string& file_path){
	if (!file_path.empty()) {
		config_file_path_ = file_path;
		load_json_file();
	}

	while (true) {
		if (is_key_pressed(activation_key_)){
			std::cout << termcolor::cyan << "\nStarting input injection...\n";
			do_work();
			std::cout << termcolor::cyan << "Finished input injection\n";
		}
		else if (is_key_pressed(reload_config_key_)) {
			load_json_file();
		}
		else if (is_key_pressed(exit_key_)) {
			return;
		}

		my_sleep(10);
	}
}

template <typename T>
T get_setting(const nlohmann::json j, const std::string& setting_name) {
	try {
		const auto& settings = j["settings"];
		const auto value = settings[setting_name].get<T>();

		std::cout << '\t' << termcolor::yellow << setting_name << termcolor::white << 
			": " << std::setw(20 - setting_name.length()) << termcolor::white << value << "\n";

		return value;
	} catch (const std::exception& e) {
		std::cout << termcolor::red << "There was an error finding " << termcolor::white << setting_name << "\n";
		std::cout << e.what() << "\n";
		return T{};
	}
}

void Simulator::load_json_file() {
	if (config_file_path_.empty()) {
		std::cout << termcolor::red << "No file path set\n";
		return;
	}

	std::ifstream i(config_file_path_);
	std::cout << termcolor::green << "Loading file: " << termcolor::white << config_file_path_ << "\n";
	
	if (i.fail()) {
		std::cout << termcolor::red << "The config file \"" << config_file_path_ << "\" doesn't exist\n";
		return;
	}
	json j;
	try {
		i >> j;
	} catch (const std::exception& e) {
		std::cout << termcolor::red << e.what() << "\n";
		return;
	}

	std::cout << termcolor::bold << termcolor::green << "\nSettings:\n";

	// get the settings
	activation_key_ = get_setting<int>(j, "activation_key");
	exit_key_ = get_setting<int>(j, "exit_key");
	reload_config_key_ = get_setting<int>(j, "reload_config_key");
	automatic_newline_ = get_setting<bool>(j, "automatic_newline");
	default_delay_ = get_setting<int>(j, "default_delay");

	commands_.clear();

	for (const auto& command : j["commands"]) {
		int delay = -1;
		std::string command_string;

		try {
			command_string = command.at(0).get<std::string>();

			if (command.size() > 1) {
				delay = command.at(1).get<int>();
			}
		} catch (const std::exception& e) {
			std::cout << termcolor::red << e.what() << "\n";
			return;
		}

		add_command(command_string, delay);
	}
}

void Simulator::add_command(const std::string& data, const int delay) {
	add_command(utf8_decode(data), delay);
}

void Simulator::add_command(const std::wstring& data, const int delay) {
	commands_.emplace_back(std::make_pair(data, delay));
}

bool Simulator::is_key_pressed(const int keycode) {
	return GetAsyncKeyState(keycode) & 0x0001;
}

bool Simulator::check_activation_key() const {
	return is_key_pressed(activation_key_);
}

bool Simulator::check_exit_key() const {
	return is_key_pressed(exit_key_);
}

void Simulator::do_work() const {
	for (const auto& [command, delay] : commands_) {
		// we don't want any delay if the user manually set it to 0
		if (delay != 0) {
			my_sleep(delay);
		} else if (delay == -1 && default_delay_ != 0) {
			my_sleep(default_delay_);
		}

		std::cout << termcolor::green << "Injecting: " << termcolor::white;
		std::wcout << command << (command[command.length() - 1] != L'\n' ? L"\n" : L"");

		auto final_command = command;
		if (automatic_newline_) {
			final_command.push_back(L'\n');
		}
		send_data(final_command);
	}
}

void Simulator::send_data(const std::wstring& str) {
	std::vector<INPUT> inputs;
	for (const auto c : str) {

		INPUT ip{};
		ip.type = INPUT_KEYBOARD;

		if (c == L'\n') {
			ip.ki.wVk = 13;
		}
		else {
			ip.ki.wScan = c;
			ip.ki.dwFlags = KEYEVENTF_UNICODE;
		}

		inputs.emplace_back(ip);
		ip.ki.dwFlags = KEYEVENTF_KEYUP;
		inputs.emplace_back(ip);
	}

	SendInput(inputs.size(), inputs.data(), sizeof(INPUT));
}

void Simulator::my_sleep(int ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
