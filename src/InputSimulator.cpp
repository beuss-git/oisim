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

void Simulator::start() const {
	while (true) {
		if (check_activation_key()) {
			std::cout << termcolor::cyan << "\nStarting input injection...\n";
			do_work();
			std::cout << termcolor::cyan << "Finished input injection\n";
		} else if (check_exit_key()) {
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

void Simulator::load_json_file(const std::string& file_path) {
	std::ifstream i(file_path);
	std::cout << termcolor::green << "Loading file: " << termcolor::white << file_path << "\n";
	
	if (i.fail()) {
		std::cout << termcolor::red << "The config file \"" << file_path << "\" doesn't exist\n";
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
	automatic_newline_ = get_setting<bool>(j, "automatic_newline");
	default_delay_ = get_setting<int>(j, "default_delay");

	for (const auto& command : j["commands"]) {
		int delay = -1;
		std::string command_string;

		try {
			command_string = command[0].get<std::string>();

			if (command.size() > 1) {
				delay = command[1].get<int>();
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

		send_data(command);
	}
}

void Simulator::send_data(const std::wstring& str) {
	for (const auto c : str) {
		try {
			send_keycode(c);
		} catch (std::out_of_range& e) {
			std::cout << e.what() << "\n";
			std::cout << termcolor::red << "Character \'" << termcolor::white;
			std::wcout << c;
			std::cout << termcolor::red << '\'' << " is not supported\n";
		}
	}
}

void Simulator::send_keycode(const wchar_t ch) {
	INPUT ip{};
	ip.type = INPUT_KEYBOARD;

	// press key
	if (ch == L'\n') {
		ip.ki.wVk = 13;
	} else {
		ip.ki.wScan = ch;
		ip.ki.dwFlags = KEYEVENTF_UNICODE;
	}
	SendInput(1, &ip, sizeof(INPUT));

	// release key
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));
}
void Simulator::my_sleep(int ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
