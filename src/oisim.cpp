#include "InputSimulator.h"
#include "KeyPrinter.h"

int main() {
	//KeyPrinter::start();
	Simulator simulator;

	//simulator.add_command("L\n", 10);
	//	simulator.add_command("Ringenes Herre - Atter en konge\n", 10);
	//	simulator.add_command("201\n", 10);
	//	simulator.add_command("2\n", 10);

	//simulator.add_command("l\n", 10);
	//	simulator.add_command(L"Flåklypa\n", 10);
	//	simulator.add_command("88\n", 10);
	//	simulator.add_command("1\n", 10);

	//simulator.add_command("s\n", 10);

	//simulator.add_command("B\n", 10);
	//	simulator.add_command("1\n", 10);
	//	simulator.add_command("23\n", 10);

	//simulator.add_command("b\n", 10);
	//	simulator.add_command("2\n", 10);
	//	simulator.add_command("4\n", 10);

	//simulator.add_command("b\n", 10);
	//	simulator.add_command("1\n", 10);
	//	simulator.add_command("8\n", 10);

	//simulator.add_command("Q\n", 10);
	//	simulator.add_command("31\n", 10);
	//	simulator.add_command("4\n", 10);

	simulator.start("config.json");
}
