#pragma once
#include <iostream>
#include <Windows.h>

namespace KeyPrinter {
	void start() {
		while (true) {
			for (int i = 0; i < 255; i++) {
				if (GetAsyncKeyState(i) & 0x0001) {
					std::cout << "Pressing: " << i << "\n";
				}
			}
		}
	}
}
