#include "InputSimulator.h"

#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>
#include <Windows.h>
#include <fstream>

#include "Utf8Utils.h"

extern std::unordered_map<wchar_t, int> character_map;

Simulator::Simulator(const int activation_key, const int exit_key) : activation_key_(activation_key), exit_key_(exit_key) {
	std::locale::global(std::locale(""));
}

void Simulator::start() const {
	while (true) {
		if (check_activation_key()) {
			do_work();
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
		return settings[setting_name].get<T>();
	} catch (const std::exception& e) {
		std::cout << "There was an error finding " << setting_name << "\n";
		std::cout << e.what() << "\n";
		return T{};
	}
}

void Simulator::load_json_file(const std::string& file_path) {
	std::ifstream i(file_path);
	if (i.fail()) {
		std::cout << "The config file \"" << file_path << "\" doesn't exist\n";
		return;
	}
	json j;
	try {
		i >> j;
	} catch (const std::exception& e) {
		std::cout << e.what() << "\n";
		return;
	}

	// get the settings
	activation_key_ = get_setting<int>(j, "activation_key");
	exit_key_ = get_setting<int>(j, "exit_key");
	automatic_newline_ = get_setting<bool>(j, "automatic_newline");
	default_delay_ = get_setting<int>(j, "default_delay");

	for (const auto& command : j["commands"]) {
		int delay = -1;
		std::string command_string;

		try {
			if (command.size() > 1) {
				delay = command[1].get<int>();
			}
			command_string = command[0].get<std::string>();
		} catch (const std::exception& e) {
			std::cout << e.what() << "\n";
			return;
		}

		add_command(command_string, delay);
	}

}

void Simulator::add_command(const std::string& data, int delay) {
	add_command(utf8_decode(data), delay);
}

void Simulator::add_command(const std::wstring& data, int delay) {
	commands_.emplace_back(std::make_pair(data, delay));
}

bool Simulator::is_key_pressed(int keycode) {
	return GetAsyncKeyState(keycode) & 0x8000;
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

		std::wcout << L"Injecting: " << command << L"\n";

		send_data(command);
	}
}

void Simulator::send_data(const std::wstring& str) {
	for (const auto c : str) {
		try {
			const int keycode = character_map.at(toupper(c));

			const bool should_shift = isupper(c);

			send_keycode(keycode, should_shift);
		} catch (std::out_of_range& e) {
			std::cout << e.what() << "\n";
			std::cout << "Character \'" << c << '\'' << " is not supported\n";
		}
	}
}

void Simulator::send_keycode(int keycode, bool should_shift) {
	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	if (should_shift) {
		// press shift key;
		ip.ki.dwFlags = KEYEVENTF_SCANCODE;
		ip.ki.wScan = MapVirtualKey(VK_LSHIFT, 0);
		SendInput(1, &ip, sizeof(INPUT));
	}

	// press key
	ip.ki.wVk = keycode;
	ip.ki.dwFlags = 0;
	SendInput(1, &ip, sizeof(INPUT));

	// release key
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));

	if (should_shift) {
		// release shift key;
		ip.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		ip.ki.wScan = MapVirtualKey(VK_LSHIFT, 0);
		SendInput(1, &ip, sizeof(INPUT));
	}
}

void Simulator::my_sleep(int ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

std::unordered_map<wchar_t, int> character_map = {
	{L'\t', 9},
	{L'\n', 13},
	{L' ', 32},
	{L'0', 48},
	{L'1', 49},
	{L'2', 50},
	{L'3', 51},
	{L'4', 52},
	{L'5', 53},
	{L'6', 54},
	{L'7', 55},
	{L'8', 56},
	{L'9', 57},
	{L'A', 65},
	{L'B', 66},
	{L'C', 67},
	{L'D', 68},
	{L'E', 69},
	{L'F', 70},
	{L'G', 71},
	{L'H', 72},
	{L'I', 73},
	{L'J', 74},
	{L'K', 75},
	{L'L', 76},
	{L'M', 77},
	{L'N', 78},
	{L'O', 79},
	{L'P', 80},
	{L'Q', 81},
	{L'R', 82},
	{L'S', 83},
	{L'T', 84},
	{L'U', 85},
	{L'V', 86},
	{L'W', 87},
	{L'X', 88},
	{L'Y', 89},
	{L'Z', 90},
	{L'Æ', 222},
	{L'Ø', 192},
	{L'Å', 221},
	{L'\'', 191},
	{L'*', 106},
	{L'+', 107 },
	{L'-', 109},
	{L'.', 110},
	{L'/', 111}
};
